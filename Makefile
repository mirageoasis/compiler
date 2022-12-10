CC = gcc

CFLAGS = -lfl

OBJS = main.o util.o lex.yy.o parse.o

hw2_binary: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o hw2_binary

main.o: main.c globals.h util.h scan.h parse.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

parse.o: parse.c parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.c

lex.yy.o: tiny.l scan.h util.h globals.h
	flex -o lex.yy.c tiny.l
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	-rm hw2_binary
	-rm $(OBJS)