FLAGS=--std=c++11 -g

all: recorder

recorder: recorder.o ir_reader.o
	g++ -o recorder recorder.o ir_reader.o $(FLAGS)

recorder.o: recorder.cpp ir_reader.h
	g++ -c recorder.cpp -o recorder.o $(FLAGS)

ir_reader.o: ir_reader.cpp ir_reader.h
	g++ -c ir_reader.cpp -o ir_reader.o $(FLAGS)

clean:
	rm *.o recorder
