#ifndef VM_H_
#define VM_H_

int initVm(int stackSize);
void abortVm();
int interpret(char* pPC);

char* loadByteCode(char* pPath);

int getDWordValue(char* pPC);
short getWordValue(char* pPC);

#endif
