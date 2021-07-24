#include "sabotaje.h"


void resolver_sabotaje_superbloque_blocks(){
	//TODO
//sobreescribir “cantidad de bloques” del superbloque con la cantidad de bloques real en el disco (tamaño del blocks.ims)
	struct stat st;
	stat(path_blocks, &st);
	uint32_t size_blocks_ims = st.st_size;
	uint32_t sizeUnBlock = block_size; // si modifican el campo block_size no se.
	uint32_t blocksReales = size_blocks_ims / sizeUnBlock;
	actualizar_valor_blocks_superbloque(blocksReales);
}

void resolver_sabotaje_superbloque_bitmap(){
//TODO
//Corregir el bitmap con lo que se halle en la metadata de los archivos.
	//Constatar contra el bitmap que esten todos los valores correctos.
//	Si el bloque está cruzado en un archivo, tiene que estar marcado como usado, y si no , tiene que estar marcado como libre
	t_list* bloquesUsadosPorFiles = obtener_bloques_usados();
//TODO	list_any_satisfy(bloquesUsadosPorFiles,comparar_bitmap());

//	t_bitarray* bitarray = leer_bitmap();
//
//	    bool actualizar_bitmap(int bloques){
//	        if(bitarray_test_bit(bitarray,bloques) == 0)
//	            bitarray_set_bit(bitarray,bloques);
//	    }
//	    list_iterate(lista_bloques,actualizar_bitmap);
}

void detectar_sabotaje_superbloque_bitmap(){
	t_list* bloques_usados = obtener_bloques_usados();
	t_bitarray* bitarray = leer_bitmap();

	bool inconsistencia_bitmap(int bloque){
		if(bitarray_test_bit(bitarray,bloque)==0){
			return 1;
		}
	}
	bool no_contiene(int bloque){

	}
//	list_any_satisfy(bloques_usados,inconsistencia_bitmap);
//	for(int=0,i<bitarray_get_max_bit(bitarray),i++){
//		if(bitarray_test_bit(bitarray,i)==1 && list_all_satisfy(bloques_usados,no_contiene()))
//			return 1;
//	}
}

void resolver_sabotaje_files_size(char* archivo){
	//TODO
//Asumir correcto lo encontrado en los bloques.
//Hay que recorrer todos los bloques del archivo y obtener y asumir como correcto el tamaño encontrado dentro de los bloques recorriendolos en orden.
//	Ej. OOOOOOOOOOOOOOOOOOOOOOOOO recorremos hasta encontrar el primer carácter que no es una O, llegué al size 400, ese será el size del archivo.
//	PONER UN CENTINELA PARA DECIR HASTA ACÁ LLEGUE
	char* path = obtener_path_files(archivo);
	t_config* archivo_recurso = config_create(path);

}

void resolver_sabotaje_files_blockcount(char* archivo){
	//Actualizar el valor de Block_count en base a lo que está en la lista de Blocks.
	char* path = obtener_path_files(archivo);
	t_config* archivo_recurso = config_create(path);
	char** blocks = config_get_array_value(archivo_recurso,"BLOCKS");
	config_set_value(archivo_recurso,"BLOCK_COUNT",string_length(blocks));
	config_save(archivo_recurso);
	config_destroy(archivo_recurso);
}

void resolver_sabotaje_files_blocks(char*archivo){
	//TODO
	//Lista de blocks alterado.
	//Entra en juego el md5_archivo. Concatenamos los bloques, calculamos el md5 hasta el size, y lo comparamos con el md5 que tenemos guardado.
	//Si no coinciden restaurar archivo, escribiendo tantos caracteres de llenado hasta completar el size, en el orden de bloques que tenemos
	char* path = obtener_path_files(archivo);
	t_config* archivo_recurso = config_create(path);
}


void actualizar_valor_blocks_superbloque(uint32_t blocksReales){
	FILE* superbloque = abrirSuperbloque("rb+");
	fseek(superbloque,sizeof(uint32_t),SEEK_SET);
	fwrite(blocksReales,sizeof(uint32_t),1,superbloque);
	log_info("Resolviendo sabotaje -> Se actualiza valor de blocks de superbloque a %d",blocksReales);
}

FILE* abrirSuperbloque(char* modo){
	FILE * superbloque = fopen(path_superbloque,modo);
	if(superbloque == -1){
		log_error(logger, "Error al leer superbloque");
		exit(1);
	}
	return superbloque;
}

t_list* listaArchivosDeBitacora() {

	DIR *d;
	t_list* listaArchivos = list_create();
	struct dirent *dir;
	d = opendir(path_bitacoras);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if( son_iguales( dir->d_name, "." ) != 0 &&
					son_iguales( dir->d_name, ".." ) != 0){

				char* cadena = string_duplicate(dir->d_name);
				list_add(listaArchivos,cadena);

			}
		}
		closedir(d);
	}

	return listaArchivos;
}

t_list* obtener_bloques_usados(){
	t_list* lista_bloques = list_create();

	list_add_all(lista_bloques,obtener_bloques_recursos());
	list_add_all(lista_bloques,obtener_bloques_bitacora());

	return lista_bloques;
}

t_list* obtener_bloques_recursos(){
	t_list* lista_bloques = list_create();

	void traer_bloques_recursos(char* archivo){
		char* path_recurso = obtener_path_files(archivo);
		log_info("ABRIR Y OBTENER BLOQUES -> Se desea entrar en %s", path_recurso);
		t_config* recurso = config_create(path_recurso);
		sumar_bloques_config(lista_bloques,recurso);
		config_destroy(recurso);
	}
	if(archivo_recursos_existe("Oxigeno.ims")){
		traer_bloques_recursos("Oxigeno.ims");
	}
	if(archivo_recursos_existe("Comida.ims")){
		traer_bloques_recursos("Comida.ims");
	}
	if(archivo_recursos_existe("Basura.ims")){
		traer_bloques_recursos("Basura.ims");
	}

	return lista_bloques;
}

t_list* obtener_bloques_bitacora(){

	t_list* lista_bloques = list_create();
	t_list* listaArchivos = listaArchivosDeBitacora();

	void traer_bloques_bitacoras(char* archivo){
		char* path_bitacora = obtener_path_bitacora(archivo);
		log_info("ABRIR Y OBTENER BLOQUES -> Se desea entrar en %s", path_bitacora);
		t_config* bitacora = config_create(path_bitacora);
		sumar_bloques_config(lista_bloques,bitacora);
		config_destroy(bitacora);
	}

	list_iterate(listaArchivos,traer_bloques_bitacoras);

	return lista_bloques;
}

char* obtener_path_bitacora(char* nombreBitacora){
	char* pathArchivoBitacora = string_new();
	string_append_with_format(&pathArchivoBitacora, "%s/%s", path_bitacoras, nombreBitacora);
	return pathArchivoBitacora;
}
