#include <errno.h>
#include <sys/sem.h>
#include <nanvix/pm.h>

/**
 * @brief Effectue une opération atomique sur le sémaphore identifié par semid.
 * 
 * Si op <= 0 : Down (acquire)
 * Si op > 0  : Up (release)
 * 
 * @param semid Identifiant du sémaphore dans la table.
 * @param op    Opération à effectuer sur le sémaphore (négatif pour down, positif pour up).
 * 
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
PUBLIC int semop(int semid, int op) {
    // Vérifier la validité de l'ID du sémaphore.
    if (semid < 0 || semid >= SEM_MAX) {
        errno = EINVAL; // Erreur : sémaphore invalide.
        return -1;
    }

    struct sem_t *sem = &semtab[semid];

    // Vérifier si le sémaphore est valide.
    if (sem->valid != SEM_VALID) {
        errno = EINVAL; // Erreur : sémaphore non valide.
        return -1;
    }

    // Section critique : vous pouvez ajouter des mécanismes de verrouillage ici si nécessaire.

    if (op <= 0) { // Down (acquire)
        sem->value--; // Décrémenter la valeur du sémaphore.

        if (sem->value < 0) {
            // Le compteur est négatif : le processus doit se bloquer.
            sem->waiting_queue[sem->waiting] = curr_proc; // Ajouter le processus courant à la file d'attente.
            sem->waiting++;  // Incrémenter le nombre de processus en attente.
            sleep(sem->waiting_queue, sem->waiting); // Bloquer le processus courant.
        }

    } else { // Up (release)
        sem->value++; // Incrémenter la valeur du sémaphore.

        if (sem->value <= 0 && sem->waiting > 0) {
            // Si des processus sont en attente, les réveiller.
            wakeup(&sem->waiting_queue[0]); // Réveiller le premier processus en attente.
            
            // Déplacer les autres processus dans la file.
            for (int i = 0; i < sem->waiting - 1; i++) {
                sem->waiting_queue[i] = sem->waiting_queue[i + 1];
            }
            
            // Réduire le nombre de processus en attente.
            sem->waiting--;
        }
    }

    return 0; // Opération réussie.
}
