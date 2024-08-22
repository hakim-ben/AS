/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SEM_H_
#define SEM_H_ 
 
	#include <nanvix/config.h>
	#include <nanvix/pm.h> 	  

	#define SEM_INVALID 0 /**< Invalid semaphore. */ 
	#define SEM_VALID	1 /**< Valid semaphore.   */
	
	/**
	 * @brief Comand values for semaphores.
	 */
	/**@{*/
	#define GETVAL   0 /**< Returns the value of a semaphore. */
	#define SETVAL   1 /**< Sets the value of a semaphore.    */
	#define IPC_RMID 3 /**< Destroys a semaphore.            */
	/**@}*/
  
    #define WAIT_MAX 64 /**< Maximum number of processes waiting on a semaphore. */
	 
	/* Forward definitions. */
	extern int semget(unsigned);
	extern int semctl(int, int, int);
	extern int semop(int, int); 
	 
 
	/**
	 * @brief Semaphore structure.
	 */ 
	struct sem_t { 
		int valid; /** whene this value is 0 in the table means that it's initialised . */
		int value; /**< Semaphore value. */ 
		unsigned key; /**< Semaphore key. */
		int waiting; /**< Number of processes waiting on this semaphore. */ 
		struct process *waiting_queue[WAIT_MAX]; /**< Array of waiting processes. */
	}; 
 
	EXTERN struct sem_t semtab[SEM_MAX]; /**< Semaphore table. */
 
	/**
	 * @brief Initializes a semaphore. */ 
	EXTERN void sem_init(void);
 
#endif /* SEM_H_ */
