#ifndef HELPER_H__
#define HELPER_H__

#include "structs.h"
#include "enums.h"

#include <stdio.h>

int isLeapYear(const int);

int hasICSTag(const char*, const char*);

MYERRNO ICSTimeStampReader(const char*, DateTime*);

MYERRNO ICSVEventCounter(const char*, int*);

int isValidYear(const int);

int isValidMonth(const int);

int isValidDay(const int);

int isValidHour(const int);

int isValidMinute(const int);

int isValidDate(const Date); // TODO

void removeNewLineChar(char*);

int myatoi(const char*, int*);

void icsTagRemover(char*, char*);

int promptYN(char*, ...);

#endif
