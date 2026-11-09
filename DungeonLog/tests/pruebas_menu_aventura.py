"""Pruebas de integracion de consola. Python solo se usa durante las pruebas."""
from pathlib import Path
import subprocess

EXE = Path(__file__).resolve().parents[1] / "build" / "DungeonLog.exe"


def ejecutar(entrada, argumentos=("--aventura",)):
    resultado = subprocess.run(
        [str(EXE), *argumentos], input=entrada, text=True, encoding="ascii",
        capture_output=True, timeout=10, check=True
    )
    return resultado.stdout


# Mapa inicial: solo la sala 0 debe estar identificada.
salida = ejecutar("7\n0\n")
assert "[@0]" in salida and "[ ?]" in salida
assert "Relicario del Alba" not in salida and "Galeria de los Huesos" not in salida
assert "Norte" not in salida.split("Salidas:", 1)[1].splitlines()[0]
assert "\x1b" not in salida
print("OK mapa: identidades desconocidas ocultas, sala actual y salidas")

# Direccion bloqueada, retroceso vacio, encuentro y recogida no repetible.
salida = ejecutar("1\n1\n2\n1\n2\n3\n3\n6\n7\n0\n")
assert "No hay salida alli" in salida and "No hay pasos que desandar" in salida
assert "Rata osaria" in salida and "Arrastra pequenos huesos" in salida
assert "No hay un objeto disponible" in salida
assert "[@1]" in salida and "[ 0]" in salida
print("OK consola: bloqueos, encuentros, recoleccion y mapa descubierto")

# Ruta 0,1,4,7,8. Recoger, intentar descartar, ordenar, consultar camino y regresar.
ida = "1\n2\n1\n3\n1\n3\n1\n2\n"
final = "3\n4\n2\n5\n4\n1\n5\n2\n2\n2\n2\n"
salida = ejecutar(ida + final)
assert "No puedes descartarlo" in salida
assert "Corazon del Alba" in salida and "CAMINO DE REGRESO" in salida
assert salida.count("VICTORIA - EL ALBA REGRESA") == 1
assert "Memoria de las estructuras liberada" in salida
assert "\x1b" not in salida
assert "VICTORIA - EL ALBA REGRESA" in ejecutar(ida + final, ("--aventura", "--color"))
print("OK partida completa: reliquia protegida, inventario, pila y victoria ASCII")

# Mismo proceso: fabricar reliquia en mesa, salir, iniciar aventura nueva.
salida = ejecutar("2\n1\n1\n5\n0\n1\n4\n0\n7\n0\n0\n", ())
aventura = salida.split("La entrada es tu unico refugio", 1)[1]
assert "La bolsa esta vacia" in aventura and "[@0]" in aventura
assert "VICTORIA" not in aventura and "Relicario del Alba" not in aventura
print("OK selector: mesa y aventura aisladas dentro del mismo proceso")

for entrada in ("", "1\n", "4\n", "4\n2\n", "texto\n0\n"):
    assert "Memoria de las estructuras liberada" in ejecutar(entrada)
print("OK aventura: entradas invalidas y EOF en submenus")
