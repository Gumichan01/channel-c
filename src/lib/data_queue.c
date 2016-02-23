

#include "data_queue.h"

#include <stdio.h>
#include <stdlib.h>


// Data node
struct data_t
{
    void * data;
    struct data_t * next;
};


void data_destroy(struct data_t *dataq)
{
    if(dataq != NULL)
    {
        data_destroy(dataq->next);
        free(dataq);
    }
}


struct data_queue_t *data_queue_create(void)
{
    struct data_queue_t *q = malloc(sizeof(struct data_queue_t));

    if(q == NULL)
        return NULL;

    q->head = NULL;
    q->tail = NULL;

    return q;
}


void data_queue_destroy(struct data_queue_t *queue)
{
    data_destroy(queue->head);
    free(queue);
}


int main(void)
{
    struct data_queue_t *q = NULL;
    q = data_queue_create();

    if(q == NULL)
    {
        perror("queue create");
        return -1;
    }

    data_queue_destroy(q);

    return 0;
}













