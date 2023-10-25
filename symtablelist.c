/* COMMENT SOMETHING HERE */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "symtable.h"

/* you CANNOT dereference a void pointer for assignment,
just in case of printing 
SymTable_T is a pointer to SymTable given by h*/

/* building the data types (ADTs) PERHAPS MORE GENERAL? */
/* TO DO sizeof(struct Node) will be 4 by padding? 8 the longest?*/
struct Node {
    const char *key;
    const void *item;    /* the item (any type )*/
    struct Node *next;
};

/* when allocating the memory*/
struct SymTable {
    struct Node *first;
    size_t len;
};

/* THESE ARE FUNCTIONS in .h*/
/* QUESTION: can I keep using SymTable_T*/

/* return a new SymTable object that contains no bindings, or NULL if insufficient memory is available.*/
/* treating as a stack */
SymTable_T SymTable_new(void) {
    /* declare pointer to a symtable */
    SymTable_T oTable;
    /* allocate memory - GIVE THE TYPE NOT POINTER
    bc pointers are always 8 bytes on 64-bit
    QUESTION: clearly this first allocation in the heap is in the
    implementation but why is this even necessary */
    oTable = (SymTable_T)malloc(sizeof(struct SymTable));
    /* if insufficient space */
    if (oTable == NULL) {
        return NULL;
    }
    /* set the first node pointer to NULL (empty) */
    oTable->first = NULL;

    return oTable;
}

/* free all memory occupied by oSymTable. */
/* treating as a stack (will go through all nodes )*/
void SymTable_free(SymTable_T oSymTable) {
    struct Node *curr, *next;

    assert (oSymTable != NULL);

    for (curr = oSymTable->first; curr != NULL; curr = next) {
        next = curr->next;
        /* free the defensive key copy and the node object */
        free((char*)curr->key);
        free(curr);
    }

    /* free memory of SymTable itself, left with 1 struct pointer*/
    free(oSymTable);

    (*oSymTable).len = 0;
}

/* return the number of bindings in oSymTable */
size_t SymTable_getLength(SymTable_T oSymTable) {

    assert (oSymTable != NULL);

    return (*oSymTable).len;
}

/* If oSymTable does not contain a binding with key pcKey, then SymTable_put 
must add a new binding to oSymTable consisting of key pcKey and value 
pvValue and return 1 (TRUE). Otherwise the function must leave oSymTable 
unchanged and return 0 (FALSE). If insufficient memory is available, 
then the function must leave oSymTable unchanged and return 0 (FALSE)
QUESTION: add it at the end? alphanumerical? */
int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue) {

    struct Node *new;
    const char *keyCopy;    /* POT BUG: should this be const? */

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    assert(pvValue != NULL);

    if (SymTable_contains(oSymTable, pcKey) == 1) {
        return 0;   /* already exists - leave unchanged */
    } 

    new = (struct Node*) malloc(sizeof(struct Node));   /* QUESTION: do i have to cast this? */
    if (new == NULL) {
        return 0;   /* insufficient memory - leave unchanged */
    }

    /* make a defensive copy of the key */
    keyCopy = (const char*) malloc((strlen(pcKey)+1)*sizeof(char));
    if (keyCopy == NULL) {
        return 0;   /* insufficient memory for the key - leave unchanged? QUESTION*/
    }
    strcpy(keyCopy,pcKey);

    /* reorganize links between pointers */
    new->next = oSymTable->first;    /* if table was empty, new->next is NULL*/
    oSymTable->first = new;  

    /* store address of defensive key copy, and store value */
    new->key = keyCopy;
    new->item = pvValue;    /* do not dereference bc pvValue is a void pointer */

    /* NOTE the members are actually POINTERS (so addresses) */

    (*oSymTable).len++;

    return 1;

   }

/* If oSymTable contains a binding with key pcKey, then SymTable_replace 
must replace the binding's value with pvValue and return the old value. 
Otherwise it must leave oSymTable unchanged and return NULL.*/
void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue){

    struct Node *curr;
    const void *old_val;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    assert(pvValue != NULL);

    curr = oSymTable->first;

    /* not using contains bc worst case it traverses twice */
    while(curr != NULL) {
        if (*(curr->key) == *pcKey) {
            old_val = curr->item;
            /* replace binding's value */
            curr->item = pvValue;
            return (void*)old_val;
        }
        curr = curr->next;
    }

    return NULL;   /* no matches, unchanged */

}

/* return the value of the binding within oSymTable whose key is pcKey, 
or NULL if no such binding exists*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    struct Node *curr;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    curr = oSymTable->first;

    while(curr != NULL) {
        if (*(curr->key) == *pcKey) {
            return (void*)curr->item;   /* returning a void pointer to value */
        }
        curr = curr->next;
    }

    return NULL;   /* if no matches above */
}

/* SymTable_contains must return 1 (TRUE) if oSymTable contains a 
binding whose key is pcKey, and 0 (FALSE) otherwise*/
/* QUESTION: pcKey is an address or string provided by the client?*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Node *curr;

    assert (oSymTable != NULL);
    assert (pcKey != NULL);

    curr = oSymTable->first;

    /* assuming that if there is a key, the binding is not null
    (or counting null values too)*/
    while(curr != NULL) {
        /* you can dereference the key pointer member (char)*/
        /* QUESTION: should I match the value (makes sense) or the addresses (that I suppose the client would have to know?)*/
        if (*(curr->key) == *pcKey) {
            return 1;
        }
        /* move to next node*/
        curr = curr->next;
    }

    return 0;   /* returns 0 if empty too*/

}

/* If oSymTable contains a binding with key pcKey, then SymTable_remove 
must remove that binding from oSymTable and return the binding's value. 
Otherwise the function must not change oSymTable and return NULL.*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){

    struct Node *curr, *prev;
    const void *old_val;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    curr = oSymTable->first;
    prev = oSymTable->first;   /* the same at first*/

    /* not using contains bc worst case it traverses twice */
    while(curr != NULL) {
        if (*(curr->key) == *pcKey) {
            old_val = curr->item;
            /* reorganize links between pointers - should work for edge cases */
            prev->next = curr->next;
            /* free memory */
            free((char*)curr->key);
            free(curr);
            /* QUESTION: have i properly handled curr? should i do anything else w it?*/
            return (void*)old_val;
        }
        prev = curr;
        curr = curr->next;
    }

    (*oSymTable).len--;

    return NULL;   /* no matches, unchanged */
}


/* apply function *pfApply to each binding (and members) in oSymTable, 
passing pvExtra as an extra parameter.*/
void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra) {
    /* pfApply is a function pointer. when dereferenced, yields a
    function that takes 3 params and returns a void pointer*/

    struct Node *curr;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);  /* QUESTION: can you do this */
    assert(pvExtra != NULL);

    curr = oSymTable->first;

    while(curr != NULL) {
        /* apply the function (not return)*/
        (*pfApply) (curr->key, (void*)curr->item, (void*)pvExtra);
        curr = curr->next;
    }

}