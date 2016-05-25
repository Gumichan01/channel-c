#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "../src/channel.h"

#ifndef N
#define N 10000
#endif
#ifndef M
#define M 100
#endif
#ifndef BIG_SIZE
#define BIG_SIZE 4096
#endif

#define TIMER_START clock_gettime(CLOCK_MONOTONIC, &t0)
#define TIMER_STOP clock_gettime(CLOCK_MONOTONIC, &t1);
#define TIME \
    (((double)t1.tv_sec - t0.tv_sec) + \
     ((double)t1.tv_nsec - t0.tv_nsec) / 1.0E9)


static inline void
nap()
{
    struct timespec ts = {0, 10 * 1000 * 1000};
    nanosleep(&ts, NULL);
}

void *
writer(void *arg)
{
    struct channel *chan = *(struct channel**)arg;
    int rc;

    for(int i = 0; i < N; i++) {
        rc = channel_send(chan, &i);
        if(rc <= 0)
            perror("channel_send");
    }
    return NULL;
}

void *
writer_close(void *arg)
{
    struct channel *chan = *(struct channel**)arg;
    int rc;

    for(int i = 0; i < N; i++) {
        rc = channel_send(chan, &i);
        if(rc <= 0)
            perror("channel_send");
    }
    channel_close(chan);
    return NULL;
}

void *
writer_sleep_close(void *arg)
{
    struct channel *chan = *(struct channel**)arg;
    int rc;

    for(int i = 0; i < N; i++) {
        if(i % (N / 20) == 0)
            nap();
        rc = channel_send(chan, &i);
        if(rc <= 0)
            perror("channel_send");
    }
    channel_close(chan);
    return NULL;
}

void *
reader(void *arg)
{
    struct channel *chan = *(struct channel**)arg;
    int i, rc;
    long int s = 0;

    while(1) {
        rc = channel_recv(chan, &i);
        if(rc <= 0) {
            if(rc < 0)
                perror("channel_recv");
            break;
        }
        s += i;
    }
    return (void*)s;
}

void *
echo_int(void *arg)
{
    struct channel **chans = *(struct channel ***)arg;

    while(1) {
        int rc, val;
        rc = channel_recv(chans[0], &val);
        if(rc <= 0) {
            if(rc < 0)
                perror("channel_recv");
            return NULL;
        }
        rc = channel_send(chans[1], &val);
        if(rc <= 0) {
            perror("channel_send");
            return NULL;
        }
    }
}

void *
echo_big(void *arg)
{
    struct channel **chans = *(struct channel ***)arg;
    void *big = malloc(BIG_SIZE);
    if(big == NULL) {
        perror("malloc");
        return NULL;
    }

    while(1) {
        int rc;
        rc = channel_recv(chans[0], big);
        if(rc <= 0) {
            if(rc < 0)
                perror("channel_recv");
            free(big);
            return NULL;
        }
        rc = channel_send(chans[1], big);
        if(rc <= 0) {
            perror("channel_send");
            free(big);
            return NULL;
        }
    }
}

int
main()
{
    struct channel *chan, *chans[2], **chansp = chans;
    pthread_t thr0, thr1;
    int n;
    long int s;
    void *p1, *p2;
    int rc, v, ok;
    int one = 1;
    unsigned char *big1, *big2;
    struct timespec t0, t1;

    setvbuf(stdout, NULL, _IONBF, 0);

    printf("Basic test... ");
    TIMER_START;
    chan = channel_create(sizeof(int), M, 0);
    if(chan == NULL) {
        perror("channel_create");
        exit(1);
    }
    pthread_create(&thr0, NULL, writer_close, &chan);
    n = 0;
    s = 0;
    while(1) {
        int j;
        rc = channel_recv(chan, &j);
        if(rc < 0)
            perror("channel_recv");
        if(rc == 0)
            break;
        n++;
        s += j;
    }
    pthread_join(thr0, NULL);
    channel_destroy(chan);
    TIMER_STOP;

    if(n == N && s == (long)N * (N - 1) / 2)
        printf("success (%lfs, %lfµs).\n", TIME, TIME / N * 1E6);
    else
        printf("failure: %d %ld.\n", n, s);


    printf("Test using two writers... ");
    TIMER_START;
    chan = channel_create(sizeof(int), M, 0);
    if(chan == NULL) {
        perror("channel_create");
        exit(1);
    }
    pthread_create(&thr0, NULL, writer, &chan);
    pthread_create(&thr1, NULL, writer, &chan);
    s = 0;
    for(int i = 0; i < 2 * N; i++) {
        int j;
        rc = channel_recv(chan, &j);
        if(rc <= 0)
            perror("channel_recv");
        s += j;
    }
    pthread_join(thr0, NULL);
    pthread_join(thr1, NULL);
    channel_destroy(chan);
    TIMER_STOP;

    if(s == 2L * N * (N - 1) / 2)
        printf("success (%lfs, %lfµs).\n", TIME, TIME / N * 1E6);
    else
        printf("failure: %ld.\n", s);


    printf("Test using two readers... ");
    TIMER_START;
    chan = channel_create(sizeof(int), M, 0);
    if(chan == NULL) {
        perror("channel_create");
        exit(1);
    }
    pthread_create(&thr0, NULL, reader, &chan);
    pthread_create(&thr1, NULL, reader, &chan);
    for(int i = 0; i < 2 * N; i++) {
        rc = channel_send(chan, &i);
        if(rc <= 0)
            perror("channel_recv");
    }
    channel_close(chan);
    pthread_join(thr0, &p1);
    pthread_join(thr1, &p2);
    channel_destroy(chan);
    TIMER_STOP;

    s = (long)p1 + (long)p2;
    if(s == N * (2L * N - 1))
        printf("success (%lfs, %lfµs).\n", TIME, TIME / N * 1E6);
    else
        printf("failure: %ld.\n", s);


    printf("Test using a closed channel... ");
    ok = 1;
    chan = channel_create(sizeof(int), M, 0);
    if(chan == NULL) {
        perror("channel_create");
        exit(1);
    }
    rc = channel_send(chan, &one);
    if(rc < 0) {
        perror("channel_send");
        ok = 0;
    }
    channel_close(chan);
    rc = channel_recv(chan, &v);
    if(rc < 0 || v != 1) {
        perror("channel_recv");
        ok = 0;
    }
    rc = channel_send(chan, &one);
    if(rc != -1 || errno != EPIPE) {
        fprintf(stderr, "channel_send(closed): %d %d\n", rc, errno);
        ok = 0;
    }
    rc = channel_recv(chan, &v);
    if(rc != 0) {
        fprintf(stderr, "channel_recv(closed): %d %d\n", rc, errno);
        ok = 0;
    }
    channel_destroy(chan);
    if(ok)
        printf("success (%lfs, %lfµs).\n", TIME, TIME / N * 1E6);
    else
        printf("failure.\n");


    printf("Test on a full channel (write data)... ");
    chan = channel_create(sizeof(int), M, 0);
    if(chan == NULL) {
        perror("channel_create");
        exit(1);
    }
    pthread_create(&thr0, NULL, writer_close, &chan);
    n = 0;
    s = 0;
    while(1) {
        int j;
        if(n % (N / 20) == 0)
            nap();
        rc = channel_recv(chan, &j);
        if(rc < 0)
            perror("channel_recv");
        if(rc == 0)
            break;
        n++;
        s += j;
    }
    pthread_join(thr0, NULL);
    channel_destroy(chan);

    if(n == N && s == (long)N * (N - 1) / 2)
        printf("success.\n");
    else
        printf("failure: %d %ld.\n", n, s);

    printf("Test on an empty channel (read data)... ");
    chan = channel_create(sizeof(int), M, 0);
    if(chan == NULL) {
        perror("channel_create");
        exit(1);
    }
    pthread_create(&thr0, NULL, writer_sleep_close, &chan);
    n = 0;
    s = 0;
    while(1) {
        int j;
        rc = channel_recv(chan, &j);
        if(rc < 0)
            perror("channel_recv");
        if(rc == 0)
            break;
        n++;
        s += j;
    }
    pthread_join(thr0, NULL);
    channel_destroy(chan);

    if(n == N && s == (long)N * (N - 1) / 2)
        printf("success.\n");
    else
        printf("failure: %d %ld.\n", n, s);

    for(int synchrone = 0; synchrone < 2; synchrone++) {
        printf("Test echo%s... ", synchrone ? " synchronous channel" : "");
        TIMER_START;
        chans[0] = channel_create(sizeof(int), synchrone ? 0 : M, 0);
        if(chans[0] == NULL) {
            if(synchrone && errno == EINVAL) {
                printf("not implemented.\n");
                break;
            }
            perror("channel_create");
            exit(1);
        }
        chans[1] = channel_create(sizeof(int), synchrone ? 0 : M, 0);
        if(chans[1] == NULL) {
            perror("channel_create");
            exit(1);
        }
        pthread_create(&thr0, NULL, echo_int, &chansp);
        ok = 1;
        for(int i = 0; i < N; i++) {
            int j;
            rc = channel_send(chans[0], &i);
            if(rc <= 0) {
                perror("channel_send");
                exit(1);
            }
            rc = channel_recv(chans[1], &j);
            if(rc <= 0) {
                perror("channel_recv");
                exit(1);
            }
            if(i != j)
                ok = 0;
        }
        channel_close(chans[0]);
        pthread_join(thr0, NULL);
        channel_destroy(chans[0]);
        channel_destroy(chans[1]);
        TIMER_STOP;

        if(ok)
            printf("success (%lfs, %lfµs).\n", TIME, TIME / N * 1E6);
        else
            printf("failure.\n");


        printf("Test echo big%s... ", synchrone ? " synchronous" : "");
        TIMER_START;
        big1 = malloc(BIG_SIZE);
        if(big1 == NULL) {
            perror("malloc");
            exit(1);
        }
        big2 = malloc(BIG_SIZE);
        if(big1 == NULL) {
            perror("malloc");
            exit(1);
        }
        chans[0] = channel_create(BIG_SIZE, synchrone ? 0 : M, 0);
        if(chans[0] == NULL) {
            perror("channel_create");
            exit(1);
        }
        chans[1] = channel_create(BIG_SIZE, synchrone ? 0 : M, 0);
        if(chans[1] == NULL) {
            perror("channel_create");
            exit(1);
        }
        pthread_create(&thr0, NULL, echo_big, &chansp);
        ok = 1;
        for(int i = 0; i < N; i++) {
            big1[i % BIG_SIZE] = (i & 0xFF);
            rc = channel_send(chans[0], big1);
            if(rc <= 0) {
                perror("channel_send");
                exit(1);
            }
            rc = channel_recv(chans[1], big2);
            if(rc <= 0) {
                perror("channel_recv");
                exit(1);
            }
            if(big2[i % BIG_SIZE] != (i & 0xFF))
                ok = 0;
        }
        channel_close(chans[0]);
        pthread_join(thr0, NULL);
        channel_destroy(chans[0]);
        channel_destroy(chans[1]);
        free(big1);
        free(big2);
        TIMER_STOP;

        if(ok)
            printf("success (%lfs, %lfµs).\n", TIME, TIME / N * 1E6);
        else
            printf("failure.\n");
    }

    return 0;
}
