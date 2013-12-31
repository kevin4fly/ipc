#include  "syn_generic_queue.h"
#include  <stdio.h>

struct generic_queue *qu;

void string_free(void *string)
{
    free(*(char **)string);
}

void *enqueue(void *arg)
{
    int i = (int)arg;
    char buffer[10];
    sprintf(buffer,"hello%d",i);
    char *name = strdup(buffer);
    generic_queue_in(qu,&name);
    pthread_exit(0);
}

void *dequeue(void *arg)
{
    char *name;
    generic_queue_out(qu,&name);
    printf("del %s\n",name);
    free(name);
    pthread_exit(0);
}

int main()
{
    int i;
    pthread_t pthread_test[300];
    generic_queue_init(&qu,sizeof(char *),string_free);

    for( i=0 ; i<150 ; i++ )
    {
        pthread_create(&pthread_test[i],NULL,enqueue,(void *)i);
    }

    for( i=150 ; i<250 ; i++ )
    {
        pthread_create(&pthread_test[i],NULL,dequeue,NULL);
    }

    for( i=0 ; i<250 ; i++ )
    {
        pthread_join(pthread_test[i],NULL);
    }

    char *name;
    while( !generic_queue_isempty(qu) )
    {
        generic_queue_out(qu,&name);
        printf("out %s\n",name);
        free(name);
    }
    //for( i=0 ; i<2 ; i++ )
    //{
    //    generic_queue_out(qu,&name);
    //    puts(name);
    //    free(name);
    //}

    generic_queue_free(qu);
    return 0;
}
