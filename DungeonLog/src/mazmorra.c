#include <stddef.h>
#include "mazmorra.h"

static const Objeto objetos[TOTAL_OBJETOS] = {
    {1, "Pocion de ceniza", OBJETO_POCION, 10},
    {2, "Moneda del sepulturero", OBJETO_TESORO, 5},
    {3, "Caliz de obsidiana", OBJETO_TESORO, 40},
    {4, "Amuleto marchito", OBJETO_TESORO, 25},
    {5, "Corazon del Alba", OBJETO_RELIQUIA, 100}
};

/* El orden 40,20,60,10,30,50,70 facilita demostrar un ABB pequeno. */
static const Enemigo enemigos[TOTAL_ENEMIGOS] = {
    {40, "Custodio hueco", "Una armadura vacia vigila el santuario.", 3},
    {20, "Monje de ceniza", "Repite plegarias ante una vela extinguida.", 2},
    {60, "Espectro del pozo", "Su reflejo aparece antes que su silueta.", 3},
    {10, "Rata osaria", "Arrastra pequenos huesos entre las grietas.", 1},
    {30, "Escriba ciego", "Custodia libros que nadie debe abrir.", 2},
    {50, "Caballero caido", "Su espada descansa sobre una tumba rota.", 3},
    {70, "Sombra del relicario", "Una sombra inmovil rodea la reliquia.", 4}
};

/* Salidas siempre en orden norte, este, sur, oeste. -1 significa pared.
   Los IDs 0..8 son indices del arreglo y todas las conexiones son reciprocas. */
static const Sala salas_iniciales[TOTAL_SALAS] = {
    {0, "Umbral de las Cenizas", "La ultima luz del exterior cae sobre una puerta oxidada.",
     {-1, 1, 3, -1}, -1, -1, 1},
    {1, "Galeria de los Huesos", "Huesos pulidos dibujan un sendero bajo arcos quebrados.",
     {-1, 2, 4, 0}, 2, 10, 0},
    {2, "Capilla del Silencio", "Una vela negra ilumina un altar cubierto de ceniza.",
     {-1, -1, 5, 1}, 1, 20, 0},
    {3, "Archivo Ciego", "Los estantes conservan mapas de un reino olvidado.",
     {0, 4, 6, -1}, 4, 30, 0},
    {4, "Santuario Hueco", "Una armadura aguarda bajo una campana sin badajo.",
     {1, 5, 7, 3}, -1, 40, 0},
    {5, "Pozo de los Ecos", "El agua devuelve susurros en lugar de reflejos.",
     {2, -1, 8, 4}, -1, 60, 0},
    {6, "Cripta del Juramento", "Un caballero arrodillado protege un caliz ennegrecido.",
     {3, 7, -1, -1}, 3, 50, 0},
    {7, "Antesala del Olvido", "Nombres borrados cubren las paredes de piedra fria.",
     {4, 8, -1, 6}, -1, -1, 0},
    {8, "Relicario del Alba", "El Corazon del Alba late entre las manos de una estatua.",
     {5, -1, -1, 7}, 5, 70, 0}
};

/* Copia las nueve salas a memoria del llamador. No usa malloc; permite reinicio. */
void mazmorra_inicializar(Sala salas[TOTAL_SALAS])
{
    int i;
    for (i = 0; i < TOTAL_SALAS; i++) {
        salas[i] = salas_iniciales[i];
    }
}

/* Consulta id y retorna datos constantes del catalogo, o NULL. No liberar. */
const Objeto *mazmorra_objeto(int id)
{
    int i;
    for (i = 0; i < TOTAL_OBJETOS; i++) {
        if (objetos[i].id == id) {
            return &objetos[i];
        }
    }
    return NULL;
}

/* Consulta id y retorna datos constantes del catalogo, o NULL. No liberar. */
const Enemigo *mazmorra_enemigo(int id)
{
    int i;
    for (i = 0; i < TOTAL_ENEMIGOS; i++) {
        if (enemigos[i].id == id) {
            return &enemigos[i];
        }
    }
    return NULL;
}
