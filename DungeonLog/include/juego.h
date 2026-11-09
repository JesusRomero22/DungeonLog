#ifndef JUEGO_H
#define JUEGO_H

#include "inventario.h"
#include "historial.h"
#include "bestiario.h"

typedef enum { PARTIDA_PREPARADA, PARTIDA_GANADA, PARTIDA_TERMINADA } EstadoPartida;
typedef enum {
    ACCION_OK, ACCION_BLOQUEADA, ACCION_SIN_MEMORIA, ACCION_SIN_HISTORIAL,
    ACCION_SIN_OBJETO, ACCION_RELIQUIA_PROTEGIDA, ACCION_PARTIDA_INACTIVA
} ResultadoAccion;
typedef struct {
    int sala_actual;
    EstadoPartida estado;
    Sala salas[TOTAL_SALAS];
    Inventario inventario;
    Historial historial;
    Bestiario bestiario;
} Juego;

void juego_inicializar(Juego *juego);
void juego_menu_provisional(Juego *juego);
void juego_liberar(Juego *juego);
ResultadoAccion juego_avanzar(Juego *juego, Direccion direccion);
ResultadoAccion juego_retroceder(Juego *juego);
ResultadoAccion juego_recoger(Juego *juego);
ResultadoAccion juego_descartar(Juego *juego, int id_objeto);
void juego_comprobar_victoria(Juego *juego);
void juego_menu_aventura(Juego *juego);

#endif
