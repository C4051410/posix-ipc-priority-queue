/*
 * Replace the following string of 0s with your student number
 * c4051410
 */
#include "ipc_jobqueue.h"
#include "proc.h"   // for do_critical_work

/* 
 * DO NOT EDIT the ipc_jobqueue_new function.
 */
ipc_jobqueue_t* ipc_jobqueue_new(proc_t* proc) {
    ipc_jobqueue_t* ijq = ipc_new(proc, "ipc_jobq", sizeof(pri_jobqueue_t));
    
    if (!ijq) 
        return NULL;
    
    if (proc->is_init)
        pri_jobqueue_init((pri_jobqueue_t*) ijq->addr);
    
    return ijq;
}


job_t* ipc_jobqueue_dequeue(ipc_jobqueue_t* ijq, job_t* dst) {
    if (!ijq)
        return pri_jobqueue_dequeue(NULL, dst);

    do_critical_work(ijq->proc);

    pri_jobqueue_t* q = (pri_jobqueue_t*) ijq->addr;
    return pri_jobqueue_dequeue(q, dst);
}


void ipc_jobqueue_enqueue(ipc_jobqueue_t* ijq, job_t* job) {
    if (!ijq) {
        pri_jobqueue_enqueue(NULL, job);
        return;
    }

    do_critical_work(ijq->proc);

    pri_jobqueue_t* q = (pri_jobqueue_t*) ijq->addr;
    pri_jobqueue_enqueue(q, job);
}


bool ipc_jobqueue_is_empty(ipc_jobqueue_t* ijq) {
    if (!ijq)
        return pri_jobqueue_is_empty(NULL);

    do_critical_work(ijq->proc);

    pri_jobqueue_t* q = (pri_jobqueue_t*) ijq->addr;
    return pri_jobqueue_is_empty(q);
}


bool ipc_jobqueue_is_full(ipc_jobqueue_t* ijq) {
    if (!ijq)
        return pri_jobqueue_is_full(NULL);

    do_critical_work(ijq->proc);

    pri_jobqueue_t* q = (pri_jobqueue_t*) ijq->addr;
    return pri_jobqueue_is_full(q);
}


job_t* ipc_jobqueue_peek(ipc_jobqueue_t* ijq, job_t* dst) {
    if (!ijq)
        return pri_jobqueue_peek(NULL, dst);

    do_critical_work(ijq->proc);

    pri_jobqueue_t* q = (pri_jobqueue_t*) ijq->addr;
    return pri_jobqueue_peek(q, dst);
}


int ipc_jobqueue_size(ipc_jobqueue_t* ijq) {
    if (!ijq)
        return pri_jobqueue_size(NULL);

    do_critical_work(ijq->proc);

    pri_jobqueue_t* q = (pri_jobqueue_t*) ijq->addr;
    return pri_jobqueue_size(q);
}


int ipc_jobqueue_space(ipc_jobqueue_t* ijq) {
    if (!ijq)
        return pri_jobqueue_space(NULL);

    do_critical_work(ijq->proc);

    pri_jobqueue_t* q = (pri_jobqueue_t*) ijq->addr;
    return pri_jobqueue_space(q);
}


void ipc_jobqueue_delete(ipc_jobqueue_t* ijq) {
    if (!ijq)
        return;

    ipc_delete(ijq);
}
