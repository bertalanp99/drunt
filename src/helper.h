#ifndef HELPER_H__
#define HELPER_H__

#include "structs.h"
#include "enums.h"

#include <stdio.h>
#include <stdbool.h>


////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

static char days[7] = {'M', 'T', 'W', 'T', 'F', 'S', 'S'};

/////////////////
/// FUNCTIONS ///
/////////////////

int isLeapYear(const int);

int hasICSTag(const char*, const char*);

MYERRNO ICSVEventCounter(const char*, int*);

bool isValidPriority(int);

bool isValidICSTimeStamp(const char*);

bool hasNewLineChar(const char*);

char* mystrncpy(char*, const char*, const int);

bool isValidICS(const char*);

MYERRNO isNonEmptyFile(const char*);

MYERRNO ICSTimeStampReader(const char*, DateTime*);

bool isValidYear(const int);

bool isValidMonth(const int);

bool isValidDay(const int, const int, const int);

bool isValidHour(const int);

bool isValidMinute(const int);

bool isValidDate(const Date);

bool isValidTime(const Time);

bool isValidDateTime(const DateTime);

bool isValidVEvent(const VEvent);

RelativeDate compareDateTime(const DateTime, const DateTime);

RelativeDate compareDate(const Date, const Date);

RelativeDate compareTime(const Time, const Time);

void removeNewLineChar(char*);

int myatoi(const char*, int*);

void icsTagRemover(char*, char*);

bool promptYN(char*, ...);

bool compareVEvent(const VEvent, const VEvent);

DateTime currentDateTime(void);

DateTime addDaysToDateTime(const DateTime, const int);

bool monthOverflows(const int);

bool dayOverflows(const int, const int, const int);

void printVEvent(const VEvent, const int);

MYERRNO parseDateTime(DateTime*, const char*, const char*, const char*, const char*, const char*);

void printDayHeader(const Date);

void printMonthHeader(const Date);

void printMonthTitle(const int, const int, const char);

void printHourHeader(const int);

int zellerToISO(const Date);

void printSep(const char, const int, const bool);

void printMonth(Date, const Calendar*);

void printYear(Date, const int);

int countVEvents(const Calendar*, const DateTime, const DateTime);

#endif
