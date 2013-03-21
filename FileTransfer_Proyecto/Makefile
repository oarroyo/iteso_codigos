#Makefile
#
# Instrucciones para compilar

OBJS_SERVER = main_server.o server.o netlib/tcp.o
OBJS_CLIENT = main_client.o client.o netlib/tcp.o
CC = g++
CFLAGS = -Wall -O2

all: $(OBJS_SERVER) server client

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) $^ -o $@

client: ${OBJS_CLIENT}
	$(CC) $(CFLAGS) $^ -o $@
	
%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_SERVER) $(OBJS_CLIENT) server client
	rm -rf html

doc:
	doxygen


