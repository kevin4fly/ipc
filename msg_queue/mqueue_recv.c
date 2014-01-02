#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <assert.h>
#include  <sys/types.h>
#include  <errno.h>
#include  <unistd.h>
#include  <mqueue.h>

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        fprintf(stderr,"usage: %s name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int oflags = O_RDONLY;
    mqd_t msg_queue = mq_open(argv[argc-1],oflags);
    if( msg_queue < 0 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    };

    struct mq_attr mqueue_attr;
    if( mq_getattr(msg_queue,&mqueue_attr) < 0)
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    int maxsize = mqueue_attr.mq_msgsize;
    char *massage = malloc(maxsize);
    //int priority;

    if( mq_receive(msg_queue,massage,maxsize,NULL) < 0 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    printf("received: %s\n",massage);

    free(massage);
    mq_close(msg_queue);
    return 0;
}
