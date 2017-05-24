#include "work.h"


struct work_job_t* create_list(BYTE *seed, BYTE *target, BYTE *start, int client_fd, int id, struct work_job_t* head, struct work_job_t* curr){
    printf("\n creating list with headnode as [%d]\n",id);
    struct work_job_t *ptr = (struct work_job_t*)malloc(sizeof(struct work_job_t));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->seed = seed;
    ptr->difficulty = target;
    ptr->start = start;
    ptr->client_fd = client_fd;
    ptr->id = id;
    ptr->next = NULL;
    ptr->prev = NULL;

    head = curr = ptr;
    return ptr;
}

struct work_job_t* add_to_list(BYTE *seed, BYTE *target, BYTE *start, int client_fd, int id, bool add_to_end, struct work_job_t* head, struct work_job_t* curr)
{
    if(NULL == head)
    {
        return (create_list(seed, target, start, client_fd, id, head, curr));
    }

    if(add_to_end)
        printf("\n Adding node to end of list with id [%d]\n",id);
    else
        printf("\n Adding node to beginning of list with id [%d]\n",id);

    struct work_job_t *ptr = (struct work_job_t*)malloc(sizeof(struct work_job_t));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->seed = seed;
    ptr->difficulty = target;
    ptr->start = start;
    ptr->client_fd = client_fd;
    ptr->id = id;
    ptr->next = NULL;
    ptr->prev = NULL;


    if(add_to_end)
    {
        ptr->prev = curr;
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
        ptr->next = head;
        head->prev = ptr;
        head = ptr;
    }
    return ptr;
}

struct work_job_t* search_in_list(int client_fd, struct work_job_t **prev, struct work_job_t* head, struct work_job_t* curr)
{
    struct work_job_t *ptr = head;
    struct work_job_t *tmp = NULL;
    bool found = false;

    printf("\n Searching the list for client_fd [%d] \n",client_fd);

    while(ptr != NULL)
    {
        if(ptr->client_fd == client_fd)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if(true == found)
    {
        if(prev)

            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

int delete_from_list(int client_fd, struct work_job_t* head, struct work_job_t* curr)
{
    struct work_job_t *prev = NULL;
    struct work_job_t *del = NULL;

    printf("\n Deleting client_fd [%d] from list\n",client_fd);

    del = search_in_list(client_fd,&prev, head, curr);

    if(del == NULL)
    {
        return -1;
    }

    while(del){

      if(prev != NULL)
          prev->next = del->next;
          del->next->prev = del->prev;

      if(del == curr)
      {
          curr = prev;
      }
      else if(del == head)
      {
          head = del->next;
          del->next->prev = NULL;
      }

      free(del);
      del = search_in_list(client_fd,&prev, head, curr);
    }
    del = NULL;

    return 0;
}

void print_list(struct work_job_t* head, struct work_job_t* curr)
{
    struct work_job_t *ptr = head;

    printf("\n -------Printing list Start------- \n");
    while(ptr != NULL)
    {
        printf("\n id [%d] client_fd [%d]\n",ptr->id, ptr->client_fd);
        ptr = ptr->next;
    }
    printf("\n -------Printing list End------- \n");

    return;
}
