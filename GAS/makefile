targets = hello exit maximum power factorial elf toupper
interfile = hello.o exit.o maximum.o power.o factorial.o elf.o toupper.o

target: $(targets)

hello: hello.o
	ld -s -o hello hello.o
hello.o: hello.s
	as -o hello.o hello.s
exit: exit.o
	ld -m elf_i386 -o exit exit.o
exit.o: exit.s
	as --32 -o exit.o exit.s
maximum: maximum.o
	ld -m elf_i386 -o maximum maximum.o
maximum.o: maximum.s
	as --32 -o maximum.o maximum.s
power: power.o
	ld -m elf_i386 -o power power.o
power.o: power.s
	as -gstabs --32 -o power.o power.s
factorial: factorial.o
	ld -m elf_i386 -o factorial factorial.o
factorial.o: factorial.s
	as -gstabs --32 -o factorial.o factorial.s
elf: elf.o
	gcc -m32 -o elf elf.o
elf.o: elf.c
	gcc -m32 -c elf.c -o elf.o
toupper: toupper.o
	ld -m elf_i386 -o toupper toupper.o
toupper.o: toupper.s
	as -gstabs --32 -o toupper.o toupper.s

run_power:
	./power
	echo $?
run_toupper:
	-rm toupper.uppercase
	./toupper toupper.s toupper.uppercase

.PHONY: clean
clean:
	-rm $(targets) $(interfile) toupper.uppercase
