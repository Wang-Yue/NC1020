all: nc1020
main.o: main.cpp nc1020.h
	c++ -c main.cpp
nc1020.o: nc1020.cpp nc1020.h
	c++ -c nc1020.cpp
nc1020: main.o nc1020.o
	c++ -o nc1020 nc1020.o main.o -lSDL2
clean:
	rm main.o nc1020.o nc1020
