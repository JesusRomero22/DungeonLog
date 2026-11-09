"""Pruebas reproducibles de consola. Solo usa la biblioteca estandar de Python.
Ejecutar desde la raiz: python tests/pruebas_menu.py
El juego en C no depende de Python.
"""
from pathlib import Path
import subprocess

EXE = Path(__file__).resolve().parents[1] / "build" / "DungeonLog.exe"


def ejecutar(entrada, argumentos=()):
    resultado = subprocess.run(
        [str(EXE), "--mesa", *argumentos], input=entrada, text=True,
        encoding="ascii", capture_output=True, timeout=10, check=True
    )
    return resultado.stdout


salida = ejecutar("texto\n\n2x\n999999999999999999999999999999\n" + "9" * 150 + "\n0\n")
assert "Escribe un numero entero" in salida
assert "Opcion invalida" in salida
assert "Entrada demasiado larga" in salida
assert "\x1b" not in salida
print("OK entradas: texto, vacia, mezcla, desbordamiento y linea larga")

for entrada in ("", "1\n", "1\n1\n", "2\n1\n", "3\n1\n"):
    assert "Memoria de las estructuras liberada" in ejecutar(entrada)
print("OK EOF: salida limpia en menu principal y submenus")

# Agregar dos pociones, buscar, ordenar y quitar una; apilar y desapilar.
entrada = (
    "1\n1\n1\n" * 2  # Agregar dos unidades del objeto 1.
    + "1\n3\n1\n"     # Buscar objeto 1.
    + "1\n4\n"         # Ordenar inventario.
    + "1\n2\n1\n"     # Quitar una unidad.
    + "2\n1\n0\n"     # Apilar sala 0.
    + "2\n1\n4\n"     # Apilar sala 4.
    + "2\n2\n" * 3     # Extraer 4, extraer 0, intentar en vacio.
    + "0\n"
)
salida = ejecutar(entrada)
assert "Pocion de ceniza x2" in salida
assert "Extraida: [4]" in salida and "Extraida: [0]" in salida
assert "Pila vacia" in salida
print("OK menu: inventario y pila")

entrada = "".join(f"3\n1\n{id_enemigo}\n" for id_enemigo in (40, 20, 60, 10, 30, 50, 70))
salida = ejecutar(entrada + "3\n2\n40\n3\n3\n40\n3\n5\n4\n0\n")
assert "Registro eliminado" in salida and "ID no encontrado" in salida
assert "Relicario del Alba" in salida
assert "\x1b" not in salida
assert "\x1b[35m" in ejecutar("0\n", ("--color",))
assert "\x1b" not in ejecutar("0\n", ("--sin-color",))
print("OK menu: bestiario, atlas, ANSI opcional y modo sin color")
