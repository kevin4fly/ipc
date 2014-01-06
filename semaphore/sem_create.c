#include  <unistd.h>
#include  <fcntl.h>
#include  <stdlib.h>
#include  <stdio.h>
#include  <errno.h>

#include  <semaphore.h>

int main(int argc, char *argv[])
{
    /* readable and writable and value to 1 as default settings for creating
     * semaphore
     * */
    int oflag = O_CREAT | O_RDWR, value = 1;
    /* u+wr,g+wr,o+wr */
    int mode = 0666;
    int opt;
    /* suppress the error massage*/
    //opterr = 0;
    while( (opt = getopt(argc,argv,"en:")) != -1 )
    {
        switch ( opt )
        {
            case 'e' :
                oflag |= O_EXCL;
                break;
            case 'n' :
                value = atoi(optarg);
                break;
            default : /* '?' */
                fprintf(stderr,"usage: %s [-e] [-n value] name\n",argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if( optind >= argc )
    {
        fprintf(stderr,"usage: %s [-e] [-n value] name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    //if sem_open is failed because it already exists, remove it and re-open it,
    //else print error information
    sem_t *semaphore = sem_open(argv[argc-1],oflag,mode,value);
    if( semaphore == SEM_FAILED )
    {
        if( errno == EEXIST  )
        {
            if( sem_unlink(argv[argc-1]) < 0 )
            {
                perror("perror");
                exit(EXIT_FAILURE);
            }
            semaphore = sem_open(argv[argc-1],oflag,mode);
            if( semaphore == SEM_FAILED )
            {
                perror("perror");
                exit(EXIT_FAILURE);
            }
        }
    }

    sem_close(semaphore);
    return 0;
}
