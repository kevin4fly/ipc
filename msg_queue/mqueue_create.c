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
    int oflags = O_RDWR | O_CREAT;
    /* u+wr,g+wr,o+wr */
    int mode = 0666;
    int opt;
    /* suppress the error massage*/
    //opterr = 0;
    while((opt = getopt(argc,argv,"e")) != -1 )
    {
        switch ( opt )
        {
            case 'e' : oflags |= O_EXCL;
                break;
            default : /* '?' */
                fprintf(stderr,"usage: %s [-e] name\n",argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if( optind != argc-1 )
    {
        fprintf(stderr,"usage: %s [-e] name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd_t msg_queue = mq_open(argv[optind],oflags,mode,NULL);
    //if mq_open is failed because it already exists, remove it and re-open it,
    //else print error information
    if( msg_queue < 0 )
    {
        if( errno == EEXIST )
        {
            if( mq_unlink(argv[optind]) < 0 )
            {
                perror("perror");
                exit(EXIT_FAILURE);
            }
            msg_queue = mq_open(argv[optind],oflags,mode,NULL);
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
