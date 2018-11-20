default: run

main:
	clear 
	gcc -w -g -o client client-chat.c  `pkg-config gtk+-3.0 --cflags --libs`

run: main
	./client 
debug: main
	gdb ./client

clean:
	rm -f main