
CC=gcc
CFLAGS=-Wall -O3 -pthread
LFLAGS=-lm -lrt


EXE=fwd_chan fwd_pipe bfwd_chan sync_fwd_chan nfwd_chan
CHAN_HEADER=../../src/channel.h
CHAN_SRC=../../src/channel.c
CHAN_OBJ=../channel.o

all : benchmark

benchmark: $(EXE)
	@echo "make: Benchmark programs done"


$(CHAN_OBJ) : $(CHAN_SRC) $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

# Channel
fwd_chan : fwd_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

bfwd_chan : bfwd_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

sync_fwd_chan : sync_fwd_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

nfwd_chan : nfwd_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)


fwd_chan.o : fwd_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

bfwd_chan.o : bfwd_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

sync_fwd_chan.o : sync_fwd_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

nfwd_chan.o : nfwd_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

# Pipe
fwd_pipe : fwd_pipe.o
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

fwd_pipe.o : fwd_pipe.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)


# Clean
clean :
	rm -rf *.o $(CHAN_OBJ)

clear : clean
	rm -rf $(EXE)
