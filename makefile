server.o: server.c
	gcc $< -o $@

client.o: client.c
	gcc $< -o $@

server: server.o
	./server.o

client: client.o
	./client.o