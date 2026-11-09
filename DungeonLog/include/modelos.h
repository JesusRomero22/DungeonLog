#ifndef MODELOS_H
#define MODELOS_H

#define MAX_NOMBRE 48
#define MAX_DESCRIPCION 180
#define TOTAL_SALAS 9
#define SIN_CONTENIDO (-1)

typedef enum { OBJETO_POCION, OBJETO_TESORO, OBJETO_RELIQUIA } TipoObjeto;
typedef enum { NORTE, ESTE, SUR, OESTE, TOTAL_DIRECCIONES } Direccion;

typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    TipoObjeto tipo;
    int valor;
} Objeto;

typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    char descripcion[MAX_DESCRIPCION];
    int peligro;
} Enemigo;

typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    char descripcion[MAX_DESCRIPCION];
    int salidas[TOTAL_DIRECCIONES];
    int id_objeto;
    int id_enemigo;
    int visitada;
} Sala;

#endif
