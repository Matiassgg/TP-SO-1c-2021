#ifndef SHARED_SHARED_PROTOCOLO_H_
#define SHARED_SHARED_PROTOCOLO_H_

#include "sockets.h"
#include "buffer.h"
#include "paquete.h"
#include <commons/collections/list.h>
#include <commons/string.h>

void enviar_mensaje(char* mensaje, int socket_cliente);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
int recibir_operacion(int);
t_buffer* recibir_buffer(int socket_cliente);
char* recibir_mensaje(t_log*, int);
t_list* recibir_paquete(int);

#endif /* SHARED_SHARED_PROTOCOLO_H_ */
