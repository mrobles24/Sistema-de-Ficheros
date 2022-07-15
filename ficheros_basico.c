/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "ficheros_basico.h"

struct superbloque SB;

// ***** FUNCIONES NIVEL 2 ***** //

/* tamMB:
    Calcula el tamaño en bloques necesario para el mapa de bits.
*/
int tamMB(unsigned int nbloques)
{
    int resto = (nbloques / 8) % BLOCKSIZE;
    if (resto != 0)
    {
        return ((nbloques / 8) / BLOCKSIZE) + 1;
    }

    return ((nbloques / 8) / BLOCKSIZE);
}

/* tamAI:
    Calcula el tamaño en bloques del array de inodos.
*/
int tamAI(unsigned int ninodos)
{
    int resto = (ninodos * INODOSIZE) % BLOCKSIZE;
    if (resto != 0)
    {
        return ((ninodos * INODOSIZE) / BLOCKSIZE) + 1;
    }
    return (ninodos * INODOSIZE) / BLOCKSIZE;
}

/* initSB:
    Inicializa los datos del superbloque mediante una variable de tipo
    struct superbloque.
*/
int initSB(unsigned int nbloques, unsigned int ninodos)
{
    // Inicializamos variables
    SB.posPrimerBloqueMB = posSB + tamSB; // posSB = 0, tamSB = 1
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;

    // Al final sobreescribimos el Superbloque
    return bwrite(posSB, &SB);
}

/* initMB:
    Inicializa el mapa de bits poniendo a 1 los bits que representan los metadatos.
    (FORMA EFICIENTE SIN USAR escribir_bit())
*/
int initMB()
{
    // Buffer que contiene bloques de MB
    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB, '\0', BLOCKSIZE);
    // Buffer que contiene bloques completos a 1 de MB
    unsigned char bufferBloques[BLOCKSIZE];
    memset(bufferBloques, 255, BLOCKSIZE);
    // Buffer que contiene bytes de 1 bloque del MB
    unsigned char bufferBytes[BLOCKSIZE];
    memset(bufferBytes, '\0', BLOCKSIZE);

    // Leemos superbloque
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Calculamos tamaño entero de metadatos y los escribimos con 1 en el MB
    int tamMetadatos = SB.posPrimerBloqueDatos;
    int bytesMetadatos = tamMetadatos / 8;
    int tamBloques = bytesMetadatos / 1024;

    // Si los metadatos ocupan mas de 1 bloque en el MB, rellenamos los anteriores con 1
    if (tamBloques > 0)
    {
        for (int i = SB.posPrimerBloqueMB; i < SB.posPrimerBloqueMB + tamBloques; i++)
        {
            if (bwrite(i, bufferBloques) == -1)
            {
                perror("Error");
                return -1;
            }
        }
    }

    // Rellenamos con 1 los bytes necesarios del bloque siguiente
    for (int j = 0; j < bytesMetadatos; j++)
    {
        bufferBytes[j] = 255;
    }

    // Calculamos tamaño extra exacto de bits de metadatos y los escribimos con 1 en el MB
    int exactBytesMetadatos = tamMetadatos % 8;

    int result = 0;
    for (int i = 7; i > 7 - exactBytesMetadatos; i--)
    { // Calculamos el int con potencias de 2
        result = result + pow(2, i);
    }

    bufferBytes[bytesMetadatos] = result;

    // En el resto del bloque, escribimos 0
    for (int k = bytesMetadatos + 1; k < BLOCKSIZE; k++)
    {
        bufferBytes[k] = 0;
    }

    // Escribimos el bloque de metadatos en el MB
    if (bwrite(SB.posPrimerBloqueMB + tamBloques, bufferBytes) == -1)
    {
        perror("Error");
        return -1;
    }

    // Escribimos 0's en el resto de MB
    for (int i = SB.posPrimerBloqueMB + tamBloques + 1; i <= SB.posUltimoBloqueMB; i++)
    {
        if (bwrite(i, bufferMB) == -1)
        {
            perror("Error");
            return -1;
        }
    }

    // Restamos tamaño de metadatos de bloques libres y salvamos SB
    SB.cantBloquesLibres = SB.cantBloquesLibres - tamMetadatos;
    if (bwrite(posSB, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    return 1;
}

/* initAI:
    Esta función se encargará de inicializar la lista de inodos libres.
*/
int initAI()
{
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    // LEER SUPERBLOQUE
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    unsigned int contInodos = SB.posPrimerInodoLibre + 1;

    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    { // para cada bloque del AI

        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        { // para cada inodo del AI

            inodos[j].tipo = 'l'; // libre
            if (contInodos < SB.totInodos)
            {                                               // si no hemos llegado al último inodo
                inodos[j].punterosDirectos[0] = contInodos; // enlazamos con el siguiente
                contInodos++;
            }
            else
            { // si hemos llegado al último inodo
                inodos[j].punterosDirectos[0] = UINT_MAX;
                // hay que salir del bucle, el último bloque no tiene por qué estar completo !!!
            }
        }
        if (bwrite(i, inodos) == -1)
        {
            perror("Error");
            return -1;
        }
    }
    return 0;
}

// ***** FUNCIONES NIVEL 3 ***** //

/* escribir_bit:
    Esta función escribe el valor indicado por el parámetro bit: 0 (libre) ó 1 (ocupado)
    en un determinado bit del MB que representa el bloque nbloque. La utilizaremos cada
    vez que necesitemos reservar o liberar un bloque.
*/
int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    // LEER SUPERBLOQUE
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Obtenemos el byte del dispositivo que contiene el bit deseado y el bloque físico
    // absoluto que lo contiene
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Ahora que ya tenemos ubicado el bit en el dispositivo, leemos con bread() el bloque físico
    // que lo contiene y cargamos el contenido en un buffer
    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB, '\0', BLOCKSIZE);
    bread(nbloqueabs, &bufferMB);
    // Realizamos el módulo de posbyte para obtener su posición en el buffer
    posbyte = posbyte % BLOCKSIZE;

    // Mascara para localizar al bit
    unsigned char mascara = 128; // 10000000
    mascara >>= posbit;          // desplazamiento de bits a la derecha

    // Escribimos el bit correspondiente segun sea 0 o 1
    if (bit == 0)
    {
        bufferMB[posbyte] &= ~mascara;
    }
    else if (bit == 1)
    {
        bufferMB[posbyte] |= mascara;
    }

    // Escibimos el buffer con el bit actualizado en el bloque correspondiente
    bwrite(nbloqueabs, bufferMB);

    return 0;
}

/* leer_bit:
    Lee un determinado bit del MB y devuelve el valor del bit leído.
*/
char leer_bit(unsigned int nbloque)
{

    // LEER SUPERBLOQUE
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Obtenemos el byte del dispositivo que contiene el bit deseado y el bloque físico
    // absoluto que lo contiene
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    // Ahora que ya tenemos ubicado el bit en el dispositivo, leemos con bread() el bloque físico
    // que lo contiene y cargamos el contenido en un buffer
    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB, '\0', BLOCKSIZE);
    bread(nbloqueabs, &bufferMB);
    // Realizamos el módulo de posbyte para obtener su posición en el buffer
    posbyte = posbyte % BLOCKSIZE;

    // Mascara para localizar al bit
    unsigned char mascara = 128;  // 10000000
    mascara >>= posbit;           // desplazamiento de bits a la derecha
    mascara &= bufferMB[posbyte]; // operador AND para bits
    mascara >>= (7 - posbit);     // desplazamiento de bits a la derecha

    // Devolvemos la máscara, que será o 0 o 1
    return mascara;
}

/* reservar_bloque:
   Encuentra el primer bloque libre, consultando el MB (primer bit a 0), lo ocupa
   (poniendo el correspondiente bit a 1 con la ayuda de la función escribir_bit())
   y devuelve su posición.
*/
int reservar_bloque()
{
    // LEER SUPERBLOQUE
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    if (SB.cantBloquesLibres > 0)
    { // Si hay bloques libres
        int nbloqueabs;
        int posbyte;
        unsigned char bufferMB[BLOCKSIZE];
        unsigned char bufferAux[BLOCKSIZE];
        memset(bufferAux, 255, BLOCKSIZE);

        for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; i++)
        { // Iteramos sobre los bloques del MB

            bread(i, &bufferMB); // Cargamos cada bloque en un buffer

            if ((memcmp(bufferMB, bufferAux, BLOCKSIZE) != 0))
            { // Si el bloque es diferente a 255 (tiene algun 0)

                nbloqueabs = i; // Actualizamos nbloqueabs

                for (int j = 0; j < BLOCKSIZE; j++)
                { // Iteramos sobre el bloque para encontrar primer byte con 0

                    if (bufferMB[j] != 255)
                    { // Si el byte es diferente a 255 (tiene algun 0)

                        posbyte = j;   // Actualizamos posbyte
                        j = BLOCKSIZE; // Salimos del bucle, byte encontrado
                    }
                }

                i = SB.posUltimoBloqueMB + 1; // Salimos del bucle, bloque encontrado
            }
        }

        // Una vez encontrado el nbloqueabs y el posbyte de este que contiene el 0, buscamos el bit
        unsigned char mascara = 128;
        int posbit = 0;

        // Encontrar el primer bit a 0 en ese byte
        while (bufferMB[posbyte] & mascara)
        {
            bufferMB[posbyte] <<= 1; // desplazamiento de bits a la izquierda
            posbit++;
        }

        // Finalmente determinamos el nbloque fisico que podemos reservar
        int nbloque = ((nbloqueabs - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;

        // Ocupamos el bloque (decimos que no esta libre)
        escribir_bit(nbloque, 1);

        // Actualizamos y salvamos el superbloque
        SB.cantBloquesLibres--;
        if (bwrite(posSB, &SB) == -1)
        {
            perror("Error");
            return -1;
        }

        // Escribimos 0's en zona de datos
        unsigned char bufferVacio[BLOCKSIZE];
        memset(bufferVacio, '\0', BLOCKSIZE);
        bwrite(nbloque, &bufferVacio);

        return nbloque;
    }
    return -1;
}

/* liberar_bloque:
   Libera un bloque determinado (con la ayuda de la función escribir_bit()).
*/
int liberar_bloque(unsigned int nbloque)
{
    // Ponemos a 0 en el MB del bloque correspondiente
    escribir_bit(nbloque, 0);
    // Actualizamos y salvamos el superbloque
    SB.cantBloquesLibres++;
    if (bwrite(posSB, &SB) == -1)
    {
        perror("Error");
        return -1;
    }
    return nbloque;
}

/* escribir_inodo:
   Escribe el contenido de una variable de tipo struct inodo en un determinado inodo del array de inodos, inodos.
*/
int escribir_inodo(unsigned int ninodo, struct inodo inodo)
{
    // LEER SUPERBLOQUE
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }
    // Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado.
    unsigned int posBloqueInodo = SB.posPrimerBloqueAI + ((INODOSIZE * ninodo) / BLOCKSIZE);
    // Lo leemos de su posición absoluta del dispositivo empleando como buffer de lectura un array de inodos,
    // del tamaño de la cantidad de inodos que caben en un bloque
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    bread(posBloqueInodo, inodos);
    // Una vez que tenemos el bloque en memoria escribimos el inodo en el lugar correspondiente del array
    memcpy(&inodos[ninodo % (BLOCKSIZE / INODOSIZE)], &inodo, INODOSIZE);
    if (bwrite(posBloqueInodo, inodos) != -1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/* leer_inodo:
   Lee un determinado inodo del array de inodos para volcarlo en una variable de tipo struct inodo pasada por referencia.
*/
int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    // LEER SUPERBLOQUE
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }
    // Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado.
    unsigned int posBloqueInodo = SB.posPrimerBloqueAI + ((INODOSIZE * ninodo) / BLOCKSIZE);
    // Lo leemos de su posición absoluta del dispositivo empleando como buffer de lectura un array de inodos,
    // del tamaño de la cantidad de inodos que caben en un bloque
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    if (bread(posBloqueInodo, inodos) == -1)
    {
        perror("Error");
        return -1;
    }
    // El inodo solicitado está en la posición ninodo%(BLOCKSIZE/INODOSIZE) del buffer
    *inodo = inodos[ninodo % (BLOCKSIZE / INODOSIZE)];
    return 0;
}

/* reservar_inodo:
   Encuentra el primer inodo libre (dato almacenado en el superbloque), lo reserva (con la ayuda de la función escribir_inodo()),
   devuelve su número y actualiza la lista enlazada de inodos libres.
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    // LEER SUPERBLOQUE
    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }
    // Comprobar si hay inodos libres y si no hay inodos libres indicar error y salir.
    if (SB.cantInodosLibres == 0)
    {
        perror("Error");
        return -1;
    }
    /* Primeramente actualizar la lista enlazada de inodos libres de tal manera que el superbloque apunte al siguiente de la lista.
       Tendremos la precaución de guardar en una variable auxiliar posInodoReservado cual era el primer inodo libre,
       ya que éste es el que hemos de devolver.*/
    struct inodo inodoReservado;
    int posInodoReservado = SB.posPrimerInodoLibre;
    leer_inodo(posInodoReservado, &inodoReservado);
    SB.posPrimerInodoLibre = inodoReservado.punterosDirectos[0]; // punterosDirectos[0] apunta a la lista de inodos libres

    // A continuación inicializamos todos los campos del inodo al que apuntaba inicialmente el superbloque
    inodoReservado.tipo = tipo;
    inodoReservado.permisos = permisos;
    inodoReservado.nlinks = 1;
    inodoReservado.tamEnBytesLog = 0;
    inodoReservado.atime = time(NULL);
    inodoReservado.ctime = time(NULL);
    inodoReservado.mtime = time(NULL);
    inodoReservado.numBloquesOcupados = 0;
    memset(inodoReservado.punterosDirectos, 0, sizeof(inodoReservado.punterosDirectos));
    memset(inodoReservado.punterosIndirectos, 0, sizeof(inodoReservado.punterosIndirectos));

    // Decrementar la cantidad de inodos libres, y reescribir el superbloque.
    SB.cantInodosLibres = SB.cantInodosLibres - 1;

    if (bwrite(posSB, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Utilizar la función escribir_inodo() para escribir el inodo inicializado en la posición
    // del que era el primer inodo libre, posInodoReservado.
    escribir_inodo(posInodoReservado, inodoReservado);

    return posInodoReservado;
}

// ***** FUNCIONES NIVEL 4 ***** //

/* obtener_nRangoBL:
   Obtiene el rango de punteros en el que se sitúa el bloque lógico que buscamos (0:D, 1:I0, 2:I1, 3:I2)
   y obtenemos además la dirección almacenada en el puntero correspondiente del inodo, ptr
*/
int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
{
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }
    else if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }
    else if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }
    else
    {
        *ptr = 0;
        perror("Bloque lógico fuera de rango");
        return -1;
    }
}

/* obtener_indice:
  Generaliza la obtención de los índices de los bloques de punteros.
*/
int obtener_indice(unsigned int nblogico, int nivel_punteros)
{
    if (nblogico < DIRECTOS)
    {
        return nblogico; // ej nblogico=8
    }
    else if (nblogico < INDIRECTOS0)
    {
        return nblogico - DIRECTOS; // ej nblogico=204
    }
    else if (nblogico < INDIRECTOS1)
    { // ej nblogico=30.004
        if (nivel_punteros == 2)
        {
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }
    else if (nblogico < INDIRECTOS2)
    { // ej nblogico=400.004
        if (nivel_punteros == 3)
        {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }
        else if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;
        }
    }
    return -1;
}

/* traducir_bloque_inodo:
  Esta función se encarga de obtener el nº de bloque físico correspondiente a un bloque lógico determinado del inodo indicado.
  (VERSIÓN OPTIMIZADA)
*/
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, unsigned char reservar)
{
    struct inodo inodo;
    unsigned int ptr, ptr_ant;
    int salvar_inodo, nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];

    leer_inodo(ninodo, &inodo);
    ptr = 0;
    ptr_ant = 0;
    salvar_inodo = 0;
    nRangoBL = obtener_nRangoBL(&inodo, nblogico, &ptr); // 0:D, 1:I0, 2:I1, 3:I2
    nivel_punteros = nRangoBL;                           // el nivel_punteros +alto es el que cuelga del inodo
    while (nivel_punteros > 0)
    { // iterar para cada nivel de punteros indirectos
        if (ptr == 0)
        { // no cuelgan bloques de punteros
            if (reservar == 0)
            {
                return -1; // bloque inexistente -> no imprimir nada por pantalla!!!
            }
            else
            { // reservar bloques de punteros y crear enlaces desde el  inodo hasta el bloque de datos
                salvar_inodo = 1;
                ptr = reservar_bloque(); // de punteros
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL); // fecha actual
                if (nivel_punteros == nRangoBL)
                { // el bloque cuelga directamente del inodo
                    inodo.punterosIndirectos[nRangoBL - 1] = ptr;
                    if (DEBUGN4)
                    {
                        fprintf(stderr, "[traducir_bloque_inodo()→ inodo.punterosIndirectos[%d] = %d (reservado BF %d para BL %d)]\n", nRangoBL - 1, inodo.punterosIndirectos[nRangoBL - 1], inodo.punterosIndirectos[nRangoBL - 1], nblogico);
                    }
                }
                else
                { // el bloque cuelga de otro bloque de punteros
                    buffer[indice] = ptr;
                    if (DEBUGN4)
                    {
                        fprintf(stderr, "[traducir_bloque_inodo()→ punteros_nivel%d[%d] = %d (reservado BF %d para BL %d)]\n", nivel_punteros, indice, buffer[indice], buffer[indice], nblogico);
                    }
                    bwrite(ptr_ant, buffer); // salvamos en el dispositivo el buffer de punteros modificado
                }
                memset(buffer, 0, BLOCKSIZE); // ponemos a 0 todos los punteros del buffer
            }
        }
        else
        {
            bread(ptr, buffer); // leemos del dispositivo el bloque de punteros ya existente
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;        // guardamos el puntero actual
        ptr = buffer[indice]; // y lo desplazamos al siguiente nivel
        nivel_punteros--;
    } // al salir de este bucle ya estamos al nivel de datos

    if (ptr == 0)
    { // no existe bloque de datos
        if (reservar == 0)
        {
            return -1; // error lectura ∄ bloque
        }
        else
        {
            salvar_inodo = 1;
            ptr = reservar_bloque(); // de datos
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if (nRangoBL == 0)
            {
                inodo.punterosDirectos[nblogico] = ptr;
                if (DEBUGN4)
                {
                    fprintf(stderr, "[traducir_bloque_inodo()→ inodo.punterosDirectos[%d] = %d (reservado BF %d para BL %d)]\n", nblogico, inodo.punterosDirectos[nblogico], inodo.punterosDirectos[nblogico], nblogico);
                }
            }
            else
            {
                buffer[indice] = ptr;
                if (DEBUGN4)
                {
                    fprintf(stderr, "[traducir_bloque_inodo()→ punteros_nivel%d[%d] = %d (reservado BF %d para BL %d)]\n", nivel_punteros, indice, buffer[indice], buffer[indice], nblogico);
                }
                bwrite(ptr_ant, buffer); // salvamos en el dispositivo el buffer de punteros modificado
            }
        }
    }

    if (salvar_inodo == 1)
    {
        escribir_inodo(ninodo, inodo); // sólo si lo hemos actualizado
    }
    return ptr; // nº de bloque físico correspondiente al bloque de datos lógico, nblogico
}

// ***** FUNCIONES NIVEL 6 ***** //

/* liberar_inodo:
  Libera un inodo de tal forma que el inodo pasa a la cabeza de la lista de inodos libres
  y libera todos los bloques de datos que estaba ocupando más todos los bloques indices.
*/
int liberar_inodo(unsigned int ninodo)
{
    struct inodo inodo;
    struct superbloque SB;

    // Leemos el inodo
    if (leer_inodo(ninodo, &inodo) == 1)
    {
        perror("Error");
        return -1;
    }

    // Liberamos todos los bloques del inodo
    int bloquesLibres = liberar_bloques_inodo(0, &inodo);
    if (bloquesLibres == -1)
    {
        perror("Error");
        return -1;
    }
    
    if (DEBUGN6)
    {
        fprintf(stderr, "Bloques liberados: %d", bloquesLibres);
    }

    // Restamos la cantidad de bloques liberados a los previamente ocupados
    inodo.numBloquesOcupados -= bloquesLibres;

    // Marcamos el inodo como libre y tamaño 0
    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;

    // Leemos el superbloque
    if (bread(posSB, &SB) == 1)
    {
        perror("Error");
        return -1;
    }

    // Apuntamos al primer inodo libre y actualizamos con el que acabamos de liberar
    inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = ninodo;

    // Incrementamos inodos libres
    SB.cantInodosLibres++;

    // Escribimos en el inodo
    if (escribir_inodo(ninodo, inodo) == 1)
    {
        perror("Error");
        return -1;
    }

    // Salvamos el superbloque
    if (bwrite(posSB, &SB) == 1)
    {
        perror("Error");
        return -1;
    }

    return ninodo;
}

/* liberar_bloques_inodo:
  Libera todos los bloques ocupados (con la ayuda de la función liberar_bloque())
  a partir del bloque lógico indicado por el argumento primerBL.
*/

int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{
    unsigned int nivel_punteros, indice, ptr, nBL, ultimoBL;
    unsigned int bloques_punteros[3][NPUNTEROS];
    unsigned char bufAux_punteros[BLOCKSIZE];
    int nRangoBL;
    int ptr_nivel[3];
    int indices[3];
    int liberados = 0;
    int breads = 0;
    int bwrites = 0;

    // Si el fichero esta vacio
    if (inodo->tamEnBytesLog == 0)
    {
        return 0;
    }

    // Obtenemos ultimo bloque logico del inodo
    if ((inodo->tamEnBytesLog % BLOCKSIZE) == 0)
    {
        ultimoBL = (inodo->tamEnBytesLog / BLOCKSIZE) - 1;
    }
    else
    {
        ultimoBL = (inodo->tamEnBytesLog / BLOCKSIZE);
    }

    memset(bufAux_punteros, 0, BLOCKSIZE);
    ptr = 0;
    if (DEBUGN6)
    {
        fprintf(stderr, "[liberar_bloques_inodo()→ primer BL %d, ultimo BL %d]\n", primerBL, ultimoBL);
    }
    // Recorremos bloques logicos
    for (nBL = primerBL; nBL <= ultimoBL; nBL++)
    {
        // 0:D, 1:I0, 2:I1, 3:I2
        nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr);
        if (nRangoBL < 0)
        {
            perror("Error");
            return -1;
        }

        nivel_punteros = nRangoBL;

        // Cuelgan bloques de punteros
        while (ptr > 0 && nivel_punteros > 0)
        {
            indice = obtener_indice(nBL, nivel_punteros);
            if (indice == 0 || nBL == primerBL)
            {

                if (bread(ptr, bloques_punteros[nivel_punteros - 1]) == 1)
                {
                    perror("Error");
                    return -1;
                }
                breads++;
            }

            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }

        // Si existe bloque de datos
        if (ptr > 0)
        {
            liberar_bloque(ptr);
            liberados++;

            // Si es un puntero Directo
            if (nRangoBL == 0)
            {
                inodo->punterosDirectos[nBL] = 0;
            }
            else
            {
                nivel_punteros = 1;
                if (DEBUGN6)
                {
                    fprintf(stderr, "[liberar_bloques_inodo()→ liberado BF %d de datos para BL %d)]\n", ptr, nBL);
                }
                while (nivel_punteros <= nRangoBL)
                {
                    indice = indices[nivel_punteros - 1];
                    bloques_punteros[nivel_punteros - 1][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros - 1];

                    // Si no cuelgan más bloques ocupados, liberamos bloque de punteros
                    if (memcmp(bloques_punteros[nivel_punteros - 1], bufAux_punteros, BLOCKSIZE) == 0)
                    {

                        liberar_bloque(ptr);
                        liberados++;
                        if (DEBUGN6)
                        {
                            fprintf(stderr, "[liberar_bloques_inodo()→ liberado BF  %d nivel_punteros%d correspondiente BL %d)]\n", ptr, nivel_punteros, nBL);
                        }
                        
                        if (nivel_punteros == nRangoBL)
                        {
                            inodo->punterosIndirectos[nRangoBL - 1] = 0;
                        }

                        nivel_punteros++;
                    }
                    else
                    {
                        bwrite(ptr, bloques_punteros[nivel_punteros - 1]);
                        bwrites++;
                        // Colocamos margen de error
                        nivel_punteros = nRangoBL + 1;
                    }
                }
            }
        }
    }
    if (DEBUGN6)
    {
        fprintf(stderr, "[liberar_bloques_inodo()→ total bloques liberados:%d, total_breads:%d, total_bwrites:%d]\n", liberados,breads,bwrites);
    }
    return liberados;
}
