udp : main.o
	cc -o udp main.o
main.o : main.c packet.h
	cc -c main.c
clean :
	rm main.o udp
