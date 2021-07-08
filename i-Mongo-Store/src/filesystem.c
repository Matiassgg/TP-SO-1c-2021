#include "filesystem.h"

void sincronizar_blocks(){
	sleep(tiempo_sincronizacion);
	memcpy(contenido_blocks,contenido_blocks_aux,block_size * blocks);
	if(msync(contenido_blocks,block_size * blocks,MS_SYNC)==-1)
		log_error(logger, "Error con la sincronizacion de blocks");
	else
		log_info(logger, "Se sincronizo correctamente blocks");
}

void inicializar_paths_aux(){
	path_superbloque = string_new();
	path_blocks = string_new();
	path_files = string_new();
	path_bitacoras = string_new();

	string_append_with_format(&path_superbloque, "%s/SuperBloque.ims", punto_montaje);
	string_append_with_format(&path_blocks, "%s/Blocks.ims", punto_montaje);
	string_append_with_format(&path_files, "%s/Files", punto_montaje);
	string_append_with_format(&path_bitacoras, "%s/Bitacoras", path_files);

	if (!directorio_existe(path_files)) {
		mkdir(path_files, 0700);
	}

	if (!directorio_existe(path_bitacoras)) {
		mkdir(path_bitacoras, 0700);
	}

}

void crear_superbloque() {
	FILE* superbloque = fopen(path_superbloque,"wb");
	t_bitarray* bitarray = crear_bitmap();

	fwrite(&block_size,sizeof(uint32_t),1,superbloque);
	fwrite(&blocks,sizeof(uint32_t),1,superbloque);
	fwrite(bitarray->bitarray, bitarray->size, 1,superbloque);

	free(bitarray);

	fclose(superbloque);
}

void obtener_superbloque(){
	FILE* superbloque = fopen(path_superbloque, "r");

	if(superbloque){
		log_info(logger, "MONGO-STORE :: SE LEE EL SUPERBLOQUE");
		leer_superbloque(superbloque);
		fclose(superbloque);
	}
	else{
		log_info(logger, "MONGO-STORE :: NO SE PUDO LEER EL SUPERBLOQUE");
		exit(0);
	}
}

void leer_superbloque(FILE* archivo){
	FILE* superbloque = fopen(path_superbloque,"rb");

	fread(&block_size,sizeof(uint32_t),1,superbloque);
	fread(&blocks,sizeof(uint32_t),1,superbloque);
	bitarray_string = malloc(blocks / 8);
	fread(bitarray_string,blocks/8,1,superbloque);	// TODO hara falta leer el bitarray? eso se lee en el momento por lo gral

//	t_bitarray* bitarray = bitarray_create(bitarray_string,blocks/8);

	fclose(superbloque);
}

char* dar_hash_md5(char* archivo){
	char* system_command = string_new();
	char* path_aux = string_duplicate("aux.tmp");
	string_append_with_format(&system_command, "md5sum %s > %s", archivo,path_aux);
	system(system_command);
	free(system_command);

	int archivo_aux = open(path_aux, O_RDWR);
	if(archivo_aux == -1){
		log_error(logger, "no se abrio archivo");
	}
	struct stat file_st;
	fstat(archivo_aux, &file_st);

	log_info(logger, "file_st.st_size %i", file_st.st_size);
	char* contenido = malloc(file_st.st_size+1);
	read(archivo_aux,contenido,file_st.st_size);
//	log_info(logger, "contenido %s", contenido);

	char** hash = string_split(contenido," ");

	eliminar_archivo(path_aux);

	return hash[0];
}

void obtener_blocks(){
	log_info(logger, "MONGO-STORE :: SE OBTENDRAN LOS %d BLOQUES DE %d TAMANIO EN EL ARCHIVO %s", blocks, block_size, path_blocks);
	int fd = crear_archivo(path_blocks);
	if(fd == -1) {
		log_error(logger, "MONGO-STORE :: NO SE ABRIO EL ARCHIVO BLOCKS CORRECTAMENTE");
	}
	else {
		log_info(logger, "MONGO-STORE :: SE ABRIO EL ARCHIVO BLOCKS CORRECTAMENTE");struct stat file_st;
		fstat(fd, &file_st);
		contenido_blocks = calloc(1,file_st.st_size);
//		log_info(logger, "%s\nHASH de %s:\n%s\n",contenido_blocks, path_blocks, dar_hash_md5(path_blocks));

		contenido_blocks = mmap(NULL,file_st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		contenido_blocks_aux = calloc(1,file_st.st_size);
		memcpy(contenido_blocks_aux,contenido_blocks,file_st.st_size);
//		log_info(logger, "%s",contenido_blocks);
		if(contenido_blocks == MAP_FAILED) {
			log_error(logger,"Error  mapping - file_st.st_size %i", file_st.st_size);
			exit(1);
		}

//		close(fd);
	}
}

void crear_blocks(){
	log_info(logger, "MONGO-STORE :: SE INICIALIZARAN LOS %d BLOQUES DE %d TAMANIO EN EL ARCHIVO %s", blocks, block_size, path_blocks);
	int fd = crear_archivo(path_blocks);
	if(fd == -1) {
		log_error(logger, "MONGO-STORE :: NO SE CREO EL ARCHIVO BLOCKS CORRECTAMENTE");
	}
	else {
		log_info(logger, "MONGO-STORE :: SE CREO EL ARCHIVO BLOCKS CORRECTAMENTE");
		contenido_blocks = calloc(1,block_size * blocks);
		write(fd,contenido_blocks,block_size * blocks);
//		log_info(logger, "%s\nHASH de %s:\n%s\n",contenido_blocks, path_blocks, dar_hash_md5(path_blocks));
		struct stat file_st;
		fstat(fd, &file_st);
		contenido_blocks = mmap(NULL,file_st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		contenido_blocks_aux = calloc(1,block_size * blocks);
		memcpy(contenido_blocks_aux,contenido_blocks,block_size * blocks);
//		log_info(logger, "%s",contenido_blocks);
		if(contenido_blocks == MAP_FAILED) {
			log_error(logger,"Error  mapping - file_st.st_size %i", file_st.st_size);
			exit(1);
		}

//		close(fd);
	}
}

void crear_bitacoras_de_tripulantes(uint32_t tripulantes){
	for(int i = 1; i <= tripulantes; i++){
		int fd;
		char* pathArchivoBitacora = string_new();
		string_append_with_format(&pathArchivoBitacora, "%s/Tripulante%d.ims", path_bitacoras, i);

		if((fd = crear_archivo(pathArchivoBitacora))==-1){
			close(fd);
		}
		free(pathArchivoBitacora);
	}
}

void inicializar_bitmap(){
	path_superbloque = string_new();

	string_append_with_format(&path_superbloque, "%s/SuperBloque.ims", punto_montaje);

	if (!directorio_existe(path_superbloque)) {
		log_error(logger, "NO EXISTE EL SUPERBLOQUE");
		exit(0);
	}

	t_bitarray* bitarray = crear_bitmap();
	FILE* superbloque = fopen(path_superbloque,"ab+");

	if(superbloque){
		fseek(superbloque,2*sizeof(uint32_t),SEEK_SET);
		fwrite(bitarray->bitarray, bitarray->size, 1, superbloque);
		free(bitarray);

		fclose(superbloque);
	}


}

t_bitarray* crear_bitmap() {
	bitarray_string = malloc(blocks / 8);

		for (int i = 0; i < blocks / 8; i++) {
			bitarray_string[i] = '0';
		}

		t_bitarray* bitarray = bitarray_create_with_mode(bitarray_string,blocks/8,LSB_FIRST);
		for (int i = 0; i < blocks / 8; i++) {
			bitarray_clean_bit(bitarray, i);
		}

		return bitarray;

}

t_bitarray* leer_bitmap(){
	FILE* superbloque = fopen(path_superbloque,"rb");

	fseek(superbloque,2*sizeof(uint32_t),SEEK_SET);

	if(superbloque){

		fread(bitarray_string,blocks/8,1,superbloque);

		t_bitarray* bitarray = bitarray_create(bitarray_string,blocks/8);

		fclose(superbloque);

		return bitarray;
	}

	return NULL;

}

void subir_bitmap(t_bitarray* bitarray){
	FILE* superbloque = fopen(path_superbloque,"ab+");

	if(superbloque){
		fseek(superbloque,2*sizeof(uint32_t),SEEK_SET);
		fwrite(bitarray->bitarray,bitarray->size,1,superbloque);

		fclose(superbloque);
	}

}

bool directorio_existe(char* path) {
	struct stat st = { 0 }; // stat(2) precisa de un struct stat a modo de buffer para llenar info del archivo que nosotros no necesitamos.
	char* aux = string_duplicate(path);
	bool existe = (stat(aux, &st) != -1);
	free(aux);
	return existe;
}

bool archivo_existe(char* nombreArchivo){

    if (access(nombreArchivo, F_OK) != 0){
        log_warning(logger,"'%s' no existe\n", nombreArchivo);
        return false;
    }
    else {
        log_info(logger,"'%s' existe\n", nombreArchivo);
        return true;
    }

}

int eliminar_archivo(char* archivo){
	if(remove(archivo)!=0){
		return 1;
	}else
		return 0;
}

int crear_archivo(char* archivo){
	return open(archivo, O_RDWR | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

char* obtener_path_files(char* pathRelativo) {
	char* path = string_new();
	string_append_with_format(&path, "%s/%s", path_files,pathRelativo);

	return path;
}

bool archivo_recursos_existe(char* nombreArchivo){
	char* ruta_archivo_recursos = obtener_path_files(nombreArchivo);
	return archivo_existe(ruta_archivo_recursos);
}

int crear_archivo_recursos(char* nombreArchivo, char caracter_llenado){
	char* ruta_archivo_recursos = obtener_path_files(nombreArchivo);
	FILE* archivo = fopen(ruta_archivo_recursos, "w+");
	if(archivo){
		char* file_generico = string_duplicate("SIZE=0\nBLOCK_COUNT=0\nBLOCKS=[]\nCARACTER_LLENADO=");
		string_append_with_format(&file_generico,"%c\nMD5_ARCHIVO=", caracter_llenado);
		fwrite(file_generico,string_length(file_generico),1,archivo);

		fclose(archivo);
		string_append(&file_generico,dar_hash_md5(ruta_archivo_recursos));

		return 1;
	}
	return 0;
}

int eliminar_archivo_recursos(char* nombreArchivo){
	char* ruta_archivo_recursos = obtener_path_files(nombreArchivo);
	return eliminar_archivo(ruta_archivo_recursos);
}

void vaciar_archivo(char* archivo){
	char* ruta_completa_archivo = obtener_path_files(archivo);
	FILE * file = fopen(ruta_completa_archivo, "wb");
	if(file==NULL)
		log_warning(logger,"No se pudo abrir el archivo %s al querer vaciarlo",archivo);
	fclose(file);
}

int dar_bloque_libre(){
	t_bitarray* bitarray = leer_bitmap();
	for(int i=0;i<blocks;i++){
		if(bitarray_test_bit(bitarray, i) == 0){
			bitarray_set_bit(bitarray,i);
			subir_bitmap(bitarray);
			bitarray_destroy(bitarray);
			return i;
		}
	}
	bitarray_destroy(bitarray);
	return -1;
}

//
//t_list* subir_buffer_blocks(t_buffer* buffer, uint32_t ultimo_bloque){
//	uint32_t offset = 0;
//	t_list* bloques = list_create();
//	int bloque_libre;
//	while(offset < buffer->size){
//		if(ultimo_bloque != -1){
//			uint32_t espacio_libre = espacio_libre_bloque(ultimo_bloque);
//
//			if(espacio_libre > 0)
//				bloque_libre = ultimo_bloque;
//			else
//				bloque_libre = dar_bloque_libre();
//		}
//		else
//			bloque_libre = dar_bloque_libre(); //TODO hacer verificacion
//		list_add(bloques, bloque_libre);
//
//		uint32_t tamanio_subida = minimo((buffer->size-offset),block_size);
//
//		memcpy(contenido_blocks + (bloque_libre*block_size),buffer->stream,tamanio_subida);
//
//		offset += tamanio_subida;
//	}
//
//	return bloques;
//}

void* obtener_informacion_bloque(uint32_t bloque){
	void* informacion = malloc(block_size);
	memcpy(informacion, contenido_blocks_aux + (bloque*block_size),block_size);

	return informacion;
}

t_list* agregar_caracteres_blocks(char* stream_a_agregar, int ultimo_bloque, int tamanio_restante){
	char* stream = string_duplicate(stream_a_agregar);
	uint32_t offset = 0;
	t_list* bloques = list_create();
	int bloque_libre;

	uint32_t cant_caracteres = string_length(stream);
	log_info(logger, "ultimo_bloque %i - tamanio_restante %i",ultimo_bloque,tamanio_restante);
	while(offset < cant_caracteres){
		log_info(logger, "Se entra al while");
		uint32_t tamanio_subida;
		if(ultimo_bloque != -1){
			log_info(logger, "Se entra al ultimo_bloque != -1");
			bloque_libre = ultimo_bloque;
			ultimo_bloque = -1;
			if(tamanio_restante != -1)
				tamanio_subida = minimo((cant_caracteres - offset),tamanio_restante);
			else
				log_info(logger, "No se entra al tamanio_restante != -1");
		}
		else{
			log_info(logger, "No se entra al ultimo_bloque != -1");
			bloque_libre = dar_bloque_libre(); //TODO hacer verificacion
			list_add(bloques, bloque_libre);
			tamanio_subida = minimo((cant_caracteres - offset),block_size);
			log_info(logger, "tamanio_subida = min(%i,%i)",(cant_caracteres - offset),block_size);
		}

		log_info(logger, "bloque_libre %i - stream %s - tamanio_subida %i\ncontenido_blocks_aux + (bloque_libre*block_size) %s", bloque_libre, stream + offset, tamanio_subida, (char*)contenido_blocks_aux);

		memcpy(contenido_blocks_aux + (bloque_libre*block_size), stream + offset, tamanio_subida);

		offset += tamanio_subida;
	}

	return bloques;
}


char* obtener_stream_tarea(char caracter, t_list* bloques, uint32_t size){
	//------------ORDEN------------
		//1. SIZE
		//2. BLOCK_COUNT
		//3. BLOCKS   					 	} COMO ES LISTA IRA DENTRO DE UN FOR
		//4. CARACTER_LLENADO
		//5. MD5_ARCHIVO
	//-----------------------------
	uint32_t cant_bloques = (uint32_t) list_size(bloques);

	char* stream = string_duplicate("SIZE=");
	string_append_with_format(&stream, "%i\n", size);
	string_append_with_format(&stream, "BLOCK_COUNT=%i\n", cant_bloques);
	string_append_with_format(&stream, "BLOCKS=[");
	for(int i=0;i<cant_bloques;i++){
		string_append_with_format(&stream, "%i", list_get(bloques,i));
		if(i!=cant_bloques-1)
			string_append_with_format(&stream, ",");
	}
	string_append_with_format(&stream, "]\n");
	string_append_with_format(&stream, "CARACTER_LLENADO=%c\n", caracter);
	string_append_with_format(&stream, "MD5_ARCHIVO=%s\n", "XD");

	return stream;
}

void sumar_bloques_config(t_list* bloques, t_config* config){
	uint32_t cant_bloques = config_get_int_value(config, "BLOCK_COUNT");
	if(cant_bloques != 0){
		char** bloques_config = config_get_array_value(config, "BLOCKS");
		for(int i=0; i<cant_bloques;i++){
			bool esta_presente(uint32_t bloque){
				return atoi(bloques_config[i]) == bloque;
			}
			if(!list_any_satisfy(bloques, esta_presente))
				list_add(bloques, atoi(bloques_config[i]));
		}
	}
}

char* actualizar_stream_tarea(char caracter, t_list* bloques, uint32_t size, t_config* config){
	sumar_bloques_config(bloques, config);
	uint32_t size_old = config_get_int_value(config, "SIZE");
	return obtener_stream_tarea(caracter, bloques, size+size_old);
}

void actualizar_archivo_file(char caracter, t_list* bloques, t_config* config, uint32_t size){
	char* stream = actualizar_stream_tarea(caracter, bloques, size, config);

	FILE* archivo = fopen(config->path,"w+");
	fwrite(stream,string_length(stream),1,archivo);
	fclose(archivo);
}

t_buffer* serializar_tarea(tarea_Mongo* tarea) {
	t_buffer* buffer = crear_buffer();
	uint32_t offset = 0;

	char* caracteres = string_repeat(obtener_caracter_llenado(tarea->tarea),tarea->parametro);

	buffer->size = string_length(caracteres)+1;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, caracteres, buffer->size);

	return buffer;
}

int ultimo_bloque_config(t_config* config){
	uint32_t cant_bloques = config_get_int_value(config, "BLOCK_COUNT");
	if(cant_bloques != 0){
		char** bloques_config = config_get_array_value(config, "BLOCKS");

		return atoi(bloques_config[cant_bloques-1]);
	}
	return -1;
}
int tamanio_restante_config(t_config* config){
	uint32_t cant_bloques = config_get_int_value(config, "BLOCK_COUNT");
	if(cant_bloques != 0){
		uint32_t size = config_get_int_value(config, "SIZE");
		return (cant_bloques*block_size) - size;

	}
	return -1;
}

void subir_tarea(char* caracteres, char* archivo){
	t_config* config = config_create(archivo);
	int ultimo_bloque = ultimo_bloque_config(config);
	int tamanio_restante = tamanio_restante_config(config);
	t_list* bloques = agregar_caracteres_blocks(caracteres, ultimo_bloque, tamanio_restante);


	actualizar_archivo_file(caracteres[0], bloques, config, string_length(caracteres));

}



