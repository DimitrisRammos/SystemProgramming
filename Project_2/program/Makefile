# paths
MODULES = ../modules

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CXXFLAGS = -Wall -no-pie -fPIE -g

# Αρχεία .o
OBJS = dataServer.o
OBJS1 = remoteClient.o


# the executable program
EXEC = dataServer
EXEC1 = remoteClient


# ./dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>
# ./remoteClient -i <server_ip> -p <server_port> -d <directory>
# parameters
ARGS = -p 12516 -s 2 -q 5 -b 512
ARGS1 = -i 127.0.0.1 -p 12516 -d Server


$(EXEC) $(EXEC1): $(OBJS) $(OBJS1)
	g++ -lpthread -pthread $(OBJS) -o $(EXEC)
	g++ -lpthread -pthread $(OBJS1) -o $(EXEC1)


run_server: $(EXEC)
	./$(EXEC) $(ARGS)

run_client: $(EXEC1)
	./$(EXEC1) $(ARGS1)

gdb: $(OBJS)
	g++ $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)