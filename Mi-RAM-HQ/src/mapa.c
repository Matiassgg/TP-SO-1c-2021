#include "mapa.h"

void ASSERT_CREATE(NIVEL* nivel,char id, int err) {
    if(err) {
        nivel_destruir(nivel);
        nivel_gui_terminar();
        fprintf(stderr, "Error al crear '%c': %s\n", id, nivel_gui_string_error(err));
    }
}

int rnd() {
	return (rand() % 3) - 1;
}

void iniciar_mapa_vacio() {
	int cols, rows;
	//int err;

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&cols, &rows);

	nivel = nivel_crear("La nave de amongo");

	while (1) {

		// Todo
		// Queda loopeando de forma media raris el mapa
		nivel_gui_dibujar(nivel);

	}

}

int crear_tripulante(NIVEL* nivel, t_tripulante* tripulante){
	return personaje_crear(nivel, tripulante->id + '0', tripulante->posicion.pos_x, tripulante->posicion.pos_y);
}

