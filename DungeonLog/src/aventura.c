#include <stddef.h>
#include "juego.h"
#include "mazmorra.h"

/* Registra el enemigo de destino por valor. Solo el ABB reserva memoria.
   Sin enemigo o con ID ya descubierto no se requiere una nueva reserva. */
static int registrar_encuentro(Juego *juego, int destino)
{
    const Enemigo *enemigo = mazmorra_enemigo(juego->salas[destino].id_enemigo);
    return enemigo == NULL ||
        bestiario_insertar(&juego->bestiario, *enemigo) != REGISTRO_SIN_MEMORIA;
}

/* Consulta inventario real y sala actual; no reserva ni libera memoria.
   Una partida terminada no puede convertirse en ganada mediante esta consulta. */
void juego_comprobar_victoria(Juego *juego)
{
    if (juego->estado == PARTIDA_PREPARADA && juego->sala_actual == 0 &&
        inventario_buscar(&juego->inventario, ID_CORAZON_ALBA) != NULL) {
        juego->estado = PARTIDA_GANADA;
    }
}

/* Confirma una entrada ya preparada: no quedan operaciones que puedan fallar.
   Recibe un destino valido, marca visita y comprueba el objetivo. Sin malloc. */
static void confirmar_entrada(Juego *juego, int destino)
{
    juego->sala_actual = destino;
    juego->salas[destino].visitada = 1;
    juego_comprobar_victoria(juego);
}

/* Avanza desde un Juego inicializado. Primero reserva pila y registro enemigo.
   Si falla el ABB, desapila el nodo recien creado: la ruta queda como antes.
   Sala actual y visitadas solo cambian al confirmar ambas operaciones. */
ResultadoAccion juego_avanzar(Juego *juego, Direccion direccion)
{
    int destino, descartado;
    if (juego->estado != PARTIDA_PREPARADA) {
        return ACCION_PARTIDA_INACTIVA;
    }
    if (direccion < NORTE || direccion >= TOTAL_DIRECCIONES) {
        return ACCION_BLOQUEADA;
    }
    destino = juego->salas[juego->sala_actual].salidas[direccion];
    if (destino < 0 || destino >= TOTAL_SALAS) {
        return ACCION_BLOQUEADA;
    }
    if (!historial_apilar(&juego->historial, juego->sala_actual)) {
        return ACCION_SIN_MEMORIA;
    }
    if (!registrar_encuentro(juego, destino)) {
        historial_desapilar(&juego->historial, &descartado);
        return ACCION_SIN_MEMORIA;
    }
    confirmar_entrada(juego, destino);
    return ACCION_OK;
}

/* Consulta el destino antes de extraer. Si registrar el encuentro falla,
   conserva el tope. Desapilar libera un nodo; nunca vuelve a apilar la sala. */
ResultadoAccion juego_retroceder(Juego *juego)
{
    int destino;
    if (juego->estado != PARTIDA_PREPARADA) {
        return ACCION_PARTIDA_INACTIVA;
    }
    if (!historial_consultar_tope(&juego->historial, &destino)) {
        return ACCION_SIN_HISTORIAL;
    }
    if (!registrar_encuentro(juego, destino)) {
        return ACCION_SIN_MEMORIA;
    }
    historial_desapilar(&juego->historial, &destino);
    confirmar_entrada(juego, destino);
    return ACCION_OK;
}

/* Copia el objeto de la sala a la lista. Solo retira el contenido de la sala
   si agregar tuvo exito, evitando perdidas o duplicados tras fallo de malloc. */
ResultadoAccion juego_recoger(Juego *juego)
{
    Sala *sala;
    const Objeto *objeto;
    if (juego->estado != PARTIDA_PREPARADA) {
        return ACCION_PARTIDA_INACTIVA;
    }
    sala = &juego->salas[juego->sala_actual];
    objeto = mazmorra_objeto(sala->id_objeto);
    if (objeto == NULL) {
        return ACCION_SIN_OBJETO;
    }
    if (!inventario_agregar(&juego->inventario, *objeto)) {
        return ACCION_SIN_MEMORIA;
    }
    sala->id_objeto = SIN_CONTENIDO;
    juego_comprobar_victoria(juego);
    return ACCION_OK;
}

/* Descarta una unidad por ID. La regla de mision pertenece al juego,
   no a la lista reutilizable. Quitar puede liberar un nodo, nunca reservar. */
ResultadoAccion juego_descartar(Juego *juego, int id_objeto)
{
    const NodoInventario *nodo;
    if (juego->estado != PARTIDA_PREPARADA) {
        return ACCION_PARTIDA_INACTIVA;
    }
    nodo = inventario_buscar(&juego->inventario, id_objeto);
    if (nodo == NULL) {
        return ACCION_SIN_OBJETO;
    }
    if (nodo->objeto.tipo == OBJETO_RELIQUIA) {
        return ACCION_RELIQUIA_PROTEGIDA;
    }
    inventario_quitar(&juego->inventario, id_objeto);
    return ACCION_OK;
}
