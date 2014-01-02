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
    int oflags = O_WRONLY;
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
    int priority=0;

    int opt;
    /* suppress the error massage*/
    //opterr = 0;
    while((opt = getopt(argc,argv,"m:p:")) != -1 )
    {
        switch ( opt )
        {
            case 'm' :
                strcpy(massage,optarg);
                break;
            case 'p' :
                priority = atoi(optarg);
                break;
            default : /* '?' */
                fprintf(stderr,"usage: %s <-m massage> [-p priority] name\n",argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if( optind >=  argc )
    {
        fprintf(stderr,"usage: %s <-m massage> [-p priority] name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    if( mq_send(msg_queue,massage,maxsize,priority) < 0)
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    free(massage);
    mq_close(msg_queue);
    return 0;
}
