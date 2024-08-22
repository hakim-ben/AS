#include "sys/sem.h"
#include <errno.h>
/**
 * @brief Détruit un sémaphore et réveille tous les processus en attente.
 * 
 * @param semaphore_to_destroy Pointeur vers le sémaphore à détruire.
 */
void destroy(struct sem_t *semaphore_to_destroy) {
    // Marquer le sémaphore comme invalide.
    semaphore_to_destroy->valid = SEM_INVALID;
    // Réinitialiser les autres champs.
    semaphore_to_destroy->value = 0;
    semaphore_to_destroy->waiting = 0;

    // Optionnel : Réinitialiser la liste des processus en attente
    for (int i = 0; i < WAIT_MAX; i++) {
        semaphore_to_destroy->waiting_queue[i] = NULL; // Valeur par défaut indiquant aucun processus en attente
    }
}

/**
 * @brief Exécute une commande sur un sémaphore spécifié.
 * 
 * @param semid Indice du sémaphore dans la table des sémaphores.
 * @param cmd Commande à exécuter (GETVAL, SETVAL, IPC_RMID).
 * @param val Valeur à utiliser pour certaines commandes comme SETVAL.
 * 
 * @return La valeur du sémaphore ou 0 en cas de succès, -1 en cas d'échec.
 */
int semctl(int semid, int cmd, int val) {
    // Récupère le sémaphore à partir de l'ID (indice dans la table des sémaphores).
    struct sem_t *sem = &semtab[semid];

    // Vérifier si le sémaphore est valide.
    if (sem->valid != SEM_VALID) {
        errno = EINVAL; // Erreur : sémaphore invalide.
        return -1;
    }

    switch (cmd) {
    case GETVAL:
        // Retourner la valeur actuelle du sémaphore.
        return sem->value;

    case SETVAL:
        // Définir la valeur du sémaphore.
        sem->value = val;
        return 0;

    case IPC_RMID:
        // Détruire le sémaphore.
        destroy(sem);
        return 0;

    default:
        errno = EINVAL; // Erreur : commande invalide.
        return -1;
    }
}
