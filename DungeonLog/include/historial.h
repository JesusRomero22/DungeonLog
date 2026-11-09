#ifndef HISTORIAL_H
#define HISTORIAL_H

typedef struct NodoHistorial {
    int id_sala;
    struct NodoHistorial *siguiente;
} NodoHistorial;

typedef struct { NodoHistorial *tope; } Historial;

void historial_inicializar(Historial *historial);
int historial_apilar(Historial *historial, int id_sala);
int historial_desapilar(Historial *historial, int *id_sala);
int historial_consultar_tope(const Historial *historial, int *id_sala);
int historial_buscar(const Historial *historial, int id_sala);
void historial_liberar(Historial *historial);

#endif
