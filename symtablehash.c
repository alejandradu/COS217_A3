/* ----------- Symbol Table hash table implementation --------------- */
/* Q: URGENT - CONSTANT INITIAL LENGTH? line 10 magic number? */
/* Q: does this mathc/have to match the storage spaces in 
the graphic diagram? 
Q: do I really need to cast the result of malloc? 
QUESTION: will not initialize iBuckets and suppose it starts at 0? */

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
    int iBucket;
    /* pointer to (the head of array of) Binding_T */
    /* DO THIS BC UNKNOWN LENGTH */
    /* using equivalence a[i] = *(a + i) */
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
    /* Current size of bucket array to be created */
    size_t size = auBucketCounts[oSymTable->iBucket];
    /* counter */
    size_t i = 0;

    /* Allocate memory for table head */
    /* Get pointer to heap that will store a SymTable */
    oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));

    if (oSymTable == NULL) { 
        return NULL;
    }

    /* Allocate memory for array of bucket heads */
    /* Get pointer to heap that will store an array of Binding* */
    oSymTable->buckets = (Binding_T*) malloc(sizeof(Binding_T) * size);

    if (oSymTable->buckets == NULL) {
        /* free previously allocated memory */
        free(oSymTable);
        return NULL;
    }

    /* Initialize bucket heads to NULL */
    for (; i < size ; i++) {
        *(oSymTable->buckets + i) = NULL;
    }

    return oSymTable;
}
