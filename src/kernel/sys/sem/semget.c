#include <errno.h>
#include <sys/sem.h>

/**
 * @brief Crée un nouveau sémaphore avec la clé donnée.
 * 
 * @param key Clé unique pour le sémaphore.
 * 
 * @return L'indice du sémaphore créé, ou -1 en cas d'échec.
 */
int create(unsigned key) {
    // Parcourt la table pour trouver un emplacement libre.
    for (int i = 0; i < SEM_MAX; i++) {
        // Trouver le premier sémaphore invalide (espace vide dans la table).
        if (semtab[i].valid == SEM_INVALID) {
            // Initialiser le sémaphore.
            semtab[i].valid = SEM_VALID;
            semtab[i].key = key;
            semtab[i].value = 1; // Initialise la valeur par défaut du sémaphore.
            semtab[i].waiting = 0; // Aucun processus n'attend sur ce sémaphore pour l'instant.
            
            // Retourner l'indice du sémaphore créé.
            return i;
        }
    }
    
    // Tous les sémaphores sont utilisés.
    errno = ENOSPC; // Erreur : pas d'espace dans la table des sémaphores.
    return -1;
}

/**
 * @brief Récupère ou crée un sémaphore avec la clé donnée.
 * 
 * @param key Clé unique pour identifier le sémaphore.
 * 
 * @return L'indice du sémaphore existant ou nouvellement créé, ou -1 en cas d'échec.
 */
int semget(unsigned key) {
    // Parcourt la table pour chercher un sémaphore avec la clé donnée.
    for (int i = 0; i < SEM_MAX; i++) {
        // Si le sémaphore est valide et correspond à la clé.
        if (semtab[i].valid == SEM_VALID && semtab[i].key == key) {
            // Sémaphore trouvé, retourner son indice.
            return i;
        }
    }
    
    // Si aucun sémaphore avec cette clé n'existe, en créer un nouveau.
    return create(key);
}
