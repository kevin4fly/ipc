#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <assert.h>
#include  <errno.h>
#include  <mqueue.h>

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        fprintf(stderr,"usage: %s name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd_t msg_queue;
    struct mq_attr mqueue_attr;

    if( (msg_queue = mq_open(argv[1],O_RDONLY) ) < 0 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    if( mq_getattr(msg_queue,&mqueue_attr) < 0 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    printf("mq.maxmsg = %ld\nmq.msgsize = %ld\nmq.curmsgs = %ld\n", \
            mqueue_attr.mq_maxmsg, mqueue_attr.mq_msgsize, mqueue_attr.mq_curmsgs);
    mq_close(msg_queue);
    return 0;
}
