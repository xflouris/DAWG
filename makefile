SRC = dawg.c args.c
OBJ = $(SRC:.c=.o)
CC  = gcc

CFLAGS = -Wall
OFLAGS = 

all: dawg

dawg: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(OFLAGS)

clean:
	rm -f dawg $(OBJ) *~

$(OBJ) : dawg.h dawg.c args.h args.c makefile
