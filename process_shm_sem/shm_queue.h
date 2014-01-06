#ifndef  SHM_QUEUE_H
#define  SHM_QUEUE_H

/**< an queue in the shared memory with semaphore supported to synchronize
 * datas among process
 *
 * */
#include  <stdio.h>
#include  <stdlib.h>
#include  <fcntl.h>
#include  <string.h>
#include  <assert.h>
#include  <unistd.h>

#include  <sys/mman.h>
#include  <semaphore.h>

/* max massage size in the shared memory queue */
#define   MSG_MAXSIZE    128
/* max massage numbers in the shared memory queue */
#define   MAX_MSG_NO     100

/**< shm_queue: the queue created in the shared memory with semaphore suppored
 * to synchronize it
 * .queue_mutex: to lock the whole queue
 * .nstored: to indicate fullfilled elements int the queue
 * .nempty: to indicate empty elements in the queue 
 * .front: to store the front point of the queue
 * .rear: to store the rear point of the queue
 * .queue_datas: to store the all massage datas of the queue
 *
 * */

struct shm_queue
{
    sem_t queue_mutex;
    sem_t nstored;
    sem_t nempty;
    int front;
    int rear;
    char queue_datas[MSG_MAXSIZE * MAX_MSG_NO];
};

void queue_init(const char *shm_name);
int queue_isempty(struct shm_queue *qu);
int queue_isfull(struct shm_queue *qu);
void queue_in(struct shm_queue *qu, const char *msg);
void queue_out(struct shm_queue *qu, char *msgbuf);
void queue_getinfo(struct shm_queue *qu);
void queue_free(const char *shm_name);

#endif  /*SHM_QUEUE_H*/
