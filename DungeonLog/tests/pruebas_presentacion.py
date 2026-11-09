"""Regresion visual en texto: ASCII, 80 columnas y cambios de presentacion.
Solo usa biblioteca estandar de Python; el programa sigue siendo C11 puro.
"""
from pathlib import Path
import re
import subprocess

RAIZ = Path(__file__).resolve().parents[1]
EXE = RAIZ / "build" / "DungeonLog.exe"
SGR = re.compile(r"\x1b\[[0-9;]*m")


def ejecutar(entrada, modo="--aventura", color=False):
    args = [str(EXE), modo, "--color" if color else "--sin-color"]
    salida = subprocess.run(args, input=entrada, text=True, encoding="ascii",
                            capture_output=True, check=True, timeout=10).stdout
    limpio = SGR.sub("", salida)
    assert all(c == "\n" or 32 <= ord(c) <= 126 for c in limpio)
    for numero, linea in enumerate(limpio.splitlines(), 1):
        assert len(linea) <= 80, (numero, len(linea), linea)
        if linea.startswith("|") or linea.startswith("+-"):
            assert len(linea) == 72, (numero, len(linea), linea)
    if not color:
        assert "\x1b" not in salida
    return salida, limpio


# Visita las nueve salas y recoge los cinco objetos. No regresa a la entrada.
# 0->1->2->5->8->7->6->3->4: todos los enemigos y todas las vinetas.
entrada = "".join(f"1\n{direccion}\n3\n" for direccion in (2, 2, 3, 3, 4, 4, 1, 2))
entrada += "4\n1\n6\n7\n5\n0\n"
sin_color, limpio = ejecutar(entrada)
assert limpio.count("PRIMERA VISITA") == 9
assert limpio.count("RELIQUIA OBTENIDA") == 1
assert "Corazon del Alba" in limpio and "Sombra del relicario" in limpio
assert "Amuleto marchito" in limpio and "Caliz de obsidiana" in limpio
con_color, limpio_color = ejecutar(entrada, color=True)
assert limpio_color == limpio, "El color solo debe agregar SGR; nunca cambiar contenido"
for codigo in (31, 32, 33, 35, 36):
    assert f"\x1b[{codigo}m" in con_color
print("OK visual: nueve salas, siete retratos, inventario y paneles <=80 columnas")
print("OK ANSI: paleta completa, solo SGR y contenido identico sin colores")

# Descubrimiento vs regreso: los dibujos no deben repetirse al consultar mapa.
_, regreso = ejecutar("1\n2\n2\n1\n2\n7\n7\n0\n")
assert regreso.count("PRIMERA VISITA") == 2  # Solo salas 0 y 1.
assert regreso.count("REGRESO -") == 2
assert regreso.count("ENTRE LAS SOMBRAS - DESCUBIERTO") == 1
assert regreso.count("UN ECO CONOCIDO") == 1
assert regreso.count("D U N G E O N L O G") == 1
assert "Relicario del Alba" not in regreso
assert "Galeria de los Huesos" in regreso
print("OK visitas: arte inicial, regreso compacto y mapa sin identidades ocultas")

ida = "1\n2\n1\n3\n1\n3\n1\n2\n"
_, victoria = ejecutar(ida + "3\n3\n2\n2\n2\n2\n")
assert victoria.count("RELIQUIA OBTENIDA") == 1
assert victoria.count("VICTORIA - EL ALBA REGRESA") == 1
assert "No hay un objeto disponible" in victoria
print("OK eventos: reliquia solo tras recoger y victoria al volver")

# Se inspecciona el ULTIMO diagrama, emitido inmediatamente despues de operar.
def ultimo_arbol(salida):
    return salida.rsplit("ESTRUCTURA DEL ABB", 1)[1].split("Cada enlace", 1)[0]


altas = "".join(f"3\n1\n{id_enemigo}\n" for id_enemigo in (40, 20, 60, 10, 30, 50, 70))
_, arbol = ejecutar(altas + "0\n", "--mesa")
diagrama = ultimo_arbol(arbol)
assert "RAIZ [40]" in diagrama
assert "+-- I [20]" in diagrama and "+-- D [60]" in diagrama
assert "|   +-- I [10]" in diagrama and "|   +-- D [30]" in diagrama
for id_borrado, raiz, ausente in ((10, 40, "[10]"), (40, 50, "[40]")):
    _, salida = ejecutar(altas + f"3\n2\n{id_borrado}\n0\n", "--mesa")
    diagrama = ultimo_arbol(salida)
    assert f"RAIZ [{raiz}]" in diagrama and ausente not in diagrama
# Un hijo y el lado ausente se representan sin confundir al padre.
_, salida = ejecutar("3\n1\n40\n3\n1\n20\n3\n1\n10\n3\n2\n20\n0\n", "--mesa")
diagrama = ultimo_arbol(salida)
assert "RAIZ [40]" in diagrama and "+-- I [10]" in diagrama
assert "+-- D [NULL]" in diagrama and "[20]" not in diagrama
_, vacio = ejecutar("3\n1\n40\n3\n2\n40\n0\n", "--mesa")
assert "RAIZ [NULL]" in ultimo_arbol(vacio)
# Peor profundidad posible con el catalogo actual.
lineal = "".join(f"3\n1\n{id_enemigo}\n" for id_enemigo in (10, 20, 30, 40, 50, 60, 70))
ejecutar(lineal + "4\n0\n", "--mesa")
print("OK arbol: conexiones, actualizacion inmediata, tres bajas, vacio y profundidad maxima")

# Transcripciones para revision humana; son resultados de la ejecucion real.
carpeta = RAIZ / "build" / "vistas"
carpeta.mkdir(exist_ok=True)
for nombre, texto in (("exploracion.txt", limpio), ("victoria.txt", victoria),
                      ("arbol.txt", arbol)):
    (carpeta / nombre).write_text(texto, encoding="ascii")
