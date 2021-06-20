#ifndef MI_RAM_HQ_SRC_MEMORIA_H_
#define MI_RAM_HQ_SRC_MEMORIA_H_

#include "utils_ram.h"
#include "mapa.h"

#define TAMANIO_PCB 8
#define TAMANIO_TCB 21
#define TAMANIO_OFFSET 1000

pthread_t hiloReceiveMapa;

void iniciar_memoria(void);
void preparar_memoria(void);
void crear_tabla(uint32_t id_patota);
uint32_t obtener_direccion_pcb(uint32_t id_patota);
uint32_t obtener_direccion_tarea(uint32_t id_patota, uint32_t offset);
t_pcb* crear_pcb(t_patota* patota);
t_tcb* crear_tcb(t_tripulante* tripulante);
void preparar_memoria_para_esquema_de_segmentacion(void);
void preparar_memoria_para_esquema_de_paginacion(void);

uint32_t dar_direccion_logica(uint32_t nro_segmento, uint32_t offset);
uint32_t dar_segmento_direccion_logica(uint32_t dir_logica);
uint32_t dar_offset_direccion_logica(uint32_t dir_logica);
t_tarea* obtener_tarea_memoria(t_tripulante* tripulante);

void escribir_en_memoria(void* informacion, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void modificar_memoria(void* informacion, uint32_t id_patota, e_tipo_dato tipo_dato);
void mover_tripulante_memoria(t_mover_hacia* mover_hacia);
void* leer_memoria(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato);

t_segmento* buscar_segmento_id(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato);
void* leer_memoria_segmentacion(t_segmento* segmento);
void modificar_memoria_segmentacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void escribir_en_memoria_segmentacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato);
t_segmento* dar_nuevo_segmento(t_segmento* segmento, uint32_t size);
void subir_segmento_memoria(t_segmento* segmento, void* stream);
void subir_segmento_libre(t_segmento* segmento);
t_tabla_segmentos* dar_tabla_segmentos(uint32_t id_patota);
void subir_tabla_segmento(t_segmento* segmento, uint32_t id_patota, uint32_t id_tripulante, e_tipo_dato tipo_dato);
t_segmento* buscar_segmento_libre(uint32_t espacio_requerido);

t_segmento* sacar_de_tabla_segmentacion(uint32_t id, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void liberar_segmento(t_segmento* segmento);
void sacar_de_memoria(uint32_t id, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void expulas_tripulante(t_tripulante* tripulante);

void escribir_en_memoria_paginacion(t_pagina* pagina, bool esta_en_memoria, uint32_t idPedido, bool modificado);

void* convertir(char* algoritmo_nombre);
void seleccionar_victima_LRU(void);
void seleccionar_victima_CLOCK(void);
bool hay_marcos_libres(void);
t_marco* buscar_entrada(void* marco);
t_marco* asignar_entrada_marco_libre(void);
t_marco* obtenerMarco(void);
bool ambos_bits_apagados(t_marco* entrada);
bool bit_uso_apagado(t_marco* entrada);
bool bit_uso(t_marco* entrada);
int indice_elemento(t_list* lista, void* elemento);
int cantidad_de_marcos;
int cantidad_de_marcos_swap;

#endif /* MI_RAM_HQ_SRC_MEMORIA_H_ */
