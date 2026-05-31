#ifndef MQUEUE_LIB_H
#define MQUEUE_LIB_H
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CheckError(call, msg) \
    do { if ((call) == 0) { fprintf(stderr, "CheckError: %s\n", msg); exit(EXIT_FAILURE); } } while(0)
mqd_t MQ_Create(const char *name, long maxmsg, long msgsize, int mode);
mqd_t MQ_Open(const char *name, int oflag);
int MQ_Close(mqd_t mqdes);
int MQ_Unlink(const char *name);
int MQ_Send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int prio);
ssize_t MQ_Receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *prio);
int MQ_PrintAttr(mqd_t mqdes, const char *label);

#endif /* MQUEUE_LIB_H */