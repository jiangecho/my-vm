#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#define DEBUG_LEVEL		1
#define byte char
#define MAX_FILE_LENGTH 0xFFFF
#define MAX_INS_LEN		10
#define MAX_LABEL_LEN	20
#define MAX_VAR_LEN		20

typedef struct variable{
	int type;
	char* pName;
	struct variable* next;
}Var;

typedef struct label{
	char* pLabelName;
	Var* pVars;
	int addr;
	int nVars;
	struct label* next;
}Label;

typedef struct callINS{
	int fromAddr;
	char* targetLabelName;
	struct callINS* next;
}CallINS;


char* loadSourceFile(char* pathName);

void eatWhiteSpaces(char** ppSrcCode);

int parse(char* pSrcCode, char* pTargetByteCode);

byte readByteValue(char** ppSrcCode);
short readWordValue(char** ppSrcCode);
int readDWordValue(char** ppSrcCode);
void readMneumonic(char** ppSrcCode, char* pmneumonic);
int getSting(char** ppSrcCode, char* pdest);

void moveToNextLine(char** ppSrcCode);


Label* initLabel(char* pLabelName,int len, int addr);
CallINS* initCallINS(int fromAddr, char* pTargetLabelName, int len);
Var* initVar(int type, char* pVarName, int len);

int addLabel(Label* pLabels, Label* pLabel);
int addCallINS(CallINS* pCallINSs, CallINS* pCallINS);
int addVar(Label* pLabel, Var* pVar);
int findCallINSLabelAddr(CallINS* pCallINS, Label* pLabels);
void updateCallINSsLabelAddr(char* pTargetByteCodeStart, CallINS* pCallINSs, Label* pLabels);
void updateLabelsArgc(char* pTargetByteCodeStart, Label* pLabels);

//TODO
void freeLabels(Label* pLabels);
void freeCallINSs(CallINS* pCallINSs);
void freeVars(Var* pVars);

#endif
