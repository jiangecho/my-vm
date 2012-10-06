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

void start(struct thread* pthread)
{
	struct thread* p = pCurrentThread;
	do{
		if(pthread == p)
		{
			//TODO
			gpPC = pthread->PC;
			gpStackTop = pthread->pStack->pStackTop;
			gpStackBottom = pthread->pStack->pStackBottom;
			gpCurFrame = pthread->pStack->pCurFrame;
			break;
		}
		else
		{
			p = p->next;
		}
	}
	while(p != pCurrentThread);
}

static void addThread(struct thread* pthread)
{

}

static void removeThread(struct thread* pthread)
{

}
static struct thread* self()
{

}

