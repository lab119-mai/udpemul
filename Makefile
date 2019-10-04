udp : main.o TabCRC.o
	cc -o udp main.o TabCRC.o
main.o : main.c packet.h
	cc -c main.c
TabCRC.o : TabCRC.c
	cc -c TabCRC.c
clean :
	rm *.o udp
run: udp
	./udp r2.bin