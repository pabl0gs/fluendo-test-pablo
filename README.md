# fluendo-test

## Tabla de Contenidos

- [Comentarios generales](#comments)
- [Estructura](#estructura)
- [Uso](#uso)

## Comentarios generales
Desde la honestidad, los requerimentos que se pedían en el Test eran demasiado complejos para mí. Por ello, hay algunos puntos que no cumplen los requerimientos:
- El programa no puede ser llamado usando gst-launch-0.10 filesrc location=file.txt.gz ! gzdec ! filesink location="file.txt", sino tal y como está explicado en la sección Uso.
- El procesamiento no se realiza correctamente. Si bien el archivo se descomprime, pasa a través de la pipeline y se guarda, no es legible a su salida. Por eso he decidido guardarlo como ".txt" automáticamente.

Otros comentarios:
- El proyecto no usa CMake u cualquier otro "montador de proyectos" automático. Por ello, para ahorrar tiempo y evitaros tener que crear un proyecto y compilarlo, he guardado el ejecutable en la carpeta "Test/Executable".

## Estructrura
El proyecto está dividido en varias carpetas:
- src, include: carpetas con el código fuente y headers files.
- Test: contiene a su vez una carpeta con archivos de prueba para descomprimir (Test_files), una carpeta donde guardar los resultados (Results), y una carpeta donde está guardado el ejecutable (Executable).

## Uso
En la carpeta "Executable": Proyecto_Fluendo.exe -orig <path-to-source-file.gz> -out <path-to-output-file.txt>\.

El código también implementa una función ¨usage()¨ a la que se puede llamar con -help: Proyecto_Fluendo.exe -help.
