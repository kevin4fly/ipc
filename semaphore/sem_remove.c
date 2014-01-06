#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <errno.h>

#include  <semaphore.h>


int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        fprintf(stderr,"usage: %s name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    if( sem_unlink(argv[1]) != 0 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    return 0;
}
