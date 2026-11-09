#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "inventario.h"
#include "historial.h"
#include "bestiario.h"

typedef enum {
    ESTILO_INFO, ESTILO_TESORO, ESTILO_ENEMIGO, ESTILO_EXITO, ESTILO_ESPECIAL
} EstiloVisual;

void interfaz_configurar_color(int activado);
void interfaz_panel(const char *titulo);
void interfaz_portada(void);
void interfaz_inventario(const Inventario *inventario);
void interfaz_historial(const Historial *historial, const Sala salas[]);
void interfaz_bestiario(const Bestiario *bestiario, OrdenRecorrido orden);
void interfaz_salas(const Sala salas[]);
void interfaz_sala_actual(const Sala *sala);
void interfaz_encuentro(const Sala *sala, int descubierto);
void interfaz_mapa(const Sala salas[], int sala_actual);
void interfaz_victoria(void);
void interfaz_linea(const char *texto);
void interfaz_cerrar_panel(void);
void interfaz_mensaje(EstiloVisual estilo, const char *texto);
void interfaz_entrada_sala(const Sala *sala, int primera_visita);
void interfaz_reliquia(void);
void interfaz_arbol(const Bestiario *bestiario);
void interfaz_menu_aventura(void);

#endif
