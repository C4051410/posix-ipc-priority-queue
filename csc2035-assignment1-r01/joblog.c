/*
 * Replace the following string of 0s with your student number
 * c4051410
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "joblog.h"

/* 
 * DO NOT EDIT the new_log_name function. It is a private helper 
 * function provided for you to create a log name from a process 
 * descriptor for use when reading, writing and deleting a log file.
 * 
 * You must work out what the function does in order to use it properly
 * and to clean up after use.
 */
static char* new_log_name(proc_t* proc) {
    static char* joblog_name_fmt = "%s/%.31s%07d.txt";
                                // string format for the name of a log file
                                // declared static to have only one instance

    if (!proc)
        return NULL;

    char* log_name;
            
    asprintf(&log_name, joblog_name_fmt, JOBLOG_PATH, proc->type_label,
        proc->id);

    return log_name;
}

/* 
 * DO NOT EDIT the joblog_init function that sets up the log directory 
 * if it does not already exist.
 */
int joblog_init(proc_t* proc) {
    if (!proc) {
        errno = EINVAL;
        return -1;
    }
        
    int r = 0;
    if (proc->is_init) {
        struct stat sb;
    
        if (stat(JOBLOG_PATH, &sb) != 0) {
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }  else if (!S_ISDIR(sb.st_mode)) {
            unlink(JOBLOG_PATH);
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }
    } 

    joblog_delete(proc);    
    
    return r;
}


job_t* joblog_read(proc_t* proc, int entry_num, job_t* job) {
    int old_errno = errno;

    
    if (!proc || entry_num < 0) {
        return NULL;
    }

    char *log_name = new_log_name(proc);
    if (!log_name) {
        errno = old_errno;
        return NULL;
    }

    FILE *fp = fopen(log_name, "r");
    if (!fp) {
        free(log_name);
        errno = old_errno;
        return NULL;
    }

   
    long offset = (long) entry_num * JOB_STR_SIZE;
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        free(log_name);
        errno = old_errno;
        return NULL;
    }

    
    char record[JOB_STR_SIZE];
    size_t nread = fread(record, 1, JOB_STR_SIZE, fp);

    if (nread != JOB_STR_SIZE) {
        
        fclose(fp);
        free(log_name);
        errno = old_errno;
        return NULL;
    }

    
    char job_str[JOB_STR_SIZE];
    memcpy(job_str, record, JOB_STR_SIZE - 1);
    job_str[JOB_STR_SIZE - 1] = '\0';

    job_t *result = str_to_job(job_str, job);

    fclose(fp);
    free(log_name);

    if (!result) {
        errno = old_errno;
        return NULL;
    }

    errno = old_errno;
    return result;
}


void joblog_write(proc_t* proc, job_t* job) {
    int old_errno = errno;

    
    if (!proc || !job) {
        errno = old_errno;
        return;
    }

    char *log_name = new_log_name(proc);

    if (!log_name) {
        errno = old_errno;
        return;
    }

    
    FILE *fp = fopen(log_name, "a");
    if (!fp) {
        free(log_name);
        errno = old_errno;
        return;
    }

    
    char entry[JOB_STR_SIZE];
    char *s = job_to_str(job, entry);

    if (!s) {
        
        fclose(fp);
        free(log_name);
        errno = old_errno;
        return;
    }

    size_t len = strlen(s);
    if (len != JOB_STR_SIZE - 1) {
        
        fclose(fp);
        free(log_name);
        errno = old_errno;
        return;
    }

    
    size_t written = fwrite(s, 1, len, fp);
    int c = fputc('\n', fp);

    if (written != len || c == EOF) {
        
        fclose(fp);
        free(log_name);
        errno = old_errno;
        return;
    }

    fclose(fp);
    free(log_name);
    errno = old_errno;
}


void joblog_delete(proc_t* proc) {
    int old_errno = errno;

    if (!proc) {
        errno = old_errno;
        return;
    }

    char *log_name = new_log_name(proc);
    if (!log_name) {
        errno = old_errno;
        return;
    }

    
    (void) unlink(log_name);

    free(log_name);
    errno = old_errno;
}
