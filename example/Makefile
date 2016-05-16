
CC=gcc
CFLAGS=-Wall -g -pthread
LFLAGS=-lm -lrt


EXE=prod
CHAN_HEADER=../src/channel.h
CHAN_SRC=../src/channel.c
CHAN_OBJ=../channel.o

all : benchmark

benchmark: $(EXE)
	@echo "make: Example program done"


$(CHAN_OBJ) : $(CHAN_SRC) $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

# Channel
prod : prod.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

prod.o : prod.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)


# Clean
clean :
	rm -rf *.o

clear : clean
	rm -rf $(EXE)
