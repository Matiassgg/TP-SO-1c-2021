#include "filesystem.h"

void inicializar_paths_aux(){
	ruta_superbloque = string_new();
	path_files = string_new();
	path_bloques = string_new();
	path_bitacoras = string_new();

	string_append_with_format(&path_bloques, "%s/Blocks.ims", punto_montaje);
	string_append_with_format(&path_files, "%s/Files", punto_montaje);
	string_append_with_format(&ruta_superbloque, "%s/SuperBloque.ims", punto_montaje);
	string_append_with_format(&path_bitacoras, "%s/Bitacoras", path_files);

	/*
	if (!directorio_existe(path_files)) {
		mkdir(path_files, 0700);
	}

	if (!directorio_existe(path_bitacoras)) {
		mkdir(path_bitacoras, 0700);
	}
	*/
}

void crear_superbloque() {
	FILE* superbloque = fopen(ruta_superbloque,"w");

	fprintf(superbloque,"BLOCK_SIZE=%i\nBLOCKS=%i\nBITMAP=",block_size,blocks);
	fwrite(crear_bitmap(),sizeof(t_bitarray),1,superbloque);

	fprintf(superbloque,"\n");
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


bool directorio_existe(char* path) {
	struct stat st = { 0 }; // stat(2) precisa de un struct stat a modo de buffer para llenar info del archivo que nosotros no necesitamos.
	char* aux = string_duplicate(path);
	bool existe = (stat(aux, &st) != -1);
	free(aux);
	return existe;
}


void leer_superbloque(FILE* archivo){
	char* leido = malloc(sizeof(char));
	int i=0;

	while(fread(leido+i,1,1,archivo)){
		i++;
		leido= (char*) realloc(leido,i+1);
	}
	leido= (char*) realloc(leido,i+1);
	t_bloque_FS* bloqueAux = crear_bloque(leido,0);
	free(leido);

//	magic_number = string_duplicate(obtener_dato_string(bloqueAux, "MAGIC_NUMBER"));
//	blocks = obtener_dato_int(bloqueAux,"BLOCKS");
//	block_size = obtener_dato_int(bloqueAux,"BLOCK_SIZE");

	dictionary_destroy(bloqueAux->diccionario);
	free(bloqueAux->stream);
	free(bloqueAux);
}


//void inicializar_bloques(){
//	path_bloques = string_new();
//
//	string_append_with_format(&path_bloques, "%s/Blocks", punto_montaje);
//
//
//	if (!directorio_existe(path_bloques)) {
//			mkdir(path_bloques, 0700);
//	}
//
//	crearBloques(path_bloques,ruta_superbloque);
//}
//
//
//void crearBloques(char* path_bloques,char* path_superbloque){
//
//	ruta_bloques = string_duplicate(path_bloques);
//
//	log_info(logger, "MONGO-STORE :: SE INICIALIZARAN LOS %d BLOQUES DE %d TAMANIO", blocks, block_size);
//
//	for(int i = 1; i <= blocks; i++){
//		char* pathArchivoBloque = string_new();
//		string_append_with_format(&pathArchivoBloque, "%s", path_bloques);
//		string_append_with_format(&pathArchivoBloque, "/%d.", i);
//
//		if(!archivo_existe(pathArchivoBloque)){
//			FILE* bloque = fopen(pathArchivoBloque, "w");
//			fclose(bloque);
//		}
//
//		free(pathArchivoBloque);
//	}
//}


void inicializar_bitmap(){
	path_superbloque = string_new();

	string_append_with_format(&path_superbloque, "%s/SuperBloque.ims", punto_montaje);

	if (!directorio_existe(path_superbloque)) {
		log_info(logger, "NO EXISTE EL SUPERBLOQUE");
		exit(0);
//		mkdir(path_metadata, 0700);
	}

	crear_bitmap(path_superbloque);
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

