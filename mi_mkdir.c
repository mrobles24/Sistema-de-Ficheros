/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

int main (int argc, char **argv) {
    char *disco, *path;
    int permisos;

    //Comprueba los argumentos que sean correctos
    if(argc < 4) {
        fprintf(stderr, "Formato válido: ./mi_mkdir <disco> <permisos> </ruta> \n");
        return -1;
    }

    disco = argv[1];
    permisos = atoi(argv[2]);
    path = argv[3];

    //Comprueba que los permisos sea un número válido
    if(permisos < 0 || permisos > 7){
        fprintf(stderr, "Error: modo inválido: <<9>> \n");
        return -1;
    }

    if(bmount(disco) == -1){
        perror("Error");
        return -1;
    }

    //Crea el directorio
    int estado = mi_creat(path, permisos);

    //Comprueba si hay algún error buscando la entrada
    if(estado < 0){
        
        //Muestra el error de buscar entrada
        mostrar_error_buscar_entrada(estado);
        
        //Desmonta dispositivo
        if(bumount() == -1){
            perror("Error");
            return -1;
        }

        return -1;
    }

    //Desmonta dispositivo
    if(bumount() == -1){
        perror("Error");
        return -1;
    }

    return 0;
}