all_objects = target
all_middle_files = scanner.o hash.o symbol_table.o const_table.o buffer.o lr1.o lr1_parl.o main.o stack.o
cc = gcc

all: $(all_objects)

target: $(all_middle_files)
	cc -o target $(all_middle_files)

hash.o: hash.c
	cc -c hash.c
buffer.o: buffer.c global_define.h
	cc -c buffer.c
stack.o: stack.c global_define.h
	cc -c stack.c
symbol_table.o: symbol_table.c global_define.h
	cc -c symbol_table.c
const_table.o: const_table.c global_define.h
	cc -c const_table.c
scanner.o: scanner.c words.h global_define.h
	cc -c scanner.c
lr1_parl.o: lr1_parl.c global_define.h
	cc -c lr1_parl.c
lr1.o: lr1.c global_define.h words.h sub_funs.h
	cc -c lr1.c
main.o: main.c global_define.h
	cc -c main.c

run:
	./target
rf:
	./target > debug_info.txt

debug:
	gdb ./target

.PHONY: clean
clean:
	-rm $(all_objects) $(all_middle_files)

