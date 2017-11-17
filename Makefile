CFILES=dbHandler.c debug.c drunt.c errorHandler.c helper.c interactiveHandler.c commandHandler.c
VPATH=src/
CC=gcc
CFLAGS=-Wall -Wextra -Wno-sign-compare -std=c99 -g

all: clean drunt

drunt: dbHandler.o debug.o errorHandler.o helper.o interactiveHandler.o commandHandler.o

clean:
	rm -vf drunt
	rm -vf *.o
	rm -vf newCal.ics
