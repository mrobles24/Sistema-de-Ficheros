******* PRACTICA SISTEMAS OPERATIVOS II ******* 
**************** CURSO 2022 *******************

**** AUTORES *****
* Miquel Robles  *
* Eloy Rafael    *
* Soufyane Youbi *
******************
GRUPO:   INODO   *
******************

** Descripción de la Practica **

La práctica implementará un sistema de ficheros (basado en el ext2 de Unix) en un disco virtual (fichero), 
el módulo de gestión de ficheros del sistema operativo y la interacción de procesos de usuarios.

Estructura general
La práctica estará compuesta por los siguientes elementos:
- Un conjunto de bibliotecas, divididas en niveles, que darán la funcionalidad a las primitivas para acceder 
    al sistema de ficheros y gestionarlo.
- Un programa mi_mkfs para crear, con la ayuda de dichas bibliotecas, el sistema de ficheros en el disco.
- El sistema de ficheros (SF) estará realmente contenido en un fichero (simulando un dispositivo de memoria secundaria), 
    al que llamaremos disco y tiene la siguiente estructura:
- Un conjunto de programas para operar por consola, entre ellos uno que muestre el contenido de un fichero por pantalla (mi_cat),
    uno que cambie sus permisos de acceso (mi_chmod), uno uno que cree enlaces físicos (mi_ln), uno que liste directorios (mi_ls), 
    uno que borre ficheros y directorios (mi_rm), y uno que muestre la metainformación de un fichero (mi_stat).
- Su funcionamiento es similar al funcionamiento básico de los correspondientes programas cat, chmod, ln, ls, rm/rmdir, 
    stat de GNU/Linux.
- Un programa simulador encargado de crear unos procesos de prueba que accedan de forma concurrente al sistema de ficheros 
    (utilizando un semáforo binario) de modo que se pueda comprobar el correcto funcionamiento de las citadas bibliotecas.



** Entrega 2 **

Ficheros Adjuntos:

- El programas ficticio: leer_sf.c 
- El programa  mi_mkfs.c
- Los siguientes ficheros de funciones de la librería del sistema de ficheros (y sus cabeceras correspondientes .h): bloques.c, 
  ficheros_basico.c, ficheros.c y directorios.c
- Todos los programas correspondientes a los comandos implementados (como mínimo mi_mkdir.c, mi_ls.c, mi_chmod.c, mi_stat.c, mi_escribir.c, mi_cat.c, mi_link.c, y mi_rm.c).
- El makefile necesario para compilar todos los programas 
- Un fichero README.txt con las observaciones que consideréis (mejoras realizadas, restricciones del programa, sintaxis específica, y 
  los nombres de los miembros del grupo).
- El scripte2.sh



*** MEJORAS REALIZADAS ***

- Colores e información en mi_ls
- Caché de ultima lectura y escritura


