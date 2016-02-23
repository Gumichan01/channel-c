

#ifndef DATA_QUEUE_H
#define DATA_QUEUE_H


struct data_t;

// Very simple queue of data
typedef struct data_queue_t
{
    int eltsize;
    struct data_t * head;
    struct data_t * tail;

} data_queue_t;


struct data_queue_t *data_queue_create(const int eltsize);
void data_queue_destroy(struct data_queue_t *queue);
int data_queue_write(struct data_queue_t *queue, const void *data);
int data_queue_read(struct data_queue_t *queue, void *data);




#endif // DATA_QUEUE_H
