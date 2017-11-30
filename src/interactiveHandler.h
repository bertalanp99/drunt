#ifndef INTERACTIVEHANDLER_H__
#define INTERACTIVEHANDLER_H__

#include "structs.h"
#include "errorHandler.h"
#include "dbHandler.h"
#include "commandHandler.h"
#include "arrays.h"
#include "enums.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define MAX_PATHLENGTH 128
#define DEFAULTFILE "calendar.ics"
#define BUFFSIZE 512
#define MAX_COMMANDLENGTH 256
#define TOKEN_BUFFSIZE 64
#define MAX_TOKEN_NUMBER 16
#define TOKEN_DELIMITER " \t\r\n"

///////////////////////
// GLOBALS VARIABLES //
///////////////////////

Calendar calendar;
char* file;

///////////////
// FUNCTIONS //
///////////////

char* shell_readline(void);

char** shell_splitline(char*);

bool shell_execute(char**);

void shell(void);

void shell_say(ShellSays, const char*, ...);

bool shell_readTimeStamp(DateTime*, TIMESTAMPTYPE);

MYERRNO shell_readString(char**, const size_t, const bool (*)(char*));

MYERRNO shell_readNum(int*, const bool (*)(int));

bool shell_promptYN(char*, ...);

bool shell_handleError(const MYERRNO, const char*, const int, const char*);

#endif
