#ifndef INVENTARIO_H
#define INVENTARIO_H

#include "modelos.h"

typedef struct NodoInventario {
    Objeto objeto;
    int cantidad;
    struct NodoInventario *siguiente;
} NodoInventario;

typedef struct { NodoInventario *cabeza; } Inventario;

void inventario_inicializar(Inventario *inventario);
const NodoInventario *inventario_buscar(const Inventario *inventario, int id);
int inventario_agregar(Inventario *inventario, Objeto objeto);
int inventario_quitar(Inventario *inventario, int id);
void inventario_ordenar_por_valor(Inventario *inventario);
void inventario_liberar(Inventario *inventario);

#endif
