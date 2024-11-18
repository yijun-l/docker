CC = gcc
CFLAGS = -g -Wall -Wextra
SRC = main.c include/container.c
OBJ = $(SRC:.c=.o)
EXEC = adocker

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $^

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	sudo ./$(EXEC)

clean:
	rm -f $(OBJ) $(EXEC)