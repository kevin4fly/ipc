#include  "syn_htable.h"

int hash(int key)
{
    return key % MAGICNO;
}

void htable_init(struct hashtable **ht, void (*value_free)(void *value))
{
    //alloc space for the hashtable
    *ht = malloc(sizeof(struct hashtable));
    assert(*ht);
    //alloc space for the hashnode pointer array of the hashtable
    (*ht)->htable = malloc(sizeof(struct hashnode *) * HTSIZE);
    assert((*ht)->htable);
    //set all the elements of hashnode pointer array to NULL
    memset((*ht)->htable,0,(sizeof(struct hashnode *) * HTSIZE));
    //initiate the posix thread mutex lock
    assert(pthread_mutex_init(&((*ht)->htable_lock),NULL) == 0);
    //register the callback function
    (*ht)->value_free = value_free;
}

struct hashnode *hashnode_new(int key, void *value)
{
    struct hashnode *new = malloc(sizeof(struct hashnode));
    assert(new);
    new->key = key;
    new->value = strdup(value);
    assert(new->value);
    assert(pthread_mutex_init(&new->ref_cnt_lock,NULL) == 0 );
    new->ref_cnt = 1;
    new->next = NULL;
    return new;
}

void hashnode_free(struct hashtable *ht, struct hashnode *entry)
{
    assert(ht);
    assert(entry);
    /* we are able to call ht->value_free only if it is NOT NULL, since
     * sometimes we don't have things to release then we give a NULL parameter
     * to htable_init function
     */
    if( ht->value_free != NULL )
    {
        ht->value_free(entry->value);
    }
    assert(pthread_mutex_destroy(&entry->ref_cnt_lock) == 0);
    free(entry);
}

static void hashlist_free(struct hashtable *ht, struct hashnode *list)
{
    struct hashnode *cur = list, *cur_next;
    while( cur != NULL )
    {
        cur_next = cur->next;
        hashnode_free(ht, cur);
        cur = cur_next;
    }
}

void htable_destroy(struct hashtable *ht)
{
    assert(ht);
    int i;
    pthread_mutex_lock(&ht->htable_lock);
    for( i=0 ; i<HTSIZE ; i++ )
    {
        if( ht->htable[i] != NULL )
        {
            hashlist_free(ht, ht->htable[i]);
        }
    }
    pthread_mutex_unlock(&ht->htable_lock);
    assert(pthread_mutex_destroy(&ht->htable_lock) == 0);
    free(ht->htable);
    free(ht);
}

struct hashnode *htable_search(struct hashtable *ht, int key)
{
    assert(ht);
    int hash_key = hash(key);
    pthread_mutex_lock(&ht->htable_lock);
    struct hashnode *p = ht->htable[hash_key];
    while( p != NULL )
    {
        if( p->key == key )
        {
            pthread_mutex_unlock(&ht->htable_lock);
            return p;
        }
        p = p->next;
    }
    pthread_mutex_unlock(&ht->htable_lock);
    return NULL;
}

/* if the new key to be added does NOT exist, create a new node and add it
 * into the hash table, else if it DO exist, the member ref_cnt increases by 1
 *
 * */
void htable_add(struct hashtable *ht,int key, void *value)
{
    assert(ht);
    struct hashnode *entry = htable_search(ht, key);
    if( entry == NULL )
    {
        struct hashnode *new = hashnode_new(key, value);
        int hash_key = hash(new->key);
        pthread_mutex_lock(&ht->htable_lock);
        new->next = ht->htable[hash_key];
        ht->htable[hash_key] = new;
    }
    else
    {
        pthread_mutex_lock(&entry->ref_cnt_lock);
        pthread_mutex_unlock(&ht->htable_lock);
        entry->ref_cnt++;
        pthread_mutex_lock(&entry->ref_cnt_lock);
    }
    pthread_mutex_unlock(&ht->htable_lock);
}

/* if the key to be deleted does NOT exist, return 0 to indicate that delete
 * failed. otherwise, if the reference count larger than 1, decreasing it by
 * 1, else remove the hashnode from the list
 *
 * */
int htable_del(struct hashtable *ht, int key, struct hashnode *valptr)
{
    assert(ht);
    int hash_key = hash(key);
    pthread_mutex_lock(&ht->htable_lock);
    struct hashnode *pre_del = ht->htable[hash_key];
    if( pre_del == NULL )
    {
        valptr = NULL;
        pthread_mutex_unlock(&ht->htable_lock);
        return -1;
    }
    if( pre_del->key == key )
    {
        if( valptr != NULL )
        {
            memcpy(valptr,pre_del,sizeof(struct hashnode));
            valptr->value = strdup((char *)pre_del->value);
            assert(valptr->value);
        }
        if( --pre_del->ref_cnt == 0 )
        {
            ht->htable[hash_key] = pre_del->next;
            hashnode_free(ht, pre_del);
        }
        pthread_mutex_unlock(&ht->htable_lock);
        return 1;
    }
    struct hashnode *del = pre_del->next;
    while( del != NULL )
    {
        if( del->key == key )
        {
            if( valptr != NULL )
            {
                memcpy(valptr,del,sizeof(struct hashnode));
                valptr->value = strdup((char *)del->value);
                assert(valptr->value);
            }
            if( --del->ref_cnt == 0 )
            {
                pre_del->next = del->next;
                hashnode_free(ht, del);
            }
            pthread_mutex_unlock(&ht->htable_lock);
            return 1;
        }
        pre_del = del;
        del = pre_del->next;
    }
    valptr = NULL;
    pthread_mutex_unlock(&ht->htable_lock);
    return 0;
}
