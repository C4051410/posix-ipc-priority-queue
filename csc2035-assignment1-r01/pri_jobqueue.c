/*
 * Replace the following string of 0s with your student number
 * c4051410
 */

#include <stdlib.h>
#include <string.h>
#include "pri_jobqueue.h"


static job_t* qarr(pri_jobqueue_t* q) {
    return q ? q->jobs : NULL;
}


pri_jobqueue_t* pri_jobqueue_new() {
    pri_jobqueue_t* q = malloc(sizeof(pri_jobqueue_t));
    if (!q)
        return NULL;
    pri_jobqueue_init(q);
    return q;
}


void pri_jobqueue_init(pri_jobqueue_t* q) {
    if (!q)
        return;

    q->buf_size = JOB_BUFFER_SIZE;
    q->size = 0;

    for (int i = 0; i < JOB_BUFFER_SIZE; i++)
        job_init(&q->jobs[i]);
}


bool pri_jobqueue_is_empty(pri_jobqueue_t* q) {
    if (!q)
        return true;
    return q->size == 0;
}


bool pri_jobqueue_is_full(pri_jobqueue_t* q) {
    if (!q)
        return true;
    return q->size == q->buf_size;
}


int pri_jobqueue_size(pri_jobqueue_t* q) {
    if (!q)
        return 0;
    return q->size;
}


int pri_jobqueue_space(pri_jobqueue_t* q) {
    if (!q)
        return 0;
    return q->buf_size - q->size;
}


static int find_best(pri_jobqueue_t* q) {
    int best = -1;
    unsigned int best_pri = 999999; 

    for (int i = 0; i < q->size; i++) {
        unsigned int pri = q->jobs[i].priority;

        if (pri >= 1 && pri < best_pri) {
            best_pri = pri;
            best = i;
        }
    }
    return best;
}


job_t* pri_jobqueue_peek(pri_jobqueue_t* q, job_t* dst) {
    if (!q || q->size == 0)
        return NULL;

    int idx = find_best(q);
    if (idx < 0)
        return NULL;

    return job_copy(&q->jobs[idx], dst);
}


job_t* pri_jobqueue_dequeue(pri_jobqueue_t* q, job_t* dst) {
    if (!q || q->size == 0)
        return NULL;

    int idx = find_best(q);
    if (idx < 0)
        return NULL;

    job_t temp;
    job_copy(&q->jobs[idx], &temp);

    for (int i = idx; i < q->size - 1; i++)
        q->jobs[i] = q->jobs[i + 1];

    q->size--;

    return job_copy(&temp, dst);
}


void pri_jobqueue_enqueue(pri_jobqueue_t* q, job_t* job) {
    if (!q || !job)
        return;

    if (q->size >= q->buf_size)
        return;

    job_copy(job, &q->jobs[q->size]);
    q->size++;
}


void pri_jobqueue_delete(pri_jobqueue_t* q) {
    if (!q)
        return;
    free(q);
}
 