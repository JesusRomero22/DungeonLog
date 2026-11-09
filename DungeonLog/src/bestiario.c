#include <stdlib.h>
#include "bestiario.h"

/* Inicializa el arbol recibido sin reservar memoria. */
void bestiario_inicializar(Bestiario *bestiario)
{
    bestiario->raiz = NULL;
}

/* Inserta una copia por ID. enlace apunta al lugar donde falta un hijo.
   El arbol posee cada nodo reservado; duplicados y fallo no cambian nada. */
ResultadoRegistro bestiario_insertar(Bestiario *bestiario, Enemigo enemigo)
{
    NodoBestiario **enlace = &bestiario->raiz;
    NodoBestiario *nuevo;
    while (*enlace != NULL) {
        if (enemigo.id == (*enlace)->enemigo.id) {
            return REGISTRO_REPETIDO;
        }
        enlace = enemigo.id < (*enlace)->enemigo.id
            ? &(*enlace)->izquierdo : &(*enlace)->derecho;
    }
    nuevo = malloc(sizeof *nuevo);
    if (nuevo == NULL) {
        return REGISTRO_SIN_MEMORIA;
    }
    nuevo->enemigo = enemigo;
    nuevo->izquierdo = NULL;
    nuevo->derecho = NULL;
    *enlace = nuevo;
    return REGISTRO_INSERTADO;
}

/* Busca id por comparaciones. Retorna datos prestados o NULL, sin malloc.
   No conservar el puntero despues de eliminar registros o liberar el arbol. */
const Enemigo *bestiario_buscar(const Bestiario *bestiario, int id)
{
    const NodoBestiario *actual = bestiario->raiz;
    while (actual != NULL) {
        if (id == actual->enemigo.id) {
            return &actual->enemigo;
        }
        actual = id < actual->enemigo.id ? actual->izquierdo : actual->derecho;
    }
    return NULL;
}

/* Elimina id y libera un nodo; 0 indica que no existe. Sin malloc.
   Dos hijos: copia el sucesor inorden y desenlaza ese sucesor.
   Cero o un hijo: reemplaza el enlace por el unico hijo, o NULL. */
int bestiario_eliminar(Bestiario *bestiario, int id)
{
    NodoBestiario **enlace = &bestiario->raiz;
    NodoBestiario *eliminado;
    while (*enlace != NULL && (*enlace)->enemigo.id != id) {
        enlace = id < (*enlace)->enemigo.id
            ? &(*enlace)->izquierdo : &(*enlace)->derecho;
    }
    if (*enlace == NULL) {
        return 0;
    }
    eliminado = *enlace;
    if (eliminado->izquierdo != NULL && eliminado->derecho != NULL) {
        NodoBestiario **sucesor = &eliminado->derecho;
        while ((*sucesor)->izquierdo != NULL) {
            sucesor = &(*sucesor)->izquierdo;
        }
        eliminado->enemigo = (*sucesor)->enemigo;
        eliminado = *sucesor;
        *sucesor = eliminado->derecho;
    } else {
        *enlace = eliminado->izquierdo != NULL
            ? eliminado->izquierdo : eliminado->derecho;
    }
    free(eliminado);
    return 1;
}

/* Copia registros en el arreglo del llamador segun el orden recibido.
   usados limita las escrituras a capacidad. No modifica nodos ni usa malloc. */
static void recorrer_nodo(const NodoBestiario *nodo, OrdenRecorrido orden,
                         Enemigo salida[], int capacidad, int *usados)
{
    if (nodo == NULL || *usados >= capacidad) {
        return;
    }
    if (orden == RECORRIDO_PREORDEN) {
        salida[(*usados)++] = nodo->enemigo;
    }
    recorrer_nodo(nodo->izquierdo, orden, salida, capacidad, usados);
    if (orden == RECORRIDO_INORDEN && *usados < capacidad) {
        salida[(*usados)++] = nodo->enemigo;
    }
    recorrer_nodo(nodo->derecho, orden, salida, capacidad, usados);
    if (orden == RECORRIDO_POSTORDEN && *usados < capacidad) {
        salida[(*usados)++] = nodo->enemigo;
    }
}

/* Recorre sin imprimir. salida debe tener capacidad posiciones disponibles.
   Devuelve el numero copiado (a lo sumo capacidad); no reserva memoria. */
int bestiario_recorrer(const Bestiario *bestiario, OrdenRecorrido orden,
                      Enemigo salida[], int capacidad)
{
    int usados = 0;
    if (capacidad <= 0 || orden < RECORRIDO_INORDEN ||
        orden > RECORRIDO_POSTORDEN) {
        return 0;
    }
    recorrer_nodo(bestiario->raiz, orden, salida, capacidad, &usados);
    return usados;
}

/* Postorden: libera hijos antes del padre. Usa la pila de llamadas. */
static void liberar_nodo(NodoBestiario *nodo)
{
    if (nodo != NULL) {
        liberar_nodo(nodo->izquierdo);
        liberar_nodo(nodo->derecho);
        free(nodo);
    }
}

/* Libera todos los nodos propios y deja el bestiario recibido vacio. */
void bestiario_liberar(Bestiario *bestiario)
{
    liberar_nodo(bestiario->raiz);
    bestiario->raiz = NULL;
}
