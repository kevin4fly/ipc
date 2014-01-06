#include  "shm_queue.h"

void queue_init(const char *shm_name)
{
    assert(shm_name);
    /* allocate shared memory descriptor */
    int oflag = O_CREAT | O_RDWR;
    int mode = 0666;
    int shm_fd = shm_open(shm_name,oflag,mode);
    if( shm_fd == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    /* allocate the space in shared memory */
    int prot = PROT_READ | PROT_WRITE;
    struct shm_queue *shm_ptr = mmap(NULL,sizeof(struct shm_queue),prot,MAP_SHARED,shm_fd,0);
    if( shm_ptr == MAP_FAILED )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd,sizeof(struct shm_queue));
    close(shm_fd);

    /* initialize the queue */
    if( sem_init(&shm_ptr->queue_mutex,1,1) == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    if( sem_init(&shm_ptr->nstored,1,0) == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    if( sem_init(&shm_ptr->nempty,1,MAX_MSG_NO) == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    shm_ptr->front = 0;
    shm_ptr->rear = 0;
    memset(shm_ptr->queue_datas,0,sizeof(shm_ptr->queue_datas));
}

int queue_isempty(struct shm_queue *qu)
{
    assert(qu);
    sem_wait(&qu->queue_mutex);
    if( qu->rear == qu->front )
    {
        sem_post(&qu->queue_mutex);
        return 1;
    }
    sem_post(&qu->queue_mutex);
    return 0;
}

int queue_isfull(struct shm_queue *qu)
{
    assert(qu);
    sem_wait(&qu->queue_mutex);
    if( (qu->front + 1) % MAX_MSG_NO == qu->rear )
    {
        sem_post(&qu->queue_mutex);
        return 1;
    }
    sem_post(&qu->queue_mutex);
    return 0;
}

void queue_in(struct shm_queue *qu, const char *msg)
{
    assert(qu);
    assert(msg);
    sem_wait(&qu->nempty);
    sem_wait(&qu->queue_mutex);
    strcpy(&qu->queue_datas[qu->front * MSG_MAXSIZE], msg);
    qu->front = (qu->front+1) % MAX_MSG_NO;
    sem_post(&qu->queue_mutex);
    sem_post(&qu->nstored);
}

void queue_out(struct shm_queue *qu, char *msgbuf)
{
    assert(qu);
    assert(msgbuf);
    sem_wait(&qu->nstored);
    sem_wait(&qu->queue_mutex);
    strcpy(msgbuf, &qu->queue_datas[qu->rear * MSG_MAXSIZE]);
    qu->rear = (qu->rear+1) % MAX_MSG_NO;
    sem_post(&qu->queue_mutex);
    sem_post(&qu->nempty);
}

void queue_getinfo(struct shm_queue *qu)
{
    assert(qu);
    sem_wait(&qu->queue_mutex);
    int empty_val = 0, stored_val = 0;
    sem_getvalue(&qu->nempty,&empty_val);
    sem_getvalue(&qu->nstored,&stored_val);
    printf("front: %d\nrear: %d\nempty: %d\nstored: %d\n",qu->front, qu->rear, empty_val, stored_val);
    sem_post(&qu->queue_mutex);
}

void queue_free(const char *shm_name)
{
    assert(shm_name);
    /* open the shared memory fd */
    int oflag = O_RDWR;
    /* mode to 0 since we don't create new shared memory fd */
    int mode = 0;
    int shm_fd = shm_open(shm_name,oflag,mode);
    if( shm_fd == -1 )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }

    /* open the shared memory space */
    int prot = PROT_READ | PROT_WRITE;
    struct shm_queue *shm_ptr = mmap(NULL,sizeof(struct shm_queue),prot,MAP_SHARED,shm_fd,0);
    if( shm_ptr == MAP_FAILED )
    {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    close(shm_fd);

    /* destroy the semaphore */
    sem_destroy(&shm_ptr->nstored);
    sem_destroy(&shm_ptr->nempty);
    sem_destroy(&shm_ptr->queue_mutex);
    /* release the shared memory space */
    munmap(shm_ptr,sizeof(struct shm_queue));
    /* unlink the shared memory descriptor */
    shm_unlink(shm_name);
}
