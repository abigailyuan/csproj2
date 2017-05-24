#ifndef WORK_H
#define WORK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
#include<stdbool.h>
#include "sha256.h"
#include "uint256.h"


struct work_job_t{
    BYTE *seed;
    BYTE *target;
    BYTE *start;
    BYTE *solutionBYTE;
    int id;
    int client_fd;
    struct work_job_t *next;
    struct work_job_t *prev;
};

struct work_job_t* create_list(BYTE *seed, BYTE *target, BYTE *start, int client_fd, int id, struct work_job_t* head, struct work_job_t* curr);
struct work_job_t* add_to_list(BYTE *seed, BYTE *target, BYTE *start, int client_fd, int id, bool add_to_end, struct work_job_t* head, struct work_job_t* curr);
struct work_job_t* search_in_list(int client_fd, struct work_job_t **prev, struct work_job_t* head, struct work_job_t* curr);
int delete_from_list(int client_fd, struct work_job_t* head, struct work_job_t* curr);
void print_list(struct work_job_t* head, struct work_job_t* curr);

#endif
