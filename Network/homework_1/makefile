cc = gcc

homework1: homework_1.o
	cc -o homework_1 homework_1.o -lpthread

homework1.o: homework1.c
	cc -c homework_1.c

r:
	-rm *.tmp
	./homework_1

rf:
	-rm *.tmp
	./homework_1 > tmp.txt

dg:
	gdb ./homework_1

.PHONY: clean
clean:
	-rm homework_1 homework_1.o *tmp
