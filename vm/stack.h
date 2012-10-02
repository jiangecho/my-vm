#ifndef STACK_H_
#define STATCK_H_

#define MAX_STACK_SIZE 10000
#define SIZE_OF_ARGs(argc) ((argc) * sizeof(int))

#define DWORD_SIZE 4
#define BYTE_SIZE 1

struct frame{
	char* PC;
	struct frame* pPreFrame;
};

typedef struct stack{
	char* pStackTop;
	char* pStackBottom;
	char* pStack;
	struct frame* pCurFrame;

}Stack;

// return the current Fp
int pushFrame(int argc);
int popFrame(int argc);

void storeDWordTo(char* p, int value);
int loadDWordFrom(char* p);

void pushI(int value);
int popI();

int loadI(int index);
void storeI(int index, int value);

struct stack* initStack(int stackSize);
void destroyStack(char* pStackBottom);

#endif
