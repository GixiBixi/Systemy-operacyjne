#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "mqueue_lib.h"

/* ------------------------------------------------------------------ */
mqd_t MQ_Create(const char *name, long maxmsg, long msgsize, int mode)
{
    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = maxmsg;
    attr.mq_msgsize = msgsize;
    attr.mq_curmsgs = 0;

    mqd_t mqd = mq_open(name,
                        O_RDONLY | O_CREAT | O_EXCL,
                        (mode_t)mode,
                        &attr);
    if (mqd == (mqd_t)(-1)) {
        perror("MQ_Create: mq_open");
        return (mqd_t)0;
    }
    return mqd;
}

/* ------------------------------------------------------------------ */
mqd_t MQ_Open(const char *name, int oflag)
{
    mqd_t mqd = mq_open(name, oflag);
    if (mqd == (mqd_t)(-1)) {
        perror("MQ_Open: mq_open");
        return (mqd_t)0;
    }
    return mqd;
}

/* ------------------------------------------------------------------ */
int MQ_Close(mqd_t mqdes)
{
    if (mq_close(mqdes) == -1) {
        perror("MQ_Close: mq_close");
        return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
int MQ_Unlink(const char *name)
{
    if (mq_unlink(name) == -1) {
        perror("MQ_Unlink: mq_unlink");
        return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
int MQ_Send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int prio)
{
    if (mq_send(mqdes, msg_ptr, msg_len, prio) == -1) {
        perror("MQ_Send: mq_send");
        return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
ssize_t MQ_Receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *prio)
{
    ssize_t n = mq_receive(mqdes, msg_ptr, msg_len, prio);
    if (n == -1) {
        perror("MQ_Receive: mq_receive");
        return 0;
    }
    return n;
}

/* ------------------------------------------------------------------ */
int MQ_PrintAttr(mqd_t mqdes, const char *label)
{
    struct mq_attr attr;
    if (mq_getattr(mqdes, &attr) == -1) {
        perror("MQ_PrintAttr: mq_getattr");
        return 0;
    }
    printf("[%s] Atrybuty kolejki komunikatow:\n", label);
    printf("  mq_flags   = %ld\n",  attr.mq_flags);
    printf("  mq_maxmsg  = %ld  (maks. liczba komunikatow)\n",    attr.mq_maxmsg);
    printf("  mq_msgsize = %ld  (maks. rozmiar komunikatu [B])\n", attr.mq_msgsize);
    printf("  mq_curmsgs = %ld  (aktualna liczba komunikatow)\n",  attr.mq_curmsgs);
    return 1;
}