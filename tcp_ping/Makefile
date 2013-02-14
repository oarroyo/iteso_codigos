#Makefile
#
# Instrucciones para compilar

NAME = tcp_ping
OBJS = main.o tcp_ping.o server.o client.o netlib/tcp.o ping.o

CC = g++
CFLAGS = -Wall -O2

all: $(OBJS) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(NAME)
	rm -rf html

doc:
	doxygen
	


