/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

static struct UltimaEntrada UltimaEntradaEscritura;
static struct UltimaEntrada UltimaEntradaLectura;

// ***** FUNCIONES NIVEL 7 ***** //

/* extraer_camino:
    Dado un camino, separa su contenido en dos; la parte inicial y la parte final
    del camino, es decir, el primer directorio o fichero y el resto del camino
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    const char *posBarra;

    // Comprobamos que comience por "/"
    if (camino[0] != '/')
    {
        return -1;
    }

    // Buscamos la posicion de la barra
    camino++;
    posBarra = strchr(camino, '/');

    // Si hay barra hay un directorio, si no hay es un fichero
    if (posBarra != NULL)
    {
        // Hacemos la separación y señalamos el tipo
        strcpy(final, posBarra);
        strncpy(inicial, camino, posBarra - camino);
        inicial[posBarra - camino] = 0;
        *tipo = 'd';
    }
    else
    {
        // Hacemos la separación y señalamos el tipo
        strcpy(inicial, camino);
        strcpy(final, "");
        *tipo = 'f';
    }

    return EXIT_SUCCESS;
}

/* buscar_entrada:
    Esta función nos buscará una determinada entrada entre las entradas del inodo correspondiente a su directorio padre.
*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
{

    struct superbloque SB;

    // Leemos superbloque
    if (bread(0, &SB) == -1)
    {
        return -1;
    }

    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial) + 1];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    // Si es directorio raiz
    if (strcmp(camino_parcial, "/") == 0)
    {
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }

    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial) + 1);

    if (extraer_camino(camino_parcial, inicial, final, &tipo) == -1)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

    if (DEBUGN7)
    {
        fprintf(stderr, "[buscar_entrada()→ inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
    }

    // Buscamos entrada cuyo nombre se encuentra en inicial
    leer_inodo(*p_inodo_dir, &inodo_dir);

    if ((inodo_dir.permisos & 4) != 4)
    {
        if (DEBUGN7)
        {
            fprintf(stderr, "[buscar_entrada()→ El inodo %d no tiene permisos de lectura]\n", *p_inodo_dir);
        }
        return ERROR_PERMISO_LECTURA;
    }

    // Calculamos cantidad de entradas que contiene el inodo
    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada); // CALCULAR NUMERO DE ENTRADAS

    num_entrada_inodo = 0;
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    if (cant_entradas_inodo > 0)
    {

        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == -1)
        {
            return -1;
        }

        while ((num_entrada_inodo < cant_entradas_inodo) && (strcmp(inicial, entrada.nombre) != 0))
        {
            num_entrada_inodo++;

            // Leemos siguiente entrada
            memset(entrada.nombre, 0, sizeof(entrada.nombre));
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == -1)
            {
                return -1;
            }
        }
    }

    // Si la entrada no existe
    if (strcmp(inicial, entrada.nombre) != 0)
    {
        switch (reservar)
        {
        case 0: // Modo consulta
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
        case 1: // Modo escritura
            // No permitir escritura al ser fichero
            if (inodo_dir.tipo == 'f')
            {
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            // Comprobar permisos escritura
            if ((inodo_dir.permisos & 2) != 2)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd')
                {
                    if (strcmp(final, "/") == 0)
                    {
                        // Reservamos inodo como directorio y lo asignamos a la entrada
                        entrada.ninodo = reservar_inodo('d', permisos);
                        if (DEBUGN7)
                        {
                            fprintf(stderr, "[buscar_entrada()→ reservado inodo %d tipo d con permisos %d para %s]\n", entrada.ninodo, permisos, entrada.nombre);
                        }
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    // Reservamos inodo como fichero y lo asignamos a la entrada
                    entrada.ninodo = reservar_inodo('f', permisos);
                    if (DEBUGN7)
                    {
                        fprintf(stderr, "[buscar_entrada()→ reservado inodo %d tipo f con permisos %d para %s]\n", entrada.ninodo, permisos, entrada.nombre);
                    }
                }

                // Escribimos entrada en directorio padre
                if (mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == -1)
                {
                    if (entrada.ninodo != -1)
                        liberar_inodo(entrada.ninodo);

                    return -1;
                }
                else
                {
                    if (DEBUGN7)
                    {
                        fprintf(stderr, "[buscar_entrada()→ Creada entrada: %s, %d]\n", entrada.nombre, entrada.ninodo);
                    }
                }
            }
        }
    }

    // Si hemos llegado al final del camino
    if ((strcmp(final, "/") == 0) || (strcmp(final, "") == 0))
    {
        // Modo escritura
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        // Asignamos valores
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
        return 0;
    }
    else
    {
        *p_inodo_dir = entrada.ninodo;
        // Recursividad
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }

    return 0;
}

/* mostrar_error_buscar_entrada:
    Funcion que nos hace prints de los errores encontrados en buscar_entrada.
*/
void mostrar_error_buscar_entrada(int error)
{
    switch (error)
    {
    case -1:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -2:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -3:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break;
    case -6:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -7:
        fprintf(stderr, "Error: No es un directrio.\n");
        break;
    }
}

// ***** FUNCIONES NIVEL 8 ***** //

/* mi_creat:
    Función de la capa de directorios que crea un fichero/directorio y su entrada de directorio.
*/
int mi_creat(const char *camino, unsigned char permisos)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    mi_waitSem();
    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);
    mi_signalSem();
    return error;
}

/* mi_dir:
    Pone el contenido del directorio en un buffer de memoria y devuelve el número de entradas.
*/
int mi_dir(const char *camino, char *buffer)
{

    struct inodo inodo;
    unsigned int p_inodo_dir = 0, p_inodo = 0, p_entrada = 0, numEnt;
    int error;
    char tamBytes[16], tipo[2];

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
        return error;

    if (leer_inodo(p_inodo, &inodo) == -1)
        return -1;

    if (inodo.tipo != 'd')
        return -1;

    if ((inodo.permisos & 4) != 4)
        return -1; // Comprobamos el permiso de lectura que tiene el inodo.

    // Guardamos la cantidad de entradas
    numEnt = inodo.tamEnBytesLog / sizeof(struct entrada);
    // Utilizamos un buffer para que no haya que acceder al dispositivo cada vez que haya que leer una entrada
    struct entrada buffer_entradas[numEnt];

    // Lectura de la entrada
    if (mi_read_f(p_inodo, &buffer_entradas, 0, sizeof(struct entrada) * numEnt) == -1)
        return -1;

    strcpy(buffer, "");

    for (int i = 0; i < numEnt; i++)
    {

        // Lectura inodo
        if (leer_inodo(buffer_entradas[i].ninodo, &inodo) == -1)
            return -1;

        if (inodo.tipo == 'd')
        {
            strcat(buffer, "\x1b[32m");
        }
        else
        {
            strcat(buffer, "\x1b[33m");
        }

        // Concatenamos su nombre al buffer
        sprintf(tipo, "%c", inodo.tipo);
        strcat(buffer, tipo);
        strcat(buffer, "\t\t");

        // Concatenamos permisos
        if ((inodo.permisos & 4) == 4)
        {
            strcat(buffer, "r");
        }
        else
        {
            strcat(buffer, "-");
        }

        if ((inodo.permisos & 2) == 2)
        {
            strcat(buffer, "w");
        }
        else
        {
            strcat(buffer, "-");
        }

        if ((inodo.permisos & 1) == 1)
        {
            strcat(buffer, "x");
        }
        else
        {
            strcat(buffer, "-");
        }

        strcat(buffer, "\t");

        // Concatenamos tiempos
        struct tm *ts;
        char mtime[80];
        ts = localtime(&inodo.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        strcat(buffer, mtime);

        // Devolvemos el número de carácteres escritos
        sprintf(tamBytes, "\t\t%d", inodo.tamEnBytesLog);
        strcat(buffer, tamBytes);
        strcat(buffer, "\t\t");
        strcat(buffer, buffer_entradas[i].nombre);
        strcat(buffer, "\033[0m");
        strcat(buffer, "\n");
    }

    // Devolvemos el número de entradas
    return numEnt;
}

/* mi_chmod:
    Busca la entrada *camino con buscar_entrada() para obtener el nº de inodo, si existe, llama
    a mi_chmod_f de la capa de ficheros.
*/
int mi_chmod(const char *camino, unsigned char permisos)
{
    unsigned int p_inodo_dir = 0, p_inodo = 0, p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0)
    {
        return error;
    }

    return mi_chmod_f(p_inodo, permisos);
}

/* mi_stat:
    Busca la entrada *camino con buscar_entrada() para obtener el p_inodo, si existe, llama
    a mi_stat_f de la capa de ficheros.
*/
int mi_stat(const char *camino, struct STAT *p_stat)
{
    unsigned int p_inodo_dir = 0, p_inodo = 0, p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        return error;
    }
    fprintf(stderr, "nº de inodo: %d\n", p_inodo);
    return mi_stat_f(p_inodo, p_stat);
}

// ***** FUNCIONES NIVEL 9 ***** //

/* mi_write:
    Buscamos la entrada camino con la caché o con buscar_entrada() para obtener el p_inodo. Si existe,
    llama a mi_write_f de la capa de ficheros.
*/
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
{
    unsigned int p_inodo_dir = 0, p_inodo = 0, p_entrada = 0;
    int bytesEscritos, error;

    // Comprobamos escritura sobre el mismo inodo
    if (strcmp(camino, UltimaEntradaEscritura.camino) == 0)
    {
        // Cogemos ultima entrada
        p_inodo = UltimaEntradaEscritura.p_inodo;

        if (DEBUGN9)
        {
            fprintf(stderr, "\n\x1b[31m[mi_write() → Utilizamos la caché de escritura en vez de llamar a buscar_entrada()]\x1b[0m\n");
        }
    }
    else
    {
        // Si no es la última entrada, la buscamos
        if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6)) < 0)
        {
            return error;
        }
        // Actualizamos ultima entrada
        UltimaEntradaEscritura.p_inodo = p_inodo;
        strcpy(UltimaEntradaEscritura.camino, camino);

        if (DEBUGN9)
        {
            fprintf(stderr, "\n\x1b[31m[mi_write() → Actualizamos la caché de escritura]\x1b[0m\n");
        }
    }

    if ((bytesEscritos = mi_write_f(p_inodo, buf, offset, nbytes)) == -1)
    {
        return -1;
    }

    return bytesEscritos;
}

/* mi_read:
    Buscamos la entrada camino con la caché o con buscar_entrada() para obtener el p_inodo. Si existe,
    llama a mi_read_f de la capa de ficheros.
*/
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
{
    int bytesLeidos, error;
    unsigned int p_inodo_dir = 0, p_inodo = 0, p_entrada = 0;

    // Comprobamos lectura sobre el mismo inodo
    if (strcmp(camino, UltimaEntradaLectura.camino) == 0)
    {
        // Cogemos ultima entrada
        p_inodo = UltimaEntradaLectura.p_inodo;

        if (DEBUGN9)
        {
            fprintf(stderr, "\n\x1b[31m[mi_read() → Utilizamos la caché de lectura en vez de llamar a buscar_entrada()]\x1b[0m\n");
        }
    }
    else
    {
        // Si no es la última entrada, la buscamos
        if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
        {
            return error;
        }
        // Actualizamos ultima entrada
        UltimaEntradaLectura.p_inodo = p_inodo;
        strcpy(UltimaEntradaLectura.camino, camino);

        if (DEBUGN9)
        {
            fprintf(stderr, "\n\x1b[31m[mi_read() → Actualizamos la caché de lectura]\x1b[0m\n");
        }
    }

    if ((bytesLeidos = mi_read_f(p_inodo, buf, offset, nbytes)) == -1)
        return -1;
    return bytesLeidos;
}

// ***** FUNCIONES NIVEL 10 ***** //

/* mi_link:
    Crea el enlace de una entrada de directorio camino2 al inodo especificado por otra entrada de directorio camino1.
*/
int mi_link(const char *camino1, const char *camino2)
{
    mi_waitSem();
    unsigned int p_inodo_dir1 = 0, p_inodo_dir2 = 0, p_inodo1 = 0, p_inodo2 = 0, p_entrada1 = 0, p_entrada2 = 0;
    int error;

    struct inodo inodo;

    // Comprobación de que el camino1 existe obteniendo inodo
    if ((error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 4)) < 0)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en buscar_entrada para camino1\n");
        }

        return error;
    }

    // Leemos inodo
    if (leer_inodo(p_inodo1, &inodo) < 0)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en leer_inodo\n");
        }
        return -1;
    }

    // Comprobamos que sea tipo fichero
    if (inodo.tipo != 'f')
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error, no es un fichero\n");
        }
        return -1;
    }

    // Comprueba que tenga permisos de lectura
    if ((inodo.permisos & 4) == 0)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error, inodo asociado al fichero no tiene permisos de lectura\n");
        }
        return -1;
    }

    // Comprobación de que la entrada de camino2 no existe y la creamos con permisos 6
    if ((error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6)) < 0)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error, la entrada para camino2 ya existe\n");
        }
        return error;
    }

    struct entrada entrada2;

    if (mi_read_f(p_inodo_dir2, &entrada2, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) == 1)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en mi_read_f\n");
        }
        return -1;
    }

    entrada2.ninodo = p_inodo1;

    // Escribimos la entrada modificada en p_inodo_dir2
    if (mi_write_f(p_inodo_dir2, &entrada2, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) < 0)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en mi_write_f\n");
        }
        return -1;
    }

    // Liberamos inodo asociado a la entrada
    if (liberar_inodo(p_inodo2) == EXIT_FAILURE)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en liberar inodo\n");
        }
        return -1;
    }

    // Incrementamos cantidad de enlaces
    inodo.nlinks++;
    // Actualizamos ctime
    inodo.ctime = time(NULL);
    // Salvamos inodo
    int resultado;
    resultado = escribir_inodo(p_inodo1, inodo);

    mi_signalSem();

    return resultado;
}

/* mi_unlink:
    Borra la entrada de directorio especificada y, en caso de que fuera el último enlace existente,
    borra el propio fichero/directorio.
*/
int mi_unlink(const char *camino)
{
    mi_waitSem();
    unsigned int p_inodo_dir = 0, p_inodo = 0, p_entrada = 0;
    struct inodo inodo;
    struct entrada ultima_entrada;
    int error, nEntradas;

    // Comprobamos que la entrada camino existe y obtenemos su número de entrada
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en buscar entrada\n");
        }
        mi_signalSem();
        return error;
    }

    if (leer_inodo(p_inodo, &inodo) == -1)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en leer_inodo\n");
        }
        mi_signalSem();
        return -1;
    }

    // Si se trata de un directorio y no está vacío salimos de la función, porque no se puede borrar
    if (inodo.tipo == 'd' && inodo.tamEnBytesLog > 0)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error: El directorio %s no está vacio. \n", camino);
        }
        mi_signalSem();
        return -1;
    }

    // Lectura del inodo asociado al directorio
    if (leer_inodo(p_inodo_dir, &inodo) == -1)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en leer_inodo\n");
        }
        mi_signalSem();
        return -1;
    }

    nEntradas = inodo.tamEnBytesLog / sizeof(struct entrada);

    // Si no es la última entrada
    if (p_entrada != nEntradas - 1)
    {
        // Lectura de la última entrada
        if (mi_read_f(p_inodo_dir, &ultima_entrada, (nEntradas - 1) * sizeof(struct entrada), sizeof(struct entrada)) == -1)
        {
            if (DEBUGN10)
            {
                fprintf(stderr, "Error en mi_read\n");
            }
            mi_signalSem();
            return -1;
        }
        // Escribimos en la posición de entrada que queremos eliminar
        if (mi_write_f(p_inodo_dir, &ultima_entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) == -1)
        {
            if (DEBUGN10)
            {
                fprintf(stderr, "Error en mi_write\n");
            }
            mi_signalSem();
            return -1;
        }
    }

    // Truncamos el inodo
    if (mi_truncar_f(p_inodo_dir, inodo.tamEnBytesLog - sizeof(struct entrada)) == -1)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en mi_truncar\n");
        }
        mi_signalSem();
        return -1;
    }

    // Leemos el inodo asociado a la entrada eliminada
    if (leer_inodo(p_inodo, &inodo) == -1)
    {
        if (DEBUGN10)
        {
            fprintf(stderr, "Error en leer_inodo\n");
        }
        mi_signalSem();
        return -1;
    }

    // Decrementamos el número de enlaces
    inodo.nlinks--;

    int aux;

    // Si no quedan enlaces liberamos el inodo
    if (inodo.nlinks == 0)
    {
        if ((aux = liberar_inodo(p_inodo)) == -1)
        {
            mi_signalSem();
            return -1;
        }
        else
        {
            mi_signalSem();
            return aux;
        }
    }

    // Actualizamos ctime
    inodo.ctime = time(NULL);

    // Salvamos inodo
    if ((aux = escribir_inodo(p_inodo, inodo)) == -1)
    {
        mi_signalSem();
        return -1;
    }
    else
    {
        mi_signalSem();
        return aux;
    }

    mi_signalSem();
    return 0;
}
