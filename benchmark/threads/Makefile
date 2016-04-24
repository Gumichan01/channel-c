
CC=gcc
CFLAGS=-Wall -O3 -pthread
LFLAGS=-lm -lrt


EXE=cat_chan cp_chan fwd_chan cat_pipe cp_pipe fwd_pipe cat_sock cp_sock \
bfwd_chan sync_fwd_chan
CHAN_HEADER=../../src/channel.h
CHAN_SRC=../../src/channel.c
CHAN_OBJ=../channel.o

all : benchmark

benchmark: $(EXE)
	@echo "make: Benchmark programs done"


$(CHAN_OBJ) : $(CHAN_SRC) $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

# Channel
cat_chan : cat_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

cp_chan : cp_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

fwd_chan : fwd_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

bfwd_chan : bfwd_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

sync_fwd_chan : sync_fwd_chan.o $(CHAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)


cat_chan.o : cat_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

cp_chan.o : cp_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

fwd_chan.o : fwd_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

bfwd_chan.o : bfwd_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

sync_fwd_chan.o : sync_fwd_chan.c $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

# Pipe
cat_pipe : cat_pipe.o
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

cp_pipe : cp_pipe.o
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

fwd_pipe : fwd_pipe.o
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

cat_pipe.o : cat_pipe.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

cp_pipe.o : cp_pipe.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

fwd_pipe.o : fwd_pipe.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)


# Sockets

cat_sock : cat_sock.o
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

cp_sock : cp_sock.o
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

cat_sock.o : cat_sock.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

cp_sock.o : cp_sock.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

# Clean
clean :
	rm -rf *.o $(CHAN_OBJ)

clear : clean
	rm -rf $(EXE)
