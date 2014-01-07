#include  "syn_htable.h"
#include  <unistd.h>

struct hashtable *ht;

void string_free(void *value)
{
    free(value);
}

void *pthread_add(void *arg)
{
    int loops = (int)arg;
    int i;
    for( i=0 ; i<loops; i++ )
    {
        char value[20];
        sprintf(value,"hello: %d\n",i);
        puts(value);
        htable_add(ht,i,value);
    }
    pthread_exit(0);
}

void *pthread_del(void *arg)
{
    int i = 0;
    while( i < 3 )
    {
        int j;
        for( j= 0 ; j<30 ; j++ )
        {
            sleep(1);
            struct hashnode buffer;
            if( htable_del(ht,j,&buffer) )
            {
                printf("del %d:%s\n",buffer.key, (char *)buffer.value);
                free(buffer.value);
            }
        }
        i++;
    }
    pthread_exit(0);
}

int main()
{
    htable_init(&ht,string_free);

    pthread_t producer[3], consumer;
    pthread_create(&producer[0], NULL,pthread_add,(void *)30);
    pthread_create(&producer[1], NULL,pthread_add,(void *)30);
    pthread_create(&producer[2], NULL,pthread_add,(void *)30);
    pthread_create(&consumer,NULL,pthread_del,NULL);

    pthread_join(producer[0],NULL);
    pthread_join(producer[1],NULL);
    pthread_join(producer[2],NULL);
    pthread_join(consumer,NULL);
    htable_destroy(ht);
    return 0;
}
