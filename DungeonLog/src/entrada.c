#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entrada.h"

/* Lee hasta un entero valido en [minimo,maximo]. salida debe ser un int valido.
   Devuelve 0 al terminar la entrada; no reserva memoria ni deja texto pendiente. */
int entrada_leer_entero(const char *mensaje, int minimo, int maximo, int *salida)
{
    char linea[128];
    for (;;) {
        char *fin;
        long valor;
        fputs(mensaje, stdout);
        fflush(stdout);
        if (fgets(linea, sizeof linea, stdin) == NULL) {
            return 0;
        }
        if (strchr(linea, '\n') == NULL && !feof(stdin)) {
            int caracter;
            do {
                caracter = getchar();
            } while (caracter != '\n' && caracter != EOF);
            puts("Entrada demasiado larga. Intenta otra vez.");
            continue;
        }
        errno = 0;
        valor = strtol(linea, &fin, 10);
        if (fin == linea) {
            puts("Escribe un numero entero.");
            continue;
        }
        while (isspace((unsigned char)*fin)) {
            fin++;
        }
        if (errno == ERANGE || *fin != '\0' || valor < minimo || valor > maximo) {
            printf("Opcion invalida: usa un entero entre %d y %d.\n", minimo, maximo);
            continue;
        }
        *salida = (int)valor;
        return 1;
    }
}
