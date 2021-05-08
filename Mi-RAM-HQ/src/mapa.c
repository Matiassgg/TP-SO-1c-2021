#include "mapa.h"

int crear_tripulante(NIVEL* nivel, t_tripulante* tripulante){
	return personaje_crear(nivel, 'x', tripulante->posicion.pos_x, tripulante->posicion.pos_y);
}

