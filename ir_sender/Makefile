all: nec light nec_raw

nec: nec.cpp ir_sender.o
	g++ -o nec nec.cpp ir_sender.o -lpigpio -lm -pthread -lrt

light: light.cpp ir_sender.o
	g++ -o light light.cpp ir_sender.o -lpigpio -lm -pthread -lrt

nec_raw: nec_raw.cpp ir_sender.o
	g++ -o nec_raw nec_raw.cpp ir_sender.o -lpigpio -lm -pthread -lrt

ir_sender.o: ir_sender.cpp
	g++ -c ir_sender.cpp -o ir_sender.o -lpigpio -lm -pthread -lrt

clean:
	rm nec light nec_raw
