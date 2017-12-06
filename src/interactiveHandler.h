#ifndef INTERACTIVEHANDLER_H__
#define INTERACTIVEHANDLER_H__

#include "structs.h"
#include "errorHandler.h"
#include "dbHandler.h"
#include "enums.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define MAX_PATHLENGTH 128
#define MAX_COMMANDLENGTH 256
#define TOKEN_BUFFSIZE 64
#define MAX_TOKEN_NUMBER 16
#define TOKEN_DELIMITER " \t\r\n"

#define YEAR_MIN 1950
#define YEAR_MAX 2050

///////////////
// FUNCTIONS //
///////////////

char* shell_readline(void);

char** shell_splitline(char*);

bool shell_execute(char**);

void shell(void);

void shell_say(ShellSays, const char*, ...);

MYERRNO shell_readTimestamp(DateTime*, TimestampType);

MYERRNO shell_readString(const char*, char**, const size_t);

MYERRNO shell_readNum(const char*, int*);

MYERRNO shell_readNumWithCriterion(const char*, int*, bool (*criterion)(int));

bool shell_promptYN(char*, ...);

bool shell_handleError(const MYERRNO, const char*, const int, const char*);

#endif
