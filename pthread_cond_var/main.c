#include  "syn_generic_queue.h"
#include  <stdio.h>
#include  <unistd.h>

struct generic_queue *qu;

void string_free(void *string)
{
    free(*(char **)string);
}

void *enqueue(void *arg)
{
    int i, nloops = (int)arg;
    for(  i= 0; i<nloops ; i++ )
    {
        char buffer[10];
        sprintf(buffer,"hello%d",i);
        char *name = strdup(buffer);
        generic_queue_in(qu,&name);
    }
    pthread_exit(0);
}

void *dequeue(void *arg)
{
    while( 1 )
    {
        sleep(1);
        char *name;
        generic_queue_out(qu,&name);
        printf("del %s\n",name);
        free(name);
        pthread_mutex_lock(&qu->queue_lock);
        if( generic_queue_isempty(qu))
        {
            pthread_mutex_unlock(&qu->queue_lock);
            break;
        }
        pthread_mutex_unlock(&qu->queue_lock);
    }
    pthread_exit(0);
}

int main()
{
    int i = 30;
    pthread_t producer, consumer;
    generic_queue_init(&qu,sizeof(char *),string_free);

    pthread_create(&producer, NULL,enqueue,(void *)i);
    pthread_create(&consumer,NULL,dequeue,NULL);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    generic_queue_free(qu);
    return 0;
}
