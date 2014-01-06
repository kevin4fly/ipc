#ifndef  SYN_GENERIC_QUEUE_H
#define  SYN_GENERIC_QUEUE_H

/**< generic queue implementaion with posix thread synchronization supported
 *   via posix thread mutex lock and condition variable
 *
 *   */

#include  <string.h>
#include  <stdlib.h>
#include  <assert.h>

#include  <pthread.h>

/* the size of the generic queue */
#define   QUEUE_MAX_ELEMENTS    100

/**< generic_queue: the generic queue with pthread synchronization supported
 * .queue_lock: to lock the whole queue
 * .front_rear_lock: to lock front and rear fields
 * .notfull: to test if the queue is full 
 * .notempty: to test if the queue is empty
 * .front: to store the front point of the queue 
 * .rear: to store the rear point of the queue
 * .total: to store the max elements of the queue
 * .element_size: to store the element size in the queue
 * .element_free: the callback function to release the elements in the queue
 * .queue_element: the queue space to accommodate the elements
 *
 * */
struct generic_queue
{
    pthread_mutex_t queue_lock;
    pthread_cond_t notfull;
    pthread_cond_t notempty;
    int front;
    int rear;
    int total;
    int element_size;
    void *queue_element;
    void (*element_free)(void *element);
};

void generic_queue_init(struct generic_queue **qu, const int element_size, void (*element_free)(void *element));
int generic_queue_isempty(struct generic_queue *qu);
int generic_queue_isfull(struct generic_queue *qu);
void generic_queue_in(struct generic_queue *qu, const void *entry);
void generic_queue_out(struct generic_queue *qu, void *entry);
void generic_queue_free(struct generic_queue *qu);

#endif  /*GENERIC_QUEUE_H*/
