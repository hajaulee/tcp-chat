default: run

main:
	clear 
	gcc -w -g -o client client-chat.c  `pkg-config gtk+-3.0 --cflags --libs`
	gcc -o server server.c -g 

run: main
	./client 
debug: main
	gdb ./client
server: main
	./server
clean:
	rm -f client