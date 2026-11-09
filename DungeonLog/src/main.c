#include <stdio.h>
#include <string.h>
#include "juego.h"
#include "interfaz.h"
#include "entrada.h"

/* Cada modo posee un Juego propio y lo libera. No comparte nodos ni salas
   con otras sesiones; volver al selector permite iniciar una nueva. */
static void ejecutar_modo(int modo)
{
    Juego sesion;
    juego_inicializar(&sesion);
    if (modo == 1) {
        juego_menu_aventura(&sesion);
    } else {
        juego_menu_provisional(&sesion);
    }
    juego_liberar(&sesion);
    puts("\n  Memoria de las estructuras liberada. Sesion finalizada.");
}

/* modo=0 selector, 1 aventura, 2 mesa. Sin malloc. Rechaza modos incompatibles.
   Los parametros de salida deben apuntar a enteros validos. */
static int leer_argumentos(int argc, char *argv[], int *modo, int *color)
{
    int i;
    *modo = 0;
    *color = 0;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--color") == 0) {
            *color = 1;
        } else if (strcmp(argv[i], "--sin-color") == 0) {
            *color = 0;
        } else if (strcmp(argv[i], "--aventura") == 0 || strcmp(argv[i], "--mesa") == 0) {
            if (*modo != 0) {
                return 0;
            }
            *modo = strcmp(argv[i], "--aventura") == 0 ? 1 : 2;
        } else {
            return 0;
        }
    }
    return 1;
}

/* Selector o modo directo. Cada sesion libera sus nodos tambien ante EOF.
   Sin colores es el comportamiento predeterminado. */
int main(int argc, char *argv[])
{
    int modo, color;
    if (!leer_argumentos(argc, argv, &modo, &color)) {
        puts("Uso: DungeonLog.exe [--aventura | --mesa] [--color | --sin-color]");
        return 1;
    }
    interfaz_configurar_color(color);
    interfaz_portada();
    if (modo != 0) {
        ejecutar_modo(modo);
        return 0;
    }
    for (;;) {
        interfaz_panel("DUNGEONLOG - Elige tu camino");
        interfaz_linea("[1] NUEVA AVENTURA  Recupera el Corazon del Alba");
        interfaz_linea("[2] MESA DE PRUEBAS Examina las estructuras de datos");
        interfaz_linea("[0] SALIR          Deja descansar a los olvidados");
        interfaz_cerrar_panel();
        if (!entrada_leer_entero("  Modo: ", 0, 2, &modo) || modo == 0) {
            break;
        }
        ejecutar_modo(modo);
    }
    return 0;
}
