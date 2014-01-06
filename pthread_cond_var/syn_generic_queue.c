#include  "syn_generic_queue.h"

/**< generic_queue_init: initialize the generic cyclic queue
 * @qu: the queue to be initialized
 * @element_size: the size of the element of the queue
 * @element_free: the callback function to free the allocated space by the element
 *
 * */
void generic_queue_init(struct generic_queue **qu, const int element_size, void (*element_free)(void *element))
{
    assert(element_size > 0);
    /* allocate space for struct generic queue a since pointer is being
     * transferred as the parameter
     * */
    (*qu) = malloc(sizeof(struct generic_queue));
    assert(*qu);

    /* initialize the mutex lock and condition variable */
    pthread_mutex_init(&(*qu)->queue_lock,NULL);
    pthread_cond_init(&(*qu)->notfull,NULL);
    pthread_cond_init(&(*qu)->notempty,NULL);
    (*qu)->front = 0;
    (*qu)->rear = 0;
    (*qu)->total = QUEUE_MAX_ELEMENTS;
    (*qu)->element_size = element_size;

    /* allocate the space to accomodate the elements to get in and out */
    (*qu)->queue_element = malloc((*qu)->total * (*qu)->element_size);
    assert((*qu)->queue_element);

    /* register the callback function provided by the caller to free the space
     * allocated by the element itself
     * */
    (*qu)->element_free = element_free;
}

/**< generic_queue_isempty: test if the generic cyclic queue is empty
 * @qu: the queue to be tested
 *
 * */
int generic_queue_isempty(struct generic_queue *qu)
{
    assert(qu);
    return qu->front == qu->rear;
}

/**< generic_queue_isfull: test if the generic cyclic queue is full
 * @qu: the queue to be tested
 *
 * */
int generic_queue_isfull(struct generic_queue *qu)
{
    assert(qu);
    return (qu->front+1) % (qu->total) == qu->rear;
}

/**< generic_queue_in: add an element into the generic cyclic queue
 * @qu: the queue the element to be added into
 * @entry: the element to be added
 *
 * */
void generic_queue_in(struct generic_queue *qu, const void *entry)
{
    assert(qu);
    pthread_mutex_lock(&qu->queue_lock);
    /* if the queue is full, wait until the generic_queue_out is called to
     * dequeue an element and then send a signal
     *
     * */
    while( generic_queue_isfull(qu) )
    {
        pthread_cond_wait(&qu->notfull,&qu->queue_lock);
    }
    void *dest_addr = (char *)qu->queue_element + qu->front * qu->element_size;
    memcpy(dest_addr,entry,qu->element_size);
    qu->front=(qu->front+1)%qu->total;
    pthread_mutex_unlock(&qu->queue_lock);
    pthread_cond_signal(&qu->notempty);
}

/**< generic_queue_out: delete an element from the generic cyclic queue
 * @qu: the queue the element to be deleted from
 * @entry: the space the deleted element to be saved
 *
 * */
void generic_queue_out(struct generic_queue *qu, void *entry)
{
    assert(qu);
    pthread_mutex_lock(&qu->queue_lock);
    /* if the queue is empty, wait until the generic_queue_out is called to
     * enqueue an element and then send a signal
     *
     * */
    while( generic_queue_isempty(qu) )
    {
        pthread_cond_wait(&qu->notempty,&qu->queue_lock);
    }
    void *src_addr =  (char *)qu->queue_element + qu->element_size * qu->rear;
    //if the entry not NULL, save the dequeued element into it
    if( entry != NULL )
    {
        memcpy(entry,src_addr,qu->element_size);
    }
    qu->rear=(qu->rear+1)%qu->total;
    pthread_mutex_unlock(&qu->queue_lock);
    pthread_cond_signal(&qu->notfull);
}

/**< generic_queue_free: free the generic queue
 * @qu: the queue to be freed
 *
 * */
void generic_queue_free(struct generic_queue *qu)
{
    assert(qu);
    int index;
    if( qu->element_free != NULL )
    {
        pthread_mutex_lock(&qu->queue_lock);
        //release all the spaces that still exist in the queue
        for( index=qu->rear; index != qu->front; index=(index+1)%qu->total )
        {
            void *element_addr = (char *)qu->queue_element + qu->element_size * index;
            qu->element_free(element_addr);
        }
    }
    free(qu->queue_element);
    pthread_mutex_unlock(&qu->queue_lock);

    //assert if we destroy them unsuccessfully
    assert(pthread_mutex_destroy(&qu->queue_lock) == 0);
    assert(pthread_cond_destroy(&qu->notfull) == 0);
    assert(pthread_cond_destroy(&qu->notempty) == 0);
    free(qu);
}
