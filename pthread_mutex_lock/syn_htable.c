#include  "syn_htable.h"

static int hash(int key)
{
    return key % MAGICNO;
}

void htable_init(struct hashtable **ht, void (*value_free)(void *value))
{
    /* allocate the space for the hashtable since a pointer is being
       transferred */
    *ht = malloc(sizeof(struct hashtable));
    assert(*ht);
    /* allocate the space for the pointer elements in the hash table array */
    (*ht)->htable = malloc(sizeof(struct hashnode *) * HTSIZE);
    assert((*ht)->htable);
    /* initialize all the pointer elements of the array to NULL for the sake
       of using and releasing easily */
    memset((*ht)->htable,0,(sizeof(struct hashnode *) * HTSIZE));
    /* initialize the hash table lock */
    assert(pthread_mutex_init(&((*ht)->htable_lock),NULL) == 0);
    /* register the callback function */
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

void hashlist_free(struct hashtable *ht, struct hashnode *list)
{
    struct hashnode *cur = list, *cur_next;
    /* free each node of the list */
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
        /* free the lists range from 0 to HTSIZE-1 that are still in hash
         * table when we are going to release */
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
        /* if we find the key then return the pointer */
        if( p->key == key )
        {
            pthread_mutex_unlock(&ht->htable_lock);
            return p;
        }
        p = p->next;
    }
    pthread_mutex_unlock(&ht->htable_lock);
    /* we do NOT find the key */
    return NULL;
}

/* if the new key to be added does NOT exist, create a new node and add it
 * into the hash table, else if it DO exist, the member ref_cnt is increased by 1
 *
 * */
void htable_add(struct hashtable *ht,int key, void *value)
{
    assert(ht);
    struct hashnode *entry = htable_search(ht, key);
    /* the new key to be added cannot be found in the hash table, create a new
     * node and initiate it properly*/
    if( entry == NULL )
    {
        struct hashnode *new = hashnode_new(key, value);
        int hash_key = hash(new->key);
        pthread_mutex_lock(&ht->htable_lock);
        new->next = ht->htable[hash_key];
        ht->htable[hash_key] = new;
        pthread_mutex_unlock(&ht->htable_lock);
    }
    /* the new key to be added can be found, just add the reference of it by 1 */
    else
    {
        pthread_mutex_lock(&entry->ref_cnt_lock);
        entry->ref_cnt++;
        pthread_mutex_unlock(&entry->ref_cnt_lock);
    }
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
    struct hashnode *del = ht->htable[hash_key];
    /* the corresponding key to the node of the hashtable does NOT exist  */
    if( del == NULL )
    {
        valptr = NULL;
        pthread_mutex_unlock(&ht->htable_lock);
        return 0;
    }
    /* the key can be found and it is the first node of the list */
    if( del->key == key )
    {
        if( valptr != NULL )
        {
            memcpy(valptr,del,sizeof(struct hashnode));
            valptr->value = strdup((char *)del->value);
            assert(valptr->value);
        }
        pthread_mutex_lock(&del->ref_cnt_lock);
        /* if the ref_cont is 0 then remove it from the list */
        if( --del->ref_cnt == 0 )
        {
            pthread_mutex_unlock(&del->ref_cnt_lock);
            ht->htable[hash_key] = del->next;
            hashnode_free(ht, del);
        }
        else
        {
            pthread_mutex_unlock(&del->ref_cnt_lock);
        }
        pthread_mutex_unlock(&ht->htable_lock);
        return 1;
    }
    /* the key is not the first node of the list */
    struct hashnode *pre_del = del;
    del = pre_del->next;
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
            pthread_mutex_lock(&del->ref_cnt_lock);
            if( --del->ref_cnt == 0 )
            {
                pthread_mutex_unlock(&del->ref_cnt_lock);
                pre_del->next = del->next;
                hashnode_free(ht, del);
            }
            else
            {
                pthread_mutex_unlock(&del->ref_cnt_lock);
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
