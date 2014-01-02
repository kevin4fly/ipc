#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <string.h>
#include  <assert.h>
#include  <errno.h>
#include  <mqueue.h>

int main(int argc, char *argv[])
{
    int oflags = O_RDWR | O_CREAT;
    /* u+wr,g+wr,o+wr */
    int mode = 0666;
    int opt;

    mqd_t msg_queue;
    struct mq_attr mqueue_attr;
    memset(&mqueue_attr,0,sizeof(struct mq_attr));

    /* suppress the error massage*/
    //opterr = 0;
    while( (opt=getopt(argc,argv,"em:z:")) != -1 )
    {
        switch ( opt )
        {
            case 'e' :
                oflags |= O_EXCL;
                break;
            case 'm' :
                mqueue_attr.mq_maxmsg = atoi(optarg);
                break;
            case 'z' :
                mqueue_attr.mq_msgsize = atoi(optarg);
                break;
            default : /* '?' */
                fprintf(stderr,"usage: %s [-e] [-m maxmsg -z maxsize] name\n",argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if( optind >= argc )
    {
        fprintf(stderr,"usage: %s [-e] [-m maxmsg -z maxsize] name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* if maxmsg and maxsize are omitted, set them to default */
    mqueue_attr.mq_maxmsg = mqueue_attr.mq_maxmsg > 0 ? mqueue_attr.mq_maxmsg : 10;
    mqueue_attr.mq_msgsize = mqueue_attr.mq_msgsize > 0 ? mqueue_attr.mq_msgsize : 8192;

    if( (msg_queue=mq_open(argv[argc-1],oflags,mode,&mqueue_attr)) < 0 )
    {
        if( errno == EEXIST )
        {
            mq_unlink(argv[argc-1]);
            msg_queue = mq_open(argv[argc-1],oflags,mode,&mqueue_attr);
        }
        if( msg_queue < 0 )
        {
            perror("perror");
            exit(EXIT_FAILURE);
        }
    }

    mq_close(msg_queue);
    return 0;
}
