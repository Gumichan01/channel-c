# Library Documentation #

## Data queue ##

*data_queue* is a simple implementation of a queue used by the channel.

Example of use:

    int main(void)
    {
        int p,i;
        struct data_queue_t *q = NULL;

        q = data_queue_create(sizeof(int));

        if(q == NULL)
        {
            perror("queue create");
            return -1;
        }

        // Write data
        for(i = 0; i < 5; i++)
        {
            p = i+1;
            data_queue_write(q,&p);
            printf("sent data %d\n", p);
        }

        // Read data
        while(data_queue_read(q,&p) == 1)
        {
            printf("received data %d\n", p);
        }

        data_queue_destroy(q);
        return 0;
    }
