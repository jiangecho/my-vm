#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>

#include "../common/opCode.h"
#include "assembler.h"



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

int readLabelName(char** ppSrcCode, char* pLabelName)
{
	char* p = pLabelName;
	memset(pLabelName, '\0', MAX_LABEL_LEN);

	while(ISLETTER(**ppSrcCode))
	{
		*p ++ = **ppSrcCode;
		(*ppSrcCode)++;
	}

	return p - pLabelName;

}

Label* initLabel(char* pLabelName, int len, int addr)
{
	Label* pLabel = (Label* )malloc(sizeof(Label));
	char* p = (char* )malloc(len + 1);

	if((pLabel != NULL) && (pLabelName != NULL))
	{
		//including the terminating null byte('\0')
		strcpy(p, pLabelName);

		pLabel->pLabelName = p;
		pLabel->addr = addr;
		pLabel->next = NULL;
	}
	else 
	{
		//TODO logs
		//under linux enviroment, while ptr is NULL, no operation is performed
		free(pLabel);
		free(p);
	}

	return pLabel;
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
		printf("initCallINS error\n");
	}
	return pCallINS;
}

int addLabel(Label* pLabels, Label* pLabel)
{
	assert(pLabels != NULL && (pLabel != NULL));

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


//pTargetCode: pointer to the targetCode, which is now store in a char buffer
void updateCallINSsLabelAddr(char* pTargetCode, CallINS* pCallINSs, Label* pLabels)
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
			*(int* )(pTargetCode + pCallINS->fromAddr) = addr;
		}

	}

}

int parse(char* pSrcCode, char* pTargetBin)
{
	char mneumonic[MAX_INS_LEN];
	char labelName[MAX_LABEL_LEN];
	char* pTargetBinStart = pTargetBin;

	int labelNameLen;

	Label *pLabel = NULL;
	CallINS* pCallINS = NULL;

	Label *pLabels = initLabel(NULL, 0, 0);
	CallINS* pCallINSs = initCallINS(0, NULL, 0);


	*pTargetBin ++ = 'B';
	*pTargetBin ++ = '3';
	*pTargetBin ++ = '2';

	while(*pSrcCode != '\0')
	{
		//is label?
		if(ISLETTER(*pSrcCode))
		{
			labelNameLen = readLabelName(&pSrcCode, labelName);
			pLabel = initLabel(labelName, labelNameLen, pTargetBin - pTargetBinStart);
			addLabel(pLabels, pLabel);

			moveToNextLine(&pSrcCode);
		}
		else
		{
			eatWhiteSpaces(&pSrcCode);
			readMneumonic(&pSrcCode, mneumonic);

			//TODO parse mneumonic
			if(!strcasecmp(mneumonic, "NOP"))
			{
				*pTargetBin++ = (char)NOP;
			}
			else if(!strcasecmp(mneumonic, "PUSH"))
			{
				*pTargetBin++ = (char)PUSH;
			}
			else if(!strcasecmp(mneumonic, "POP"))
			{
				*pTargetBin++ = (char)POP;
			}
			else if(!strcasecmp(mneumonic, "ICONST"))
			{
				int value = 0;
				*pTargetBin++ = (char)ICONST;
				eatWhiteSpaces(&pSrcCode);
				value = readDWordValue(&pSrcCode);

				//*((int* )pTargetBin) = value;
				*pTargetBin ++ = (char)value;
				*pTargetBin ++ = (char)(value >> 8);
				*pTargetBin ++ = (char)(value >> 16);
				*pTargetBin ++ = (char)(value >> 24);


				//pTargetBin += 4;
			}
			else if(!strcasecmp(mneumonic, "ADD"))
			{
				*pTargetBin++ = (char)ADD;
			}
			else if(!strcasecmp(mneumonic, "RET_V"))
			{
				*pTargetBin++ = (char)RET_V;
			}
			else if(!strcasecmp(mneumonic, "RET_I"))
			{
				*pTargetBin++ = (char)RET_I;
			}
			else if(!strcasecmp(mneumonic, "CALL"))
			{
				*pTargetBin++ = (char)CALL;

				eatWhiteSpaces(&pSrcCode);
				labelNameLen = readLabelName(&pSrcCode, labelName);
				pCallINS = initCallINS(pTargetBin - pTargetBinStart, labelName, labelNameLen);
				addCallINS(pCallINSs, pCallINS);

				pTargetBin += 4; // to store the called function's addr


			}


			moveToNextLine(&pSrcCode);

		}
	}

	updateCallINSsLabelAddr(pTargetBinStart, pCallINSs, pLabels);

	return pTargetBin - pTargetBinStart;
}

void help()
{
	printf("./assembler srcFileName targetFileName\n");
}

int main(int argc, char** argv)
{
	char* pSrcCode;
	char targetBin[MAX_FILE_LENGTH];
	int targetBinLen = -1;
	int fd = -1;
	int ret = -1;

	//tmp
	char* p = targetBin;
	int i = 0;

	if(argc != 3)
	{
		help();
	}
	else
	{
		pSrcCode = loadSourceFile(*(argv + 1));
		memset(targetBin, '\0', MAX_FILE_LENGTH);
		targetBinLen = parse(pSrcCode, targetBin);

		fd = open(*(argv + 2), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		
		if(fd > 0)
		{
			write(fd, targetBin, targetBinLen);
			close(fd);
			ret = 0;
		}
		else
		{
			ret = -2;
		}

	}



	for(i = 0; i < targetBinLen; i++)
	{
		printf(" %x", *p);
		p++;
	}
	printf("\n");

	return ret;
}
