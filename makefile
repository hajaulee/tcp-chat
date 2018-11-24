default: main

main:
	clear 
	gcc -Wall -g -o client client-chat.c -lgthread-2.0 `pkg-config gtk+-3.0 --cflags --libs`
	gcc -o server server.c -g -Wall -pthread

run: main
	./client 
debug: main
	gdb ./client
server: main
	./server
clean:
	rm -f client