/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "semaforo_mutex_posix.h"
 
sem_t *initSem() {
   sem_t *sem;
 
   sem = sem_open(SEM_NAME, O_CREAT, S_IRWXU, SEM_INIT_VALUE);
   if (sem == SEM_FAILED) {
      return NULL;
   }
   return sem;
}
 
void waitSem(sem_t *sem) {
   sem_wait(sem);
}

void deleteSem() {
   sem_unlink(SEM_NAME);
}
 
void signalSem(sem_t *sem) {
   sem_post(sem);
}
 