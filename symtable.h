/* INTERFACE (HEADER) FILE */

#include <stdlib.h>

/* THESE ARE ADTs */

struct SymTable;
typedef struct SymTable *SymTable_T;

/* THESE ARE FUNCTIONS - NEED TO COMMENT */

/* COMMENT */
SymTable_T SymTable_new(void);

/* COMMENT */
void SymTable_free(SymTable_T oSymTable);

/* COMMENT */
size_t SymTable_getLength(SymTable_T oSymTable);

/* COMMENT */
int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue);

/* COMMENT */
void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue);

/* COMMENT */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* COMMENT */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* COMMENT */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* COMMENT */
void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra);