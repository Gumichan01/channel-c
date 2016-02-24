
#ifndef CHANNEL_H
#define CHANNEL_H

struct channel_t;

/* flags */
#define CHANNEL_PROCESS_SHARED      0x00000001
#define CHANNEL_PROCESS_BATCH       0x00000010
#define CHANNEL_PROCESS_PTR         0x00000100
#define CHANNEL_PROCESS_LOCKFREE    0x00001000
#define CHANNEL_PROCESS_NONBLOCK    0x00010000  // Non-blocking channel (bonus)
#define CHANNEL_PROCESS_SCHEDULER   0x00100000  // channel with scheduling (bonus)


struct channel_t *channel_create(int eltsize, int size, int flags);
void channel_destroy(struct channel_t *channel);
int channel_send(struct channel_t *channel, const void *data);
int channel_close(struct channel_t *channel);
int channel_recv(struct channel_t *channel, void *data);

typedef struct channel_t channel_t;

#endif // CHANNEL_H
