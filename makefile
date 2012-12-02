CC = gcc -O3 -funroll-loops -std=c99
all:
	$(CC) main.c -o Othello
