CC = gcc
CFLAGS = -O3

all:	gencols makematrix makepatlist

gencols:	gencols.o boxes.o lists.o output.o
	$(CC) -o gencols gencols.o boxes.o lists.o output.o

makematrix:	makematrix.o lists.o
	$(CC) -o makematrix makematrix.o lists.o

makepatlist:	makepatlist.o lists.o
	$(CC) -o makepatlist makepatlist.o lists.o

clean:
	rm *.o gencols makematrix makepatlist
