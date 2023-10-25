/* ----------- Symbol Table linked list implementation -------------- */
 /* QUESTION: clearly this first allocation in the heap is in the
    implementation but why is this even necessary */
     /* QUESTION: can you do this */
     /* Question: asserting void pointers? */

#include "symtable.h"

/* Each item is stored in a Node. Nodes are linked. */
struct Node {
    /* unique key (string)*/
    const char *key;   
    /* the item (any type )*/  
    const void *item;   
    /* address of next node*/
    struct Node *next;  
};

/* A SymTable is a struct that points to the first Node */
struct SymTable {
    /* address of first binding */
    struct Node *first;
    /* size of list (number of bindings)*/
    size_t len;         
};


SymTable_T SymTable_new(void) {

    SymTable_T oTable;
    /* Allocate memory for head of list */
    oTable = (SymTable_T)malloc(sizeof(struct SymTable));

    if (oTable == NULL) {      /* insufficient space*/
        return NULL;
    }

    oTable->first = NULL;      /* initialize head of list to NULL */
    return oTable;
}


void SymTable_free(SymTable_T oSymTable) {

    struct Node *curr, *next;
    assert (oSymTable != NULL);

    for (curr = oSymTable->first; curr != NULL; curr = next) {
        next = curr->next;
        /* free the defensive key copy and the node object */
        free((char*)curr->key);
        free(curr);
    }

    /* free memory of SymTable head itself */
    free(oSymTable);
    /* reset length */
    (*oSymTable).len = 0;
}


size_t SymTable_getLength(SymTable_T oSymTable) {

    assert (oSymTable != NULL);
    return (*oSymTable).len;
}


int SymTable_put(SymTable_T oSymTable,
                 const char *pcKey, const void *pvValue) {

    struct Node *new;
    const char *keyCopy;  

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (SymTable_contains(oSymTable, pcKey) == 1) {
        return 0;   /* already exists - leave unchanged */
    } 

    new = (struct Node*) malloc(sizeof(struct Node)); 
    if (new == NULL) {
        return 0;   /* insufficient memory - leave unchanged */
    }

    /* make a defensive copy of the key */
    keyCopy = (const char*) malloc((strlen(pcKey)+1)*sizeof(char));
    if (keyCopy == NULL) {
        return 0;   /* insufficient memory for the key - unchanged */
    }
    strcpy((char*)keyCopy,pcKey);

    /* reorganize links between pointers */
    new->next = oSymTable->first;  
    oSymTable->first = new;  

    /* store address of defensive key copy, and store value */
    new->key = keyCopy;
    new->item = pvValue;  

    /* update length */
    (*oSymTable).len++;

    return 1;

   }


void *SymTable_replace(SymTable_T oSymTable,
                       const char *pcKey, const void *pvValue){

    struct Node *curr;
    const void *old_val;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    curr = oSymTable->first;

    /* traverses the list at most 1 time */
    while(curr != NULL) {
        if (strcmp(curr->key, pcKey) == 0) {
            old_val = curr->item;
            /* replace binding's value */
            curr->item = pvValue;
            return (void*)old_val;
        }
        curr = curr->next;
    }

    return NULL;   /* no matches, unchanged */

}


void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
                   struct Node *curr;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    curr = oSymTable->first;

    /* traverses the list at most 1 time */
    while(curr != NULL) {
        if (strcmp(curr->key, pcKey) == 0) {
            return (void*)curr->item; 
        }
        curr = curr->next;
    }

    return NULL;   /* if no matches above */
}


int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Node *curr;

    assert (oSymTable != NULL);
    assert (pcKey != NULL);

    curr = oSymTable->first;

    while(curr != NULL) {
        /* find matches for the given key */
        if (strcmp(curr->key, pcKey) == 0) {
            return 1;
        }
        /* move to next node*/
        curr = curr->next;
    }

    return 0;  

}


void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){

    struct Node *curr, *prev, *next_node;
    const void *old_val;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    curr = oSymTable->first;
    prev = oSymTable->first;   /* initialize at the same point */

    /* traverses the list at most 1 time */
    while(curr != NULL) {
        if (strcmp(curr->key, pcKey) == 0) {
            old_val = curr->item;

            next_node = curr->next;
            /* free memory */
            free((char*)curr->key);
            free(curr);
            if (curr == prev) { 
                /* edge case removing first binding */
                oSymTable->first = next_node;
            } else {
                prev->next = next_node;
            }
            
            /* update length */
            (*oSymTable).len--;
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

    struct Node *curr;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    curr = oSymTable->first;

    while(curr != NULL) {
        (*pfApply) (curr->key, (void*)curr->item, (void*)pvExtra);
        curr = curr->next;
    }

}