# Makefile

CC=gcc
MANDEL_CFLAGS=-g -O3 -std=gnu99 -ffast-math -Wall -pthread `pkg-config --cflags gtk+-3.0`
MANDEL_LFLAGS=`pkg-config --libs gtk+-3.0` -lm -lrt
CFLAGS=-Wall -g -pthread
LFLAGS=-lm -lrt


# Directories
SRC_DIR=./src/
TEST_DIR=./test/
BENCH_DIR=./benchmark/
BENCH_THREAD_DIR=$(BENCH_DIR)threads/
BENCH_PROC_DIR=$(BENCH_DIR)process/

# Files
CHAN_HEADER=$(SRC_DIR)channel.h
CHAN_SRC=$(SRC_DIR)channel.c
MANDEL_SRC=$(SRC_DIR)mandelbrot.c
TEST_SRC=$(TEST_DIR)chan-test.c
CHAN_OBJ=channel.o
CHANNEL_LIB=channel.a

all: $(CHANNEL_LIB)

$(CHANNEL_LIB) : $(CHAN_OBJ)
	ar rcs $@ $^

$(CHAN_OBJ) : $(CHAN_SRC) $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)


mandelbrot : $(MANDEL_SRC) $(CHAN_SRC)
	$(CC) $(MANDEL_CFLAGS) $^ -o $@ $(MANDEL_LFLAGS)

test : chan-test
	@echo "Launch test."
	@echo "Testing ... "
	./chan-test


chan-test : $(TEST_SRC) $(CHAN_SRC)
	@echo $@" - Compiling "$(TEST_SRC)"... "
	$(CC) -std=gnu99 $(CFLAGS) $^ -o $@ $(LFLAGS)
	@echo $@" -  done."

# Clean
clean :
	rm -rf *.o $(SRC_DIR)$(CHAN_OBJ)

clear : clean
	rm -rf $(CHANNEL_LIB)
	rm -rf mandelbrot chan-test
