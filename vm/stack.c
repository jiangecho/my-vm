#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"

extern char* gpStackTop;
extern char* gpStackBottom;
extern struct frame* gpCurFrame;
extern char* gpPC;

static int sstackSize;

// return the current Fp
int pushFrame()
{
	struct frame* pFrame;

	//TODO implement the following line in macro
	//if((gpStackTop - gpStackBottom) < sizeof(struct frame))
	if(((gpStackBottom + sstackSize) - gpStackTop) < sizeof(struct frame))
	{
		printf("stack over follow\n");
		return -1;
	}

	pFrame = (struct frame* )gpStackTop;
	pFrame->PC = gpPC;
	pFrame->pPreFrame = gpCurFrame;

	gpStackTop += sizeof(struct frame);
	gpCurFrame = pFrame;

	return 0;

}

int popFrame()
{
	int ret = -1;
	gpStackTop = (char* )gpCurFrame;
	if(gpStackTop > gpStackBottom)
	{
		gpPC = gpCurFrame->PC;
		gpCurFrame = gpCurFrame->pPreFrame;	
		ret = 0;
	}

	return ret;
}

void pushI(int value)
{
	*gpStackTop ++ = (char)value;
	*gpStackTop ++ = (char)(value >> 8);
	*gpStackTop ++ = (char)(value >> 16);
	*gpStackTop ++ = (char)(value >> 24);
}

int popI()
{
	int value =   (((int)(*(--gpStackTop)) << 24) & 0xFF000000)
				| (((int)(*(--gpStackTop)) << 16) & 0x00FF0000)
				| (((int)(*(--gpStackTop)) << 8)  & 0x0000FF00)
				| ((int)(*(--gpStackTop)) & 0x000000FF);

	return value;
}

char* initStack(int stackSize)
{
	char* p = NULL;
	assert(stackSize > 0);
	sstackSize = (stackSize > MAX_STACK_SIZE) ?  MAX_STACK_SIZE : stackSize;

	p = (char* )malloc(sstackSize);

	if(p != NULL)
	{
		memset(p, '\0', sstackSize);
	}

	return p;
}

void destroyStack(char* pStackBottom)
{
	free(pStackBottom);
}

//int main()
//{
//	return 0;
//}
