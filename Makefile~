block:block.o function.o
		gcc block.o function.o -o block -lncurses

function.o:function.h function.c
		gcc -c function.c -lncurses

block.o:function.h block.c
		gcc -c block.c -lncurses