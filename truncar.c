/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "ficheros.h"

int main(int argc, char **argv)
{

    // Si el numero de argumentos no es valido, lo anunciamos
    if (argc != 4)
    {
        fprintf(stderr, "Formato válido:  truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return -1;
    }

    // Montamos el dispositivo 
    if (bmount(argv[1]) == 1)
    {
        perror("Error");
        return -1;
    }

    // Recuperamos numero de bytes y de inodo
    int nbytes = atoi(argv[3]);
    int ninodo = atoi(argv[2]);

    // Si el inodo esta vacio, lo liberamos
    if (nbytes == 0)
    {
        if (liberar_inodo(ninodo) == 1)
        {
            return -1;
        }
    }
    // Si no esta vacio, lo truncamos
    else
    {
        if (mi_truncar_f(ninodo, nbytes) == 1)
        {
            perror("Error");
            return -1;
        }
    }

    // Leemos el estado del inodo
    struct STAT stat;
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    mi_stat_f(ninodo, &stat);
    
    // Anunciamos sus datos por pantalla
    fprintf(stderr,"\nDATOS DEL INODO\n");
    fprintf(stderr,"tipo=%c\n", stat.tipo);
    fprintf(stderr,"permisos=%d\n", stat.permisos);

    ts = localtime(&stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    fprintf(stderr, "atime: %s \nctime: %s \nmtime: %s\n", atime, ctime, mtime);

    // Comprobamos el estado del tamaño en bytes y de los bloques ocupados
    fprintf(stderr,"tamEnBytesLog=%d\n", stat.tamEnBytesLog);
    fprintf(stderr,"numBloquesOcupados=%d\n", stat.numBloquesOcupados);

    // Desmontamos el dispositivo
    if (bumount() == 1)
    {   
        return -1;
    }
}