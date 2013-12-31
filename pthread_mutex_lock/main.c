#include  "syn_htable.h"
#include  <unistd.h>

struct hashtable *ht;

void string_free(void *value)
{
    free(value);
}

void *pthread_add(void *arg)
{
    int i = (int)arg;
    if( i % 10 == 9 )
    {
        sleep(1);
    }
    char value[16] = "hello";
    char src[5];
    sprintf(src,"%d",i);
    strcat(value,src);
    puts(value);
    htable_add(ht,i,value);
    pthread_exit(0);
}

void *pthread_del(void *arg)
{
    int i = (int)arg;
    struct hashnode buffer;
    if( htable_del(ht,i,&buffer) )
    {
        printf("del %d:%s\n",buffer.key, (char *)buffer.value);
        free(buffer.value);
    }
    pthread_exit(0);
}

int main()
{
    htable_init(&ht,string_free);

    int i;
    pthread_t pthread_test[300];
    for( i=0 ; i<200 ; i++ )
    {
        pthread_create(&pthread_test[i],NULL,pthread_add,(void *)i);
    }

    for( i=0 ; i<100 ; i++ )
    {
        pthread_create(&pthread_test[i+200],NULL,pthread_del,(void *)i);
    }

    for( i=0 ; i<200 ; i++ )
    {
        struct hashnode *buffer;
        if( (buffer=htable_search(ht,i)) != NULL )
            printf("search %d:%s\n",buffer->key,(char *)buffer->value);
    }

    for( i=0 ; i<300 ; i++ )
    {
        pthread_join(pthread_test[i],NULL);
    }

    htable_destroy(ht);
    return 0;
}
