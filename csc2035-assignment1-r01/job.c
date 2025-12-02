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


job_t* job_copy(job_t* src, job_t* dst) {
   
    if (!src)
        return NULL;

    
    if (strlen(src->label) != MAX_NAME_SIZE - 1)
        return NULL;

   
    if (src == dst)
        return src;

    
    if (!dst) {
        return job_new(src->pid, src->id, src->priority, src->label);
    }

    
    *dst = *src;  
    return dst;
}


void job_init(job_t* job) {
    if (!job)
        return;

    job->pid = 0;
    job->id = 0;
    job->priority = 0;

    
    memcpy(job->label, PAD_STRING, MAX_NAME_SIZE - 1);
    job->label[MAX_NAME_SIZE - 1] = '\0';
}


bool job_is_equal(job_t* j1, job_t* j2) {
    
    if (j1 == j2)
        return true;

    
    if (!j1 || !j2)
        return false;

    
    if (j1->pid != j2->pid)
        return false;
    if (j1->id != j2->id)
        return false;
    if (j1->priority != j2->priority)
        return false;

    return strcmp(j1->label, j2->label) == 0;
}


job_t* job_set(job_t* job, pid_t pid, unsigned int id, unsigned int priority,
    const char* label) {
    if (!job)
        return NULL;

    job->pid = pid;
    job->id = id;
    job->priority = priority;

    
    if (!label || !label[0]) {
        memcpy(job->label, PAD_STRING, MAX_NAME_SIZE - 1);
    } else {
        
        size_t len = strnlen(label, MAX_NAME_SIZE - 1);
        memcpy(job->label, label, len);
        if (len < MAX_NAME_SIZE - 1) {
            memset(job->label + len, '*', (MAX_NAME_SIZE - 1) - len);
        }
    }

    
    job->label[MAX_NAME_SIZE - 1] = '\0';

    return job;
}


char* job_to_str(job_t* job, char* str) {
    if (!job)
        return NULL;

    
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

    
    if (written < 0 || written >= JOB_STR_SIZE) {
        if (allocated)
            free(str);
        return NULL;
    }

    return str;
}


job_t* str_to_job(char* str, job_t* job) {
    if (!str)
        return NULL;

    
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
    char label_buf[MAX_NAME_SIZE];   

    int scanned = sscanf(str, JOB_STR_FMT,
                         &pid_int,
                         &id,
                         &priority,
                         label_buf);

    
    if (scanned != 4) {
        if (allocated)
            free(job);
        return NULL;
    }

    
    if (strlen(label_buf) != MAX_NAME_SIZE - 1) {
        if (allocated)
            free(job);
        return NULL;
    }

    
    job_set(job, (pid_t) pid_int, id, priority, label_buf);

    return job;
}


void job_delete(job_t* job) {
    if (!job)
        return;

    free(job);
}
