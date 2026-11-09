#include <limits.h>
#include <stdlib.h>
#include "inventario.h"

/* Prepara una lista vacia. No reserva memoria; llamar antes del primer uso. */
void inventario_inicializar(Inventario *inventario)
{
    inventario->cabeza = NULL;
}

/* Busca id en la lista recibida. El puntero prestado no debe liberarse
   ni conservarse despues de modificar, ordenar o liberar el inventario. */
const NodoInventario *inventario_buscar(const Inventario *inventario, int id)
{
    const NodoInventario *actual = inventario->cabeza;
    while (actual != NULL && actual->objeto.id != id) {
        actual = actual->siguiente;
    }
    return actual;
}

/* Agrega una unidad del objeto por valor. La lista posee el nuevo nodo.
   Devuelve 0 si malloc falla o la cantidad desbordaria; no cambia la lista. */
int inventario_agregar(Inventario *inventario, Objeto objeto)
{
    NodoInventario *actual = inventario->cabeza;
    NodoInventario *nuevo;
    while (actual != NULL) {
        if (actual->objeto.id == objeto.id) {
            if (actual->cantidad == INT_MAX) {
                return 0;
            }
            actual->cantidad++;
            return 1;
        }
        actual = actual->siguiente;
    }
    nuevo = malloc(sizeof *nuevo);
    if (nuevo == NULL) {
        return 0;
    }
    nuevo->objeto = objeto;
    nuevo->cantidad = 1;
    nuevo->siguiente = inventario->cabeza;
    inventario->cabeza = nuevo;
    return 1;
}

/* Quita una unidad de id. Libera el nodo si su cantidad llega a cero.
   Devuelve 0 si no existe. La proteccion de objetos de mision sera del juego. */
int inventario_quitar(Inventario *inventario, int id)
{
    NodoInventario *actual = inventario->cabeza;
    NodoInventario *anterior = NULL;
    while (actual != NULL && actual->objeto.id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == NULL) {
        return 0;
    }
    if (--actual->cantidad > 0) {
        return 1;
    }
    if (anterior == NULL) {
        inventario->cabeza = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }
    free(actual);
    return 1;
}

/* Selection sort sobre la lista recibida: O(n^2), espacio adicional O(1).
   Intercambia objeto Y cantidad; no reserva memoria ni modifica enlaces. */
void inventario_ordenar_por_valor(Inventario *inventario)
{
    NodoInventario *posicion;
    for (posicion = inventario->cabeza; posicion != NULL;
         posicion = posicion->siguiente) {
        NodoInventario *menor = posicion;
        NodoInventario *actual;
        for (actual = posicion->siguiente; actual != NULL;
             actual = actual->siguiente) {
            if (actual->objeto.valor < menor->objeto.valor) {
                menor = actual;
            }
        }
        if (menor != posicion) {
            Objeto temporal = posicion->objeto;
            int cantidad = posicion->cantidad;
            posicion->objeto = menor->objeto;
            posicion->cantidad = menor->cantidad;
            menor->objeto = temporal;
            menor->cantidad = cantidad;
        }
    }
}

/* Libera todos los nodos propios y deja la lista recibida vacia. */
void inventario_liberar(Inventario *inventario)
{
    while (inventario->cabeza != NULL) {
        NodoInventario *siguiente = inventario->cabeza->siguiente;
        free(inventario->cabeza);
        inventario->cabeza = siguiente;
    }
}
