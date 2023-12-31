/* ----------- Symbol Table hash table implementation --------------- */

#include "symtable.h"

/* possible total bucket counts */
static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
    16381, 32749, 65521};
/* number of bucket counts available */
static const size_t numBucketCounts = sizeof(auBucketCounts) / 
    sizeof(auBucketCounts[0]);


/* Each item is stores in a binding. Bindings make a linked
list for each bucket in Table */
struct Binding
{
    /* unique key (string)*/
    const char *key;
    /* the item (any type) */
    const void *item;
    /* address of next binding */
    struct Binding *next;
};

/* Pointer to a Binding object (local typedef) */
typedef struct Binding *Binding_T;

/* A Table is struct that points to an array of pointers to bindings */
struct SymTable
{
    /* current bucket count index */
    size_t iBucket;
    /* pointer to (the head of array of) Binding_T */
    Binding_T *buckets;
    /* Total number of bindings */
    size_t totBins;
};


/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}


SymTable_T SymTable_new(void) {
    /* Create pointer to the table object */
    SymTable_T oSymTable;
    /* Size of bucket array */
    size_t size;
    /* counter */
    size_t i = 0;

    /* Allocate memory for table head */
    oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));

    if (oSymTable == NULL) { 
        return NULL;
    }

    /* initialize members */
    oSymTable->iBucket = 0;
    oSymTable->totBins = 0;

    /* get number of buckets to be created */
    size = auBucketCounts[oSymTable->iBucket];

    /* Allocate memory for array of bucket heads */
    oSymTable->buckets = (Binding_T*) malloc(sizeof(Binding_T) * size);

    if (oSymTable->buckets == NULL) {
        /* free previously allocated memory */
        free(oSymTable);
        return NULL;
    }

    /* Initialize bucket heads to NULL */
    for (; i < size ; i++) {
        oSymTable->buckets[i] = NULL;
    }

    return oSymTable;
}


void SymTable_free(SymTable_T oSymTable) {
    
    Binding_T curr_bucket, curr_bin, next_bin;
    size_t i = 0;
    size_t buckNum;

    assert (oSymTable != NULL);

    /* current number of buckets to free */
    buckNum = auBucketCounts[oSymTable->iBucket];

    for (; i < buckNum; i++) {
        curr_bucket = oSymTable->buckets[i];
        if (curr_bucket != NULL) {

            curr_bin = curr_bucket;
            /* free the linked bindings at the bucket */
            while (curr_bin != NULL) {
                next_bin = curr_bin->next;

                /* free the defensive key copy and the node object */
                free((char*)curr_bin->key);
                free(curr_bin);

                /* advance binding */
                curr_bin = next_bin;
            }
        }
    }

    /* reset length */
    oSymTable->totBins = 0;
    /* free memory of SymTable head and buckets */
    free(oSymTable->buckets);
    free(oSymTable);
    
}


size_t SymTable_getLength(SymTable_T oSymTable) {

    assert (oSymTable != NULL);
    return oSymTable->totBins;
}


/* Expand the available buckets and redistribute all existing
*  bindings of oSymTable. Modifies input table if enough memory is 
*  available, otherwise leaves it unchanged
*/
static void SymTable_expand(SymTable_T oSymTable) {
    
    /* Temp pointer to expanded table */
    Binding_T *NewBuckets;
    /* Pointers to bucket heads of small table */
    Binding_T curr, curr_next;
    /* counter */
    size_t i, oldCount, newCount, hash;

    assert(oSymTable != NULL);

    oldCount = auBucketCounts[oSymTable->iBucket];
    newCount = auBucketCounts[oSymTable->iBucket + 1];

    /* Allocate memory for array of bucket heads */
    NewBuckets = (Binding_T*) malloc(sizeof(Binding_T) * newCount);

    /* if memory was available */
    if (NewBuckets != NULL) {

        /* track increased bucket count */
        oSymTable->iBucket++;  
        /* Initialize bucket heads to NULL */
        for (i=0; i < newCount ; i++) {
            NewBuckets[i] = NULL;
        }
    
        /* Redistribute existing bindings */ 
        for (i=0; i < oldCount; i++) {
            curr = oSymTable->buckets[i];
            while (curr != NULL) {
                curr_next = curr->next;
                /* get new hash*/
                hash = SymTable_hash(curr->key, newCount);
                /* redistribute */
                curr->next = NewBuckets[hash];
                NewBuckets[hash] = curr;
                /* advance binding */
                curr = curr_next;
            }
        }
    
        /* free memory of last bucket array */
        free(oSymTable->buckets);
        oSymTable->buckets = NewBuckets;
    }
}


int SymTable_put(SymTable_T oSymTable,
                 const char *pcKey, const void *pvValue) {
    
    size_t hash;
    Binding_T new;
    const char *keyCopy;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (SymTable_contains(oSymTable, pcKey) == 1) {
        return 0;   /* already exists - leave unchanged */
    }

    /* allocate space for new binding */
    new = (Binding_T) malloc(sizeof(struct Binding)); 
    if (new == NULL) {
        return 0;   /* insufficient memory - leave unchanged */
    }

    /* make a defensive copy of the key */
    keyCopy = (const char*) malloc((strlen(pcKey)+1)*sizeof(char));
    if (keyCopy == NULL) {
        /* free previously allocated memory */
        free(new);
        return 0;   /* insufficient memory for the key - unchanged */
    }
    strcpy((char*)keyCopy,pcKey);

    /* if max bindings has been reached and more buckets can be added */
    if (oSymTable->totBins == auBucketCounts[oSymTable->iBucket] &&
        oSymTable->iBucket < numBucketCounts-1) {

        SymTable_expand(oSymTable);
    }

    /* locate bucket */
    hash = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucket]);

    /* reorganize links between pointers */
    new->next = oSymTable->buckets[hash];
    oSymTable->buckets[hash] = new;

    /* store address of defensive key copy, and store value */
    new->key = keyCopy;
    new->item = pvValue;

    /* update length */
    oSymTable->totBins++;

    return 1;

}


void *SymTable_replace(SymTable_T oSymTable,
                       const char *pcKey, const void *pvValue) {

    size_t hash;
    Binding_T bucket_head;
    const void *old_val;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    hash = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucket]);
    /* Send pointer to bucket at hash code */
    bucket_head = oSymTable->buckets[hash];

    /* only search non-empty bucket */
    while(bucket_head != NULL) {
        /* find matches for the given key */
        if (strcmp(bucket_head->key, pcKey) == 0) {
            old_val = bucket_head->item;
            /* replace binding's value */
            bucket_head->item = pvValue;
            return (void*)old_val;
        }
        /* move to next node*/
        bucket_head = bucket_head->next;
    } 

    return NULL;   /* no matches, unchanged */
    
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {

    size_t hash;
    Binding_T bucket_head;

    assert (oSymTable != NULL);
    assert (pcKey != NULL);

    hash = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucket]);
    
    /* Send pointer to bucket at hash code */
    bucket_head = oSymTable->buckets[hash];

    /* only search non-empty bucket */
    while(bucket_head != NULL) {
        /* find matches for the given key */
        if (strcmp(bucket_head->key, pcKey) == 0) {
            return 1;
        }
        /* move to next node*/
        bucket_head = bucket_head->next;
    } 

    return 0;

}


void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

    size_t hash;
    Binding_T bucket_head;

    assert (oSymTable != NULL);
    assert (pcKey != NULL);

    hash = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucket]);
    /* Send pointer to bucket at hash code */
    bucket_head = oSymTable->buckets[hash];

    /* only search non-empty bucket */
    while(bucket_head != NULL) {
        /* find matches for the given key */
        if (strcmp(bucket_head->key, pcKey) == 0) {
            return (void*)bucket_head->item;
        }
        /* move to next node*/
        bucket_head = bucket_head->next;
    } 

    return NULL;    /* If no matches above */

}


void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){

    size_t hash;
    Binding_T curr, prev, next_bin;
    const void *old_val;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    hash = SymTable_hash(pcKey, auBucketCounts[oSymTable->iBucket]);
    /* Send pointer to bucket at hash code */
    curr = oSymTable->buckets[hash];
    prev = oSymTable->buckets[hash];

    /* traverses the list at most 1 time */
    while(curr != NULL) {
        if (strcmp(curr->key, pcKey) == 0) {
            old_val = curr->item;

            next_bin = curr->next;
            /* free memory */
            free((char*)curr->key);
            free(curr);
            if (curr == prev) { 
                /* edge case removing first binding at bucket */
                oSymTable->buckets[hash] = next_bin;
            } else {
                prev->next = next_bin;
            }
            
            /* update length */
            oSymTable->totBins--;
            return (void*)old_val;
        }
        prev = curr;
        curr = curr->next;
    }

    return NULL;   /* no matches, unchanged */
}


void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra) {

    Binding_T curr_bucket;
    size_t i;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    for (i=0; i < auBucketCounts[oSymTable->iBucket]; i++) {
        curr_bucket = oSymTable->buckets[i];
        while (curr_bucket != NULL) {
            /* apply function */
            (*pfApply) (curr_bucket->key, (void*)curr_bucket->item, 
            (void*)pvExtra);
            /* advance binding */
            curr_bucket = curr_bucket->next;
        }
    }

}