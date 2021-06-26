#include "filesystem.h"

void inicializar_paths_aux(){
	ruta_superbloque = string_new();
	path_files = string_new();
	ruta_blocks = string_new();
	path_bitacoras = string_new();

	string_append_with_format(&ruta_blocks, "%s/Blocks.ims", punto_montaje);
	string_append_with_format(&path_files, "%s/Files", punto_montaje);
	string_append_with_format(&ruta_superbloque, "%s/SuperBloque.ims", punto_montaje);
	string_append_with_format(&path_bitacoras, "%s/Bitacoras", path_files);

	if (!directorio_existe(path_files)) {
		mkdir(path_files, 0700);
	}

	if (!directorio_existe(path_bitacoras)) {
		mkdir(path_bitacoras, 0700);
	}

}

void crear_superbloque() {
	FILE* superbloque = fopen(ruta_superbloque,"wb");
	t_bitarray* bitarray = crear_bitmap();

	fwrite(&block_size,sizeof(uint32_t),1,superbloque);
	fwrite(&blocks,sizeof(uint32_t),1,superbloque);
	fwrite(bitarray->bitarray, bitarray->size, 1,superbloque);

	free(bitarray);

	fclose(superbloque);
}

void obtener_superbloque(){
	FILE* superbloque = fopen(ruta_superbloque, "r");

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
	char* leido = malloc(sizeof(char));
	int i=0;

	while(fread(leido+i,1,1,archivo)){
		i++;
		leido= (char*) realloc(leido,i+1);
	}

	leido= (char*) realloc(leido,i+1);

	free(leido);
}

void crear_blocks(char* ruta_blocks,char* path_superbloque){
 ruta_blocks = string_duplicate(ruta_blocks);

 log_info(logger, "MONGO-STORE :: SE INICIALIZARAN LOS %d BLOQUES DE %d TAMANIO", blocks, block_size);
//TODO

}

void crear_bitacoras_de_tripulantes(uint32_t tripulantes){
	for(int i = 1; i <= tripulantes; i++){
		char* pathArchivoBitacora = string_new();
		string_append_with_format(&pathArchivoBitacora, "%s", path_bitacoras);
		string_append_with_format(&pathArchivoBitacora, "/Tripulante%d.", i);

		if(!archivo_existe(pathArchivoBitacora)){
			FILE* bloque = fopen(pathArchivoBitacora, "w");
			fclose(bloque);
		}
		free(pathArchivoBitacora);
	}
}

void inicializar_bitmap(){
	path_superbloque = string_new();

	string_append_with_format(&path_superbloque, "%s/SuperBloque.ims", punto_montaje);

	if (!directorio_existe(path_superbloque)) {
		log_info(logger, "NO EXISTE EL SUPERBLOQUE");
		exit(0);
	}

	crear_bitmap();
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

	FILE* superbloque = fopen(ruta_superbloque,"rb");

	fseek(superbloque,2*sizeof(uint32_t),SEEK_SET);

	if(superbloque){

		fread(bitarray_string,blocks/8,1,superbloque);

		t_bitarray* bitarray = bitarray_create(bitarray_string,blocks/8);

		fclose(superbloque);

		return bitarray;
	}

	return NULL;

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
	FILE * file;
	file = fopen(archivo, "wb");
	if(file==NULL){
		return 0;
	}
	return 1;
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

int crear_archivo_recursos(char* nombreArchivo){
	char* ruta_archivo_recursos = obtener_path_files(nombreArchivo);
	if(crear_archivo(ruta_archivo_recursos)){
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
