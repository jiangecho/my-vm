#ifndef THREAD_H_
#define THREAD_H_

#define DEFAULT_STACK_SIZE 1024

#define READY 0
#define RUNNING 1

struct thread{
	int threadID;
	int status;
	int priority;
	struct stack* pStack;
	char* PC;
	char* pName;

	// now we find the children via their father
	struct thread* pFather;
	//struct thread* pChildren;

	struct thread* next;
	struct thread* prev;

};

struct thread* create(char* pMethod, int stackSize, int priority, char* PC, char* pName);
struct thread* self();
void destroy(struct thread* pthread);
struct thread* getNextReadyThread();
int start(struct thread* pthread);
void stop(struct thread* pthread);

void suspend(struct thread* pthread);
void resume(struct thread* pthread);

struct thread* getNextReadyThread();
struct thread* switchToNextThread();

#endif
