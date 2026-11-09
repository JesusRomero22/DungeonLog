#include <stdio.h>
#include "juego.h"
#include "entrada.h"
#include "interfaz.h"
#include "mazmorra.h"

/* Inicializa el estado recibido y sus estructuras vacias. Sin malloc.
   Llamar una vez antes de usarlo; liberar antes de volver a inicializar. */
void juego_inicializar(Juego *juego)
{
    juego->sala_actual = 0;
    juego->estado = PARTIDA_PREPARADA;
    mazmorra_inicializar(juego->salas);
    inventario_inicializar(&juego->inventario);
    historial_inicializar(&juego->historial);
    bestiario_inicializar(&juego->bestiario);
}

/* Ejecuta una operacion solicitada sobre la lista propia de juego.
   Devuelve 0 con EOF. Solo agregar puede reservar; quitar puede liberar. */
static int menu_inventario(Juego *juego)
{
    int opcion, id;
    const Objeto *objeto;
    const NodoInventario *encontrado;
    interfaz_inventario(&juego->inventario);
    interfaz_linea("[1] Agregar  [2] Quitar  [3] Buscar  [4] Ordenar  [0] Volver");
    interfaz_cerrar_panel();
    if (!entrada_leer_entero("  Accion: ", 0, 4, &opcion)) {
        return 0;
    }
    if (opcion == 0) {
        return 1;
    }
    if (opcion == 4) {
        inventario_ordenar_por_valor(&juego->inventario);
        interfaz_inventario(&juego->inventario);
        return 1;
    }
    puts("  Catalogo: 1 Pocion(10), 2 Moneda(5), 3 Caliz(40),");
    puts("            4 Amuleto(25), 5 Reliquia(100).");
    if (!entrada_leer_entero("  ID de objeto: ", 1, TOTAL_OBJETOS, &id)) {
        return 0;
    }
    objeto = mazmorra_objeto(id);
    if (opcion == 1) {
        puts(inventario_agregar(&juego->inventario, *objeto)
             ? "  Unidad agregada." : "  No se pudo agregar; lista conservada.");
    } else if (opcion == 2) {
        puts(inventario_quitar(&juego->inventario, id)
             ? "  Unidad retirada." : "  Objeto no encontrado.");
    } else {
        encontrado = inventario_buscar(&juego->inventario, id);
        if (encontrado != NULL) {
            printf("  Encontrado: %s x%d\n", encontrado->objeto.nombre, encontrado->cantidad);
        } else {
            puts("  Objeto no encontrado.");
        }
    }
    return 1;
}

/* Ejecuta una operacion LIFO de prueba, sin mover al jugador.
   Apilar reserva un nodo; desapilar lo libera. 0 significa EOF. */
static int menu_historial(Juego *juego)
{
    int opcion, id;
    interfaz_historial(&juego->historial, juego->salas);
    interfaz_linea("[1] Apilar  [2] Desapilar  [3] Buscar  [4] Ver tope  [0] Volver");
    interfaz_cerrar_panel();
    if (!entrada_leer_entero("  Accion: ", 0, 4, &opcion)) {
        return 0;
    }
    if (opcion == 1 || opcion == 3) {
        if (!entrada_leer_entero("  ID de sala (0..8): ", 0, TOTAL_SALAS - 1, &id)) {
            return 0;
        }
        if (opcion == 1) {
            puts(historial_apilar(&juego->historial, id)
                 ? "  Sala apilada." : "  Sin memoria; pila conservada.");
        } else {
            puts(historial_buscar(&juego->historial, id)
                 ? "  Sala encontrada en el camino." : "  Sala ausente del camino.");
        }
    } else if (opcion == 2 || opcion == 4) {
        int exito = opcion == 2 ? historial_desapilar(&juego->historial, &id)
                               : historial_consultar_tope(&juego->historial, &id);
        if (exito) {
            printf("  %s: [%d] %s\n", opcion == 2 ? "Extraida" : "Tope",
                   id, juego->salas[id].nombre);
        } else {
            puts("  Pila vacia.");
        }
    }
    return 1;
}

/* Procesa altas, bajas o consultas del catalogo. El ABB gestiona sus nodos.
   Recibe una opcion 1..3 y devuelve 0 si la entrada termina. */
static int operar_bestiario(Bestiario *bestiario, int opcion)
{
    int id;
    const Enemigo *enemigo;
    puts("  IDs: 10 Rata, 20 Monje, 30 Escriba, 40 Custodio,");
    puts("       50 Caballero, 60 Espectro, 70 Sombra.");
    if (!entrada_leer_entero("  ID de enemigo: ", 1, 999, &id)) {
        return 0;
    }
    if (opcion == 1) {
        ResultadoRegistro resultado;
        enemigo = mazmorra_enemigo(id);
        if (enemigo == NULL) {
            puts("  Ese ID no pertenece al catalogo.");
            return 1;
        }
        resultado = bestiario_insertar(bestiario, *enemigo);
        puts(resultado == REGISTRO_INSERTADO ? "  Enemigo registrado."
             : resultado == REGISTRO_REPETIDO ? "  El ID ya existe."
             : "  Sin memoria; arbol conservado.");
    } else if (opcion == 2) {
        puts(bestiario_eliminar(bestiario, id) ? "  Registro eliminado."
                                             : "  ID no encontrado.");
    } else {
        enemigo = bestiario_buscar(bestiario, id);
        if (enemigo != NULL) {
            printf("  %s | peligro %d\n  %s\n", enemigo->nombre,
                   enemigo->peligro, enemigo->descripcion);
        } else {
            puts("  ID no encontrado.");
        }
    }
    return 1;
}

/* Menu de un paso: permite inspeccionar los tres recorridos sin mutar nodos. */
static int menu_bestiario(Juego *juego)
{
    int opcion;
    interfaz_arbol(&juego->bestiario);
    interfaz_linea("[1] Insertar  [2] Eliminar  [3] Buscar");
    interfaz_linea("[4] Inorden   [5] Preorden  [6] Postorden  [0] Volver");
    interfaz_cerrar_panel();
    if (!entrada_leer_entero("  Accion: ", 0, 6, &opcion)) {
        return 0;
    }
    if (opcion >= 1 && opcion <= 3) {
        int continuar = operar_bestiario(&juego->bestiario, opcion);
        if (continuar && opcion <= 2) {
            interfaz_arbol(&juego->bestiario);
        }
        return continuar;
    }
    if (opcion >= 4) {
        interfaz_bestiario(&juego->bestiario, (OrdenRecorrido)(opcion - 4));
    }
    return 1;
}

/* Coordina la mesa de pruebas. EOF o salir terminan limpiamente.
   Los submenus ejecutan una accion y regresan al principal. */
void juego_menu_provisional(Juego *juego)
{
    int continuar = 1;
    while (continuar) {
        int opcion;
        interfaz_panel("MESA DEL EXPLORADOR - Estructuras de datos");
        interfaz_linea("[1] Inventario      [2] Historial      [3] Bestiario y ABB");
        interfaz_linea("[4] Atlas de salas  [0] Salir");
        interfaz_cerrar_panel();
        if (!entrada_leer_entero("  Opcion: ", 0, 4, &opcion)) {
            break;
        }
        switch (opcion) {
            case 1: continuar = menu_inventario(juego); break;
            case 2: continuar = menu_historial(juego); break;
            case 3: continuar = menu_bestiario(juego); break;
            case 4: interfaz_salas(juego->salas); break;
            case 0: continuar = 0; break;
        }
    }
    juego->estado = PARTIDA_TERMINADA;
}

/* Libera todos los nodos dinamicos propios; las salas no requieren free. */
void juego_liberar(Juego *juego)
{
    inventario_liberar(&juego->inventario);
    historial_liberar(&juego->historial);
    bestiario_liberar(&juego->bestiario);
}
