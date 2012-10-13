#include "thread.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern char* gpStackTop;
extern char* gpStackBottom;
extern struct frame* gpCurFrame;
extern char* gpPC;

static struct thread* pthreadList = NULL;
static struct thread* pCurrentThread = NULL;
static int lowestID = 0;

struct thread* create(char* pMethod, int stackSize, int priority, char* PC, char* pName)
{
	struct thread* p = NULL;
	assert((pMethod != NULL) && (PC != NULL));

	p = (struct thread* )malloc(sizeof(struct thread));

	if(p != NULL)
	{
		p->threadID = lowestID ++;
		p->status = READY;
		p->priority = priority;
		p->PC = PC;
		p->pName = pName;
		p->prev = NULL;
		p->next = NULL;
		p->pFather = pCurrentThread;

		p->pStack = initStack(stackSize);
		
		if(p->pStack == NULL)
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}

	if(pthreadList != NULL)
	{
		pthreadList->next = p;
		p->prev = pthreadList;
	}
	else
	{
		pthreadList = p;
		pthreadList->next = p;
		pthreadList->prev = p;
	}

	return p;
}

int start(struct thread* pthread)
{
	int ret = -1;
	if(pthread != pCurrentThread)
	{
		//TODO
		gpPC = pthread->PC;
		gpStackTop = pthread->pStack->pStackTop;
		gpStackBottom = pthread->pStack->pStackBottom;
		gpCurFrame = pthread->pStack->pCurFrame;

		pthread->status = RUNNING;

		ret = 0;
	}
	else
	{
		ret = -1;
		printf("thread %s is runing\n" ,pthread->pName);
	}

	return ret;
}

static void addThread(struct thread* pthread)
{

}

static void removeThread(struct thread* pthread)
{

}

static struct thread* self()
{
	return pCurrentThread;

}

void stop(struct thread* pthread)
{
	assert(pthread != NULL);
	
	if(pthread != pCurrentThread)
	{
	}


}

void stopSelf()
{

}

void suspend(struct thread* pthread)
{

}
void resume(struct thread* pthread)
{

}

struct thread* getNextReadyThread()
{
	struct thread* p = pCurrentThread->next;
	struct thread* pRet = NULL;

	while(p != pCurrentThread)
	{
		if(p->status == READY)
		{
			pRet = p;
			break;
		}
		else
		{
			p = p->next;
		}
	}

	return pRet;
}

int switchToNextThread()
{
	int ret = -1;
	struct thread* p = getNextReadyThread();

	if((p != NULL) && (p != pCurrentThread))
	{
		gpPC = p->PC;
		gpStackTop = p->pStack->pStackTop;
		gpStackBottom = p->pStack->pStackBottom;
		gpCurFrame = p->pStack->pCurFrame;

		
		pCurrentThread->status = READY;
		pCurrentThread = p;
		pCurrentThread->status = RUNNING; 

		ret = 0;
	}
	else
	{
		ret = -1;
		printf("swtich thread error\n");
	}

	return ret;

}

//TODO should remove the destroyed pthread from the theadlist
static void destroy(struct thread* pthread)
{
	assert(pthread != NULL);


	if(pthread->pName != NULL)
	{
		//TODO maybe I need to implement this function in a method
		free(pthread->pName);
	}
	destroyStack(pthread->pStack->pStackBottom);
}
