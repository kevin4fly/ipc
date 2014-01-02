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

    if( mq_unlink(argv[1]) != 0 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    return 0;
}
