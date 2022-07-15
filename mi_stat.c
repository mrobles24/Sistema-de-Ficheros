/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

int main(int argc, char **argv) {
    struct STAT p_stat;
    char *nombre_dispositivo, *ruta;
    int mi_stat_err;

    //Comprobamos numero de argumentos
    if (argc != 3) {
        fprintf(stderr, "Formato válido: ./mi_stat <disco> </ruta>\n");
        return -1;
    }

    nombre_dispositivo = argv[1];
    ruta = argv[2];

    //Montamos dispositivo
    if (bmount(nombre_dispositivo) == 1) {
        perror("Error");
        return -1;
    }

    //Llamamos a la función mi_stat
    if ((mi_stat_err = mi_stat(ruta, &p_stat)) < 0) {
        mostrar_error_buscar_entrada(mi_stat_err);
    } else {

        //Imprimimos todos los datos necesarios
        fprintf(stderr,"tipo: %c\n", p_stat.tipo);
        fprintf(stderr,"permisos: %d\n", p_stat.permisos);
        
        struct tm *ts;
        char atime[80];
        char mtime[80];
        char ctime[80];
       
        ts = localtime(&p_stat.atime);
	    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	    ts = localtime(&p_stat.mtime);
	    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	    ts = localtime(&p_stat.ctime);
	    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts); 

        fprintf(stdout,"atime: %s \nctime: %s \nmtime: %s\n",atime,ctime,mtime);

        fprintf(stderr,"nlinks: %d\n", p_stat.nlinks);
        fprintf(stderr,"tamEnBytesLog: %d\n", p_stat.tamEnBytesLog);
        fprintf(stderr,"numBloquesOcupados: %d\n", p_stat.numBloquesOcupados);
    }
    
    //Desmontamos dispositivo
    if (bumount() == 1) {
        perror("Error");
        return -1;
    }

    return EXIT_SUCCESS;
}