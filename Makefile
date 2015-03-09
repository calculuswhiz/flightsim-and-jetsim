CC = gcc
CFLAGS = -Wall -lm
DEPS = plane.h earthPhys.h
OBJ = plane.o mp2.o
LIBS = -lGL -lGLU -lglut

all:
	make clean mp2

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
mp2o:
	gcc -c mp2.cpp -o mp2.o
	
mp2: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
clean:
	rm *.o mp2
