# all: testsymtablelist testsymtablehash

# testsymtablelist: testsymtable.o symtablelist.o
# 	gcc217 testsymtable.o symtablelist.o -o testsymtablelist
testsymtablehash: testsymtable.o symtablehash.o
	gcc217 testsymtable.o symtablehash.o -o testsymtablehash
short: short.o symtablehash.o
	gcc217 short.o symtablehash.o -o short
testsymtable.o: testsymtable.c symtable.h
	gcc217 -c testsymtable.c
short.o: short.c symtable.h
	gcc217 -c short.c
# symtablelist.o: symtablelist.c symtable.h
# 	gcc217 -c symtablelist.c
symtablehash.o: symtablehash.c symtable.h
	gcc217 -c symtablehash.c