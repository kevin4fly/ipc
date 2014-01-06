#include  <unistd.h>
#include  <stdlib.h>
#include  <stdio.h>
#include  <errno.h>
#include  <sys/stat.h>
#include  <fcntl.h>

#include  <sys/mman.h>

int main(int argc, char *argv[])
{
    /* readable and writable and value to 1 as default settings for creating
     * semaphore
     * */
    int oflag = O_CREAT | O_RDWR, length= 0;
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
                length = atoi(optarg);
                break;
            default : /* '?' */
                fprintf(stderr,"usage: %s [-e] <-n length> name\n",argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if( optind >= argc )
    {
        fprintf(stderr,"usage: %s [-e] <-n length> name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    /* if shm_open is failed because it already exists, remove it and re-open it,
     * else print error information
     * */
    int shm_fd = shm_open(argv[argc-1],oflag,mode);
    if( shm_fd == -1 )
    {
        if( errno == EEXIST  )
        {
            if( shm_unlink(argv[argc-1]) == -1 )
            {
                perror("perror");
                exit(EXIT_FAILURE);
            }
            shm_fd = shm_open(argv[argc-1],oflag,mode);
            if( shm_fd == -1 )
            {
                perror("perror");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* set the length of the shared memory space */
    if( ftruncate(shm_fd,length) == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    /* allocate the space */
    void *mm_ptr = mmap(NULL,length,PROT_READ| PROT_WRITE,MAP_SHARED,shm_fd,0);
    if( mm_ptr == MAP_FAILED )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);
    return 0;
}
