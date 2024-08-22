#include <nanvix/const.h> 
#include <nanvix/config.h>
#include <sys/sem.h> 

PUBLIC struct sem_t semtab[SEM_MAX]; /**<Semaphore table. */

void sem_init() {
    int i;
    for (i = 0; i < SEM_MAX; i++) {
        semtab[i].valid = SEM_VALID;
    }
}
