#include "sabotaje.h"

void resolver_sabotaje_superbloque_blocks(){
	//TODO
//sobreescribir “cantidad de bloques” del superbloque con la cantidad de bloques real en el disco (tamaño del blocks.ims)
	struct stat st;
	stat(path_blocks, &st);
	uint32_t size_blocks_ims = st.st_size;


}

void resolver_sabotaje_superbloque_bitmap(){
//TODO
//Corregir el bitmap con lo que se halle en la metadata de los archivos.
	//Constatar contra el bitmap que esten todos los valores correctos.
//	Si el bloque está cruzado en un archivo, tiene que estar marcado como usuario, y si no , tiene que estar marcado como libre
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
