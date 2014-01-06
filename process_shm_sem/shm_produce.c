#include  "shm_queue.h"

int main(int argc, char *argv[])
{
    if( argc != 4 )
    {
        fprintf(stderr,"usage: %s <-n massage_times> name\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int opt = 0;
    int nloops = 0;
    while( (opt = getopt(argc,argv,"n:")) != -1 )
    {
        switch ( opt )
        {
            case 'n' :
                nloops = atoi(optarg);
                break;
            default : /* '?' */
                fprintf(stderr,"usage: %s <-n massage_times> name\n",argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if( optind >= argc )
    {
        fprintf(stderr,"usage: %s <-n massage_times> name\n",argv[0]);
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

    int i = 0;
    pid_t mypid = getpid();
    while( i < nloops )
    {
        //sleep(1);
        char msg[MSG_MAXSIZE];
        sprintf(msg,"pid: %ld massage: %d\n",(long)mypid, i);
        queue_in(qu,msg);
        i++;
    }
    return 0;
}
