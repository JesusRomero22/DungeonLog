# DungeonLog: The Forgotten Dungeon

Proyecto academico de Estructuras de Datos en C11. Identidad visual ASCII de
fantasia oscura, sin bibliotecas externas. **Fase 3: presentacion RPG retro**.
Recupera el Corazon del Alba en la sala 8 y regresa a la entrada (sala 0).
Incluye una mesa separada para demostrar las tres estructuras. No hay combate,
salud, consumo de pociones, guardado ni generacion aleatoria.

## Compilar y ejecutar (PowerShell)

Desde la carpeta del proyecto, con GCC disponible en PATH:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/*.c -o build/DungeonLog.exe
.\build\DungeonLog.exe
```

Sin argumentos no se emiten codigos ANSI, no se limpia la pantalla y no se
requieren caracteres Unicode. El programa funciona como texto normal, incluso
con entrada redirigida. Opciones disponibles:

```powershell
.\build\DungeonLog.exe --sin-color
.\build\DungeonLog.exe --color
.\build\DungeonLog.exe --aventura
.\build\DungeonLog.exe --mesa
```

`--color` activa una paleta ANSI: amarillo/dorado para tesoros, rojo para
enemigos, verde para acciones correctas y cian/magenta para informacion especial.
Ante una terminal incompatible, usar el modo predeterminado o `--sin-color`.
Los colores solo agregan secuencias SGR; el texto y las acciones son identicos.
El programa no cambia configuraciones de Windows.

## Presentacion de la fase 3

- Portada original de la fortaleza, mostrada una vez por ejecucion.
- Paneles de 72 columnas, con prosa ajustada por palabras y arte sin Unicode.
- Nueve vinetas de salas y siete retratos originales de enemigos.
- Primera visita con ilustracion; regreso con mensaje compacto. El bestiario
  permite consultar nuevamente los retratos de enemigos descubiertos.
- Inventario con bolsa, iconos individuales, cantidades, tipo y valor.
- Ilustraciones grandes exclusivas para obtencion de reliquia y victoria.
- Mapa de salas descubiertas que conserva oculta la identidad de las demas.
- Arbol explicativo en la mesa con RAIZ, hijos I/D y enlaces. Tras cada alta o
  baja aparece el resultado inmediatamente. NULL indica un hijo ausente.

Todo el arte vive como cadenas constantes en `src/arte_ascii.c`, con declaraciones
en `include/arte_ascii.h`. Son composiciones originales de DungeonLog; las galerias
indicadas por el usuario se usaron como inspiracion tematica. Las ilustraciones
no reservan memoria y no cambian el estado del juego. No se usan pausas,
animaciones, secuencias de cursor ni borrado de pantalla.

Sin `--aventura` o `--mesa` aparece el selector de modos. Cada sesion tiene
datos propios; al terminar vuelve al selector y libera sus nodos. Elegir una
nueva aventura siempre empieza desde cero. Con modo directo, salir finaliza
el programa. Se puede combinar cualquier modo con `--color` o `--sin-color`.

## Aventura: controles y partida completa

| Opcion | Accion |
| --- | --- |
| 1 Explorar | Elegir 1 Norte, 2 Este, 3 Sur o 4 Oeste; 0 cancela |
| 2 Retroceder | Extraer la sala anterior de la pila sin volver a apilar |
| 3 Recoger | Agregar el objeto de la sala al inventario y retirarlo de la sala |
| 4 Inventario | Consultar, ordenar por valor o descartar una unidad |
| 5 Camino | Consultar el recorrido de regreso, desde el tope |
| 6 Bestiario | Mostrar enemigos descubiertos con descripcion y peligro |
| 7 Mapa | Consultar posicion y salas descubiertas |
| 0 Terminar | Finalizar la aventura actual; no se guarda |

Ruta breve para ganar:

1. Iniciar una aventura: apareces en Umbral de las Cenizas, sala 0.
2. Explorar **Este**: sala 1, Galeria de los Huesos.
3. Explorar **Sur**: sala 4, Santuario Hueco.
4. Explorar **Sur**: sala 7, Antesala del Olvido.
5. Explorar **Este**: sala 8, Relicario del Alba.
6. Elegir **Recoger objeto** para obtener el Corazon del Alba.
7. Elegir **Retroceder cuatro veces**: 7 -> 4 -> 1 -> 0.
8. Al regresar a 0 con la reliquia aparece la pantalla ASCII de victoria.

En numeros, tras iniciar el modo aventura, cada numero en su propia linea:
`1,2, 1,3, 1,3, 1,2, 3, 2,2,2,2`.

Tambien puedes regresar usando direcciones. Visitar la sala 8 sin recoger la
reliquia o regresar sin ella no gana. Los encuentros son narrativos, sin combate.
Descartar un objeto lo elimina definitivamente; la reliquia esta protegida.
El mapa conserva las salas visitadas incluso despues de retroceder; `?` oculta
las identidades no descubiertas y `@` indica la posicion actual.

## Como usar el menu provisional

Cada submenu muestra primero el contenido, ejecuta **una accion** y vuelve al
menu principal. Volver a elegir la estructura permite inspeccionar el resultado.
Las estructuras comienzan vacias. Salir o finalizar la entrada libera sus nodos.
Acceder con `--mesa` o mediante la opcion 2 del selector.

| Menu | Operaciones |
| --- | --- |
| 1 Inventario | Agregar objetos 1..5, quitar una unidad, buscar, ordenar por valor |
| 2 Historial | Apilar salas 0..8, desapilar, buscar, consultar tope |
| 3 Bestiario | Insertar IDs del catalogo, eliminar, buscar, inorden, preorden, postorden |
| 4 Atlas | Ver las nueve salas, conexiones, objetos y enemigos |
| 0 Salir | Liberar las estructuras y terminar |

Los IDs del bestiario son 10, 20, 30, 40, 50, 60 y 70. Insertarlos en el orden
40, 20, 60, 10, 30, 50, 70 construye el arbol de ejemplo de las pruebas.

En esta mesa se puede quitar cualquier objeto, incluida la reliquia: se comprueba
la lista sin reglas de mision. Apilar no mueve al jugador y eliminar un registro
no modifica el catalogo. Su Juego se crea y se libera independientemente de las
sesiones de aventura, por lo que no puede fabricar objetos para ellas.

## Archivos y responsabilidades

| Archivo o modulo | Proposito |
| --- | --- |
| `include/modelos.h` | Tipos de objetos, enemigos, salas y direcciones |
| `src/inventario.c` y su `.h` | Lista simple, cantidades y selection sort |
| `src/historial.c` y su `.h` | Pila dinamica LIFO |
| `src/bestiario.c` y su `.h` | ABB por ID, tres recorridos y eliminacion |
| `src/mazmorra.c` y su `.h` | Catalogos constantes y copia inicial de las nueve salas |
| `src/juego.c` y su `.h` | Estado, ciclo del menu y coordinacion de operaciones |
| `src/aventura.c` | Movimientos, encuentros automaticos, recoleccion, descarte y victoria |
| `src/menu_aventura.c` | Controles de aventura y traduccion de resultados a mensajes |
| `src/interfaz.c` y su `.h` | ASCII, paneles, atlas y presentacion de registros |
| `src/arte_ascii.c` y su `.h` | Cadenas constantes de arte original e iconos |
| `src/entrada.c` y su `.h` | Lectura con fgets/strtol, validacion y EOF |
| `src/main.c` | Argumentos, inicializacion y limpieza final |
| `tests/pruebas_estructuras.c` | Pruebas automaticas de contratos y datos |
| `tests/pruebas_menu.py` | Pruebas adicionales de entrada/salida; Python es solo para probar |
| `tests/pruebas_aventura.c` | Pruebas de reglas, aislamiento y fallos de memoria provocados |
| `tests/pruebas_menu_aventura.py` | Partida completa, mapa y aislamiento desde la consola |
| `tests/pruebas_presentacion.py` | Ancho, ASCII, paleta, primeras visitas y diagramas del arbol |
| `tests/probar.ps1` | Compila el programa y ejecuta todas las suites |
| `docs/diseno.md` | Decisiones, memoria, complejidad y aventura prevista |
| `docs/casos_prueba.md` | Escenarios reproducibles y resultados esperados |

## Ejecutar las pruebas

Para compilar y ejecutar todas las pruebas (GCC y Python 3 en PATH):

```powershell
.\tests\probar.ps1
```

Si Python tiene otra ubicacion: `.\tests\probar.ps1 -Python 'C:\ruta\python.exe'`.
El script se detiene ante cualquier error. Para fallos de memoria compila objetos
separados de las tres estructuras con sustitutos de malloc/free exclusivos de
las pruebas. El ejecutable normal conserva malloc/free de la biblioteca estandar.
Los binarios resultantes estan en `build/`.

No compilar el archivo de pruebas junto con `src/main.c`: ambos definen su propio
`main`. Las pruebas usan `assert`; no agregar `-DNDEBUG` a esta compilacion.

```powershell
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude tests/pruebas_estructuras.c src/inventario.c src/historial.c src/bestiario.c src/mazmorra.c -o build/pruebas_estructuras.exe
.\build\pruebas_estructuras.exe
```

Salida final esperada: `Todas las pruebas finalizaron correctamente.`

Si se dispone de Python 3, tambien se puede comprobar el menu ya compilado:

```powershell
python tests/pruebas_menu.py
python tests/pruebas_menu_aventura.py
python tests/pruebas_presentacion.py
```

Python no es necesario para compilar o ejecutar DungeonLog. Las secuencias
manuales equivalentes estan en `docs/casos_prueba.md`.

Las pruebas visuales guardan transcripciones de ejecuciones reales en
`build/vistas/exploracion.txt`, `build/vistas/victoria.txt` y `build/vistas/arbol.txt`.
Se pueden abrir en un editor con fuente monoespaciada para revisar las pantallas.
