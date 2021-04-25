 #include "paquete.h"

t_paquete* crear_paquete(op_code codigo_operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = crear_buffer();
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, char* valor)
{
	int len = strlen(valor) + 1;
	agregar_a_buffer(paquete->buffer, &len, sizeof(int));
	agregar_a_buffer(paquete->buffer, valor, len);
}

t_buffer* serializar_paquete(t_paquete* paquete)
{
    t_buffer* buffer = crear_buffer();
    agregar_a_buffer(buffer, &(paquete->codigo_operacion), sizeof(int));
    agregar_a_buffer(buffer, &(paquete->buffer->size), sizeof(int));
    agregar_a_buffer(buffer, paquete->buffer->stream, paquete->buffer->size);

    return buffer;
}

void eliminar_paquete(t_paquete* paquete){
	eliminar_buffer(paquete->buffer);
	free(paquete);
}
