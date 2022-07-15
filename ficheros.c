/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "ficheros.h"

struct inodo inodo;

// ***** FUNCIONES NIVEL 5 ***** //

/* mi_write_f:
    Escribe el contenido procedente de un buffer de memoria, buf_original, de tamaño nbytes, en un fichero/directorio 
*/
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
    unsigned int primerBL, ultimoBL, desp1, desp2, nbfisico;
    unsigned int nbytesEscritos = 0;
    char buf_bloque[BLOCKSIZE];

    //Leemos el inodo
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 2) == 2)
    {
        // Calculamos cuál va a ser el primer bloque lógico, pimerBL, donde hay que escribir
        primerBL = offset / BLOCKSIZE;

        // Calculamos cuál va a ser el último bloque lógico, ultimoBL, donde hay que escribir:
        ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

        // Calculamos el desplazamiento desp1 en el bloque para el offset:
        desp1 = offset % BLOCKSIZE;

        // Calculamos el desplazamiento desp2 en el bloque para ver donde llegan los nbytes escritos a partir del offset:
        desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        // Si solo cabe un bloque
        if (primerBL == ultimoBL)
        {
            // Entramos en S.C.
            mi_waitSem();

            // Obtenemos numero de bloq. fis.
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
            
            // Salimos de S.C.
            mi_signalSem();

            // Leemos ese bloque y lo almacenamos en su posicion
            bread(nbfisico, buf_bloque);
            memcpy(buf_bloque + desp1, buf_original, nbytes);

            // Escribimos en el bloque fisico
            nbytesEscritos += nbytes;
            bwrite(nbfisico, buf_bloque);
        }
        else
        {
            // Entramos en S.C.
            mi_waitSem();

            // Primer bloque lógico (BL8):
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);

            // Salimos de S.C.
            mi_signalSem();

            bread(nbfisico, buf_bloque);
            desp1 = offset % BLOCKSIZE;
            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
            nbytesEscritos += BLOCKSIZE - desp1;
            bwrite(nbfisico, buf_bloque);

            // Bloques intermedios
            for (int i = primerBL + 1; i < ultimoBL; i++)
            {
                // Entramos en S.C.
                mi_waitSem();

                nbfisico = traducir_bloque_inodo(ninodo, i, 1);

                // Salimos de S.C.
                mi_signalSem();

                bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
                nbytesEscritos += BLOCKSIZE;
            }

            //Último bloque lógico

            // Entramos en S.C.
            mi_waitSem();

            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
            
            // Salimos de S.C.
            mi_signalSem();

            bread(nbfisico, buf_bloque);
            desp2 = (offset + nbytes - 1) % BLOCKSIZE;
            memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
            nbytesEscritos += desp2 + 1;
            bwrite(nbfisico, buf_bloque);
        }

        // Entramos en S.C.
        mi_waitSem();

        // Actualizamos metadatos del inodo
        leer_inodo(ninodo, &inodo);
        if ((offset + nbytes) > inodo.tamEnBytesLog)
        {
            inodo.tamEnBytesLog = (offset + nbytes);
            inodo.ctime = time(NULL);
        }
        inodo.mtime = time(NULL);

        // Salvamos el inodo
        escribir_inodo(ninodo, inodo);

        // Salimos de S.C.
        mi_signalSem();
    }
    else
    {
        fprintf(stderr,"No tienes permisos para escribir \n");
    }
    return nbytesEscritos;
}

/* mi_read_f:
    Lee información de un fichero/directorio (correspondiente al nº de inodo, ninodo, pasado como argumento) 
    y la almacena en un buffer de memoria.
*/
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{

    struct inodo inodo;
    int primerBL, ultimoBL, desp1, desp2, nbfisico;
    int leidos = 0;

    leer_inodo(ninodo, &inodo);

    // Si se puede realizar la operación de lectura
    if ((inodo.permisos & 4) == 4)
    {

        // En caso de que quiera leer en una posición que no está dentro del EOF
        if (offset >= inodo.tamEnBytesLog)
        {
            return leidos;
        }

        // En caso de que pretenda leer más allá del EOF
        if ((offset + nbytes) >= inodo.tamEnBytesLog)
        {
            nbytes = inodo.tamEnBytesLog - offset;
        }

        // Calculamos BL y desplazamientos
        primerBL = offset / BLOCKSIZE;
        ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

        desp1 = offset % BLOCKSIZE;
        desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        unsigned char buf_bloque[BLOCKSIZE];

        // Si solo hay un bloque logico
        if (primerBL == ultimoBL)
        {
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);

            // Si hay un bloque físico, lo leemos y lo copiamos
            if (nbfisico != -1)
            {
                if (bread(nbfisico, buf_bloque) == 1)
                {
                    perror("Error");
                    return -1;
                }
                memcpy(buf_original, buf_bloque + desp1, nbytes);
            }
            leidos = nbytes;

            // En caso de que haya varios bloques logicos, los leemos todos
        }
        else
        {
            // Primer bloque logico
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);

            // Si hay un bloque físico, lo leemos y lo copiamos
            if (nbfisico != -1)
            {
                if (bread(nbfisico, buf_bloque) == 1)
                {
                    perror("Error");
                    return -1;
                }
                memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
            }
            leidos = BLOCKSIZE - desp1;

            // Iteramos para los otros bloques excepto el ultimo
            for (int i = primerBL + 1; i < ultimoBL; i++)
            {
                nbfisico = traducir_bloque_inodo(ninodo, i, 0);
                // Si hay un bloque físico, lo leemos y lo copiamos
                if (nbfisico != -1)
                {
                    if (bread(nbfisico, buf_bloque) == 1)
                    {
                        perror("Error");
                        return -1;
                    }
                    memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
                }
                leidos = leidos + BLOCKSIZE;
            }

            // Para el último bloque logico
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
            // Si hay un bloque físico, lo leemos y lo copiamos
            if (nbfisico != -1)
            {
                if (bread(nbfisico, buf_bloque) == 1)
                {
                    perror("Error");
                    return -1;
                }
                memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
            }
            leidos = leidos + desp2 + 1;
        }
    }
    else
    {
        fprintf(stderr, "Error, no hay permisos de lectura para el archivo\n");
        return -1;
    }

    // Entramos en S.C.
    mi_waitSem();

    // Actualizamos el atime del inodo
    leer_inodo(ninodo, &inodo);
    inodo.atime = time(NULL);
    escribir_inodo(ninodo, inodo);

    // Salimos de S.C.
    mi_signalSem();

    // Devolvemos bytes leidos
    return leidos;
}

/* mi_stat_f:
    Devuelve la metainformación de un fichero/directorio.
*/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    // Leemos el inodo 
    leer_inodo(ninodo, &inodo);
    // Guardamos su estado
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    return 0;
}

/* mi_chmod_f:
    Cambia los permisos de un fichero/directorio.
*/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    // Entramos en S.C.
    mi_waitSem();

    // Leemos el inodo
    leer_inodo(ninodo, &inodo);
    // Modificamos sus permisos
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    // Volvemos a escribir el inodo
    escribir_inodo(ninodo, inodo);
    
    // Salimos de S.C.
    mi_signalSem();

    return 0;
}

// ***** FUNCIONES NIVEL 6 ***** //

/* mi_truncar_f:
    Trunca un fichero/directorio (correspondiente al nº de inodo, ninodo, 
    pasado como argumento) a los bytes indicados como nbytes, liberando los bloques necesarios.
*/
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{
    struct inodo inodo;

    // Leemos el inodo
    if (leer_inodo(ninodo, &inodo) == 1)
    {
        perror("Error");
        return -1;
    }
    // Si el inodo tiene permisos de lectura
    if ((inodo.permisos & 2) == 2)
    {
        // En caso de sobrepasar el tamaño del fichero en bytes lógicos no se puede truncar
        if (inodo.tamEnBytesLog < nbytes)
        {
            perror("Error");
            return -1;
        }

        int primerBL;

        // Calculamos posición del Bloque logico
        if (nbytes % BLOCKSIZE == 0)
        {
            primerBL = nbytes / BLOCKSIZE;
        }
        else
        {
            primerBL = (nbytes / BLOCKSIZE) + 1;
        }

        // Liberamos bloques
        int bloquesLiberados = liberar_bloques_inodo(primerBL, &inodo);

        // Actualización de los datos del inodo
        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);

        // Truncamos inodo
        inodo.tamEnBytesLog = nbytes;

        // Resta de los bloques liberados
        inodo.numBloquesOcupados -= bloquesLiberados;

        // Escritura del inodo actualizado
        if (escribir_inodo(ninodo, inodo) == 1)
        {
            perror("Error");
            return -1;
        }
        // Devolución de la cantidad de bloques liberados
        fprintf(stderr, "Bloques liberados: %d\n", bloquesLiberados);

        return bloquesLiberados;
    }
    else
    {
        perror("Error");
        return -1;
    }
}