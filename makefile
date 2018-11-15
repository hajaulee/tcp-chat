default: run

main:
	clear 
	gcc -w -o client client-chat.c  `pkg-config gtk+-3.0 --cflags --libs`

run: main
	./client 
clean:
	rm -f main