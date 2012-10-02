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

void storeDWordTo(char* p, int value)
{
	*p ++ = (char)value;
	*p ++ = (char)(value >> 8);
	*p ++ = (char)(value >> 16);
	*p ++ = (char)(value >> 24);
}

int loadDWordFrom(char* p)
{
//	int a, b, c, d;
//	a = ((int)(*(p++)))             & 0x000000FF;
//	b = ((int)(*(p++)) << 8)        & 0x0000FF00;
//	c = ((int)(*(p++)) << 16)       & 0x00FF0000;
//	d = ((int)(*(p++)) << 24)       & 0xFF000000;
//	return a | b | c | d;

	int value = 0;
	--p;
	//Attention: if you use p++, there is an issue here
	value =   (((int)(*(++p)))       & 0x000000FF) 
				| (((int)(*(++p)) << 8)  & 0x0000FF00) 
				| (((int)(*(++p)) << 16) & 0x00FF0000) 
				| (((int)(*(++p)) << 24) & 0xFF000000);

	return value;
}

int loadWordFrom(char* p)
{
	int value = 0;
	--p;
	value = (((int)(*(++p)))  & 0x00000000)
				| ((int)(*(++p)) & 0x0000FF00);

	return value;
}

int loadByteFrom(char* p)
{
	int value = ((int)(*p) & 0x000000FF);

	return value;
}
// return the current Fp
int pushFrame(int argc)
{
	struct frame* pFrame;

	//TODO implement the following line in macro
	//if((gpStackTop - gpStackBottom) < sizeof(struct frame))
	if(((gpStackBottom + sstackSize) - gpStackTop) < sizeof(struct frame))
	{
		printf("stack over follow\n");
		return -1;
	}

	gpStackTop += SIZE_OF_ARGs(argc);

	pFrame = (struct frame* )gpStackTop;
	pFrame->PC = gpPC;
	pFrame->pPreFrame = gpCurFrame;

	gpStackTop += sizeof(struct frame);
	gpCurFrame = pFrame;

	return 0;

}

int popFrame(int argc)
{
	int ret = -1;
	gpStackTop = (char* )gpCurFrame;
	gpStackTop -= SIZE_OF_ARGs(argc);
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
	storeDWordTo(gpStackTop, value);
	gpStackTop += 4;
//	*gpStackTop ++ = (char)value;
//	*gpStackTop ++ = (char)(value >> 8);
//	*gpStackTop ++ = (char)(value >> 16);
//	*gpStackTop ++ = (char)(value >> 24);
}

int popI()
{
//	int value =   (((int)(*(--gpStackTop)) << 24) & 0xFF000000)
//				| (((int)(*(--gpStackTop)) << 16) & 0x00FF0000)
//				| (((int)(*(--gpStackTop)) << 8)  & 0x0000FF00)
//				| ((int)(*(--gpStackTop)) & 0x000000FF);
//	return value;
	gpStackTop -= 4;

	return loadDWordFrom(gpStackTop);
}

struct stack* initStack(int stackSize)
{
	char* p = NULL;
	struct stack* pStack = NULL;
	assert(stackSize > 0);
	sstackSize = (stackSize > MAX_STACK_SIZE) ?  MAX_STACK_SIZE : stackSize;

	p = (char* )malloc(sstackSize);

	if(p != NULL)
	{
		memset(p, '\0', sstackSize);
		pStack = (struct stack* )malloc(sizeof(struct stack));

		if(pStack != NULL)
		{
			pStack->pStackTop = p;
			pStack->pStackBottom = p;
			pStack->pStack = p;
			pStack->pCurFrame = (struct frame* )p;
		}
		else
		{
			free(p);
			p = NULL;
		}
	}

	return pStack;
}

void destroyStack(char* pStackBottom)
{
	free(pStackBottom);
}

int loadI(int index)
{
	// the index starts from 0
	char* p2ArgsArea = (char* )gpCurFrame - SIZE_OF_ARGs(index + 1);
	return loadDWordFrom(p2ArgsArea);


}

void storeI(int index, int value)
{
	char* p2ArgsArea = (char* )gpCurFrame - SIZE_OF_ARGs(index + 1);
	storeDWordTo(p2ArgsArea, value);
}

//int main()
//{
//	return 0;
//}
