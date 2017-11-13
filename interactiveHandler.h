#ifndef INTERACTIVEHANDLER_H__
#define INTERACTIVEHANDLER_H__

#include "structs.h"
#include "errorHandler.h"
#include "dbHandler.h"
#include "commandHandler.h"
#include "arrays.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#define MAX_PATHLENGTH 128
#define DEFAULTFILE "calendar.ics"
#define BUFFSIZE 256
#define MAX_COMMANDLENGTH 128
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

int shell_execute(char**);

void shell(void);

void shell_say(ShellSays, const char*, ...);

int shell_readTimeStamp(DateTime*, TIMESTAMPTYPE);

int shell_readString(char**);

#endif
