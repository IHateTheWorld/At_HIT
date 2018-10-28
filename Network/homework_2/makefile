cc = gcc
objects = server_sr client_sr

all: $(objects)

server_sr: server_sr.o
	cc -o server_sr server_sr.o
server_sr.o: server_sr.c
	cc -c server_sr.c

client_sr: client_sr.o
	cc -o client_sr client_sr.o
client_sr.o: client_sr.c
	cc -c client_sr.c

rss:
	./server_sr
dsg:
	gdb ./server_sr

rcs:
	./client_sr
dcg:
	gdb ./client_sr

.PHONY: clean
clean:
	-rm $(objects) server_sr.o client_sr.o file_rec.txt
