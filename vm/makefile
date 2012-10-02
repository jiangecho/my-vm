
vm: vm.o stack.o
	gcc -g -o vm vm.o stack.o

vm.o: vm.c vm.h ../common/opCode.h stack.h
	gcc -g -c vm.c

stack.o: stack.c stack.h
	gcc -g -c stack.c

clean:
	rm *.o vm *.bbc
