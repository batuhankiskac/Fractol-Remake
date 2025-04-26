# Makefile for Fractol
CC = gcc
CFLAGS = -Wall -O2 -pthread
LDFLAGS = -lX11 -lpthread -lm

SRC = src/main.c src/mandelbrot.c src/julia.c src/graphics.c src/color.c
OBJ = $(SRC:.c=.o)
INC = -Iinclude

all: fractal

fractal: $(OBJ)
	$(CC) $(CFLAGS) -o fractal $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f src/*.o fractal
