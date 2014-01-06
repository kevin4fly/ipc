#include  "shm_queue.h"

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        fprintf(stderr,"usage: %s name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    /* open the shm_fd */
    int shm_fd = shm_open(argv[argc-1],O_RDWR,0);
    if( shm_fd == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    /* open the shared memory space */
    struct shm_queue *qu = mmap(NULL,sizeof(struct shm_queue),PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
    if( qu == MAP_FAILED )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    while( 1 )
    {
        sleep(1);
        char msg[MSG_MAXSIZE];
        queue_out(qu,msg);
        puts(msg);
    }
    return 0;
}
