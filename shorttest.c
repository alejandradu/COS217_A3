/*--------------------------------------------------------------------*/
/* testsymtable.c                                                     */
/* Author: Bob Dondero                                                */
/*--------------------------------------------------------------------*/

#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#ifndef S_SPLINT_S
#include <sys/resource.h>
#endif

/*--------------------------------------------------------------------*/

#define ASSURE(i) assure(i, __LINE__)

/*--------------------------------------------------------------------*/

/* If !iSuccessful, print a message to stdout indicating that the
   test at line iLineNum failed. */

static void assure(int iSuccessful, int iLineNum)
{
   if (! iSuccessful)
   {
      printf("Test at line %d failed.\n", iLineNum);
      fflush(stdout);
   }
}

/*--------------------------------------------------------------------*/

#ifndef S_SPLINT_S
/* Set the process's "CPU time" resource limit.  After the CPU
   time limit expires, the OS will send a SIGKILL signal to the
   process. */

static void setCpuTimeLimit(void)
{
   enum {CPU_TIME_LIMIT_IN_SECONDS = 300};
   struct rlimit sRlimit;
   sRlimit.rlim_cur = CPU_TIME_LIMIT_IN_SECONDS;
   sRlimit.rlim_max = CPU_TIME_LIMIT_IN_SECONDS;
   setrlimit(RLIMIT_CPU, &sRlimit);
}
#endif

/*--------------------------------------------------------------------*/

/* Write the binding whose key is pcKey and whose string value is
   pvValue using format string pvExtra. */

static void printBinding(const char *pcKey, void *pvValue,
   void *pvExtra)
{
   assert(pcKey != NULL);
   assert(pvValue != NULL);
   assert(pvExtra != NULL);

   printf((char*)pvExtra, pcKey, (char*)pvValue);
   fflush(stdout);
}

/*--------------------------------------------------------------------*/

/* Write the binding whose key is pcKey and whose string value is
   pvValue. pvExtra is unused. */

static void printBindingSimple(const char *pcKey, void *pvValue,
   void *pvExtra)
{
   assert(pcKey != NULL);
   assert(pvValue != NULL);
   assert(pvExtra == NULL);

   printf("%s\t%s\n", pcKey, (char*)pvValue);
   fflush(stdout);
}

/*--------------------------------------------------------------------*/

/* Test the most basic SymTable functions. */

static void testBasics(void)
{
   SymTable_T oSymTable;
   char acJeter[] = "Jeter";
   char acMantle[] = "Mantle";
   char acGehrig[] = "Gehrig";
   char acRuth[] = "Ruth";
   char acShortstop[] = "Shortstop";
   char acCenterField[] = "Center Field";
   char acFirstBase[] = "First Base";
   char acRightField[] = "Right Field";

   char acBrown[] = "Brown";
   
   char *pcValue;
   int iSuccessful;
   int iFound;
   size_t uLength;

   printf("------------------------------------------------------\n");
   printf("Testing the most basic SymTable functions.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   /* Test SymTable_new(). */

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   /* Test SymTable_put() and SymTable_getLength(). */

   iSuccessful = SymTable_put(oSymTable, acJeter, acShortstop);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 1);

}

/*--------------------------------------------------------------------*/

/* Test the SymTable ADT.  Write the output of the tests to stdout.
   As always, argc is the command-line argument count, argv contains
   the command-line arguments, and argv[0] is the name of the
   executable binary file. argv[1] is the number of bindings to put
   into a potentially large SymTable object.  Exit with EXIT_FAILURE
   if argv[1] is missing or not numeric.  Otherwise return 0. */

int main(int argc, char *argv[])
{
   int iBindingCount;

   if (argc != 2)
   {
      fprintf(stderr, "Usage: %s bindingcount\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   if (sscanf(argv[1], "%d", &iBindingCount) != 1)
   {
      fprintf(stderr, "bindingcount must be numeric\n");
      exit(EXIT_FAILURE);
   }
   if (iBindingCount < 0)
   {
      fprintf(stderr, "bindingcount cannot be negative\n");
      exit(EXIT_FAILURE);
   }
   
#ifndef S_SPLINT_S
   setCpuTimeLimit();
#endif

   testBasics();
   testKeyComparison();
   testKeyOwnership();
   testRemove();
   testMap();
   testEmptyTable();
   testEmptyKey();
   testNullValue();
   testLongKey();
   testTableOfTables();
   testCollisions();
   testLargeTable(iBindingCount);

   printf("------------------------------------------------------\n");
   printf("End of %s.\n", argv[0]);
   return 0;
}