#include <stdio.h>
#include "juego.h"
#include "entrada.h"
#include "interfaz.h"
#include "mazmorra.h"

/* Traduce resultados a texto sin alterar datos ni reservar memoria. */
static void mostrar_resultado(ResultadoAccion resultado)
{
    switch (resultado) {
        case ACCION_OK: interfaz_mensaje(ESTILO_EXITO, "[OK] Accion completada."); break;
        case ACCION_BLOQUEADA:
            interfaz_mensaje(ESTILO_INFO, "[BLOQUEADO] No hay salida alli. La piedra cierra el paso."); break;
        case ACCION_SIN_MEMORIA:
            interfaz_mensaje(ESTILO_INFO, "[SIN MEMORIA] Accion cancelada; tu estado se conserva."); break;
        case ACCION_SIN_HISTORIAL:
            interfaz_mensaje(ESTILO_INFO, "No hay pasos que desandar."); break;
        case ACCION_SIN_OBJETO:
            interfaz_mensaje(ESTILO_INFO, "No hay un objeto disponible."); break;
        case ACCION_RELIQUIA_PROTEGIDA:
            interfaz_mensaje(ESTILO_ESPECIAL,
                "El Corazon del Alba debe regresar contigo. No puedes descartarlo."); break;
        case ACCION_PARTIDA_INACTIVA:
            interfaz_mensaje(ESTILO_INFO, "La aventura ya termino."); break;
    }
}

/* Guarda solo informacion visual ANTES del cambio. direccion=-1 indica regreso.
   Las reglas y reservas permanecen en aventura.c; un fallo no produce narracion
   de visita ni descubrimiento. No se agregan banderas al modelo de juego. */
static void mover_y_presentar(Juego *juego, int direccion)
{
    int destino = SIN_CONTENIDO;
    int primera = 0, descubierto = 0;
    ResultadoAccion resultado;
    if (direccion == -1) {
        historial_consultar_tope(&juego->historial, &destino);
    } else {
        destino = juego->salas[juego->sala_actual].salidas[direccion];
    }
    if (destino >= 0 && destino < TOTAL_SALAS) {
        primera = !juego->salas[destino].visitada;
        descubierto = bestiario_buscar(&juego->bestiario,
                       juego->salas[destino].id_enemigo) == NULL;
    }
    resultado = direccion == -1 ? juego_retroceder(juego)
                               : juego_avanzar(juego, (Direccion)direccion);
    mostrar_resultado(resultado);
    if (resultado == ACCION_OK) {
        interfaz_entrada_sala(&juego->salas[juego->sala_actual], primera);
        interfaz_encuentro(&juego->salas[juego->sala_actual], descubierto);
    }
}

/* Solicita direccion. Narra solo una entrada confirmada. EOF retorna 0.
   La logica administra las reservas de pila y bestiario. */
static int explorar(Juego *juego)
{
    int direccion;
    interfaz_panel("EXPLORAR - Elige una salida");
    interfaz_linea("[1] Norte  [2] Este  [3] Sur  [4] Oeste  [0] Cancelar");
    interfaz_cerrar_panel();
    if (!entrada_leer_entero("  Direccion: ", 0, 4, &direccion)) {
        return 0;
    }
    if (direccion == 0) {
        return 1;
    }
    mover_y_presentar(juego, direccion - 1);
    return 1;
}

/* No permite altas manuales. La logica protege la reliquia al descartar.
   Ordenar no reserva; descartar puede liberar un nodo. EOF retorna 0. */
static int consultar_inventario(Juego *juego)
{
    int opcion, id;
    interfaz_inventario(&juego->inventario);
    interfaz_linea("[1] Ordenar por valor  [2] Descartar unidad  [0] Volver");
    interfaz_cerrar_panel();
    if (!entrada_leer_entero("  Accion: ", 0, 2, &opcion)) {
        return 0;
    }
    if (opcion == 1) {
        inventario_ordenar_por_valor(&juego->inventario);
        interfaz_inventario(&juego->inventario);
    } else if (opcion == 2) {
        if (!entrada_leer_entero("  ID del objeto: ", 1, TOTAL_OBJETOS, &id)) {
            return 0;
        }
        mostrar_resultado(juego_descartar(juego, id));
    }
    return 1;
}

/* Ejecuta una opcion; las consultas solo leen. EOF o salir devuelven 0. */
static int ejecutar_opcion(Juego *juego, int opcion)
{
    ResultadoAccion resultado;
    int id_objeto;
    switch (opcion) {
        case 1: return explorar(juego);
        case 2:
            mover_y_presentar(juego, -1);
            break;
        case 3:
            id_objeto = juego->salas[juego->sala_actual].id_objeto;
            resultado = juego_recoger(juego);
            mostrar_resultado(resultado);
            if (resultado == ACCION_OK && id_objeto == ID_CORAZON_ALBA) {
                interfaz_reliquia();
            }
            break;
        case 4: return consultar_inventario(juego);
        case 5: interfaz_historial(&juego->historial, juego->salas); break;
        case 6: interfaz_bestiario(&juego->bestiario, RECORRIDO_INORDEN); break;
        case 7: interfaz_mapa(juego->salas, juego->sala_actual); break;
        case 0: return 0;
    }
    return 1;
}

/* Ejecuta una aventura propia. Conserva GANADA o marca TERMINADA al salir.
   main libera los nodos incluso si stdin termina en medio de una accion. */
void juego_menu_aventura(Juego *juego)
{
    int continuar = 1;
    interfaz_mensaje(ESTILO_INFO,
        "La entrada es tu unico refugio. Encuentra la reliquia en la sala 8.");
    interfaz_mensaje(ESTILO_INFO, "Salir termina esta aventura; no se guardan partidas.");
    interfaz_entrada_sala(&juego->salas[juego->sala_actual], 1);
    while (continuar && juego->estado == PARTIDA_PREPARADA) {
        int opcion;
        interfaz_sala_actual(&juego->salas[juego->sala_actual]);
        interfaz_menu_aventura();
        if (!entrada_leer_entero("  Opcion: ", 0, 7, &opcion)) {
            break;
        }
        continuar = ejecutar_opcion(juego, opcion);
    }
    if (juego->estado == PARTIDA_GANADA) {
        interfaz_victoria();
    } else {
        juego->estado = PARTIDA_TERMINADA;
    }
}
