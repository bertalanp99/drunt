#ifndef COMMANDHANDLER_H__
#define COMMANDHANDLER_H__

#include "structs.h"
#include "interactiveHandler.h"
#include "dbHandler.h"
#include "errorHandler.h"
#include "helper.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_LINELENGTH 600

////////////
// ARRAYS //
////////////

static char* commands[] = {
    "help",
    "exit",
    "load",
    "create",
    "modify",
    "delete",
    "list"
};

static size_t numberOfCommands = ( sizeof commands / sizeof(char*) ); // TODO get rid of this

///////////////
// FUNCTIONS //
///////////////

int command_help(char**);

int command_exit(char**);

int command_load(char**);

int command_create(char**);

int command_modify(char**);

int command_delete(char**);

int command_list(char**);

///////////////////////
// FUNCTION POINTERS //
///////////////////////

extern int (*runCommand[])(char**);

#endif
