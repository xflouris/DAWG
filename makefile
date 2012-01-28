SRC = dawg.c
OBJ = $(SRC:.c=.o)
CC  = gcc

CFLAGS = -Wall
OFLAGS = 

all: dawg

gapmis: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(OFLAGS)

clean:
	rm -f dawg $(OBJ) *~

$(OBJ) : dawg.h dawg.c makefile
