#include "thread.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	int argc = *PC++;
	int len = SIZE_OF_ARGs(argc);
	struct thread* p = NULL;
	struct frame* pFrame = NULL;

	//assert((pMethod != NULL) && (PC != NULL));
	assert(PC != NULL);

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
		
		if(p->pStack != NULL)
		{

			memset(p->pStack->pStackTop, 0, len);
			(p->pStack)->pStackTop += len;

			pFrame = (struct frame* )p->pStack->pStackTop;
			pFrame->PC = NULL;
			pFrame->argc = argc;
			pFrame->pPreFrame = NULL;

			p->pStack->pCurFrame = pFrame;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}

//	if(pCurrentThread == NULL)
//	{
//		pCurrentThread = p;
//	}
	if(pthreadList != NULL)
	{
		p->next = pthreadList->next;
		p->prev = pthreadList;
		pthreadList->next->prev = p;
		pthreadList->next = p;

//		pthreadList->next = p;
//		pthreadList->prev = p;
//		p->prev = pthreadList;
//		p->next = pthreadList;
	}
	else
	{
		pthreadList = p;
		pthreadList->next = p;
		pthreadList->prev = p;
	}

	printf("create thread: %d\n", p->threadID);
	return p;
}

int start(struct thread* pthread)
{
	int ret = -1;
	assert(pthread != NULL);

	if(pthread != pCurrentThread)
	{
		if(pCurrentThread != NULL)
		{
			//backup current thread's state
			pCurrentThread->status = READY;
			pCurrentThread->PC = gpPC;
			pCurrentThread->pStack->pStackTop = gpStackTop;
			pCurrentThread->pStack->pStackBottom = gpStackBottom;
			pCurrentThread->pStack->pCurFrame = gpCurFrame;
		}


		//recover from the next ready thread
		gpPC = pthread->PC;
		gpStackTop = pthread->pStack->pStackTop;
		gpStackBottom = pthread->pStack->pStackBottom;
		gpCurFrame = pthread->pStack->pCurFrame;

		pCurrentThread = pthread;
		pCurrentThread->status = RUNNING; 
		ret = 0;
	}
	else
	{
		ret = -1;
		printf("thread %d is runing\n" ,pthread->threadID);
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

struct thread* switchToNextThread()
{
	struct thread* p = getNextReadyThread();

	if(p != NULL)
	{
		start(p);
		printf("swtich to %d\n", p->threadID);
	}
	else
	{
		printf("swtich thread error\n");
	}

	return p;

}

//TODO should remove the destroyed pthread from the theadlist
static void destroy(struct thread* pthread)
{
	struct thread* p = NULL;

	assert(pthread != NULL);

	if(pthread->pName != NULL)
	{
		//TODO maybe I need to implement this function in a method
		free(pthread->pName);
	}

	destroyStack(pthread->pStack->pStackBottom);

	p = pthreadList;
	if(p->threadID == pthread->threadID)
	{
		if(p == p->next)
		{
			// the last thread has been killed
			pthreadList = NULL;
			pCurrentThread = NULL;
		}

	}
	else
	{
		p = pthreadList->next;
		while(p != pthreadList)
		{
			if(p->threadID == pthread->threadID)
			{
				p->prev->next = p->next;
				p->next->prev = p->prev;

				//attention, now p is the same as pthread
				free(p);

				break;
			}
			else
			{
				p = p->next;
			}

		}
	}
}

void stop(struct thread* pthread)
{
	assert(pthread != NULL);

	switchToNextThread();
	destroy(pthread);

}

void stopSelf()
{
	stop(pCurrentThread);
}
