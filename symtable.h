/* ---------- Header file for SymbolTable implementation ------------ */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* Symbol table ADT */
struct SymTable;
/* Pointer to a SymTable object */
typedef struct SymTable *SymTable_T;


/* Initialize a SymTable                                                                                                                                                                                                                                                                                                                                                                                                    
------------------------                                                                                                                                                                       
return: a new SymTable object with no bindings, or NULL if 
        insufficient memory is available                             
*/
SymTable_T SymTable_new(void);


/* Free all memory occupied by SymTable and its members   
------------------------    
param: oSymTable: pointer to SymTable where binding is to be inserted
                                                                                                                                                                                      
return: (void) 
*/
void SymTable_free(SymTable_T oSymTable);


/* Get length of SymTable       
------------------------    
oSymTable: pointer to SymTable where binding is to be inserted
                                                                                                                                                                                      
return: number of bindings (size_t) 
*/
size_t SymTable_getLength(SymTable_T oSymTable);


/* Insert a new binding in SymTable   
Fails if SymTable already contains pcKey or if insufficient 
memory is available           
------------------------    
param: oSymTable: pointer to SymTable where binding is to be inserted
param: pcKey: literal key of binding to be inserted
param: pvValue: value (any type) of binding to be inserted
                                                                                                                                                                                      
return: 1 (TRUE) if new binding with pcKey and pcValue is added,
0 (FALSE) otherwise 
*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue);


/* Replace a binding's value in SymTable
No changes if SymTable has no binding with key pcKey
------------------------    
param: oSymTable: pointer to SymTable where binding is to be inserted
param: pcKey: literal key of binding to be accessed
param: pvValue: value (any type) of binding to be replaced
                                                                                                                                                                                      
return: (void)
*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue);


/* Check if SymTable contains a given binding
------------------------    
param: oSymTable: pointer to SymTable where binding is to be inserted
param: pcKey: literal key of target binding
                                                                                                                                                                                     
return: 1 (TRUE) if binding is contained, 0 (FALSE) otherwise
*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);


/* Get the value of a binding in SymTable at a given key
------------------------ 
param: oSymTable: pointer to SymTable
param: pcKey: literal key of binding with target value

return: (void) void pointer to value of the target binding, NULL if 
        no such binding exists 
*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);


/* Remove binding at a given key from SymTable
Get the value of a binding in SymTable at a given key
------------------------ 
param: oSymTable: pointer to SymTable
param: pcKey: literal key of binding to be removed

return: (void) void pointer to value of the removed binding, NULL if 
        no such binding exists 
*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);


/* Apply given function to all bindings in SymTable
------------------------ 
param: oSymTable: pointer to SymTable
param: pfApply: pointer to function to be applied
param: pvExtra: extra parameter passed to pfApply

return: (void) 
*/
void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra);