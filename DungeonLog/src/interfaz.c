#include <stdio.h>
#include <string.h>
#include "interfaz.h"
#include "arte_ascii.h"
#include "mazmorra.h"

#define ANCHO_TEXTO 68
static const char BORDE[] = "+----------------------------------------------------------------------+";
static int usar_color = 0;

/* Solo secuencias SGR opcionales. No mueve cursor ni modifica la terminal. */
static void color(EstiloVisual estilo)
{
    static const char *const codigos[] = {"\033[36m", "\033[33m", "\033[31m",
                                         "\033[32m", "\033[35m"};
    if (usar_color) {
        fputs(codigos[estilo], stdout);
    }
}

static void restaurar_color(void)
{
    if (usar_color) {
        fputs("\033[0m", stdout);
    }
}

/* Dibuja un fragmento de hasta 68 caracteres en un panel de 72 columnas.
   Los datos son prestados: ninguna funcion de presentacion usa malloc/free. */
static void fragmento(const char *texto, int longitud)
{
    printf("| %-68.*s |\n", longitud, texto);
}

/* Ajusta texto por palabras, sin perder contenido. Respeta saltos explicitos.
   Se usa para prosa; el arte se imprime por lineas para conservar espacios. */
void interfaz_linea(const char *texto)
{
    do {
        int longitud = 0;
        while (texto[longitud] != '\0' && texto[longitud] != '\n' &&
               longitud < ANCHO_TEXTO) {
            longitud++;
        }
        if (longitud == ANCHO_TEXTO && texto[longitud] != '\0' &&
            texto[longitud] != '\n' && texto[longitud] != ' ') {
            int corte = longitud;
            while (corte > 0 && texto[corte] != ' ') {
                corte--;
            }
            if (corte > 0) {
                longitud = corte;
            }
        }
        fragmento(texto, longitud);
        texto += longitud;
        if (*texto == '\n') {
            texto++;
        } else {
            while (*texto == ' ') {
                texto++;
            }
        }
    } while (*texto != '\0');
}

/* Arte constante de lineas cortas, validado por las pruebas visuales. */
static void ilustracion(const char *arte)
{
    while (*arte != '\0') {
        const char *fin = strchr(arte, '\n');
        int longitud = fin != NULL ? (int)(fin - arte) : (int)strlen(arte);
        fragmento(arte, longitud);
        arte += longitud;
        if (*arte == '\n') {
            arte++;
        }
    }
}

void interfaz_configurar_color(int activado)
{
    usar_color = activado != 0;
}

/* Abre un panel. El llamador presenta contenido y cierra explicitamente. */
void interfaz_panel(const char *titulo)
{
    putchar('\n');
    color(ESTILO_ESPECIAL);
    puts(BORDE);
    interfaz_linea(titulo);
    puts(BORDE);
    restaurar_color();
}

void interfaz_cerrar_panel(void)
{
    color(ESTILO_ESPECIAL);
    puts(BORDE);
    restaurar_color();
}

/* Mensaje semantico: el texto conserva su significado sin depender del color. */
void interfaz_mensaje(EstiloVisual estilo, const char *texto)
{
    putchar('\n');
    color(estilo);
    puts(BORDE);
    interfaz_linea(texto);
    puts(BORDE);
    restaurar_color();
}

void interfaz_portada(void)
{
    interfaz_panel("DUNGEONLOG: THE FORGOTTEN DUNGEON");
    color(ESTILO_INFO);
    ilustracion(ARTE_PORTADA);
    restaurar_color();
    interfaz_cerrar_panel();
}

/* Recorre la lista solo para presentar: cantidades siguen unidas a su objeto. */
void interfaz_inventario(const Inventario *inventario)
{
    const NodoInventario *actual;
    interfaz_panel("INVENTARIO - BOLSA DEL EXPLORADOR");
    ilustracion(ARTE_BOLSA);
    interfaz_cerrar_panel();
    interfaz_linea("ID ICO OBJETO                    CANTIDAD    TIPO     VALOR");
    interfaz_cerrar_panel();
    if (inventario->cabeza == NULL) {
        interfaz_linea("La bolsa esta vacia.");
    }
    for (actual = inventario->cabeza; actual != NULL; actual = actual->siguiente) {
        char fila[128];
        TipoObjeto tipo = actual->objeto.tipo;
        const char *nombre_tipo = tipo == OBJETO_POCION ? "Pocion" :
                                  tipo == OBJETO_RELIQUIA ? "Reliquia" : "Tesoro";
        snprintf(fila, sizeof fila, "%2d %-3s %-25.25s x%-10d %-8s %5d",
                 actual->objeto.id, arte_icono_objeto(actual->objeto.id), actual->objeto.nombre,
                 actual->cantidad, nombre_tipo, actual->objeto.valor);
        color(tipo == OBJETO_RELIQUIA ? ESTILO_ESPECIAL : ESTILO_TESORO);
        interfaz_linea(fila);
        restaurar_color();
    }
    interfaz_cerrar_panel();
    interfaz_linea("<*> OBJETO DE MISION: la reliquia se protege durante la aventura.");
    interfaz_cerrar_panel();
}

/* Pila prestada: no desapila para dibujar el camino. */
void interfaz_historial(const Historial *historial, const Sala salas[])
{
    const NodoHistorial *actual;
    interfaz_panel("CAMINO DE REGRESO - Pila LIFO");
    if (historial->tope == NULL) {
        interfaz_linea("No hay salas apiladas.");
    }
    for (actual = historial->tope; actual != NULL; actual = actual->siguiente) {
        char fila[128];
        int id = actual->id_sala;
        snprintf(fila, sizeof fila, "%s [%d] %s",
                 actual == historial->tope ? "TOPE ->" : "     ->", id,
                 id >= 0 && id < TOTAL_SALAS ? salas[id].nombre : "Sala desconocida");
        interfaz_linea(fila);
    }
    interfaz_cerrar_panel();
}

/* El catalogo permite siete fichas. Los datos se copian para el recorrido. */
void interfaz_bestiario(const Bestiario *bestiario, OrdenRecorrido orden)
{
    Enemigo registros[TOTAL_ENEMIGOS];
    int cantidad = bestiario_recorrer(bestiario, orden, registros, TOTAL_ENEMIGOS);
    int i;
    interfaz_panel("BESTIARIO - CRONICAS DE LOS OLVIDADOS");
    if (cantidad == 0) {
        interfaz_linea("No hay enemigos registrados.");
    }
    for (i = 0; i < cantidad; i++) {
        char titulo[96];
        snprintf(titulo, sizeof titulo, "ID %02d | %s | peligro %d",
                 registros[i].id, registros[i].nombre, registros[i].peligro);
        color(ESTILO_ENEMIGO);
        interfaz_linea(titulo);
        ilustracion(arte_enemigo(registros[i].id));
        restaurar_color();
        interfaz_linea(registros[i].descripcion);
        interfaz_cerrar_panel();
    }
    if (cantidad == 0) {
        interfaz_cerrar_panel();
    }
}

/* Dibuja ramas verticales, con I/D explicitos y NULL en hijos faltantes.
   Es lectura recursiva: no inserta, elimina ni altera enlaces del ABB.
   El menu limita el arbol a siete nodos: el prefijo cabe en 40 caracteres. */
static void dibujar_rama(const NodoBestiario *nodo, const char *prefijo,
                         char lado, int ultimo)
{
    char fila[80], siguiente[40];
    if (nodo == NULL) {
        snprintf(fila, sizeof fila, "%s+-- %c [NULL]", prefijo, lado);
    } else {
        snprintf(fila, sizeof fila, "%s+-- %c [%02d]", prefijo, lado, nodo->enemigo.id);
    }
    interfaz_linea(fila);
    if (nodo == NULL || (nodo->izquierdo == NULL && nodo->derecho == NULL)) {
        return;
    }
    snprintf(siguiente, sizeof siguiente, "%s%s", prefijo, ultimo ? "    " : "|   ");
    dibujar_rama(nodo->izquierdo, siguiente, 'I', 0);
    dibujar_rama(nodo->derecho, siguiente, 'D', 1);
}

void interfaz_arbol(const Bestiario *bestiario)
{
    char raiz[48];
    interfaz_panel("ESTRUCTURA DEL ABB - I: izquierda / D: derecha");
    if (bestiario->raiz == NULL) {
        interfaz_linea("RAIZ [NULL]");
    } else {
        snprintf(raiz, sizeof raiz, "RAIZ [%02d]", bestiario->raiz->enemigo.id);
        interfaz_linea(raiz);
        if (bestiario->raiz->izquierdo != NULL || bestiario->raiz->derecho != NULL) {
            dibujar_rama(bestiario->raiz->izquierdo, "  ", 'I', 0);
            dibujar_rama(bestiario->raiz->derecho, "  ", 'D', 1);
        }
    }
    interfaz_linea("Cada enlace une padre e hijo; I: ID menor, D: ID mayor.");
    interfaz_cerrar_panel();
}

/* Atlas completo, exclusivo de mesa: nunca se usa como mapa de aventura. */
void interfaz_salas(const Sala salas[])
{
    int i;
    interfaz_panel("ATLAS DE DISENO - Nueve salas");
    interfaz_linea("Norte ^    [0]---[1]---[2]");
    interfaz_linea("            |     |     |");
    interfaz_linea("           [3]---[4]---[5]");
    interfaz_linea("            |     |     |");
    interfaz_linea("           [6]---[7]---[8]");
    interfaz_cerrar_panel();
    for (i = 0; i < TOTAL_SALAS; i++) {
        char conexiones[80];
        const Enemigo *enemigo = mazmorra_enemigo(salas[i].id_enemigo);
        interfaz_sala_actual(&salas[i]);
        snprintf(conexiones, sizeof conexiones, "N:%d E:%d S:%d O:%d | Enemigo: %s",
                 salas[i].salidas[NORTE], salas[i].salidas[ESTE],
                 salas[i].salidas[SUR], salas[i].salidas[OESTE],
                 enemigo ? enemigo->nombre : "Ninguno");
        interfaz_linea(conexiones);
        interfaz_cerrar_panel();
    }
}

/* Descripcion recurrente compacta; el arte de primera visita va por separado. */
void interfaz_sala_actual(const Sala *sala)
{
    static const char *const direcciones[] = {"Norte", "Este", "Sur", "Oeste"};
    const Objeto *objeto = mazmorra_objeto(sala->id_objeto);
    char texto[128];
    int d, usados;
    snprintf(texto, sizeof texto, "%s [%02d]", sala->nombre, sala->id);
    interfaz_panel(texto);
    interfaz_linea(sala->descripcion);
    usados = snprintf(texto, sizeof texto, "Salidas:");
    for (d = 0; d < TOTAL_DIRECCIONES; d++) {
        if (sala->salidas[d] != SIN_CONTENIDO) {
            usados += snprintf(texto + usados, sizeof texto - (size_t)usados,
                               " %s", direcciones[d]);
        }
    }
    interfaz_linea(texto);
    if (objeto != NULL) {
        snprintf(texto, sizeof texto, "Objeto: %s %s", arte_icono_objeto(objeto->id), objeto->nombre);
    } else {
        snprintf(texto, sizeof texto, "Objeto: Ninguno");
    }
    color(objeto ? ESTILO_TESORO : ESTILO_INFO);
    interfaz_linea(texto);
    restaurar_color();
    interfaz_cerrar_panel();
}

void interfaz_entrada_sala(const Sala *sala, int primera_visita)
{
    char texto[96];
    snprintf(texto, sizeof texto, "%s - %s", primera_visita ? "PRIMERA VISITA" : "REGRESO",
             sala->nombre);
    interfaz_panel(texto);
    if (primera_visita) {
        color(ESTILO_INFO);
        ilustracion(arte_sala(sala->id));
        restaurar_color();
    } else {
        interfaz_linea("Reconoces estas piedras. Tu camino sigue marcado en la ceniza.");
    }
    interfaz_cerrar_panel();
}

/* Retrato solo en el primer descubrimiento del enemigo; regreso compacto. */
void interfaz_encuentro(const Sala *sala, int descubierto)
{
    const Enemigo *enemigo = mazmorra_enemigo(sala->id_enemigo);
    if (enemigo != NULL) {
        interfaz_panel(descubierto ? "ENTRE LAS SOMBRAS - DESCUBIERTO" : "UN ECO CONOCIDO");
        color(ESTILO_ENEMIGO);
        interfaz_linea(enemigo->nombre);
        if (descubierto) {
            ilustracion(arte_enemigo(enemigo->id));
        }
        restaurar_color();
        interfaz_linea(enemigo->descripcion);
        interfaz_linea(descubierto ? "[DESCUBIERTO] Su nombre queda inscrito en tu bestiario."
                                  : "[RECONOCIDO] Su presencia ya habita en tu bestiario.");
        interfaz_cerrar_panel();
    }
}

/* No revela nombres, contenido ni conexiones de salas aun desconocidas. */
void interfaz_mapa(const Sala salas[], int sala_actual)
{
    int fila, columna;
    interfaz_panel("CARTOGRAFIA DE LAS CENIZAS");
    interfaz_linea("Norte ^    @ posicion actual | ? sin descubrir");
    interfaz_linea("");
    for (fila = 0; fila < 3; fila++) {
        char celdas[64] = "          ", enlaces[64] = "          ";
        for (columna = 0; columna < 3; columna++) {
            char celda[8];
            int id = fila * 3 + columna;
            if (id == sala_actual) {
                snprintf(celda, sizeof celda, "[@%d]", id);
            } else if (salas[id].visitada) {
                snprintf(celda, sizeof celda, "[ %d]", id);
            } else {
                snprintf(celda, sizeof celda, "[ ?]");
            }
            strcat(celdas, celda);
            if (columna < 2) {
                strcat(celdas, salas[id].visitada && salas[id + 1].visitada &&
                       salas[id].salidas[ESTE] == id + 1 ? "---" : "   ");
            }
            if (fila < 2) {
                strcat(enlaces, salas[id].visitada && salas[id + 3].visitada &&
                       salas[id].salidas[SUR] == id + 3 ? " |     " : "       ");
            }
        }
        color(ESTILO_INFO);
        interfaz_linea(celdas);
        if (fila < 2) {
            interfaz_linea(enlaces);
        }
        restaurar_color();
    }
    interfaz_linea("");
    for (fila = 0; fila < TOTAL_SALAS; fila++) {
        if (salas[fila].visitada) {
            char nombre[80];
            snprintf(nombre, sizeof nombre, "[%d] %s%s", fila, salas[fila].nombre,
                     fila == sala_actual ? " <- AQUI" : "");
            interfaz_linea(nombre);
        }
    }
    interfaz_cerrar_panel();
}

void interfaz_reliquia(void)
{
    interfaz_panel("RELIQUIA OBTENIDA - CORAZON DEL ALBA");
    color(ESTILO_TESORO);
    ilustracion(ARTE_RELIQUIA);
    restaurar_color();
    interfaz_linea("La piedra se abre. Una luz antigua despierta entre tus manos.");
    interfaz_linea("[OBJETIVO] Regresa al Umbral de las Cenizas, sala 0.");
    interfaz_cerrar_panel();
}

void interfaz_victoria(void)
{
    interfaz_panel("VICTORIA - EL ALBA REGRESA");
    color(ESTILO_TESORO);
    ilustracion(ARTE_VICTORIA);
    restaurar_color();
    interfaz_linea("Cruzas el umbral con el Corazon del Alba entre tus manos.");
    interfaz_linea("La luz rompe el silencio. La mazmorra recuerda tu nombre.");
    interfaz_cerrar_panel();
}

void interfaz_menu_aventura(void)
{
    interfaz_panel("SENDEROS DEL EXPLORADOR");
    interfaz_linea("[1] Explorar    [2] Retroceder    [3] Recoger objeto");
    interfaz_linea("[4] Inventario  [5] Camino        [6] Bestiario");
    interfaz_linea("[7] Mapa        [0] Terminar aventura");
    interfaz_cerrar_panel();
}
