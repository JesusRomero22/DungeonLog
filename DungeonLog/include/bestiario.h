#ifndef BESTIARIO_H
#define BESTIARIO_H

#include "modelos.h"

typedef struct NodoBestiario {
    Enemigo enemigo;
    struct NodoBestiario *izquierdo;
    struct NodoBestiario *derecho;
} NodoBestiario;

typedef struct { NodoBestiario *raiz; } Bestiario;
typedef enum { REGISTRO_SIN_MEMORIA = -1, REGISTRO_REPETIDO, REGISTRO_INSERTADO }
    ResultadoRegistro;
typedef enum { RECORRIDO_INORDEN, RECORRIDO_PREORDEN, RECORRIDO_POSTORDEN }
    OrdenRecorrido;

void bestiario_inicializar(Bestiario *bestiario);
ResultadoRegistro bestiario_insertar(Bestiario *bestiario, Enemigo enemigo);
const Enemigo *bestiario_buscar(const Bestiario *bestiario, int id);
int bestiario_eliminar(Bestiario *bestiario, int id);
int bestiario_recorrer(const Bestiario *bestiario, OrdenRecorrido orden,
                      Enemigo salida[], int capacidad);
void bestiario_liberar(Bestiario *bestiario);

#endif
