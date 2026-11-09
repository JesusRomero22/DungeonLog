# Ejecutar desde cualquier carpeta: powershell -File tests/probar.ps1
# GCC y Python 3 (para pruebas de consola) deben estar en PATH.
param([string]$Python = "python")
$ErrorActionPreference = "Stop"
Push-Location (Split-Path $PSScriptRoot -Parent)
try {
    New-Item -ItemType Directory -Force build | Out-Null
    $opciones = @("-std=c11", "-Wall", "-Wextra", "-pedantic", "-Iinclude")
    $fuentes = @(Get-ChildItem src/*.c | ForEach-Object { $_.FullName })
    & gcc @opciones @fuentes -o build/DungeonLog.exe
    if ($LASTEXITCODE -ne 0) { throw "Fallo compilacion del juego" }

    $estructuras = @("src/inventario.c", "src/historial.c", "src/bestiario.c")
    & gcc @opciones tests/pruebas_estructuras.c @estructuras src/mazmorra.c -o build/pruebas_estructuras.exe
    if ($LASTEXITCODE -ne 0) { throw "Fallo compilacion de pruebas de estructuras" }
    & ./build/pruebas_estructuras.exe
    if ($LASTEXITCODE -ne 0) { throw "Fallaron pruebas de estructuras" }

    # La sustitucion de malloc/free afecta solo estos objetos de PRUEBA.
    $objetos = @()
    foreach ($fuente in $estructuras) {
        $nombre = [IO.Path]::GetFileNameWithoutExtension($fuente)
        $objeto = "build/prueba_$nombre.o"
        & gcc @opciones -Dmalloc=prueba_malloc -Dfree=prueba_free -c $fuente -o $objeto
        if ($LASTEXITCODE -ne 0) { throw "Fallo compilacion de $fuente" }
        $objetos += $objeto
    }
    & gcc @opciones tests/pruebas_aventura.c src/aventura.c src/juego.c src/entrada.c src/interfaz.c src/arte_ascii.c src/mazmorra.c @objetos -o build/pruebas_aventura.exe
    if ($LASTEXITCODE -ne 0) { throw "Fallo compilacion de pruebas de aventura" }
    & ./build/pruebas_aventura.exe
    if ($LASTEXITCODE -ne 0) { throw "Fallaron pruebas de aventura" }

    & $Python tests/pruebas_menu.py
    if ($LASTEXITCODE -ne 0) { throw "Fallaron pruebas del menu de estructuras" }
    & $Python tests/pruebas_menu_aventura.py
    if ($LASTEXITCODE -ne 0) { throw "Fallaron pruebas del menu de aventura" }
    & $Python tests/pruebas_presentacion.py
    if ($LASTEXITCODE -ne 0) { throw "Fallaron pruebas de presentacion" }
} finally {
    Pop-Location
}
