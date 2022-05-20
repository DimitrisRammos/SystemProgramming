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



# parameters
ARGS = 
KK = rm -v !(*.cpp|*.txt|"Makefile"|*.out|*.sh)


$(EXEC) $(EXEC1): $(OBJS) $(OBJS1)
	g++ $(OBJS) -o $(EXEC)
	g++ $(OBJS1) -o $(EXEC1)

# $(CLEAN):
# 	rm -v !$(DEL)

run: $(EXEC)
	./$(EXEC) $(ARGS)

gdb: $(OBJS)
	g++ $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)