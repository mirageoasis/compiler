CC = gcc

CFLAGS = 

OBJS = main.o util.o lex.yy.o tiny.tab.o

hw2_binary: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o hw2_binary

main.o: main.c globals.h util.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

lex.yy.c: tiny.l globals.h
	flex tiny.l

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

tiny.tab.c: tiny.y globals.h
	bison tiny.y

tiny.tab.o: tiny.tab.c
	$(CC) $(CFLAGS) -c tiny.tab.c

clean:
	-rm hw2_binary
	-rm $(OBJS)
	-rm lex.yy.c