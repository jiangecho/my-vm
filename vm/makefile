
vm: vm.o stack.o thread.o
	gcc -g -o vm vm.o stack.o thread.o

vm.o: vm.c vm.h ../common/opCode.h stack.h
	gcc -g -c vm.c

stack.o: stack.c stack.h
	gcc -g -c stack.c

thread.o: thread.c thread.h stack.h
	gcc -g -c thread.c

clean:
	rm *.o vm 
