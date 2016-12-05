all: nec light nec_raw

nec: nec.c ir_sender.h
	gcc -o nec nec.c -lpigpio -lm -pthread -lrt

light: light.c ir_sender.h
	gcc -o light light.c -lpigpio -lm -pthread -lrt

nec_raw: nec_raw.c ir_sender.h
	gcc -o nec_raw nec_raw.c -lpigpio -lm -pthread -lrt

clean:
	rm nec light nec_raw
