# Это комментарий, который говорит, что переменная CC указывает компилятор, используемый для сборки
CC=gcc
#Это еще один комментарий. Он поясняет, что в переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -Wall

all: clean

clean: proj
	rm -rf *.o proj

proj: anothertest.o test.o 
	$(CC) anothertest.o test.o -o test

test.o:
	$(CC) -c test.c


anothertest.o:
	$(CC) -c anothertest.c


