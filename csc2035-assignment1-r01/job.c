/*
 * Replace the following string of 0s with your student number
 * c4051410
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "job.h"

/* 
 * DO NOT EDIT the job_new function.
 */
job_t* job_new(pid_t pid, unsigned int id, unsigned int priority, 
    const char* label) {
    return job_set((job_t*) malloc(sizeof(job_t)), pid, id, priority, label);
}

/* 
 * job_copy(job_t* src, job_t* dst)
 */
job_t* job_copy(job_t* src, job_t* dst) {
    /* 1. If src is NULL, return NULL */
    if (!src)
        return NULL;

    /* src->label must be exactly MAX_NAME_SIZE - 1 chars */
    if (strlen(src->label) != MAX_NAME_SIZE - 1)
        return NULL;

    /* 2. If src and dst are the same pointer (including both NULL handled
       above), return src without copying */
    if (src == dst)
        return src;

    /* 3. If dst is NULL, allocate a new job and copy into it */
    if (!dst) {
        return job_new(src->pid, src->id, src->priority, src->label);
    }

    /* 4. Otherwise copy src into dst and return dst */
    *dst = *src;  /* struct copy, including label array */
    return dst;
}

/* 
 * job_init(job_t* job)
 */
void job_init(job_t* job) {
    if (!job)
        return;

    job->pid = 0;
    job->id = 0;
    job->priority = 0;

    /* Set label to PAD_STRING, padded to MAX_NAME_SIZE - 1, then nul */
    memcpy(job->label, PAD_STRING, MAX_NAME_SIZE - 1);
    job->label[MAX_NAME_SIZE - 1] = '\0';
}

/* 
 * job_is_equal(job_t* j1, job_t* j2)
 */
bool job_is_equal(job_t* j1, job_t* j2) {
    /* Two identical pointers, including two NULLs, are equal */
    if (j1 == j2)
        return true;

    /* One NULL, one non-NULL -> not equal */
    if (!j1 || !j2)
        return false;

    /* Compare fields and label string */
    if (j1->pid != j2->pid)
        return false;
    if (j1->id != j2->id)
        return false;
    if (j1->priority != j2->priority)
        return false;

    return strcmp(j1->label, j2->label) == 0;
}

/*
 * job_set(job_t* job, pid_t pid, unsigned int id, unsigned int priority,
 *         const char* label)
 */
job_t* job_set(job_t* job, pid_t pid, unsigned int id, unsigned int priority,
    const char* label) {
    if (!job)
        return NULL;

    job->pid = pid;
    job->id = id;
    job->priority = priority;

    /* Handle label: NULL or empty -> PAD_STRING */
    if (!label || !label[0]) {
        memcpy(job->label, PAD_STRING, MAX_NAME_SIZE - 1);
    } else {
        /* Copy up to MAX_NAME_SIZE - 1 characters, then pad with '*' */
        size_t len = strnlen(label, MAX_NAME_SIZE - 1);
        memcpy(job->label, label, len);
        if (len < MAX_NAME_SIZE - 1) {
            memset(job->label + len, '*', (MAX_NAME_SIZE - 1) - len);
        }
    }

    /* Ensure nul terminator */
    job->label[MAX_NAME_SIZE - 1] = '\0';

    return job;
}

/*
 * job_to_str(job_t* job, char* str)
 */
char* job_to_str(job_t* job, char* str) {
    if (!job)
        return NULL;

    /* Label must be exactly MAX_NAME_SIZE - 1 characters */
    if (strlen(job->label) != MAX_NAME_SIZE - 1)
        return NULL;

    int allocated = 0;

    if (!str) {
        str = (char*) malloc(JOB_STR_SIZE);
        if (!str)
            return NULL;
        allocated = 1;
    }

    int written = snprintf(str, JOB_STR_SIZE, JOB_STR_FMT,
                           (int) job->pid,
                           job->id,
                           job->priority,
                           job->label);

    /* snprintf error or truncation beyond JOB_STR_SIZE-1 */
    if (written < 0 || written >= JOB_STR_SIZE) {
        if (allocated)
            free(str);
        return NULL;
    }

    return str;
}

/*
 * str_to_job(char* str, job_t* job)
 */
job_t* str_to_job(char* str, job_t* job) {
    if (!str)
        return NULL;

    /* String representation must be exactly JOB_STR_SIZE - 1 chars */
    if (strlen(str) != JOB_STR_SIZE - 1)
        return NULL;

    int allocated = 0;

    if (!job) {
        job = (job_t*) malloc(sizeof(job_t));
        if (!job)
            return NULL;
        allocated = 1;
    }

    int pid_int;
    unsigned int id;
    unsigned int priority;
    char label_buf[MAX_NAME_SIZE];   /* room for 31 chars + '\0' */

    int scanned = sscanf(str, JOB_STR_FMT,
                         &pid_int,
                         &id,
                         &priority,
                         label_buf);

    /* Parsing failed or wrong number of items */
    if (scanned != 4) {
        if (allocated)
            free(job);
        return NULL;
    }

    /* Label must be exactly MAX_NAME_SIZE - 1 characters */
    if (strlen(label_buf) != MAX_NAME_SIZE - 1) {
        if (allocated)
            free(job);
        return NULL;
    }

    /* Use job_set to enforce label invariants */
    job_set(job, (pid_t) pid_int, id, priority, label_buf);

    return job;
}

/* 
 * job_delete(job_t* job)
 */
void job_delete(job_t* job) {
    if (!job)
        return;

    free(job);
}
