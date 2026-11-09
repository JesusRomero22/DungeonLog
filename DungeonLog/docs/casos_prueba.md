# Casos de prueba reproducibles

## Pruebas automaticas

Consultar los comandos de compilacion y ejecucion en README.md. El ejecutable C
comprueba resultados con assert, no solo imprime ejemplos. Cada escenario de
eliminacion del ABB comienza con un arbol nuevo y verifica el inorden final,
la ausencia del ID eliminado y la raiz esperada.

| Caso | Preparacion / accion | Resultado esperado |
| --- | --- | --- |
| Lista vacia | Buscar, quitar y ordenar sin insertar | Ausencia, sin fallos |
| Duplicados | Agregar objetos 1..5 y otra unidad de 3 | Un nodo de ID 3, cantidad 2 |
| Selection sort | Ordenar la lista anterior | IDs 2,1,4,3,5; cantidad 2 permanece con ID 3 |
| Bajas de lista | Quitar cabeza 2, intermedio 4, ultimo 5 | Los otros registros permanecen |
| Cantidades | Quitar 3 dos veces | Primero cantidad 1; despues nodo ausente |
| Nodo unico | Quitar ultimo objeto 1 | Cabeza NULL |
| Pila vacia | Consultar y extraer con salida inicial 99 | Retorno 0; salida sigue en 99 |
| LIFO | Apilar 0,1,4; consultar; extraer todo | Consulta 4; extracciones 4,1,0 |
| Busqueda de pila | Buscar 0 y 8 en pila 4,1,0 | Encontrado y ausente, sin modificar tope |
| ABB vacio | Buscar y eliminar 40; recorrer | Ausencia y cero registros |
| ABB duplicado | Insertar 40 dos veces | REPETIDO; no agrega nodo |
| ABB hoja | Insertar 40,20,60,10,30,50,70; eliminar 10 | 20,30,40,50,60,70; raiz 40 |
| ABB un hijo | Insertar 40,20,10; eliminar 20 | 10,40; raiz 40 |
| ABB dos hijos | Insertar 40,20,60,10,30,50,70; eliminar 40 | 10,20,30,50,60,70; raiz 50 |
| Sucesor con hijo derecho | Insertar 40,20,60,50,55,70; eliminar 40 | 20,50,55,60,70; raiz 50 |
| Raiz unica | Insertar 40; eliminar 40 | Raiz NULL |
| Raiz con hijo | Insertar 40,20; eliminar 40 | Raiz 20 |
| Capacidad de recorrido | Recorrer con espacio para un registro | Solo una copia; siguiente posicion intacta |
| Limpieza | Liberar con nodos y volver a liberar | Cabeza/tope/raiz NULL |
| Salas | Consultar todas las conexiones y catalogos | Enlaces reciprocos e IDs validos |

Para el arbol 40,20,60,10,30,50,70:

```text
          40
        /    \
      20      60
     /  \    /  \
    10  30  50  70

Inorden:   10 20 30 40 50 60 70
Preorden:  40 20 10 30 60 50 70
Postorden: 10 30 20 50 70 60 40
```

## Reproduccion manual en el menu

Cada accion regresa al menu principal. Las siguientes secuencias incluyen ese
regreso: introducir cada numero en una linea cuando el programa lo solicite.

- Agregar pocion: `1, 1, 1`. Repetir y buscar con `1, 3, 1`: debe informar x2.
- Quitar una unidad: `1, 2, 1`. Buscar de nuevo: x1; quitar y buscar: ausente.
- Agregar caliz: `1, 1, 3`; moneda: `1, 1, 2`; ordenar: `1, 4`.
  La moneda de valor 5 debe mostrarse antes del caliz de valor 40.
- Apilar entrada: `2, 1, 0`; apilar galeria: `2, 1, 1`.
  Consultar `2, 4`: ID 1. Extraer `2, 2`: ID 1; repetir: ID 0;
  repetir otra vez: Pila vacia.
- Insertar enemigo: `3, 1, ID`. Repetir con 40,20,60,10,30,50,70.
  Mostrar inorden `3, 4`, preorden `3, 5`, postorden `3, 6`.
- Eliminar enemigo: `3, 2, ID`; buscar: `3, 3, ID`. Para probar los tres
  casos independientemente, cerrar con `0` y reiniciar antes de cada escenario.
- Atlas: `4`. Debe mostrar las nueve salas y sus conexiones.

## Entrada y compatibilidad

`tests/pruebas_menu.py` ejecuta secuencias controladas contra el programa compilado
y comprueba texto, codigos de salida y ausencia/presencia de ANSI. Incluye:

- Texto, linea vacia, `2x`, entero desbordado y linea de 150 caracteres.
- EOF en principal, submenus y solicitudes de ID: finalizacion con limpieza.
- Agregados, busqueda y ordenamiento del inventario, extracciones LIFO y vacio.
- Insercion de siete enemigos, eliminacion de raiz, busqueda ausente y atlas.
- Arranque normal y `--sin-color` sin escapes; `--color` con escapes ANSI.

## Pruebas de aventura (fase 2)

Ejecutar `tests/probar.ps1` para compilar y ejecutar todas las suites. El script
tambien conserva las pruebas originales de estructuras y mesa. Se usa C11 con
`-Wall -Wextra -pedantic`, sin `-DNDEBUG`.

`pruebas_aventura.c` prueba directamente las reglas, sin simular controles ni
necesitar terminal. `pruebas_menu_aventura.py` prueba ademas el ejecutable real.

| Caso | Preparacion / accion | Resultado esperado |
| --- | --- | --- |
| Inicio | Inicializar Juego | Sala 0 visitada, estructuras vacias |
| Cuatro direcciones | Avanzar por conexiones N/E/S/O existentes | Destino correcto y origen apilado |
| Pared y direccion invalida | N/O desde 0, direccion 99 | Sin cambios en ninguna estructura |
| Retroceso vacio | Retroceder desde el inicio | SIN_HISTORIAL, sin cambios |
| Ruta de regreso | Avanzar 0->1->4; retroceder dos veces | 4->1->0, pila vacia, 4 sigue visitada |
| Encuentros | Entrar en 1 y 4, volver a entrar | IDs 10 y 40 registrados, sin duplicados |
| Recoger | Recoger moneda en 1 dos veces, revisitar | Cantidad 1, objeto retirado; no vuelve a recogerse |
| Descartar | Quitar moneda y revisitar sala | Ausente del inventario; no reaparece en sala |
| Objetivo | Llegar a 8 sin recoger, recoger y volver | Solo gana al estar en 0 con la reliquia |
| Proteccion | Descartar ID 5 antes de ganar | RELIQUIA_PROTEGIDA, cantidad 1 conservada |
| Regreso manual | 8->7->4->1->0 mediante direcciones | Victoria, igual que mediante retroceso |
| Partida ganada | Intentar mas acciones | PARTIDA_INACTIVA, estado GANADA conservado |
| Aislamiento | Manipular una mesa independiente de una aventura | Inventario, pila, arbol y salas de aventura intactos |
| Mapa inicial | Mostrar mapa al comenzar | @0 y ?, sin nombres de salas desconocidas |
| Mapa tras avance | Visitar 1 y consultar mapa | 0 y 1 descubiertas, @1, resto oculto |
| Partida por consola | E,S,S,E, recoger, retroceder cuatro veces | Narraciones y una pantalla de victoria ASCII |
| Selector | Crear reliquia en mesa, iniciar nueva aventura | Aventura sin objetos ni descubrimientos heredados |

### Fallos de memoria reproducibles

Solo para `pruebas_aventura.exe`, los tres modulos de estructuras se compilan
con `-Dmalloc=prueba_malloc -Dfree=prueba_free`. Las funciones sustitutas estan
en el archivo de pruebas y llaman al malloc/free real. El juego normal y los
archivos fuente de las estructuras no se modifican.

El contador `hasta_fallo` provoca un fallo inmediato (0) o permite una reserva
y falla la siguiente (1). -1 desactiva el fallo. Los escenarios comprueban:

1. Fallo de apilado: posicion, visitas, ruta y bestiario intactos.
2. Apilado exitoso y fallo de registro: se libera el nodo temporal y se restaura
   la ruta anterior, conservando los enemigos descubiertos previamente.
3. Retroceso con enemigo descubierto: funciona incluso si toda reserva falla.
4. Retroceso con registro faltante: un fallo no extrae el tope; se puede reintentar.
5. Fallo al recoger reliquia: permanece en la sala y no aparece en inventario;
   al reintentar se obtiene exactamente una unidad.

`nodos_vivos` cuenta reservas exitosas menos liberaciones en estas estructuras.
Vuelve a cero despues de cada escenario. Es una comprobacion de balance de
nodos, no un detector general de errores de memoria.

## Regresion de presentacion (fase 3)

`tests/pruebas_presentacion.py` se ejecuta al final de `tests/probar.ps1`.
No modifica datos ni reglas del programa para producir las pantallas.

| Caso | Accion | Verificacion |
| --- | --- | --- |
| Cobertura artistica | Recorrer 0,1,2,5,8,7,6,3,4 y consultar bestiario | Nueve primeras visitas y siete enemigos |
| Inventario | Recoger en las salas con objeto, consultar y ordenar | Los cinco objetos y sus fichas caben en la terminal |
| Ancho | Inspeccionar cada linea emitida | Maximo 80 columnas; filas enmarcadas de 72 |
| ASCII | Decodificar y revisar salida | Solo ASCII imprimible y saltos, tras retirar SGR |
| Color opcional | Repetir recorrido con/sin color | Contenido identico; solo cambia SGR |
| Paleta | Examinar salida ANSI | Rojo, verde, amarillo, magenta y cian presentes |
| Repeticion | Entrar 1, retroceder, volver a entrar y consultar mapa | Retrato nuevo una vez; regreso compacto |
| Portada | Ejecutar una sesion | Un solo titulo artistico grande |
| Reliquia | Recogerla y volver a intentar | Una sola pantalla de obtencion |
| Victoria | Regresar con la reliquia | Una sola pantalla final |
| Arbol completo | Insertar 40,20,60,10,30,50,70 | RAIZ 40, I 20, D 60, conexiones a nietos |
| Arbol tras baja | Eliminar hoja, nodo con hijo o raiz con dos hijos | Ultimo diagrama muestra inmediatamente el resultado |
| Arbol vacio | Insertar 40, eliminar 40 | RAIZ NULL |
| Profundidad maxima | Insertar IDs 10..70 en orden | Todas las ramas caben en 80 columnas |

El recorrido de cobertura no retorna a 0, para permitir inspeccionar todos los
objetos y enemigos antes de finalizar. Una segunda ejecucion prueba victoria.
Las transcripciones quedan en `build/vistas/` para revision humana con fuente
monoespaciada. Las pruebas funcionales y de fallos de memoria de fases previas
siguen ejecutandose sin cambios en las implementaciones de las estructuras.
