# Librería de soporte a la aplicación DNS

## Introducción
Se proporcionaba una librería escrita en C++ para la creación de aplicaciones
que interactúen con servidores DNS.

Los principales puntos de entrada al API se encuentran en la clase `Message`, en
partiocular:
* el método `setQuestion` permite crear una consulta y
* la función de utilidad `makeMessage` permite procesar un *array* de bytes
recibido de un servidor DNS y obtener una nueva instancia de un `Message` para
poder inspeccionarlo cómodamente.

## Compilación
El proyecto usa la utilidad `cmake` para ser compilado, aunque **el usuario
puede modificar, a su conveniencia, el sistema de compilación.** Para crear la
librería basta con hacer:

```bash
mkdir build # Crea el directorio destino de la compilación
cd build
cmake ../ # Configura la compilación
make # Compila la librería
```
