#ifndef HELPER_H__
#define HELPER_H__

#include "structs.h"
#include "enums.h"

#include <stdio.h>

int isLeapYear(const int);

int hasICSTag(const char*, const char*);

MYERRNO ICSTimeStampReader(const char*, DateTime*);

MYERRNO ICSVEventCounter(const char*, int*);

void removeNewLineChar(char*);

int myatoi(const char*, int*);

void icsTagRemover(char*, char*);

int promptYN(char*, ...);

#endif
