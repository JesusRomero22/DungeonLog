#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "inventario.h"
#include "historial.h"
#include "bestiario.h"
#include "mazmorra.h"

/* Lista: vacio, duplicados, orden con cantidades y bajas en varias posiciones. */
static void probar_inventario(void)
{
    Inventario lista;
    const NodoInventario *actual;
    const int esperado[] = {2, 1, 4, 3, 5};
    int i;
    inventario_inicializar(&lista);
    assert(inventario_buscar(&lista, 1) == NULL);
    assert(!inventario_quitar(&lista, 1));
    inventario_ordenar_por_valor(&lista);
    for (i = 1; i <= TOTAL_OBJETOS; i++) {
        assert(inventario_agregar(&lista, *mazmorra_objeto(i)));
    }
    assert(inventario_agregar(&lista, *mazmorra_objeto(3)));
    assert(inventario_buscar(&lista, 3)->cantidad == 2);
    inventario_ordenar_por_valor(&lista);
    actual = lista.cabeza;
    for (i = 0; i < TOTAL_OBJETOS; i++) {
        assert(actual != NULL && actual->objeto.id == esperado[i]);
        assert(actual->cantidad == (esperado[i] == 3 ? 2 : 1));
        actual = actual->siguiente;
    }
    assert(actual == NULL);
    assert(inventario_quitar(&lista, 2)); /* Cabeza. */
    assert(lista.cabeza->objeto.id == 1);
    assert(inventario_quitar(&lista, 4)); /* Intermedio. */
    assert(inventario_buscar(&lista, 4) == NULL);
    assert(inventario_quitar(&lista, 5)); /* Ultimo. */
    assert(inventario_buscar(&lista, 5) == NULL);
    assert(inventario_quitar(&lista, 3));
    assert(inventario_buscar(&lista, 3)->cantidad == 1);
    assert(inventario_quitar(&lista, 3));
    assert(inventario_buscar(&lista, 3) == NULL);
    assert(inventario_quitar(&lista, 1)); /* Nodo unico. */
    assert(lista.cabeza == NULL);
    assert(inventario_agregar(&lista, *mazmorra_objeto(1)));
    inventario_liberar(&lista);
    inventario_liberar(&lista);
    assert(lista.cabeza == NULL);
    puts("OK inventario: duplicados, selection sort, cantidades y eliminaciones");
}

/* Pila: LIFO, busqueda y consulta sin mutacion, salida intacta al estar vacia. */
static void probar_historial(void)
{
    Historial pila;
    int id = 99;
    historial_inicializar(&pila);
    assert(!historial_desapilar(&pila, &id) && id == 99);
    assert(!historial_consultar_tope(&pila, &id) && id == 99);
    assert(!historial_buscar(&pila, 0));
    assert(historial_apilar(&pila, 0));
    assert(historial_apilar(&pila, 1));
    assert(historial_apilar(&pila, 4));
    assert(historial_buscar(&pila, 0));
    assert(!historial_buscar(&pila, 8));
    assert(historial_consultar_tope(&pila, &id) && id == 4);
    assert(historial_desapilar(&pila, &id) && id == 4);
    assert(historial_desapilar(&pila, &id) && id == 1);
    assert(historial_desapilar(&pila, &id) && id == 0);
    assert(!historial_desapilar(&pila, &id));
    assert(historial_apilar(&pila, 8));
    historial_liberar(&pila);
    historial_liberar(&pila);
    assert(pila.tope == NULL);
    puts("OK historial: LIFO, consulta, busqueda y vaciado");
}

/* Crea un arbol nuevo por escenario. Datos copiados, nodos propiedad del ABB. */
static void cargar_arbol(Bestiario *arbol, const int ids[], int cantidad)
{
    int i;
    bestiario_inicializar(arbol);
    for (i = 0; i < cantidad; i++) {
        const Enemigo *catalogo = mazmorra_enemigo(ids[i]);
        Enemigo enemigo = catalogo != NULL ? *catalogo : *mazmorra_enemigo(40);
        enemigo.id = ids[i];
        assert(bestiario_insertar(arbol, enemigo) == REGISTRO_INSERTADO);
    }
}

/* Compara recorrido completo y prueba que un buffer menor no se desborda. */
static void verificar_orden(const Bestiario *arbol, OrdenRecorrido orden,
                           const int esperado[], int cantidad)
{
    Enemigo salida[16];
    int i;
    assert(bestiario_recorrer(arbol, orden, salida, 16) == cantidad);
    for (i = 0; i < cantidad; i++) {
        assert(salida[i].id == esperado[i]);
    }
    salida[1].id = -123;
    assert(bestiario_recorrer(arbol, orden, salida, 1) == (cantidad > 0 ? 1 : 0));
    assert(salida[1].id == -123);
}

/* Elimina sobre arbol NUEVO cada vez: no depende del resultado de otra prueba. */
static void probar_eliminacion(const int inicial[], int cantidad, int eliminado,
                              const int esperado[], int restantes, int raiz)
{
    Bestiario arbol;
    int i;
    cargar_arbol(&arbol, inicial, cantidad);
    assert(bestiario_eliminar(&arbol, eliminado));
    assert(bestiario_buscar(&arbol, eliminado) == NULL);
    verificar_orden(&arbol, RECORRIDO_INORDEN, esperado, restantes);
    assert(restantes == 0 ? arbol.raiz == NULL : arbol.raiz->enemigo.id == raiz);
    for (i = 0; i < restantes; i++) {
        const Enemigo *original = mazmorra_enemigo(esperado[i]);
        const Enemigo *conservado = bestiario_buscar(&arbol, esperado[i]);
        assert(conservado != NULL);
        if (original != NULL) {
            assert(strcmp(conservado->nombre, original->nombre) == 0);
            assert(strcmp(conservado->descripcion, original->descripcion) == 0);
            assert(conservado->peligro == original->peligro);
        }
    }
    bestiario_liberar(&arbol);
    assert(arbol.raiz == NULL);
}

static void probar_bestiario(void)
{
    Bestiario arbol;
    const int inicial[] = {40, 20, 60, 10, 30, 50, 70};
    const int inorden[] = {10, 20, 30, 40, 50, 60, 70};
    const int preorden[] = {40, 20, 10, 30, 60, 50, 70};
    const int postorden[] = {10, 30, 20, 50, 70, 60, 40};
    const int sin_hoja[] = {20, 30, 40, 50, 60, 70};
    const int un_hijo[] = {40, 20, 10};
    const int sin_un_hijo[] = {10, 40};
    const int sin_raiz[] = {10, 20, 30, 50, 60, 70};
    const int sucesor_con_hijo[] = {40, 20, 60, 50, 55, 70};
    const int sin_raiz_2[] = {20, 50, 55, 60, 70};
    const int unico[] = {40};
    bestiario_inicializar(&arbol);
    assert(!bestiario_eliminar(&arbol, 40));
    assert(bestiario_buscar(&arbol, 40) == NULL);
    verificar_orden(&arbol, RECORRIDO_INORDEN, inorden, 0);
    cargar_arbol(&arbol, inicial, 7);
    assert(bestiario_insertar(&arbol, *mazmorra_enemigo(40)) == REGISTRO_REPETIDO);
    assert(strcmp(bestiario_buscar(&arbol, 40)->nombre, "Custodio hueco") == 0);
    assert(bestiario_buscar(&arbol, 999) == NULL);
    assert(!bestiario_eliminar(&arbol, 999));
    verificar_orden(&arbol, RECORRIDO_INORDEN, inorden, 7);
    verificar_orden(&arbol, RECORRIDO_PREORDEN, preorden, 7);
    verificar_orden(&arbol, RECORRIDO_POSTORDEN, postorden, 7);
    bestiario_liberar(&arbol);
    bestiario_liberar(&arbol);
    probar_eliminacion(inicial, 7, 10, sin_hoja, 6, 40);
    probar_eliminacion(un_hijo, 3, 20, sin_un_hijo, 2, 40);
    probar_eliminacion(inicial, 7, 40, sin_raiz, 6, 50);
    probar_eliminacion(sucesor_con_hijo, 6, 40, sin_raiz_2, 5, 50);
    probar_eliminacion(unico, 1, 40, unico, 0, 0);
    probar_eliminacion(un_hijo, 2, 40, un_hijo + 1, 1, 20);
    puts("OK bestiario: recorridos, duplicados, hoja, un hijo, dos hijos y raiz");
}

/* Verifica conexiones reciprocas, IDs validos y camino previsto a la reliquia. */
static void probar_salas(void)
{
    Sala salas[TOTAL_SALAS];
    int i, d;
    mazmorra_inicializar(salas);
    for (i = 0; i < TOTAL_SALAS; i++) {
        assert(salas[i].id == i);
        for (d = 0; d < TOTAL_DIRECCIONES; d++) {
            int destino = salas[i].salidas[d];
            assert(destino == SIN_CONTENIDO || (destino >= 0 && destino < TOTAL_SALAS));
            if (destino != SIN_CONTENIDO) {
                assert(salas[destino].salidas[(d + 2) % 4] == i);
            }
        }
        assert(salas[i].id_objeto == -1 || mazmorra_objeto(salas[i].id_objeto));
        assert(salas[i].id_enemigo == -1 || mazmorra_enemigo(salas[i].id_enemigo));
    }
    assert(salas[0].salidas[ESTE] == 1 && salas[1].salidas[SUR] == 4);
    assert(salas[4].salidas[SUR] == 7 && salas[7].salidas[ESTE] == 8);
    assert(mazmorra_objeto(salas[8].id_objeto)->tipo == OBJETO_RELIQUIA);
    puts("OK salas: conexiones reciprocas, catalogos y ruta a la reliquia");
}

int main(void)
{
    probar_inventario();
    probar_historial();
    probar_bestiario();
    probar_salas();
    puts("Todas las pruebas finalizaron correctamente.");
    return 0;
}
