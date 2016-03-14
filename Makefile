# Makefile

CC=gcc
MANDEL_CFLAGS=-g -O3 -ffast-math -Wall -pthread `pkg-config --cflags gtk+-3.0`
MANDEL_LFLAGS=`pkg-config --libs gtk+-3.0` -lm
CFLAGS=-Wall -O3 -pthread
LFLAGS=


# Directories
SRC_DIR=./src/
BENCH_DIR=./benchmark/
BENCH_THREAD_DIR=$(BENCH_DIR)threads/
BENCH_PROC_DIR=$(BENCH_DIR)process/

# Files
CHAN_HEADER=$(SRC_DIR)channel.h
CHAN_SRC=$(SRC_DIR)channel.c
MANDEL_SRC=$(SRC_DIR)mandelbrot.c

all: channel.o

channel.o : $(CHAN_SRC) $(CHAN_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)


mandelbrot : $(MANDEL_SRC) $(CHAN_SRC)
	$(CC) $(MANDEL_CFLAGS) $^ $(MANDEL_LFLAGS)

# Clean
clean :
	rm -rf *.o

clear : clean