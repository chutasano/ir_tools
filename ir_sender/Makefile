FLAGS=--std=c++11 -lpigpio -lm -pthread -lrt 

all: nec light 

nec: nec.cpp ir_sender.o
	g++ -o nec nec.cpp ir_sender.o $(FLAGS)

light: light.cpp ir_sender.o
	g++ -o light light.cpp ir_sender.o $(FLAGS)


ir_sender.o: ir_sender.cpp
	g++ -c ir_sender.cpp -o ir_sender.o $(FLAGS)

clean:
	rm nec light ir_sender.o
