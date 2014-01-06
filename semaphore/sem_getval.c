#include  <stdio.h>
#include  <stdlib.h>
#include  <fcntl.h>

#include  <semaphore.h>

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        fprintf(stderr,"usage: %s name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    sem_t *semaphore = sem_open(argv[1],O_RDONLY);
    if( semaphore == SEM_FAILED )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    int val=0;
    if( sem_getvalue(semaphore,&val) == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    printf("semaphore value: %d\n",val);

    sem_close(semaphore);
    return 0;
}
