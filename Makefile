all:
	cd ir_reader && make
	cd ir_sender && make
	ln -sf ir_reader/recorder recorder 
	ln -sf ir_sender/sender sender 
