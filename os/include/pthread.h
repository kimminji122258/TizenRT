/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/********************************************************************************
 * include/pthread.h
 *
 *   Copyright (C) 2007-2009, 2011-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************/
/**
 * @defgroup PTHREAD_KERNEL PTHREAD
 * @brief Provides APIs for Pthreads
 * @ingroup KERNEL
 */

/// @file pthread.h
/// @brief Pthread APIs

#ifndef __INCLUDE_PTHREAD_H
#define __INCLUDE_PTHREAD_H

/********************************************************************************
 * Included Files
 ********************************************************************************/

#include <tinyara/config.h>		/* Default settings */
#include <tinyara/compiler.h>	/* Compiler settings, noreturn_function */

#include <sys/types.h>			/* Needed for general types */
#include <sys/prctl.h>			/* Needed by pthread_[set|get]name_np */

#include <stdint.h>				/* C99 fixed width integer types */
#include <stdbool.h>			/* C99 boolean types */
#include <unistd.h>				/* For getpid */
#include <signal.h>				/* Needed for sigset_t */
#include <time.h>				/* Needed for struct timespec */

#include <tinyara/semaphore.h>	/* For sem_t and SEM_PRIO_* defines */

/********************************************************************************
 * Pre-processor Definitions
 ********************************************************************************/

/* Standard POSIX switches */

#ifndef _POSIX_THREADS
#define _POSIX_THREADS
#endif

#ifndef _POSIX_THREAD_ATTR_STACKSIZE
#define _POSIX_THREAD_ATTR_STACKSIZE
#endif

/* Values for the process shared (pshared) attribute */

#define PTHREAD_PROCESS_PRIVATE       0
#define PTHREAD_PROCESS_SHARED        1

/* Values for the mutext type attribute:
 *
 * PTHREAD_MUTEX_NORMAL: This type of mutex does not detect deadlock. A thread
 *   attempting to relock this mutex without first unlocking it will deadlock.
 *   Attempting to unlock a mutex locked by a different thread results in undefined
 *   behavior. Attempting to unlock an unlocked mutex results in undefined behavior.
 * PTHREAD_MUTEX_ERRORCHECK
 *   This type of mutex provides error checking. A thread attempting to relock this
 *   mutex without first unlocking it will return with an error. A thread attempting
 *   to unlock a mutex which another thread has locked will return with an error. A
 *   thread attempting to unlock an unlocked mutex will return with an error.
 * PTHREAD_MUTEX_RECURSIVE
 *   A thread attempting to relock this mutex without first unlocking it will succeed
 *   in locking the mutex. The relocking deadlock which can occur with mutexes of type
 *   PTHREAD_MUTEX_NORMAL cannot occur with this type of mutex. Multiple locks of this
 *   mutex require the same number of unlocks to release the mutex before another thread
 *   can acquire the mutex. A thread attempting to unlock a mutex which another thread
 *   has locked will return with an error. A thread attempting to unlock an unlocked
 *   mutex will return with an error.
 * PTHREAD_MUTEX_DEFAULT
 *  An implementation is allowed to map this mutex to one of the other mutex types.
 */

#define PTHREAD_MUTEX_NORMAL        0
#define PTHREAD_MUTEX_ERRORCHECK    1
#define PTHREAD_MUTEX_RECURSIVE     2
#define PTHREAD_MUTEX_DEFAULT       PTHREAD_MUTEX_NORMAL

/* Valid ranges for the pthread stacksize attribute */

#define PTHREAD_STACK_MIN             CONFIG_PTHREAD_STACK_MIN
#define PTHREAD_STACK_DEFAULT         CONFIG_PTHREAD_STACK_DEFAULT

/* Values for the pthread inheritsched attribute */

#define PTHREAD_INHERIT_SCHED         0
#define PTHREAD_EXPLICIT_SCHED        1

/* Default priority */

#define PTHREAD_DEFAULT_PRIORITY      100

/* Cancellation states used by pthread_setcancelstate() */

#define PTHREAD_CANCEL_ENABLE         (0)
#define PTHREAD_CANCEL_DISABLE        (1)

/* Cancellation types used by pthread_setcanceltype() */
#define PTHREAD_CANCEL_DEFERRED       (0)
#define PTHREAD_CANCEL_ASYNCHRONOUS   (1)

/* Thread return value when a pthread is canceled */

#define PTHREAD_CANCELED              ((FAR void*)ERROR)

/* Used to initialize a pthread_once_t */

#define PTHREAD_ONCE_INIT             (false)

/* This is returned by pthread_wait.  It must not match any errno in errno.h */

#define PTHREAD_BARRIER_SERIAL_THREAD 0x1000

/* Values for protocol mutex attribute */

#define PTHREAD_PRIO_NONE	SEM_PRIO_NONE
#define PTHREAD_PRIO_INHERIT	SEM_PRIO_INHERIT
#define PTHREAD_PRIO_PROTECT	SEM_PRIO_PROTECT

/*
 * Values for robust argument of pthread_mutexattr_get/setrobust
 *
 * PTHREAD_MUTEX_STALLED - No special actions are taken if the owner of the
 * mutex is terminated while holding the mutex lock. This can lead to
 * deadlocks if no other thread can unlock the mutex.  This is the standard
 * default value (NuttX permits you to override that default behavior
 * with a configuration option).
 *
 * PTHREAD_MUTEX_ROBUST - If the process containing the owning thread of a
 * robust mutex terminates while holding the mutex lock, the next thread
 * that acquires the mutex will be notified about the termination by the
 * return value EOWNERDEAD from the locking function. If the owning thread
 * of a robust mutex terminates while holding the mutex lock, the next
 * thread that attempts to acquire the mutex may be notified about the
 * termination by the return value EOWNERDEAD. The notified thread can
 * then attempt to make the state protected by the mutex consistent again,
 * and if successful can mark the mutex state as consistent by calling
 * pthread_mutex_consistent(). After a subsequent successful call to
 * pthread_mutex_unlock(), the mutex lock will be released and can be used
 * normally by other threads. If the mutex is unlocked without a call to
 * pthread_mutex_consistent(), it will be in a permanently unusable state
 * and all attempts to lock the mutex will fail with the error
 * ENOTRECOVERABLE. The only permissible operation on such a mutex is
 * pthread_mutex_destroy().
 */
#define PTHREAD_MUTEX_STALLED         0
#define PTHREAD_MUTEX_ROBUST          1

/*
 * Values for struct pthread_mutex_s flags.  These are non-standard and
 * intended only for internal use within the OS.
 */
#define _PTHREAD_MFLAGS_ROBUST        (1 << 0)	/* Robust (NORMAL) mutex */
#define _PTHREAD_MFLAGS_INCONSISTENT  (1 << 1)	/* Mutex is in an inconsistent state */
#define _PTHREAD_MFLAGS_NRECOVERABLE  (1 << 2)	/* Inconsistent mutex has been unlocked */

/* Definitions to map some non-standard, BSD thread management interfaces to
 * the non-standard Linux-like prctl() interface.  Since these are simple
 * mappings to prctl, they will return 0 on success and -1 on failure with the
 * err number in errno.  This is an inconsistency with the pthread interfaces.
 */

/**
 * @ingroup PTHREAD_KERNEL
 * @brief sets the name of pthread
 * @param[in] pid of pthread
 * @param[in] name for setting
 * @return On success, OK is returned. On failure, ERROR is returned and errno is set appropriately.
 * @since Tizen RT v1.0
 */
#define pthread_setname_np(thread, name) \
	prctl((int)PR_SET_NAME, (char*)name, (int)thread)

/**
 * @ingroup PTHREAD_KERNEL
 * @brief gets the name of pthread
 * @param[in] pid of pthread
 * @param[in] space for saving pthread name
 * @return On success, OK is returned. On failure, ERROR is returned and errno is set appropriately.
 * @since Tizen RT v1.0
 */
#define pthread_getname_np(thread, name) \
	prctl((int)PR_GET_NAME, (char*)name, (int)thread)

/********************************************************************************
 * Global Type Declarations
 ********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* pthread-specific types */

typedef int pthread_key_t;
typedef FAR void *pthread_addr_t;

typedef pthread_addr_t(*pthread_startroutine_t)(pthread_addr_t);
typedef pthread_startroutine_t pthread_func_t;

/**
 * @ingroup PTHREAD_KERNEL
 * @brief Structure of pthread region configuration
 */
struct pthread_region_s {
	void *address;			/* start address of the region */
	uint32_t size;			/* size of the region in bytes */
	uint32_t attributes;	/* attributes of the region */
};
/**
 * @ingroup PTHREAD_KERNEL
 * @brief Structure of pthread attr configuration
 */
struct pthread_attr_s {
	size_t stacksize;		/* Size of the stack allocated for the pthread */
	int16_t priority;		/* Priority of the pthread */
	uint8_t policy;			/* Pthread scheduler policy */
	uint8_t inheritsched;	/* Inherit parent prio/policy? */
	struct pthread_region_s region[2];	/* space for user-space region if MPU supported */
};
typedef struct pthread_attr_s pthread_attr_t;

typedef pid_t pthread_t;

typedef int pthread_condattr_t;

/**
 * @ingroup PTHREAD_KERNEL
 * @brief Structure of pthread condition configuration
 */
struct pthread_cond_s {
	sem_t sem;
};
typedef struct pthread_cond_s pthread_cond_t;
#define PTHREAD_COND_INITIALIZER { {0, 0xffff} }

/**
 * @ingroup PTHREAD_KERNEL
 * @brief Structure of pthread mutex attr configuration
 */
struct pthread_mutexattr_s {
	uint8_t pshared : 1; /* PTHREAD_PROCESS_PRIVATE or PTHREAD_PROCESS_SHARED */
#ifdef CONFIG_PRIORITY_INHERITANCE
	uint8_t proto   : 2; /* See PTHREAD_PRIO_* definitions */
#endif
#ifdef CONFIG_PTHREAD_MUTEX_TYPES
	uint8_t type    : 2; /* Type of the mutex.  See PTHREAD_MUTEX_* definitions */
#endif
#ifdef CONFIG_PTHREAD_MUTEX_BOTH
	uint8_t robust  : 1; /* PTHREAD_MUTEX_STALLED or PTHREAD_MUTEX_ROBUST */
#endif
};
typedef struct pthread_mutexattr_s pthread_mutexattr_t;

/**
 * @ingroup PTHREAD_KERNEL
 * @brief Structure of pthread mutex configuration
 */
struct pthread_mutex_s {

#ifndef CONFIG_PTHREAD_MUTEX_UNSAFE
	/* Supports a singly linked list */

	FAR struct pthread_mutex_s *flink;
#endif
	/* Payload */

	sem_t sem;				/* Semaphore underlying the implementation of the mutex */
	int pid;				/* ID of the holder of the mutex */
#ifndef CONFIG_PTHREAD_MUTEX_UNSAFE
	uint8_t flags;			/* See _PTHREAD_MFLAGS_* */
#endif
#ifdef CONFIG_PTHREAD_MUTEX_TYPES
	uint8_t type;			/* Type of the mutex.  See PTHREAD_MUTEX_* definitions */
	int nlocks;				/* The number of recursive locks held */
#endif
};
typedef struct pthread_mutex_s pthread_mutex_t;

#define __PTHREAD_MUTEX_T_DEFINED 1

#ifndef CONFIG_PTHREAD_MUTEX_UNSAFE
#ifdef CONFIG_PTHREAD_MUTEX_DEFAULT_UNSAFE
#define __PTHREAD_MUTEX_DEFAULT_FLAGS 0
#else
#define __PTHREAD_MUTEX_DEFAULT_FLAGS _PTHREAD_MFLAGS_ROBUST
#endif
#endif

#if defined(CONFIG_PTHREAD_MUTEX_TYPES) && !defined(CONFIG_PTHREAD_MUTEX_UNSAFE)
#define PTHREAD_MUTEX_INITIALIZER {NULL, SEM_INITIALIZER(1), -1, \
                                     __PTHREAD_MUTEX_DEFAULT_FLAGS, \
                                     PTHREAD_MUTEX_DEFAULT, 0}
#elif defined(CONFIG_PTHREAD_MUTEX_TYPES)
#define PTHREAD_MUTEX_INITIALIZER {SEM_INITIALIZER(1), -1, \
                                     PTHREAD_MUTEX_DEFAULT, 0}
#elif !defined(CONFIG_PTHREAD_MUTEX_UNSAFE)
#define PTHREAD_MUTEX_INITIALIZER {NULL, SEM_INITIALIZER(1), -1,\
                                     __PTHREAD_MUTEX_DEFAULT_FLAGS}
#else
#define PTHREAD_MUTEX_INITIALIZER {SEM_INITIALIZER(1), -1}
#endif

/**
 * @ingroup PTHREAD_KERNEL
 * @brief Structure of pthread barrier attr configuration
 */
struct pthread_barrierattr_s {
	int pshared;
};
typedef struct pthread_barrierattr_s pthread_barrierattr_t;

/**
 * @ingroup PTHREAD_KERNEL
 * @brief Structure of pthread barrier configuration
 */
struct pthread_barrier_s {
	sem_t sem;
	unsigned int count;
};
typedef struct pthread_barrier_s pthread_barrier_t;

typedef bool pthread_once_t;

#ifdef CONFIG_PTHREAD_CLEANUP
/* This type describes the pthread cleanup callback (non-standard) */

typedef CODE void (*pthread_cleanup_t)(FAR void *arg);
#endif

struct pthread_rwlock_s {
	pthread_mutex_t lock;
	pthread_cond_t cv;
	unsigned int num_readers;
	unsigned int num_writers;
	bool write_in_progress;
};

typedef struct pthread_rwlock_s pthread_rwlock_t;

typedef int pthread_rwlockattr_t;

#define PTHREAD_RWLOCK_INITIALIZER  {PTHREAD_MUTEX_INITIALIZER, \
                                     PTHREAD_COND_INITIALIZER, \
                                     0, 0, false}

/* Forware references */

struct sched_param;			/* Defined in sched.h */

/********************************************************************************
 * Global Variables
 ********************************************************************************/

/********************************************************************************
 * Global Function Prototypes
 ********************************************************************************/

/* Initializes a thread attributes object (attr) with default values for all of
 * the individual attributes used by a given implementation.
 */

/* To create a thread object and runnable thread, a routine must be specified
 * as the new thread's start routine.  An argument may be passed to this
 * routine, as an untyped address; an untyped address may also be returned as
 * the routine's value.  An attributes object may be used to specify details
 * about the kind of thread being created.
 */

/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_create(FAR pthread_t *thread, FAR const pthread_attr_t *attr, pthread_startroutine_t startroutine, pthread_addr_t arg);

/* A thread object may be "detached" to specify that the return value and
 * completion status will not be requested.
 */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_detach(pthread_t thread);

/* A thread may terminate it's own execution or the execution of another
 * thread.
 */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
void pthread_exit(pthread_addr_t value) noreturn_function;
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_cancel(pthread_t thread);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_setcancelstate(int state, FAR int *oldstate);

int pthread_setcanceltype(int type, FAR int *oldtype);

/**
 * @cond
 * @internal
 */
void pthread_testcancel(void);
/**
 * @endcond
 */

/* A thread may set up cleanup functions to execut when the thread exits or is canceled. */
#ifdef CONFIG_PTHREAD_CLEANUP
void pthread_cleanup_pop(int execute);
void pthread_cleanup_push(pthread_cleanup_t routine, FAR void *arg);
#endif

/* A thread can await termination of another thread and retrieve the return
 * value of the thread.
 */

/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_join(pthread_t thread, FAR pthread_addr_t *value);

/* A thread may tell the scheduler that its processor can be made available. */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
void pthread_yield(void);

/* A thread may obtain a copy of its own thread handle. */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
#define pthread_self() ((pthread_t)getpid())

/* Compare two thread IDs. */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
#define pthread_equal(t1, t2) ((t1) == (t2))

/* Thread scheduling parameters */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_getschedparam(pthread_t thread, FAR int *policy, FAR struct sched_param *param);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_setschedparam(pthread_t thread, int policy, FAR const struct sched_param *param);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_setschedprio(pthread_t thread, int prio);

/* Thread-specific Data Interfaces */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_key_create(FAR pthread_key_t *key, CODE void (*destructor)(FAR void *));
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_setspecific(pthread_key_t key, FAR const void *value);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
FAR void *pthread_getspecific(pthread_key_t key);
/**
 * @cond
 * @internal
 */
int pthread_key_delete(pthread_key_t key);
/**
 * @endcond
 */

/* The following routines create, delete, lock and unlock mutexes. */

/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_mutex_init(FAR pthread_mutex_t *mutex, FAR const pthread_mutexattr_t *attr);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_mutex_destroy(FAR pthread_mutex_t *mutex);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_mutex_lock(FAR pthread_mutex_t *mutex);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_mutex_trylock(FAR pthread_mutex_t *mutex);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_mutex_unlock(FAR pthread_mutex_t *mutex);

/* A thread can create and delete condition variables. */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_cond_init(FAR pthread_cond_t *cond, FAR const pthread_condattr_t *attr);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_cond_destroy(FAR pthread_cond_t *cond);

/* A thread can signal to and broadcast on a condition variable. */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_cond_broadcast(FAR pthread_cond_t *cond);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_cond_signal(FAR pthread_cond_t *cond);

/* A thread can wait for a condition variable to be signalled or broadcast. */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_cond_wait(FAR pthread_cond_t *cond, FAR pthread_mutex_t *mutex);

/* A thread can perform a timed wait on a condition variable. */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_cond_timedwait(FAR pthread_cond_t *cond, FAR pthread_mutex_t *mutex, FAR const struct timespec *abstime);

/* Barriers */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_barrier_destroy(FAR pthread_barrier_t *barrier);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_barrier_init(FAR pthread_barrier_t *barrier, FAR const pthread_barrierattr_t *attr, unsigned int count);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_barrier_wait(FAR pthread_barrier_t *barrier);

/* Pthread initialization */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_once(FAR pthread_once_t *once_control, CODE void (*init_routine)(void));

/* Pthread signal management APIs */
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_kill(pthread_t thread, int sig);
/**
 * @ingroup PTHREAD_KERNEL
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @details [SYSTEM CALL API]
 * @since Tizen RT v1.0
 */
int pthread_sigmask(int how, FAR const sigset_t *set, FAR sigset_t *oset);

/**
 * @addtogroup PTHREAD_KERNEL
 * @{
 */
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_init(FAR pthread_attr_t *attr);

/* An attributes object can be deleted when it is no longer needed. */

/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_destroy(pthread_attr_t *attr);

/* Set or obtain the default scheduling algorithm */

/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_setschedpolicy(FAR pthread_attr_t *attr, int policy);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_getschedpolicy(FAR const pthread_attr_t *attr, int *policy);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_setschedparam(FAR pthread_attr_t *attr, FAR const struct sched_param *param);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_getschedparam(FAR const pthread_attr_t *attr, FAR struct sched_param *param);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_setinheritsched(FAR pthread_attr_t *attr, int inheritsched);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_getinheritsched(FAR const pthread_attr_t *attr, FAR int *inheritsched);

/* Set or obtain the default stack size */
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_setstacksize(FAR pthread_attr_t *attr, long stacksize);

/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_attr_getstacksize(FAR const pthread_attr_t *attr, long *stackaddr);

/* Create, operate on, and destroy mutex attributes. */
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_mutexattr_init(FAR pthread_mutexattr_t *attr);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_mutexattr_destroy(FAR pthread_mutexattr_t *attr);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_mutexattr_getpshared(FAR const pthread_mutexattr_t *attr, FAR int *pshared);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_mutexattr_setpshared(FAR pthread_mutexattr_t *attr, int pshared);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

int pthread_mutexattr_getprotocol(FAR const pthread_mutexattr_t *attr,
				  FAR int *protocol);
int pthread_mutexattr_setprotocol(FAR pthread_mutexattr_t *attr,
				  int protocol);
int pthread_mutexattr_getrobust(FAR const pthread_mutexattr_t *attr,
				FAR int *robust);
int pthread_mutexattr_setrobust(FAR pthread_mutexattr_t *attr,
				int robust);

/* Operations on condition variables */
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_condattr_init(FAR pthread_condattr_t *attr);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_condattr_destroy(FAR pthread_condattr_t *attr);

/* Barrier attributes */
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_barrierattr_destroy(FAR pthread_barrierattr_t *attr);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_barrierattr_init(FAR pthread_barrierattr_t *attr);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_barrierattr_getpshared(FAR const pthread_barrierattr_t *attr, FAR int *pshared);
/**
 * @brief  POSIX APIs (refer to : http://pubs.opengroup.org/onlinepubs/9699919799/)
 * @since Tizen RT v1.0
 */
int pthread_barrierattr_setpshared(FAR pthread_barrierattr_t *attr, int pshared);
/**
	 * @} *///end for PTHREAD_KERNEL

/* Pthread rwlock */

int pthread_rwlock_destroy(FAR pthread_rwlock_t *rw_lock);
int pthread_rwlock_init(FAR pthread_rwlock_t *rw_lock, FAR const pthread_rwlockattr_t *attr);
int pthread_rwlock_rdlock(pthread_rwlock_t *lock);
int pthread_rwlock_timedrdlock(FAR pthread_rwlock_t *lock, FAR const struct timespec *abstime);
int pthread_rwlock_tryrdlock(FAR pthread_rwlock_t *lock);
int pthread_rwlock_wrlock(FAR pthread_rwlock_t *lock);
int pthread_rwlock_timedwrlock(FAR pthread_rwlock_t *lock, FAR const struct timespec *abstime);
int pthread_rwlock_trywrlock(FAR pthread_rwlock_t *lock);
int pthread_rwlock_unlock(FAR pthread_rwlock_t *lock);

#ifdef __cplusplus
}
#endif
#endif							/* __INCLUDE_PTHREAD_H */
