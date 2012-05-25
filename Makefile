
CC=gcc
CFLAGS=`python-config --cflags`
LDFLAGS=`python-config --ldflags`
EXEC=dolib
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)
LIB= $(SRC:.c=.so)


all: $(LIB)


%.so:	%.o
		$(CC) $(LDFLAGS) $^ -o $@

%.o:	%.c
		$(CC) $(CFLAGS) -c $^  $@

clean:
		rm *.o
