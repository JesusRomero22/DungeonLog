#ifndef ARTE_ASCII_H
#define ARTE_ASCII_H

/* Ilustraciones originales de DungeonLog. Memoria estatica: no usar free.
   Cada linea cabe en el interior de un panel de 72 columnas. */
extern const char ARTE_PORTADA[];
extern const char ARTE_BOLSA[];
extern const char ARTE_RELIQUIA[];
extern const char ARTE_VICTORIA[];
const char *arte_enemigo(int id);
const char *arte_sala(int id);
const char *arte_icono_objeto(int id);

#endif
