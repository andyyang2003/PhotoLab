# makefile

all: PhotoLab
clean:
	-rm *.o
PhotoLab: FileIO.o DIPs.o Advanced.o PhotoLab.o 
	gcc -Wall -std=c11 -g DIPs.o FileIO.o Advanced.o PhotoLab.o -o PhotoLab
DIPs.o: DIPs.c   DIPs.h Constants.h
	gcc -Wall -std=c11 -g -c DIPs.c -o DIPs.o    
Advanced.o: Advanced.c Advanced.h Constants.h
	gcc -Wall -std=c11 -g -c Advanced.c -o Advanced.o
FileIO.o: FileIO.c FileIO.h Constants.h
	gcc -Wall -std=c11 -g -c FileIO.c -o FileIO.o
PhotoLab.o: PhotoLab.c DIPs.h Constants.h FileIO.h Advanced.h
	gcc -Wall -std=c11 -g -c PhotoLab.c -o PhotoLab.o
