#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "../common/opCode.h"
#include "stack.h"

#define MAX_FILE_LENGTH 10000


char* gpStackTop;
char* gpStack;
char* gpStackBottom;
char* gpPC;
char* gpBCStart;
struct frame* gpCurFrame;
static int sargc;

char* loadByteCode(char* pPath)
{
	char* p = NULL;
	int fd = -1;
	int len = 0;
	int i = 0;

	if(pPath == NULL)
	{
		return NULL;
	}

	fd = open(pPath, O_RDONLY);
	if(fd < 0)
	{
		return NULL;
	}

	p = (char* )malloc(1 + MAX_FILE_LENGTH * sizeof(char));

	len = read(fd, p, MAX_FILE_LENGTH);

	for(i = 0; i < len; i++)
	{
		printf("%x ", *(p + i));
	}
	printf("\n ");

	*(p + len) = '\0';
	close(fd);
	return p;
}


int initVm(int stackSize)
{
	int ret = -1;
	char* p = NULL;
	if(stackSize <= 0)
	{
		ret = -1;
	}
	else
	{
		//TODO now the main method do not support any args.
		sargc = 0;
		stackSize = (stackSize > MAX_STACK_SIZE) ? MAX_STACK_SIZE : stackSize;
		p = initStack(stackSize);
		if(p != NULL)
		{
			gpStackBottom = p;
			gpStackTop = p;

			if(pushFrame(sargc) == 0)
			{
				gpCurFrame = (struct frame* )p;
				ret = 0;
			}

		}
		else
		{
			ret = -2;
		}
	}

	return ret;
}

void abortVm()
{
	destroyStack(gpStackBottom);
	free(gpBCStart);

	gpStackBottom = NULL;
	gpStackTop = NULL;
	gpCurFrame = NULL;
	gpPC = NULL;
	gpBCStart = NULL;
}

// maybe in the feature,
// every thread will have its own interpreter
// and the thread shedule will base on the linux kernel
int interpret(char* pPC)
{
	//char* PC = pPC;
	int gameOver = 0;

	//TODO implement the interpret in threaded interpret
	while(!gameOver)
	{
		switch(*gpPC++)
		{
case ICONST:
	{
//	int value = (int)(*(gpPC))
//				| ((int)(*(gpPC + 1)) << 8)
//				| ((int)(*(gpPC + 2)) << 16)
//				| ((int)(*(gpPC + 3)) << 24);
//
//	pushI(value);
//	gpPC += 4;

		*gpStackTop ++ = *gpPC ++;
		*gpStackTop ++ = *gpPC ++;
		*gpStackTop ++ = *gpPC ++;
		*gpStackTop ++ = *gpPC ++;

	}

	break;
case NOP:
	//gpPC ++;
	break;

case ADD:
	{
		int a = popI();
		int b = popI();

		pushI(a + b);

		//gpPC++;

	}
	break;

case CALL:
	{
		int targetAddr = loadDWordFrom(gpPC);

		gpPC += 4;
		//now, the first byte of one method is the count of arguments.
		sargc = (int)(*(gpBCStart + targetAddr));
		pushFrame(sargc);
		//TODO there is something wrong here
		gpPC = gpBCStart + targetAddr;
	}
	break;
case RET_V:
	if(popFrame(sargc) < 0)
	{
		gameOver = 1;
	}
	sargc = 0;
	break;

case RET_I:
	{
		int value = popI();
		printf("RET_I:%d\n", value);

		if(popFrame(sargc) < 0)
		{
			gameOver = 1;
		}
		else
		{
			pushI(value);
		}

		sargc = 0;
	}
	break;

case ISTORE:
	{
		int index = loadByteFrom(gpPC);
		int value = popI();
		storeI(index, value);

	}

	break;

case ILOAD:
	{
		int index = loadByteFrom(gpPC);
		int value = loadI(index);
		pushI(value);

	}

	break;
default:
	break;
		}
	}

	return 0;

}

static help()
{
	printf("usage:\n      vm xx.bbc\n");
}

int main(int argc, char** argv)
{
	char* p = NULL;
	if(argc < 2)
	{
		help();
		return -1;
	}

	p = loadByteCode(*(argv + 1));
	if(p != NULL )
	{
		gpBCStart = p; // the first three bytes are magic numbers, and we do not check it here
		gpPC = p + 3;
		if(initVm(1000) == 0)
		{

			interpret(gpPC);
		}
	}

	return 0;
}
