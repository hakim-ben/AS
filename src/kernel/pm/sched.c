/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h> 
#include <nanvix/klib.h>
 
PUBLIC sched_type_t current_scheduler = SCHED_RANDOM;  // Par défaut, l'ordonnanceur par priorité

/**
 * @brief Schedules a process to execution.
 *
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 *
 * @param proc Process to be resumed.
 *
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}




/// @brief 		Ordonnanceur par priorité. 
/// @details 	L'ordonnanceur par priorité choisit le processus prêt avec la plus haute priorité. 
/// @return 	Le prochain processus à exécuter. 

PRIVATE struct process * __priorityScheduling()
{
    struct process *next = IDLE;  // Commence avec le processus IDLE.
    struct process *p;

    for (p = FIRST_PROC; p <= LAST_PROC; p++)
    {
        /* Skip non-ready processes and the idle process. */
        if (p->state != PROC_READY || p == IDLE)
            continue;

        /*
         * Sélectionner le processus avec la plus haute priorité:
         * 1. Le processus avec la plus petite priority.
         * 2. Si les priorités priority sont égales, choisir celui avec la plus petite valeur nice.
         * 3. Si priority et nice sont égales, choisir celui qui a utilisé le moins de temps CPU (basé sur ktime + utime).
         */
        if (next == IDLE || 
            p->priority < next->priority || 
            (p->priority == next->priority && p->nice < next->nice) || 
            (p->priority == next->priority && p->nice == next->nice && p->ktime + p->utime < next->ktime + next->utime))
        {
            next = p;
        }
    }

    return next;
}
/// @brief  Ordonnanceur aléatoire. 
/// @details L'ordonnanceur aléatoire choisit un processus prêt au hasard. 
/// @return Le prochain processus à exécuter. 

PRIVATE struct process * __randomScheduling()
{

	struct process *next = IDLE;
	struct process *p;

	int nprocsReady = 0;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		if (p->state == PROC_READY)
		{
			p->counter++;
			nprocsReady++;
		}
	}

	if (nprocsReady == 0)
	{
		next = IDLE;
	}
	else
	{
		int random = (krand() % (nprocsReady)) + 1;
		int i = 0;
		for (p = FIRST_PROC; p <= LAST_PROC; p++)
		{
			if (p->state == PROC_READY)
			{
				i++;
				if (i == random)
				{
					next = p;
					next->counter--;
					break;
				}
			}
		}
	}

	return next;
}
 
/// @brief  Ordonnanceur FIFO. 
/// @details L'ordonnanceur FIFO choisit le premier processus prêt. 
/// @return Le prochain processus à exécuter. 
PRIVATE struct process * __fifoScheduling()
{
	struct process *next = IDLE;  // Commence avec le processus IDLE.
	struct process *p;

	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready processes and the idle process. */
		if (p->state != PROC_READY || p == IDLE)
			continue;

		/*choisir le process avec le plus grand temps d'attente*/ 
		if (next == IDLE || p->counter > next->counter)
		{
			next = p;
		}else{
			p->counter++;
		}
	}

	return next;
 }
 

/// @brief  Ordonnanceur SJF. 
/// @details L'ordonnanceur SJF choisit le processus prêt avec le plus petit temps d'exécution. 
/// @return Le prochain processus à exécuter.
 
PRIVATE struct process * __sjfScheduling()
{
	struct process *next = IDLE;  // Commence avec le processus IDLE.
	struct process *p;

	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready processes and the idle process. */
		if (p->state != PROC_READY || p == IDLE)
			continue;

		/*choisir le process avec le plus petit temps d'exécution*/  
		//si le temps d'exécution du processus actuel est inférieur au temps d'exécution du processus suivant
		  
		//on aurait pu aussi rajouter un champ temps d'execution dans le processus et l'utiliser  
		// mais j'ai preferé estimé a partir du temps d'execution total du processus
		 if (next == IDLE || p->ktime + p->utime < next->ktime + next->utime)
		{
			next = p;
		}else{
			p->counter++;
		}
	}

	return next;
} 
 

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}  



	switch (current_scheduler)
	{
    	case SCHED_FIFO:
        	next = __fifoScheduling();
        	break;
    	case SCHED_PRIORITY:
        	next = __priorityScheduling();
        	break;
    	case SCHED_RANDOM:
        	next = __randomScheduling();
        	break;
    	case SCHED_LOTTERY:
       // 	next = __lotteryScheduling();
        	break; 
		case SCHED_SJF: 
			next = __sjfScheduling(); 
			break;
    	default:
        	next = IDLE;  // Si l'ordonnanceur est inconnu, ne rien faire
        	break;
	}

	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next); 

}
