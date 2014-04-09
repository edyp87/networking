CC=gcc

CFLAGS=-std=gnu99 -Wall -W

traceroute: 	main.o icmp_libr.o
			$(CC) $(CFLAGS) -o traceroute main.o icmp_libr.o 
main.o: 	main.c
			$(CC) $(CFLASG) -c main.c
icmp_libr.o:		icmp_libr.c
			$(CC) $(CFLAGS) -c icmp_libr.c
traceroute.o: 	traceroute.c
			$(CC) $(CFLAGS) traceroute.c
clean:
		rm -rf *o
distclean: 
		rm -rf *o traceroute

