------ ESP ------
# Sistema de Ficheros

Implementación de un sistema de ficheros (basado en el ext2 de Unix) en un disco virtual (fichero), 
con módulo de gestión de ficheros del sistema operativo e interacción de procesos de usuarios.

El proyecto está compuesto por los siguientes elementos:
- Un conjunto de bibliotecas, divididas en niveles, que dan la funcionalidad a las primitivas para acceder 
    al sistema de ficheros y gestionarlo.
- Un programa mi_mkfs para crear, con la ayuda de dichas bibliotecas, el sistema de ficheros en el disco.
- El sistema de ficheros (SF) está realmente contenido en un fichero (simulando un dispositivo de memoria secundaria), 
    al que llamamos disco y tiene la siguiente estructura:
  - Un conjunto de programas para operar por consola, entre ellos uno que muestra el contenido de un fichero por pantalla (mi_cat),
    uno que cambia sus permisos de acceso (mi_chmod), uno uno que crea enlaces físicos (mi_ln), uno que lista directorios (mi_ls), 
    uno que borra ficheros y directorios (mi_rm), y uno que muestra la metainformación de un fichero (mi_stat).
     - Su funcionamiento es similar al funcionamiento básico de los correspondientes programas cat, chmod, ln, ls, rm/rmdir, 
      stat de GNU/Linux.
  - Un programa simulador encargado de crear unos procesos de prueba que accedan de forma concurrente al sistema de ficheros 
    (utilizando un semáforo binario) de modo que se pueda comprobar el correcto funcionamiento de las citadas bibliotecas.
    
<br>
Para más información sobre el proyecto, contactar con el autor.

<br>
<br>
<br>

------ ENG ------
# File System

Implementation of a file system (based on Unix's ext2) in a virtual disc, with it's own operating system file manager module and user proces interactions.

The project is composed of the following items:
- A collection of libraries, split into different levels which grant primitive acces to the file system to also be able to manage it.
- A program called mi_mkfs used to create the file system on the disc, with help from those primitives mentioned before.
- The file system (SF) is actually contained in a file (simulating a memory storage device) called disco, which has the following structure:
  - A collection of programmes used to operate through console; one that shows the contents of a file (mi_cat), another one that can change the file's access 
  permisions (mi_chmod), another one that can create physical links (mi_ln), one that shows all directories (mi_ls), one that removes files
  and directories (mi_rm) and one that shows the meta-information of a given file (mi_stat).
      - It works similarly to the basic functions of cat, chmod, ln, ls, rm/rmdir, stat programmes found in GNU/Linux.
  - A simulator program used for creating a few test processes that can concurrently access the file system (using a binary semaphore),
  to ensure the correctness of the already mentioned libraries.
  
 <br>
For more info about the project, contact the autor.

  
