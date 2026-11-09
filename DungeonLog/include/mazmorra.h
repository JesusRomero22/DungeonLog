#ifndef MAZMORRA_H
#define MAZMORRA_H

#include "modelos.h"

#define TOTAL_OBJETOS 5
#define ID_CORAZON_ALBA 5
#define TOTAL_ENEMIGOS 7

void mazmorra_inicializar(Sala salas[TOTAL_SALAS]);
const Objeto *mazmorra_objeto(int id);
const Enemigo *mazmorra_enemigo(int id);

#endif
