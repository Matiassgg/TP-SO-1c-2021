#include "sabotaje.h"

void resolver_sabotaje(){
	detectar_algun_sabotaje_en_superbloque();
	detectar_algun_sabotaje_en_files();
}

void detectar_algun_sabotaje_en_superbloque(){
	detectar_sabotaje_superbloque_blocks();
	detectar_sabotaje_superbloque_bitmap();
}

uint32_t tamanio_real_blocks_ims(){
	struct stat st;
	stat(path_blocks, &st);
	uint32_t size_blocks_ims = st.st_size;
	return size_blocks_ims;
}

void detectar_sabotaje_superbloque_blocks(){
	//Cambian el valor del campo blocks del superbloque. Ej, si teniamos 10 bloques de 80 bytes, y nos cambian a 20.
	//sobreescribir “cantidad de bloques” del superbloque con la cantidad de bloques real en el disco (tamaño del blocks.ims)


	FILE * superbloque = abrirSuperbloque("rb");
	uint32_t sizeUnBlock;
	uint32_t supuesto_blocks;
	fseek(superbloque,1,SEEK_SET);
	fread(&sizeUnBlock,sizeof(uint32_t),1,superbloque);
	fread(&supuesto_blocks,sizeof(uint32_t),1,superbloque);
	uint32_t blocks_reales = tamanio_real_blocks_ims() / sizeUnBlock;
	if(blocks_reales != supuesto_blocks ){
		resolver_sabotaje_superbloque_blocks(blocks_reales);
	}
	fclose(superbloque);
}

void resolver_sabotaje_superbloque_blocks(uint32_t blocks_reales){
	//TODO
	FILE* superbloque = abrirSuperbloque("rb+");
	fseek(superbloque,sizeof(uint32_t),SEEK_SET);
	fwrite(&blocks_reales,sizeof(uint32_t),1,superbloque);
	log_info(logger,"Resolviendo sabotaje -> Se actualiza valor de blocks de superbloque a %d",blocks_reales);
	fclose(superbloque);
}

void detectar_sabotaje_superbloque_bitmap(){
	//TODO
	//Corregir el bitmap con lo que se halle en la metadata de los archivos.
		//Constatar contra el bitmap que esten todos los valores correctos.
	//	Si el bloque está cruzado en un archivo, tiene que estar marcado como usado, y si no , tiene que estar marcado como libre


	t_list* bloques_usados = obtener_bloques_usados();
	t_bitarray* bitarray = leer_bitmap();

	bool inconsistencia_bitmap(int bloque){
		if(bitarray_test_bit(bitarray,bloque)==0){
			return 1;
		}
		return 0;
	}

	bool chequear_falsos_unos(){
		for(int i=0;i<bitarray_get_max_bit(bitarray);i++){

			bool no_esta_presente(uint32_t bloque){
				return i != bloque;
			}

			if(bitarray_test_bit(bitarray,i)==1 && list_all_satisfy(bloques_usados,no_esta_presente))
				return 1;
		}
		return 0;
	}

	if(list_any_satisfy(bloques_usados,inconsistencia_bitmap) || chequear_falsos_unos()){
		resolver_sabotaje_superbloque_bitmap(bitarray,bloques_usados);
	}
	else{
		log_info(logger, "FSCK -> No hubo sabotajes en el bitmap de superbloques.ims");
	}
}

void resolver_sabotaje_superbloque_bitmap(t_bitarray* bitarray, t_list* bloques_usados){


	//Limpiar bitmap
	for(int i=0;i<bitarray_get_max_bit(bitarray);i++){
		bitarray_clean_bit(bitarray,i);
	}

	void actualizar_bitmap(int bloque){
		bitarray_set_bit(bitarray,bloque);
	}

	list_iterate(bloques_usados,actualizar_bitmap);

	subir_bitmap(bitarray);
	log_info(logger,"Se resolvio sabotaje de bitmap (o al menos eso creo)");
}

void detectar_algun_sabotaje_en_files(){

	if(archivo_recursos_existe("Oxigeno.ims")){
		chequear_sabotajes_en("Oxigeno.ims");
	}
	if(archivo_recursos_existe("Comida.ims")){
		chequear_sabotajes_en("Comida.ims");
	}
	if(archivo_recursos_existe("Basura.ims")){
		chequear_sabotajes_en("Basura.ims");
	}

}

void chequear_sabotajes_en(char* path_relativo){
	char* path_archivo = obtener_path_files(path_relativo);
	detectar_sabotaje_files_size(path_archivo);
	detectar_sabotaje_files_blockcount(path_archivo);
	detectar_sabotaje_files_blocks(path_archivo);
}

void detectar_sabotaje_files_size(char* archivo){
	//TODO
//Asumir correcto lo encontrado en los bloques.
//Hay que recorrer todos los bloques del archivo y obtener y asumir como correcto el tamaño encontrado dentro de los bloques recorriendolos en orden.
//	Ej. OOOOOOOOOOOOOOOOOOOOOOOOO recorremos hasta encontrar el primer carácter que no es una O, llegué al size 400, ese será el size del archivo.
//	PONER UN CENTINELA PARA DECIR HASTA ACÁ LLEGUE

}

void resolver_sabotaje_files_size(char* archivo){
	t_config* archivo_recurso = config_create(archivo);

}

void detectar_sabotaje_files_blockcount(char* archivo){
	t_config* archivo_recurso = config_create(archivo);
	t_list* bloques = list_create();
	sumar_bloques_config(bloques,archivo_recurso);
	uint32_t cantidad_bloques = list_size(bloques);

	uint32_t block_count = config_get_int_value(archivo_recurso,"BLOCK_COUNT");

	if(block_count != cantidad_bloques){
		resolver_sabotaje_files_blockcount(archivo_recurso, cantidad_bloques);
	}

}

void resolver_sabotaje_files_blockcount(t_config* archivo_recurso, uint32_t cantidad_bloques){
	//Actualizar el valor de Block_count en base a lo que está en la lista de Blocks.
	config_set_value(archivo_recurso,"BLOCK_COUNT",cantidad_bloques);
	config_save(archivo_recurso);
	config_destroy(archivo_recurso);
}

void detectar_sabotaje_files_blocks(){

}

void resolver_sabotaje_files_blocks(char*archivo){
	//TODO
	//Lista de blocks alterado.
	//Entra en juego el md5_archivo. Concatenamos los bloques, calculamos el md5 hasta el size, y lo comparamos con el md5 que tenemos guardado.
	//Si no coinciden restaurar archivo, escribiendo tantos caracteres de llenado hasta completar el size, en el orden de bloques que tenemos
}


FILE* abrirSuperbloque(char* modo){
	FILE * superbloque = fopen(path_superbloque,modo);
	if(superbloque){
		return superbloque;
	}
	log_error(logger, "Error al leer superbloque");
	exit(1);
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
