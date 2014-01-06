#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/stat.h>
#include  <fcntl.h>
#include  <unistd.h>

#include  <sys/mman.h>

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        fprintf(stderr,"usage: %s name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat shm_stat;

    /* open the exist shared memory descriptor */
    int shm_fd = shm_open(argv[1],O_RDONLY,0);
    if( shm_fd == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    /* get the shared memory space info */
    fstat(shm_fd,&shm_stat);

    printf("shared memory space: %ld\n",shm_stat.st_size);

    close(shm_fd);
    return 0;
}
