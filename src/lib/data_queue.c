

#include "data_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Data node
struct data_t
{
    void * data;
    struct data_t * next;
};


/// Private functions

struct data_t *data_create(const int sz);
void data_destroy(struct data_t *dataq);


struct data_t *data_create(const int sz)
{
    struct data_t *d = malloc(sizeof(struct data_t));

    if(d == NULL)
        return NULL;

    d->next = NULL;
    d->data = malloc(sz);

    if(d->data == NULL)
    {
        data_destroy(d);
        return NULL;
    }

    return d;
}


void data_destroy(struct data_t *dataq)
{
    if(dataq != NULL)
    {
        data_destroy(dataq->next);
        free(dataq);
    }
}

void data_set_tail(struct data_queue_t *queue, struct data_t *d)
{
    queue->tail->next = d;
    queue->tail = queue->tail->next;
}


/// Public functions

struct data_queue_t *data_queue_create(const int eltsize)
{
    struct data_queue_t *q = malloc(sizeof(struct data_queue_t));

    if(q == NULL)
        return NULL;

    q->eltsize = eltsize;
    q->head = NULL;
    q->tail = NULL;

    return q;
}


void data_queue_destroy(struct data_queue_t *queue)
{
    if(queue != NULL)
    {
        data_destroy(queue->head);
        free(queue);
    }
}

/*
    Add a data in the channel
    return value : 0 on success, -1 on failure
*/
int data_queue_write(struct data_queue_t *queue, const void *data)
{
    struct data_t *d = NULL;

    if(queue == NULL && data == NULL)
        return -1;

    d = data_create(queue->eltsize);

    if(d == NULL)
        return -1;

    d->next = NULL;
    memcpy(d->data,data,queue->eltsize);

    if(queue->tail != NULL)
    {
        data_set_tail(queue,d);
    }
    else
    {
        if(queue->head == queue->tail)
        {
            // In this case head == tail == null. Normal case
            queue->head = d;
            queue->tail = queue->head;
        }
        else
        {
            // Critical error, must not be reached
            free(d);
            abort();    // By (head == tail == NULL)
        }
    }

    return 0;
}

/*
    Receive from the queue
    returnn value : 1 - data extracted, 0 - no data, -1 - failure
*/
int data_queue_read(struct data_queue_t *queue, void *data)
{
    struct data_t *d = NULL;

    if(queue == NULL && data == NULL)
        return -1;

    if(queue->head == NULL)
        return 0;       // No data available

    memcpy(data,queue->head->data,queue->eltsize);

    // Destroy the head, it was read
    d = queue->head;
    queue->head = queue->head->next;
    free(d);

    if(queue->head == NULL)
    {
        // There was on data
        queue->tail = queue->head;
    }

    return 1;
}



int main(void)
{
    int p;
    int i;
    struct data_queue_t *q = NULL;

    q = data_queue_create(sizeof(int));

    if(q == NULL)
    {
        perror("queue create");
        return -1;
    }


    for(i = 0; i < 5; i++)
    {
        p = i+1;
        data_queue_write(q,&p);
        printf("sent data %d\n", p);
    }

    p = 0;  // dump value

    while(data_queue_read(q,&p) == 1)
    {
        printf("received data %d\n", p);
    }

    data_queue_destroy(q);

    return 0;
}













