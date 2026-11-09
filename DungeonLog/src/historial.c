#include <stdlib.h>
#include "historial.h"

/* Inicializa la pila recibida sin reservar memoria. */
void historial_inicializar(Historial *historial)
{
    historial->tope = NULL;
}

/* Copia id_sala a un nodo propio. Si malloc falla devuelve 0 sin cambios. */
int historial_apilar(Historial *historial, int id_sala)
{
    NodoHistorial *nuevo = malloc(sizeof *nuevo);
    if (nuevo == NULL) {
        return 0;
    }
    nuevo->id_sala = id_sala;
    nuevo->siguiente = historial->tope;
    historial->tope = nuevo;
    return 1;
}

/* Copia el tope a la salida id_sala, que debe apuntar a un int valido.
   Devuelve 0 si esta vacia, dejando la salida intacta. No reserva memoria. */
int historial_consultar_tope(const Historial *historial, int *id_sala)
{
    if (historial->tope == NULL) {
        return 0;
    }
    *id_sala = historial->tope->id_sala;
    return 1;
}

/* Extrae el ultimo id a la salida y libera su nodo. Pila vacia: devuelve 0.
   id_sala debe apuntar a un int valido; se conserva si no hay nodo. */
int historial_desapilar(Historial *historial, int *id_sala)
{
    NodoHistorial *anterior;
    if (!historial_consultar_tope(historial, id_sala)) {
        return 0;
    }
    anterior = historial->tope;
    historial->tope = anterior->siguiente;
    free(anterior);
    return 1;
}

/* Busca id_sala de tope a fondo sin alterar la pila ni reservar memoria. */
int historial_buscar(const Historial *historial, int id_sala)
{
    const NodoHistorial *actual = historial->tope;
    while (actual != NULL) {
        if (actual->id_sala == id_sala) {
            return 1;
        }
        actual = actual->siguiente;
    }
    return 0;
}

/* Libera todos los nodos de la pila recibida y restaura el tope a NULL. */
void historial_liberar(Historial *historial)
{
    int descartado;
    while (historial_desapilar(historial, &descartado)) {
        /* Cada extraccion libera exactamente un nodo. */
    }
}
