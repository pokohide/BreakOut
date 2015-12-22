LDLIBS := -lncurses

block: block.o function.o

block.o function.o: function.h

clean:
	$(RM) *.o
	$(RM) block