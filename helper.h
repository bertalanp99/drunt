#ifndef HELPER_H__
#define HELPER_H__

#include "structs.h"

int isLeapYear(const int);

int icsTimeStampReader(const char*, DateTime*);

int myatoi(const char*, int*);

char* icsTagRemover(char*, char*);

int promptYN(char*);

#endif
