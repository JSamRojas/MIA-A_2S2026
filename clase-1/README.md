# Instalación de C++ en Ubuntu/Debian

Guía rápida para instalar el compilador de C++ (g++), verificar que quedó bien instalado, y crear/compilar/ejecutar un "Hola Mundo".

## 1. Actualizar los repositorios

```bash
sudo apt update
```

## 2. Instalar el compilador de C++

La forma más común es instalar `build-essential`, que incluye `g++`, `gcc`, `make` y otras herramientas necesarias para compilar.

```bash
sudo apt install build-essential -y
```

## 3. Verificar la instalación

Revisa la versión instalada de g++:

```bash
g++ --version
```

También se puede verificar `gcc` y `make` (si instalaste `build-essential`):

```bash
gcc --version
make --version
```

Si estos comandos muestran un número de versión (por ejemplo, `g++ (Ubuntu 13.2.0-...)`), la instalación fue exitosa.

## 4. Crear el archivo "Hola Mundo"

Crear una carpeta de trabajo (opcional) y el archivo fuente:

```bash
mkdir -p ~/cpp_test && cd ~/cpp_test
nano hola_mundo.cpp
```

Pegar el siguiente contenido en el archivo:

```cpp
#include <iostream>

int main() {
    std::cout << "¡Hola, Mundo!" << std::endl;
    return 0;
}
```

Guardar y cerrar (`Ctrl + O`, `Enter`, `Ctrl + X` si usas `nano`).

## 5. Compilar el programa

```bash
g++ hola_mundo.cpp -o hola_mundo
```

Esto genera un archivo ejecutable llamado `hola_mundo` en la misma carpeta.

## 6. Ejecutar el programa

```bash
./hola_mundo
```

Se debería ver en la terminal:

```
¡Hola, Mundo!
```

## 7. (Opcional) Comprobar todo en un solo paso

Se puede compilar y ejecutar de una vez con:

```bash
g++ hola_mundo.cpp -o hola_mundo && ./hola_mundo
```

---

### Resumen de comandos

```bash
sudo apt update
sudo apt install build-essential -y
g++ --version
mkdir -p ~/cpp_test && cd ~/cpp_test
nano hola_mundo.cpp   # pegar el código y guardar
g++ hola_mundo.cpp -o hola_mundo
./hola_mundo
```