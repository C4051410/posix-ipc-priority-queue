/*
 * Replace the following string of 0s with your student number
 * c4051410
 */

#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "sem_jobqueue.h"
#include "proc.h"

/*
 * Helper for opening or creating a named semaphore.
 * init_value is only used if this process is init.
 */
static sem_t* open_sem(const char* base, proc_t* proc, unsigned int init_value) {
    char name[MAX_NAME_SIZE];
    shobject_name(base, name);

    int flags = O_CREAT;
    unsigned int mode = S_IRUSR | S_IWUSR;

    sem_t* s = sem_open(name, flags, mode, init_value);
    return s != SEM_FAILED ? s : NULL;
}

/*
 * Helper for unlinking semaphores (used only by init-cleanup)
 */
static void unlink_sem(const char* base) {
    char name[MAX_NAME_SIZE];
    shobject_name(base, name);
    sem_unlink(name);
}

/*
 * sem_jobqueue_new
 *
 * Creates a new shared-memory jobqueue + semaphore monitor.
 */
sem_jobqueue_t* sem_jobqueue_new(proc_t* proc) {
    if (!proc) {
        errno = EINVAL;
        return NULL;
    }

    /* Allocate wrapper struct */
    sem_jobqueue_t* sjq = malloc(sizeof(sem_jobqueue_t));
    if (!sjq)
        return NULL;

    /* Create underlying shared memory queue */
    ipc_jobqueue_t* ijq = ipc_jobqueue_new(proc);
    if (!ijq) {
        free(sjq);
        return NULL;
    }

    sjq->ijq = ijq;

    /* Initial creation by init process: unlink old semaphores first */
    if (proc->is_init) {
        unlink_sem("sjq.mutex");
        unlink_sem("sjq.full");
        unlink_sem("sjq.empty");
    }

    /* Open/create semaphores */
    sjq->mutex = open_sem("sjq.mutex", proc, 1); /* unlocked */
    sjq->full  = open_sem("sjq.full",  proc, PRI_JOBQUEUE_CAPACITY); /* capacity = free slots */
    sjq->empty = open_sem("sjq.empty", proc, 0); /* empty count = 0 initially */

    if (!sjq->mutex || !sjq->full || !sjq->empty) {
        /* Clean up */
        if (sjq->mutex) sem_close(sjq->mutex);
        if (sjq->full)  sem_close(sjq->full);
        if (sjq->empty) sem_close(sjq->empty);
        ipc_jobqueue_delete(ijq);
        free(sjq);
        return NULL;
    }

    return sjq;
}

/*
 * sem_jobqueue_enqueue
 *
 * classic producer–consumer monitor pattern
 */
void sem_jobqueue_enqueue(sem_jobqueue_t* sjq, job_t* job) {
    if (!sjq || !job)
        return;

    /* Wait until space exists */
    sem_wait(sjq->full);

    /* Enter critical section */
    sem_wait(sjq->mutex);

    /* Critical work simulation */
    do_critical_work(sjq->ijq->proc);

    /* Perform the enqueue on underlying queue */
    pri_jobqueue_enqueue((pri_jobqueue_t*) sjq->ijq->addr, job);

    /* Exit critical section */
    sem_post(sjq->mutex);

    /* Signal that a job is available */
    sem_post(sjq->empty);
}

/*
 * sem_jobqueue_dequeue
 *
 * consumer removes a job
 */
job_t* sem_jobqueue_dequeue(sem_jobqueue_t* sjq, job_t* dst) {
    if (!sjq)
        return NULL;

    /* Wait until a job exists */
    sem_wait(sjq->empty);

    /* Enter critical section */
    sem_wait(sjq->mutex);

    /* Critical delay simulation */
    do_critical_work(sjq->ijq->proc);

    /* Dequeue */
    job_t* r = pri_jobqueue_dequeue((pri_jobqueue_t*) sjq->ijq->addr, dst);

    /* Exit critical section */
    sem_post(sjq->mutex);

    /* Signal free slot available */
    sem_post(sjq->full);

    return r;
}

/*
 * sem_jobqueue_peek
 */
job_t* sem_jobqueue_peek(sem_jobqueue_t* sjq, job_t* dst) {
    if (!sjq)
        return NULL;

    sem_wait(sjq->mutex);
    do_critical_work(sjq->ijq->proc);

    job_t* r = pri_jobqueue_peek((pri_jobqueue_t*) sjq->ijq->addr, dst);

    sem_post(sjq->mutex);
    return r;
}

bool sem_jobqueue_is_empty(sem_jobqueue_t* sjq) {
    if (!sjq)
        return true;

    sem_wait(sjq->mutex);
    do_critical_work(sjq->ijq->proc);

    bool r = pri_jobqueue_is_empty((pri_jobqueue_t*) sjq->ijq->addr);

    sem_post(sjq->mutex);
    return r;
}

bool sem_jobqueue_is_full(sem_jobqueue_t* sjq) {
    if (!sjq)
        return true;

    sem_wait(sjq->mutex);
    do_critical_work(sjq->ijq->proc);

    bool r = pri_jobqueue_is_full((pri_jobqueue_t*) sjq->ijq->addr);

    sem_post(sjq->mutex);
    return r;
}

int sem_jobqueue_size(sem_jobqueue_t* sjq) {
    if (!sjq)
        return -1;

    sem_wait(sjq->mutex);
    do_critical_work(sjq->ijq->proc);

    int r = pri_jobqueue_size((pri_jobqueue_t*) sjq->ijq->addr);

    sem_post(sjq->mutex);
    return r;
}

int sem_jobqueue_space(sem_jobqueue_t* sjq) {
    if (!sjq)
        return -1;

    sem_wait(sjq->mutex);
    do_critical_work(sjq->ijq->proc);

    int r = pri_jobqueue_space((pri_jobqueue_t*) sjq->ijq->addr);

    sem_post(sjq->mutex);
    return r;
}

/*
 * sem_jobqueue_delete
 *
 * Close & unlink semaphores + delete underlying ipc_jobqueue
 */
void sem_jobqueue_delete(sem_jobqueue_t* sjq) {
    if (!sjq)
        return;

    /* Close semaphores */
    sem_close(sjq->mutex);
    sem_close(sjq->full);
    sem_close(sjq->empty);

    /* Underlying queue cleanup */
    ipc_jobqueue_delete(sjq->ijq);

    /* Free wrapper */
    free(sjq);
}
