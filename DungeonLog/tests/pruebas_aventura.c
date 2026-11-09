#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "juego.h"
#include "mazmorra.h"

/* Solo los objetos de prueba de las tres estructuras sustituyen malloc/free
   con -Dmalloc=prueba_malloc -Dfree=prueba_free. El juego normal no se modifica.
   -1 no falla; 0 falla la siguiente reserva; 1 permite una y falla la segunda. */
static int hasta_fallo = -1;
static int nodos_vivos = 0;

void *prueba_malloc(size_t cantidad)
{
    void *memoria;
    if (hasta_fallo == 0) {
        return NULL;
    }
    if (hasta_fallo > 0) {
        hasta_fallo--;
    }
    memoria = malloc(cantidad);
    if (memoria != NULL) {
        nodos_vivos++;
    }
    return memoria;
}

void prueba_free(void *memoria)
{
    if (memoria != NULL) {
        nodos_vivos--;
        assert(nodos_vivos >= 0);
    }
    free(memoria);
}

/* Compara identidad de las estructuras y todo dato mutable de las salas.
   anterior es copia superficial de referencia, nunca se libera por separado. */
static void verificar_sin_cambios(const Juego *juego, const Juego *anterior)
{
    int i;
    assert(juego->sala_actual == anterior->sala_actual);
    assert(juego->estado == anterior->estado);
    assert(juego->inventario.cabeza == anterior->inventario.cabeza);
    assert(juego->historial.tope == anterior->historial.tope);
    assert(juego->bestiario.raiz == anterior->bestiario.raiz);
    for (i = 0; i < TOTAL_SALAS; i++) {
        assert(juego->salas[i].visitada == anterior->salas[i].visitada);
        assert(juego->salas[i].id_objeto == anterior->salas[i].id_objeto);
    }
}

static void probar_movimientos(void)
{
    Juego juego, anterior;
    Enemigo descubiertos[TOTAL_ENEMIGOS];
    int id;
    juego_inicializar(&juego);
    assert(juego.sala_actual == 0 && juego.salas[0].visitada);
    anterior = juego;
    assert(juego_avanzar(&juego, NORTE) == ACCION_BLOQUEADA);
    assert(juego_avanzar(&juego, OESTE) == ACCION_BLOQUEADA);
    assert(juego_avanzar(&juego, (Direccion)99) == ACCION_BLOQUEADA);
    assert(juego_retroceder(&juego) == ACCION_SIN_HISTORIAL);
    verificar_sin_cambios(&juego, &anterior);
    assert(juego_avanzar(&juego, ESTE) == ACCION_OK);
    assert(juego.sala_actual == 1 && juego.salas[1].visitada);
    assert(historial_consultar_tope(&juego.historial, &id) && id == 0);
    assert(bestiario_buscar(&juego.bestiario, 10) != NULL);
    assert(juego_avanzar(&juego, SUR) == ACCION_OK);
    assert(juego.sala_actual == 4 && juego.historial.tope->id_sala == 1);
    assert(bestiario_buscar(&juego.bestiario, 40) != NULL);
    assert(juego_retroceder(&juego) == ACCION_OK);
    assert(juego.sala_actual == 1 && juego.historial.tope->id_sala == 0);
    assert(juego.salas[4].visitada); /* Desapilar no borra el descubrimiento. */
    assert(juego_retroceder(&juego) == ACCION_OK);
    assert(juego.sala_actual == 0 && juego.historial.tope == NULL);
    assert(juego.estado == PARTIDA_PREPARADA); /* Volver sin reliquia no gana. */
    assert(juego_avanzar(&juego, SUR) == ACCION_OK);
    assert(juego.sala_actual == 3);
    assert(juego_avanzar(&juego, NORTE) == ACCION_OK);
    assert(juego.sala_actual == 0); /* Norte valido. */
    assert(juego_avanzar(&juego, ESTE) == ACCION_OK);
    assert(juego_avanzar(&juego, OESTE) == ACCION_OK);
    assert(juego.sala_actual == 0); /* Oeste valido. */
    assert(bestiario_recorrer(&juego.bestiario, RECORRIDO_INORDEN,
                             descubiertos, TOTAL_ENEMIGOS) == 3);
    assert(descubiertos[0].id == 10 && descubiertos[1].id == 30 &&
           descubiertos[2].id == 40); /* Revisitar no duplica enemigos. */
    juego_liberar(&juego);
    assert(nodos_vivos == 0);
    puts("OK movimientos: cuatro direcciones, paredes, visitas, encuentros y retroceso");
}

static void probar_recoleccion(void)
{
    Juego juego;
    juego_inicializar(&juego);
    assert(juego_recoger(&juego) == ACCION_SIN_OBJETO);
    assert(juego_avanzar(&juego, ESTE) == ACCION_OK);
    assert(juego_recoger(&juego) == ACCION_OK);
    assert(juego.salas[1].id_objeto == SIN_CONTENIDO);
    assert(inventario_buscar(&juego.inventario, 2)->cantidad == 1);
    assert(juego_recoger(&juego) == ACCION_SIN_OBJETO);
    assert(juego_retroceder(&juego) == ACCION_OK);
    assert(juego_avanzar(&juego, ESTE) == ACCION_OK);
    assert(juego_recoger(&juego) == ACCION_SIN_OBJETO);
    assert(inventario_buscar(&juego.inventario, 2)->cantidad == 1);
    assert(juego_descartar(&juego, 2) == ACCION_OK);
    assert(juego_descartar(&juego, 2) == ACCION_SIN_OBJETO);
    assert(juego_recoger(&juego) == ACCION_SIN_OBJETO); /* No reaparece. */
    juego_liberar(&juego);
    assert(nodos_vivos == 0);
    puts("OK recoleccion: desaparicion, visita repetida y descarte");
}

/* Ruta prevista sin asignar directamente posiciones ni fabricar la reliquia. */
static void llegar_al_relicario(Juego *juego)
{
    assert(juego_avanzar(juego, ESTE) == ACCION_OK);
    assert(juego_avanzar(juego, SUR) == ACCION_OK);
    assert(juego_avanzar(juego, SUR) == ACCION_OK);
    assert(juego_avanzar(juego, ESTE) == ACCION_OK);
    assert(juego->sala_actual == 8 && juego->estado == PARTIDA_PREPARADA);
}

static void probar_victoria(void)
{
    Juego juego;
    int i;
    juego_inicializar(&juego);
    llegar_al_relicario(&juego);
    assert(juego_recoger(&juego) == ACCION_OK);
    assert(juego.estado == PARTIDA_PREPARADA); /* Recoger aun no gana. */
    assert(juego_descartar(&juego, ID_CORAZON_ALBA) == ACCION_RELIQUIA_PROTEGIDA);
    inventario_ordenar_por_valor(&juego.inventario);
    assert(inventario_buscar(&juego.inventario, ID_CORAZON_ALBA)->cantidad == 1);
    for (i = 0; i < 4; i++) {
        assert(juego_retroceder(&juego) == ACCION_OK);
    }
    assert(juego.sala_actual == 0 && juego.estado == PARTIDA_GANADA);
    assert(juego.historial.tope == NULL);
    assert(juego_avanzar(&juego, ESTE) == ACCION_PARTIDA_INACTIVA);
    assert(juego_recoger(&juego) == ACCION_PARTIDA_INACTIVA);
    assert(juego_retroceder(&juego) == ACCION_PARTIDA_INACTIVA);
    assert(juego_descartar(&juego, 5) == ACCION_PARTIDA_INACTIVA);
    juego_liberar(&juego);
    juego_inicializar(&juego);
    llegar_al_relicario(&juego);
    assert(juego_recoger(&juego) == ACCION_OK);
    assert(juego_avanzar(&juego, OESTE) == ACCION_OK);
    assert(juego_avanzar(&juego, NORTE) == ACCION_OK);
    assert(juego_avanzar(&juego, NORTE) == ACCION_OK);
    assert(juego_avanzar(&juego, OESTE) == ACCION_OK);
    assert(juego.estado == PARTIDA_GANADA); /* Tambien gana sin retroceder. */
    juego_liberar(&juego);
    assert(nodos_vivos == 0);
    puts("OK victoria: objetivo, reliquia protegida y regreso por ambos metodos");
}

static void probar_fallos_movimiento(void)
{
    Juego juego, anterior;
    int vivos;
    juego_inicializar(&juego);
    /* Mantener ruta/arbol previos para comprobar que rollback no los pierde. */
    assert(juego_avanzar(&juego, SUR) == ACCION_OK);
    anterior = juego;
    vivos = nodos_vivos;
    hasta_fallo = 0; /* Falla el nodo de pila al avanzar a 4. */
    assert(juego_avanzar(&juego, ESTE) == ACCION_SIN_MEMORIA);
    verificar_sin_cambios(&juego, &anterior);
    assert(nodos_vivos == vivos);
    hasta_fallo = 1; /* Pila exitosa, falla el registro de enemigo 40. */
    assert(juego_avanzar(&juego, ESTE) == ACCION_SIN_MEMORIA);
    verificar_sin_cambios(&juego, &anterior);
    assert(nodos_vivos == vivos);
    assert(bestiario_buscar(&juego.bestiario, 40) == NULL);
    assert(bestiario_buscar(&juego.bestiario, 30) != NULL);
    hasta_fallo = -1;
    assert(juego_avanzar(&juego, ESTE) == ACCION_OK);
    hasta_fallo = 0;
    /* Retroceder a enemigo ya registrado no necesita malloc. */
    assert(juego_retroceder(&juego) == ACCION_OK);
    hasta_fallo = -1;
    /* Caso defensivo: el registro de destino no existe al retroceder. */
    assert(juego_avanzar(&juego, ESTE) == ACCION_OK);
    assert(bestiario_eliminar(&juego.bestiario, 30));
    anterior = juego;
    hasta_fallo = 0;
    assert(juego_retroceder(&juego) == ACCION_SIN_MEMORIA);
    verificar_sin_cambios(&juego, &anterior);
    hasta_fallo = -1;
    assert(juego_retroceder(&juego) == ACCION_OK);
    juego_liberar(&juego);
    assert(nodos_vivos == 0);
    puts("OK fallos: pila, registro de encuentro, rollback y retroceso");
}

static void probar_fallo_recoleccion(void)
{
    Juego juego, anterior;
    juego_inicializar(&juego);
    llegar_al_relicario(&juego);
    anterior = juego;
    hasta_fallo = 0;
    assert(juego_recoger(&juego) == ACCION_SIN_MEMORIA);
    verificar_sin_cambios(&juego, &anterior);
    assert(inventario_buscar(&juego.inventario, 5) == NULL);
    assert(juego.salas[8].id_objeto == ID_CORAZON_ALBA);
    hasta_fallo = -1;
    assert(juego_recoger(&juego) == ACCION_OK);
    assert(juego_recoger(&juego) == ACCION_SIN_OBJETO);
    juego_liberar(&juego);
    assert(nodos_vivos == 0);
    puts("OK fallo de recoleccion: objeto conservado y reintento sin duplicados");
}

static void probar_aislamiento(void)
{
    Juego aventura, mesa, anterior;
    juego_inicializar(&aventura);
    assert(juego_avanzar(&aventura, ESTE) == ACCION_OK);
    assert(juego_recoger(&aventura) == ACCION_OK);
    anterior = aventura;
    juego_inicializar(&mesa);
    assert(inventario_agregar(&mesa.inventario, *mazmorra_objeto(5)));
    assert(historial_apilar(&mesa.historial, 8));
    assert(bestiario_insertar(&mesa.bestiario, *mazmorra_enemigo(70)) == REGISTRO_INSERTADO);
    mesa.salas[1].id_objeto = -1;
    juego_liberar(&mesa);
    verificar_sin_cambios(&aventura, &anterior);
    assert(inventario_buscar(&aventura.inventario, 5) == NULL);
    assert(inventario_buscar(&aventura.inventario, 2)->cantidad == 1);
    assert(bestiario_buscar(&aventura.bestiario, 70) == NULL);
    assert(juego_retroceder(&aventura) == ACCION_OK);
    assert(aventura.estado == PARTIDA_PREPARADA);
    juego_liberar(&aventura);
    assert(nodos_vivos == 0);
    puts("OK aislamiento: datos y memoria independientes entre aventura y mesa");
}

int main(void)
{
    probar_movimientos();
    probar_recoleccion();
    probar_victoria();
    probar_fallos_movimiento();
    probar_fallo_recoleccion();
    probar_aislamiento();
    puts("Todas las pruebas de aventura finalizaron correctamente.");
    return 0;
}
