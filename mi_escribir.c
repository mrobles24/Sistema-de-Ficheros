/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

int main(int argc, char **argv) {

    int mi_write_error;

    //En caso de que haya más o menos parámetros de lo correspondido
    if (argc != 5) {
        fprintf(stderr, "Formato válido: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return -1;
    }

    fprintf(stderr,"Longitud de texto: %lu \n", strlen(argv[3]));

    //Comprobamos si es un fichero 
    if(argv[2][strlen(argv[2]) - 1] == '/') {
        fprintf(stderr, "Error: %s es un directorio.\n", argv[2]);
        return -1;
    }

    //Montamos el dispositivo
    if (bmount(argv[1]) == -1) {
        perror("Error");
        return -1;
    }

    //Comprobamos que tiene permisos de escritura
    if ((mi_write_error = mi_write(argv[2], argv[3], atoi(argv[4]), strlen(argv[3]))) < 0) {
        mostrar_error_buscar_entrada(mi_write_error);
        fprintf(stderr,"Bytes escritos: 0\n");
    } else { //Mostramos la cantidad de bytes
        fprintf(stderr,"Bytes escritos: %d\n", mi_write_error);
    }

    //Desmontamos el dispositivo
    if (bumount() == -1) {
        perror("Error");
        return -1;
    }

    return 0;
}