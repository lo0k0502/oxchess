FLAGS = -I includes

SERVER_OBJS = \
				server/main.o \
			
CLIENT_OBJS = \
				client/main.o \

server/%.o: server/%.c
	gcc $(FLAGS) -c $< -o $@

client/%.o: client/%.c
	gcc $(FLAGS) -c $< -o $@

server.o: $(SERVER_OBJS)
	gcc $(FLAGS) $(SERVER_OBJS) -o $@

client.o: $(CLIENT_OBJS)
	gcc $(FLAGS) $(CLIENT_OBJS) -o $@

server: server.o
	./server.o

client: client.o
	./client.o