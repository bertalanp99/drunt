CFILES=dbHandler.c debug.c drunt.c errorHandler.c helper.c
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

all: clean drunt

drunt: dbHandler.o debug.o errorHandler.o helper.o

clean:
	rm -vf drunt
	rm -vf *.o
	rm -vf newCal.ics
