#ifndef HELPER_H__
#define HELPER_H__

#include "structs.h"

int isLeapYear(const int);

int hasICSTag(const char*, const char*);

int icsTimeStampReader(const char*, DateTime*);

int myatoi(const char*, int*);

void icsTagRemover(char*, char*);

int promptYN(char*, ...);

#endif
