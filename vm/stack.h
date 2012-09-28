#ifndef STACK_H_
#define STATCK_H_

#define MAX_STACK_SIZE 10000

struct frame{
	char* PC;
	struct frame* pPreFrame;
};

// return the current Fp
int pushFrame();
int popFrame();
void pushI(int value);
int popI();

char* initStack(int stackSize);
void destroyStack(char* pStackBottom);

#endif
