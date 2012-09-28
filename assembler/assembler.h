#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#define byte char
#define MAX_FILE_LENGTH 0xFFFF
#define MAX_INS_LEN 10
#define MAX_LABEL_LEN 20

typedef struct label{
	char* pLabelName;
	int addr;
	struct label* next;
}Label;

typedef struct callINS{
	int fromAddr;
	char* targetLabelName;
	struct callINS* next;
}CallINS;


char* loadSourceFile(char* pathName);

void eatWhiteSpaces(char** ppSrcCode);

int parse(char* pSrcCode, char* pTargetBin);

byte readByteValue(char** ppSrcCode);
short readWordValue(char** ppSrcCode);
int readDWordValue(char** ppSrcCode);
void readMneumonic(char** ppSrcCode, char* pmneumonic);
int readLabelName(char** ppSrcCode, char* pLabelName);

void moveToNextLine(char** ppSrcCode);


Label* initLabel(char* pLabelName,int len, int addr);
CallINS* initCallINS(int fromAddr, char* pTargetLabelName, int len);
int addLabel(Label* pLabels, Label* pLabel);
int addCallINS(CallINS* pCallINSs, CallINS* pCallINS);
int findCallINSLabelAddr(CallINS* pCallINS, Label* pLabels);
void updateCallINSsLabelAddr(char* pTargetBin, CallINS* pCallINSs, Label* pLabels);

//TODO
void freeLabels(Label* pLabels);
void freeCallINSs(CallINS* pCallINSs);

#endif
