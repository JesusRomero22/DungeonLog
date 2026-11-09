# Diseno de DungeonLog - Fase 2

## Separacion de responsabilidades

`main` configura color y crea un Juego nuevo por sesion de aventura o mesa,
lo ejecuta y libera sus recursos. `juego.c` conserva inicializacion, limpieza y
la mesa original; `aventura.c` contiene reglas sin entrada/salida y
`menu_aventura.c` coordina controles y presentacion. Las estructuras no usan
stdio, no conocen salas concretas ni dependen de la interfaz. `interfaz` presenta
datos prestados: recorre lista y pila sin modificarlas; para el ABB solicita una
copia del recorrido. No hay callbacks, estructuras genericas ni bibliotecas externas.

Las funciones publicas reciben punteros a estructuras inicializadas, no NULL.
Los parametros de salida deben apuntar a memoria valida del llamador. Los
comentarios de cada funcion explican proposito, parametros y memoria.

## Datos y memoria

- Los catalogos son constantes y sus consultas retornan punteros prestados.
- Cada nodo copia su Objeto o Enemigo por valor. Los textos son arreglos fijos.
- Inventario, Historial y Bestiario son propietarios exclusivos de sus nodos.
- Solo los nodos requieren malloc/free. Las nueve salas son un arreglo en Juego.
- Cada malloc se verifica antes de cambiar enlaces. Un fallo deja intacta la estructura.
- Los destructores dejan cabeza, tope o raiz en NULL y admiten repeticion.
- Inicializar no destruye datos existentes: liberar antes de reinicializar.
- No conservar punteros de busqueda despues de eliminar o liberar. Ordenar la
  lista intercambia datos: tampoco conservar su resultado de busqueda al ordenar.

El menu limita las altas a cinco objetos y siete enemigos distintos. La pila
crece dinamicamente; cada insercion tiene un nodo nuevo. Los modulos de
estructuras pueden utilizarse con otros datos. La vista del bestiario usa un
arreglo de siete posiciones porque el menu solo admite ese catalogo; si se
amplia, actualizar TOTAL_ENEMIGOS y sus datos. La API de recorrido acepta la
capacidad y devuelve la cantidad copiada, truncando al limite sin desbordarlo.

## Invariantes y algoritmos

### Lista

Un nodo por ID y cantidad positiva. Insertar un ID repetido incrementa cantidad,
conservando los datos originales; quitar resta una unidad y libera al llegar a
cero. La busqueda y eliminacion son O(n). Agregar es O(n) por la comprobacion
de duplicados; enlazar un nodo nuevo al inicio es O(1).

Selection sort busca el menor valor restante e intercambia Objeto y cantidad.
Es O(n^2), con espacio auxiliar O(1). Los enlaces no cambian. No es estable;
objetos con el mismo valor no tienen un orden relativo garantizado.

### Pila

El tope es el ultimo nodo insertado. Apilar, consultar tope y desapilar son O(1).
Buscar y recorrer son O(n). Puede haber salas repetidas: la ruta puede volver
a pasar por ellas. Consultar y buscar nunca extraen. En pila vacia, extraer
devuelve 0 sin modificar la variable de salida.

La exploracion apila la sala de origen al avanzar y desapila para
retroceder, sin apilar durante el retroceso. `Sala.visitada` es independiente
de la pila: el camino de regreso no es una bitacora permanente.

### Arbol binario de busqueda

IDs menores a la izquierda y mayores a la derecha; sin duplicados. Insertar
retorna INSERTADO, REPETIDO o SIN_MEMORIA. Buscar y eliminar cuestan O(h): en
un arbol degenerado h puede ser n. No hay balanceo automatico.

Insercion y eliminacion usan un puntero al enlace que se quiere cambiar
(`NodoBestiario **`). Ese enlace puede ser la raiz o el hijo de otro nodo;
asi se evita repetir codigo especial para borrar la raiz.

Eliminacion:

1. Hoja: sustituir su enlace por NULL y liberar.
2. Un hijo: conectar su padre con ese hijo y liberar el nodo retirado.
3. Dos hijos: copiar TODOS los datos del menor del subarbol derecho (sucesor
   inorden) al nodo objetivo. Retirar el sucesor conectando su posible hijo derecho.

Recorridos: inorden (izquierda, raiz, derecha), preorden (raiz, izquierda,
derecha) y postorden (izquierda, derecha, raiz). Son O(n) para recorrer todo el
arbol. Recorrido y liberacion son recursivos, con O(h) de pila de llamadas.
Es apropiado para este catalogo pequeno; no se pretende manejar arboles enormes.

## Aventura implementada

Objetivo: recoger el Corazon del Alba en la sala 8 y regresar a la sala 0.
No se requieren llaves ni combate. Los encuentros son narrativos y registran
enemigos. Los objetos se retiran de su sala solo despues de agregarlos con exito.
La victoria consulta el inventario real; la reliquia no puede descartarse en
el juego. La mesa permite operaciones manuales con un Juego independiente.

### Funciones principales de la aventura

- `juego_avanzar`: valida direccion y conexion, apila origen, registra encuentro,
  confirma destino, marca visita y comprueba victoria.
- `juego_retroceder`: consulta tope, prepara encuentro y desapila. Nunca apila.
- `juego_recoger`: agrega el objeto por valor y vacia el contenido de la sala.
- `juego_descartar`: protege tipos OBJETO_RELIQUIA y delega el resto a la lista.
- `juego_comprobar_victoria`: cambia PREPARADA a GANADA al estar en sala 0 con
  ID_CORAZON_ALBA. No depende de una variable duplicada de posesion.
- `juego_menu_aventura`: presenta sala actual y coordina acciones hasta victoria,
  salida o EOF. La pantalla de victoria conserva estado GANADA.

### Fallos de memoria: preparar antes de confirmar

Avanzar puede reservar dos nodos: historial y enemigo nuevo. Si falla apilar,
no hay cambios. Si falla el registro del enemigo, se desapila y libera el nodo
recien creado. Solo despues de ambos exitos cambian posicion y visitadas.
Un encuentro repetido no crea otro nodo de bestiario.

Retroceder prepara el encuentro antes de extraer. Normalmente el enemigo ya
esta registrado y no hay reservas; aun si faltase, un fallo conserva el tope.
Recoger deja el objeto en la sala si no logra insertarlo. Las consultas,
ordenamiento, descarte y comprobacion de victoria no necesitan malloc.

En aventura la pila contiene exclusivamente destinos validos generados por
movimientos. La mesa no puede inyectarle IDs: cada sesion tiene un Juego propio.
Los catalogos constantes son lo unico compartido. Salir termina la sesion;
el selector permite comenzar otra nueva, no reanudar partidas.

```text
Norte ^
  [0]---[1]---[2]
   |     |     |
  [3]---[4]---[5]
   |     |     |
  [6]---[7]---[8]
```

| ID | Sala y descripcion | N / E / S / O | Objeto | Enemigo |
| --- | --- | --- | --- | --- |
| 0 | Umbral de las Cenizas: la ultima luz del exterior cae sobre una puerta oxidada. | - / 1 / 3 / - | Ninguno | Ninguno |
| 1 | Galeria de los Huesos: huesos pulidos dibujan un sendero bajo arcos quebrados. | - / 2 / 4 / 0 | Moneda del sepulturero | Rata osaria (10) |
| 2 | Capilla del Silencio: una vela negra ilumina un altar cubierto de ceniza. | - / - / 5 / 1 | Pocion de ceniza | Monje de ceniza (20) |
| 3 | Archivo Ciego: los estantes conservan mapas de un reino olvidado. | 0 / 4 / 6 / - | Amuleto marchito | Escriba ciego (30) |
| 4 | Santuario Hueco: una armadura aguarda bajo una campana sin badajo. | 1 / 5 / 7 / 3 | Ninguno | Custodio hueco (40) |
| 5 | Pozo de los Ecos: el agua devuelve susurros en lugar de reflejos. | 2 / - / 8 / 4 | Ninguno | Espectro del pozo (60) |
| 6 | Cripta del Juramento: un caballero arrodillado protege un caliz ennegrecido. | 3 / 7 / - / - | Caliz de obsidiana | Caballero caido (50) |
| 7 | Antesala del Olvido: nombres borrados cubren las paredes de piedra fria. | 4 / 8 / - / 6 | Ninguno | Ninguno |
| 8 | Relicario del Alba: el Corazon del Alba late entre las manos de una estatua. | 5 / - / - / 7 | Corazon del Alba | Sombra del relicario (70) |

Ruta de ejemplo: 0 -> 1 -> 4 -> 7 -> 8, y regreso por la pila.

## Identidad visual

Castillo ASCII, paneles de 60 columnas, nombres de fantasia oscura y menus
numerados. Solo caracteres ASCII en el programa para evitar problemas de
codificacion en terminales. Los registros y descripciones extensas van debajo
del panel; no se presupone un ancho de terminal para truncar contenido importante.
No hay cursor especial, borrado de pantalla, pausas del sistema ni animaciones.
ANSI magenta es una opcion estetica; todas las acciones funcionan sin el.
