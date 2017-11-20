#ifndef HELPER_H__
#define HELPER_H__

#include "structs.h"
#include "enums.h"

#include <stdio.h>

int isLeapYear(const int);

int hasICSTag(const char*, const char*);

MYERRNO ICSVEventCounter(const char*, int*);

int isValidPriority(int);

int isValidICSTimeStamp(const char*);

int hasNewLineChar(const char*);

char* mystrncpy(char*, const char*, const int);

int isValidICS(const char*);

MYERRNO isNonEmptyFile(const char*);

MYERRNO ICSTimeStampReader(const char*, DateTime*);

int isValidYear(const int);

int isValidMonth(const int);

int isValidDay(const int);

int isValidHour(const int);

int isValidMinute(const int);

int isValidDate(const Date);

int isValidTime(const Time);

int isValidDateTime(const DateTime);

int isValidVEvent(const VEvent);

RELATIVEDATE compareDateTime(const DateTime, const DateTime);

RELATIVEDATE compareDate(const Date, const Date);

RELATIVEDATE compareTime(const Time, const Time);

void removeNewLineChar(char*);

int myatoi(const char*, int*);

void icsTagRemover(char*, char*);

int promptYN(char*, ...);

int compareVEvent(const VEvent, const VEvent);

DateTime currentDateTime(void);

DateTime addDaysToDateTime(const DateTime, const int);

int monthOverflows(const int);

int dayOverflows(const int, const int, const int);

void printVEventWCount(const VEvent, const int);

void printVEvent(const VEvent);

#endif
