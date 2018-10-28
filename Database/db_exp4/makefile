cc = gcc
interfiles = main.o extmem.o 

target: main

main: $(interfiles)
	cc -o main $(interfiles)

main.o: main.c extmem.h
	cc -c main.c
extmem.o: extmem.c extmem.h
	cc -c extmem.c

run:
	-rm *.blk
	./main
rf:
	-rm *.blk
	./main > tmp.txt
debug:
	gdb ./main

.PHONY: clean
clean:
	-rm $(interfiles) main *.blk tmp.txt
