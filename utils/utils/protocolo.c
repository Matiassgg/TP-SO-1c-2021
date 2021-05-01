#include "protocolo.h"

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = crear_paquete(MSGTYPE_UNKNOWN);
    agregar_a_buffer(paquete->buffer, mensaje, strlen(mensaje) + 1);
    enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
	{
		return cod_op;
	}
	else
	{
		close(socket_cliente);
		return -1;
	}
}

t_buffer* recibir_buffer(int socket_cliente)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, &buffer->size, sizeof(int), MSG_WAITALL);
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}

char* recibir_mensaje(t_log* logger, int socket_cliente)
{
	t_buffer* buffer = recibir_buffer(socket_cliente);
	char* valor = string_duplicate(buffer->stream);
	eliminar_buffer(buffer);

	return valor;
}

t_list* recibir_paquete(int socket_cliente)
{
	t_list* valores = list_create();

	t_buffer* buffer = recibir_buffer(socket_cliente);
	while(buffer->size > 0)
	{
		int tamanio;
		quitar_de_buffer(buffer, &tamanio, sizeof(int));
		char* valor = calloc(tamanio, sizeof(char));
		quitar_de_buffer(buffer, valor, tamanio);
		list_add(valores, valor);
	}
	free(buffer);

	return valores;
}

