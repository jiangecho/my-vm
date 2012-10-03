#ifndef THREAD_H_
#define THREAD_H_

struct thread{
	int threadID;
	int status;
	int priority;
	struct stack* pStack;
	char* pPC;

};
struct thread* create(char* pMethod, int stackSize, int priority);
void start(struct thread* pthread);
void stop(struct thread* pthread);
void destroy(struct thread* pthread);

void pause(struct thread* pthread);
void continue(struct thread* pthread);

#endif
