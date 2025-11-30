/*
 * Replace the following string of 0s with your student number
 * c4051410
 */
#include <stdlib.h>
#include <string.h>
#include "pri_jobqueue.h"

/*
 * Helper: return pointer to the underlying array
 */
static job_t* qarr(pri_jobqueue_t* q) {
    return q ? q->jobs : NULL;
}

/*
 * pri_jobqueue_new
 */
pri_jobqueue_t* pri_jobqueue_new() {
    pri_jobqueue_t* q = malloc(sizeof(pri_jobqueue_t));
    if (!q)
        return NULL;
    pri_jobqueue_init(q);
    return q;
}

/*
 * pri_jobqueue_init
 */
void pri_jobqueue_init(pri_jobqueue_t* q) {
    if (!q)
        return;

    q->head = 0;
    q->size = 0;

    /* Initialise all jobs to empty with priority 0 */
    for (int i = 0; i < PRI_JOBQUEUE_CAPACITY; i++) {
        job_init(&q->jobs[i]);
    }
}

/*
 * pri_jobqueue_is_empty
 */
bool pri_jobqueue_is_empty(pri_jobqueue_t* q) {
    if (!q)
        return true;
    return q->size == 0;
}

/*
 * pri_jobqueue_is_full
 */
bool pri_jobqueue_is_full(pri_jobqueue_t* q) {
    if (!q)
        return true;
    return q->size == PRI_JOBQUEUE_CAPACITY;
}

/*
 * pri_jobqueue_size
 */
int pri_jobqueue_size(pri_jobqueue_t* q) {
    if (!q)
        return -1;
    return q->size;
}

/*
 * pri_jobqueue_space
 */
int pri_jobqueue_space(pri_jobqueue_t* q) {
    if (!q)
        return -1;
    return PRI_JOBQUEUE_CAPACITY - q->size;
}

/*
 * Find index of highest-priority job.
 * Priority 1 is highest. Lower numerical = higher priority.
 * If multiple jobs share priority, earliest (lowest index from head) wins.
 */
static int find_best(pri_jobqueue_t* q) {
    int best_i = -1;
    unsigned int best_pri = 0;  /* 0 = unused */

    for (int i = 0; i < q->size; i++) {
        int idx = (q->head + i) % PRI_JOBQUEUE_CAPACITY;
        unsigned int pri = q->jobs[idx].priority;

        if (pri == 0)
            continue;  /* unused slot */

        if (best_i == -1 || pri < best_pri) {
            best_pri = pri;
            best_i = idx;
        }
    }

    return best_i;
}

/*
 * pri_jobqueue_peek
 */
job_t* pri_jobqueue_peek(pri_jobqueue_t* q, job_t* dst) {
    if (!q)
        return NULL;
    if (q->size == 0)
        return NULL;

    int best = find_best(q);
    if (best < 0)
        return NULL;

    return job_copy(&q->jobs[best], dst);
}

/*
 * pri_jobqueue_dequeue
 */
job_t* pri_jobqueue_dequeue(pri_jobqueue_t* q, job_t* dst) {
    if (!q)
        return NULL;
    if (q->size == 0)
        return NULL;

    int best = find_best(q);
    if (best < 0)
        return NULL;

    /* Copy to dst */
    job_t temp;
    job_copy(&q->jobs[best], &temp);

    /* Remove it by shifting elements to close gap */
    int last_index = (q->head + q->size - 1) % PRI_JOBQUEUE_CAPACITY;

    /* If best isn't last, shift jobs */
    while (best != last_index) {
        int next = (best + 1) % PRI_JOBQUEUE_CAPACITY;
        q->jobs[best] = q->jobs[next];
        best = next;
    }

    /* Decrease size */
    q->size--;

    /* head stays same */

    return job_copy(&temp, dst);
}

/*
 * pri_jobqueue_enqueue
 */
void pri_jobqueue_enqueue(pri_jobqueue_t* q, job_t* job) {
    if (!q || !job)
        return;

    if (q->size == PRI_JOBQUEUE_CAPACITY)
        return;

    int pos = (q->head + q->size) % PRI_JOBQUEUE_CAPACITY;
    job_copy(job, &q->jobs[pos]);
    q->size++;
}

/*
 * pri_jobqueue_delete
 */
void pri_jobqueue_delete(pri_jobqueue_t* q) {
    if (!q)
        return;
    free(q);
}
