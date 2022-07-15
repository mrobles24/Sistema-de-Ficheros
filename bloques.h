/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/
 
#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
 
#define BLOCKSIZE 1024 // bytes

// DEBUGS, poner a 0 para NO enseñar mensajes de depuración de dichos niveles
#define DEBUGN4 0
#define DEBUGN5 1
#define DEBUGN6 1
#define DEBUGN7 0
#define DEBUGN8 0
#define DEBUGN9 0
#define DEBUGN10 0
#define DEBUGN12 0
#define DEBUGN13 1
 
// ***** FUNCIONES NIVEL 1 ***** //
int bmount(const char *camino);
int bumount();
int bwrite(unsigned int nbloque, const void *buf);
int bread(unsigned int nbloque, void *buf);

// ***** FUNCIONES NIVEL 11 ***** //
void mi_signalSem();
void mi_waitSem();
