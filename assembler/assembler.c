#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>

#include "../common/opCode.h"
#include "assembler.h"
#include "errCode.h"



#define ISWHITESPACE(c) (((c) == ' ') || ((c) == '\t') || ((c) == '\n'))
#define ISLETTER(c) (((c)== '_') || ((c) >= 'a' && (c) <= 'z') \
		|| ((c) >= 'A' && (c) <= 'Z') || ((c) >= '0' && ((c) <= '9')))



char* loadSourceFile(char* pathName)
{
	char* p = NULL;
	int fd = -1;
	int len = 0;
	if(pathName == NULL)
	{
		return NULL;
	}

	fd = open(pathName, O_RDONLY);
	if(fd < 0)
	{
		return NULL;
	}

	p = (char* )malloc(1 + MAX_FILE_LENGTH * sizeof(char));

	len = read(fd, p, MAX_FILE_LENGTH);
	*(p + len) = '\0';
	close(fd);
	return p;
	
}


void eatWhiteSpaces(char** ppSrcCode)
{
	while(ISWHITESPACE(**ppSrcCode))
	{
		(*ppSrcCode)++;
	}
}


byte readByteValue(char** ppSrcCode)
{
	char value = **ppSrcCode;
	(*ppSrcCode) ++;
	return value;
}


short readWordValue(char** ppSrcCode)
{
	char tmp[10];
	char*p = tmp;
	short value = 0;
	memset(tmp, '\0', 10);

	//while(ISLETTER(**ppSrcCode))
	while(!ISWHITESPACE(**ppSrcCode))
	{
		*p ++ = **ppSrcCode;
		(*ppSrcCode) ++;
	}

	value = (short)strtol(tmp, NULL, 0);


	return value;
}

//TODO delete this method
int readDWordValue(char** ppSrcCode)
{
	char tmp[10];
	char*p = tmp;
	int value = 0;
	memset(tmp, '\0', 10);

	//while(ISLETTER(**ppSrcCode))
	while(!ISWHITESPACE(**ppSrcCode))
	{
		*p ++ = **ppSrcCode;
		(*ppSrcCode) ++;
	}

	value = (int)strtol(tmp, NULL, 0);


	return value;
}

void moveToNextLine(char** ppSrcCode)
{
	int ret = 0;
	while((**ppSrcCode != '\0') && (**ppSrcCode != '\n'))
	{
		(*ppSrcCode) ++;
	}

	if(**ppSrcCode == '\n')
	{
		(*ppSrcCode) ++;
	}

}

void readMneumonic(char** ppSrcCode, char* pmneumonic)
{

	memset(pmneumonic, '\0', MAX_INS_LEN);
	
	while(ISLETTER(**ppSrcCode))
	{
		*pmneumonic++ = **ppSrcCode;
		(*ppSrcCode)++;
	}
}

int getSting(char** ppSrcCode, char* pdest)
{
	char* p = pdest;
	memset(pdest, '\0', MAX_LABEL_LEN);

	while(ISLETTER(**ppSrcCode))
	{
		*p ++ = **ppSrcCode;
		(*ppSrcCode)++;
	}

	return p - pdest;

}

Label* initLabel(char* pLabelName, int len, int addr)
{
	Label* pLabel = (Label* )malloc(sizeof(Label));
	char* p = NULL;

	//if((pLabel != NULL) && (pLabelName != NULL))
	if(pLabel != NULL)
	{
		if(pLabelName != NULL)
		{
			p = (char* )malloc(len + 1);
			//including the terminating null byte('\0')
			strcpy(p, pLabelName);
		}

		pLabel->pLabelName = p;
		pLabel->addr = addr;
		pLabel->nVars = 0;
		pLabel->pVars = NULL;
		pLabel->next = NULL;
	}
	else 
	{
		//TODO logs
		//under linux enviroment, while ptr is NULL, no operation is performed
		free(pLabel);
		free(p);
		pLabel = NULL;
		p = NULL;
	}

	return pLabel;
}

static void freeLabel(Label* pLabel)
{
	assert(pLabel != NULL);

	free(pLabel->pLabelName);
	free(pLabel);

}

// TODO implement this method as a common macro
void freeLabels(Label* pLabels)
{
	Label* pLabel = NULL;
	Label* ptmp = NULL;
	assert(pLabels);

	pLabel = pLabels->next;
	while(pLabel != NULL)
	{
		ptmp = pLabel->next;
		freeLabel(pLabel);
		pLabel = ptmp;
	}

	if(pLabel != NULL)
	{
		freeLabel(pLabel);
	}
	
}


CallINS* initCallINS(int fromAddr, char* pTargetLabelName, int len)
{
	CallINS* pCallINS = (CallINS* )malloc(sizeof(CallINS));
	char* p = (char* )malloc(len + 1);

	if((pCallINS != NULL) && (p != NULL))
	{
		if(pTargetLabelName != NULL)
		{
			strcpy(p, pTargetLabelName);
		}
		pCallINS->fromAddr = fromAddr;
		pCallINS->targetLabelName = p;
		pCallINS->next = NULL;
	}
	else
	{
		//TODO logs
		free(pCallINS);
		free(p);
		pCallINS = NULL;
		p = NULL;
		printf("initCallINS error\n");
	}
	return pCallINS;
}

static void freeCallINS(CallINS* pCallINS)
{
	assert(pCallINS != NULL);

	free(pCallINS->targetLabelName);
	free(pCallINS);
}

void freeCallINSs(CallINS* pCallINSs)
{
	CallINS* pCallINS = NULL;
	CallINS* ptmp = NULL;
	assert(pCallINSs);

	pCallINS = pCallINSs->next;
	while(pCallINS != NULL)
	{
		ptmp = pCallINS->next;
		freeCallINS(pCallINS);
		pCallINS = ptmp;
	}

	if(pCallINS != NULL)
	{
		freeCallINS(pCallINS);
	}
	
}

Var* initVar(int type, char* pVarName, int len)
{
	Var* pVar = (Var* )malloc(sizeof(Var));
	char* p = NULL;

	if(pVar != NULL)
	{
		if(pVarName != NULL)
		{
			p = (char* )malloc(len + 1);
			strcpy(p, pVarName);
		}
		pVar->type = type;
		pVar->pName = p;
		pVar->next = NULL;
	}
	else
	{
		free(pVar);
		free(p);
		pVar = NULL;
	}

	return pVar;
}

static void freeVar(Var* pVar)
{
	assert(pVar != NULL);

	free(pVar->pName);
	free(pVar);
}

void freeVars(Var* pVars)
{
	Var* pVar = NULL;
	Var* ptmp = NULL;
	assert(pVars);

	pVar = pVars->next;
	while(pVar != NULL)
	{
		ptmp = pVar->next;
		freeVar(pVar);
		pVar = ptmp;
	}

	freeVar(pVar);
	
}

int addVar(Label* pLabel, Var* pVar)
{
	Var* p = NULL;
	assert((pLabel != NULL) && (pVar != NULL));


	if(pLabel->pVars == NULL)
	{
		pLabel->pVars = pVar;
		pLabel->pVars->next = NULL;
	}
	else
	{
		p = pLabel->pVars;

		while(p->next != NULL)
		{
			p = p->next;
		}

		p->next = pVar;
		p->next->next = NULL;
	}
	
	return 0;

}
int addLabel(Label* pLabels, Label* pLabel)
{
	assert((pLabels != NULL) && (pLabel != NULL));

	while(pLabels->next != NULL)
	{
		pLabels = pLabels->next;
	}

	pLabels->next = pLabel;
	pLabels->next->next = NULL;

	return 0;
	
}
int addCallINS(CallINS* pCallINSs, CallINS* pCallINS)
{
	assert((pCallINSs != NULL) && (pCallINS != NULL));

	while(pCallINSs->next != NULL)
	{
		pCallINSs = pCallINSs->next;
	}
	pCallINSs->next = pCallINS;
	pCallINSs->next->next = NULL;

	return 0;
}
int findCallINSLabelAddr(CallINS* pCallINS, Label* pLabels)
{
	Label* pLabel = NULL;
	assert((pCallINS != NULL) && (pLabels != NULL));

	while(pLabels->next != NULL)
	{
		pLabel = pLabels->next;
		pLabels = pLabel;
		if(0 == strcmp(pLabel->pLabelName, pCallINS->targetLabelName))
		{
			return pLabel->addr;
		}

	}

	return -1;
}

static int haveDefined(Label* pLabel, char* pVarName)
{
	Var* pVar = NULL;
	int ret = 0;
	assert((pLabel != NULL) && (pVarName != NULL));
	pVar = pLabel->pVars;
	while(pVar != NULL)
	{
		if(strcmp(pVar->pName, pVarName) == 0)
		{
			ret = 1;
			break;
		}
		else
		{
			pVar = pVar->next;
		}

	}

	return ret;
}

static int getVarIndex(Label* pLabel, char* pVarName)
{
	int index = 0;
	int hasDefined = 0;
	Var* pVar = NULL;

	assert((pLabel != NULL) && (pVarName != NULL));
	pVar = pLabel->pVars;

	while(pVar != NULL)
	{
		if(strcmp(pVar->pName, pVarName) == 0)
		{
			hasDefined = 1;
			break;
		}
		else
		{
			pVar = pVar->next;
			index ++;
		}
	}

	if(hasDefined)
	{
		return index;
	}
	else
	{
		return -1;
	}

}


//pTargetCode: pointer to the targetCode, which is now store in a char buffer
void updateCallINSsLabelAddr(char* pTargetByteCodeStart, CallINS* pCallINSs, Label* pLabels)
{
	CallINS* pCallINS;
	int addr = -1;
	assert((pCallINSs != NULL) && (pLabels != NULL));

	while(pCallINSs->next != NULL)
	{
		pCallINS = pCallINSs->next;
		pCallINSs = pCallINS;

		addr = findCallINSLabelAddr(pCallINSs, pLabels);
		if(addr >= 0)
		{
			//TODO & CAUTION
			// little endian or big endian
			*(int* )(pTargetByteCodeStart + pCallINS->fromAddr) = addr;
		}

	}

}

void updateLabelsNVars(char* pTargetByteCodeStart, Label* pLabels)
{
	Label* pLabel = NULL;
	Var* pVars = NULL;
	int n = 0; 
	
	assert((pTargetByteCodeStart != NULL) && (pLabels != NULL) && (pLabels->next != NULL));
	pLabel = pLabels->next;
	
	while(pLabel != NULL)
	{
		pVars = pLabel->pVars;
		while(pVars != NULL)
		{
			n ++;
			pVars = pVars->next;
		}
		*(pTargetByteCodeStart + pLabel->addr) = (char)n;
		n = 0;
		pLabel = pLabel->next;
	}

}

int parse(char* pSrcCode, char* ptargetByteCode)
{
	char mneumonic[MAX_INS_LEN];
	char labelName[MAX_LABEL_LEN];
	char varName[MAX_VAR_LEN];
	char* ptargetByteCodeStart = ptargetByteCode;

	int labelNameLen;
	int varNameLen;

	Label* pLabel = NULL;
	CallINS* pCallINS = NULL;
	Var* pVar = NULL;

	Label* pLabels = initLabel(NULL, 0, 0);
	CallINS* pCallINSs = initCallINS(0, NULL, 0);


	*ptargetByteCode ++ = 'B';
	*ptargetByteCode ++ = '3';
	*ptargetByteCode ++ = '2';

	while(*pSrcCode != '\0')
	{
		//is label?
		if(ISLETTER(*pSrcCode))
		{
			labelNameLen = getSting(&pSrcCode, labelName);
			pLabel = initLabel(labelName, labelNameLen, ptargetByteCode - ptargetByteCodeStart);
			addLabel(pLabels, pLabel);

			ptargetByteCode ++;//use the first byte of the method to store the number of arguments of the method
			moveToNextLine(&pSrcCode);
		}
		else
		{
			eatWhiteSpaces(&pSrcCode);
			readMneumonic(&pSrcCode, mneumonic);

			//TODO parse mneumonic
			if(!strcasecmp(mneumonic, "NOP"))
			{
				*ptargetByteCode++ = (char)NOP;
			}
			else if(!strcasecmp(mneumonic, "PUSH"))
			{
				*ptargetByteCode++ = (char)PUSH;
			}
			else if(!strcasecmp(mneumonic, "POP"))
			{
				*ptargetByteCode++ = (char)POP;
			}
			else if(!strcasecmp(mneumonic, "ICONST"))
			{
				int value = 0;
				*ptargetByteCode++ = (char)ICONST;
				eatWhiteSpaces(&pSrcCode);
				value = readDWordValue(&pSrcCode);

				//*((int* )ptargetByteCode) = value;
				*ptargetByteCode ++ = (char)value;
				*ptargetByteCode ++ = (char)(value >> 8);
				*ptargetByteCode ++ = (char)(value >> 16);
				*ptargetByteCode ++ = (char)(value >> 24);


				//ptargetByteCode += 4;
			}
			else if(!strcasecmp(mneumonic, "ADD"))
			{
				*ptargetByteCode++ = (char)ADD;
			}
			else if(!strcasecmp(mneumonic, "RET_V"))
			{
				*ptargetByteCode++ = (char)RET_V;
			}
			else if(!strcasecmp(mneumonic, "RET_I"))
			{
				*ptargetByteCode++ = (char)RET_I;
			}
			else if(!strcasecmp(mneumonic, "CALL") || !strcasecmp(mneumonic, "THREAD"))
			{
				if(!strcasecmp(mneumonic, "CALL"))
				{
					*ptargetByteCode++ = (char)CALL;
				}
				else
				{
					*ptargetByteCode++ = (char)THREAD;
				}

				eatWhiteSpaces(&pSrcCode);
				labelNameLen = getSting(&pSrcCode, labelName);
				pCallINS = initCallINS(ptargetByteCode - ptargetByteCodeStart, labelName, labelNameLen);
				addCallINS(pCallINSs, pCallINS);

				ptargetByteCode += 4; // to store the called function's addr


			}
			else if(!strcasecmp(mneumonic, "INT"))
			{
				eatWhiteSpaces(&pSrcCode);
				varNameLen = getSting(&pSrcCode, varName);
				if(!haveDefined(pLabel, varName))
				{
					pVar = initVar(0, varName, varNameLen);
					addVar(pLabel, pVar);
					printf("define %s\n", varName);

					memset(varName, '\0', MAX_VAR_LEN);
				}
				else
				{
					printf("error: %s has been defined\n", varName);
					return HAVE_DEFINED;
				}
			}
			else if(!strcasecmp(mneumonic, "ISTORE"))
			{
				int index = getVarIndex(pLabel, varName);

				eatWhiteSpaces(&pSrcCode);
				varNameLen = getSting(&pSrcCode, varName);
				//if(!haveDefined(pLabel, varName))
				//{
				//	printf("%s has not been define\n", varName);
				//	return(HAVE_NOT_DEFINED);
				//}
				//else
				//{
				//	int index = getVarIndex(pLabel, varName);
				//	//...

				//}

				index = getVarIndex(pLabel, varName);

				*ptargetByteCode++ = (char)ISTORE;
				if(index < 0)
				{
					printf("%s has not been define\n", varName);
					return(HAVE_NOT_DEFINED);
				}
				else
				{
					*ptargetByteCode++ = (char)index;
				}
			}
			else if(!strcasecmp(mneumonic, "ILOAD"))
			{
				int index = -1;
				eatWhiteSpaces(&pSrcCode);
				varNameLen = getSting(&pSrcCode, varName);
				index = getVarIndex(pLabel, varName);

				*ptargetByteCode++ = (char)ILOAD;
				if(index < 0)
				{
					printf("%s has not been define\n", varName);
					return(HAVE_NOT_DEFINED);
				}
				else
				{
					*ptargetByteCode++ = (char)index;
				}
			}


			moveToNextLine(&pSrcCode);

		}
	}

	updateCallINSsLabelAddr(ptargetByteCodeStart, pCallINSs, pLabels);
	updateLabelsNVars(ptargetByteCodeStart, pLabels);

	freeLabels(pLabels);
	freeCallINSs(pCallINSs);

	return ptargetByteCode - ptargetByteCodeStart;
}

void help()
{
	printf("./assembler srcFileName targetFileName\n");
}

int main(int argc, char** argv)
{
	char* pSrcCode;
	char* pSrcFilePath;
	char* pTargetFilePath;
	char targetByteCode[MAX_FILE_LENGTH];
	int targetByteCodeLen = -1;
	int fd = -1;
	int ret = -1;

	char* tmp = "./test.b32";
	char* tmp1 = "./test.bbc";

#if (DEBUG_LEVEL > 0)
	pSrcFilePath = "./test.b32";
	pTargetFilePath = "./test.bbc";
#else
	pSrcFilePath = *(argv + 1);
	pTargetFilePath = *(argv + 2);

#endif

	//tmp
	char* p = targetByteCode;
	int i = 0;

#if (DEBUG_LEVEL > 0)
	if(argc < 0)
#else
	if(argc != 3)
#endif
	{
		help();
	}
	else
	{
		pSrcCode = loadSourceFile(pSrcFilePath);
		memset(targetByteCode, '\0', MAX_FILE_LENGTH);
		targetByteCodeLen = parse(pSrcCode, targetByteCode);

		if(targetByteCodeLen > 0)
		{
			fd = open(pTargetFilePath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
			
			if(fd > 0)
			{
				write(fd, targetByteCode, targetByteCodeLen);
				close(fd);
				ret = 0;
			}
			else
			{
				ret = -2;
			}
		}

	}



	for(i = 0; i < targetByteCodeLen; i++)
	{
		printf(" %x", *p);
		p++;
	}
	printf("\n");

	return ret;
}
