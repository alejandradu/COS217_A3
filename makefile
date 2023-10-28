# all: testsymtablelist testsymtablehash

# testsymtablelist: testsymtable.o symtablelist.o
# 	gcc217 testsymtable.o symtablelist.o -o testsymtablelist
testsymtablehash: testsymtable.o symtablehash.o
	gcc217 testsymtable.o symtablehash.o -o testsymtablehash
shorttest: shorttest.o symtablehash.o
	gcc217 shorttest.o symtablehash.o -o shorttest
testsymtable.o: testsymtable.c symtable.h
	gcc217 -c testsymtable.c
shorttest.o: shorttest.c symtable.h
	gcc217 -c shorttest.c
# symtablelist.o: symtablelist.c symtable.h
# 	gcc217 -c symtablelist.c
symtablehash.o: symtablehash.c symtable.h
	gcc217 -c symtablehash.c