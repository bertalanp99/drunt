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

////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

extern size_t numberOfCommands;

extern char* commands[];

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

int command_find(char**);

///////////////////////
// FUNCTION POINTERS //
///////////////////////

extern int (*runCommand[])(char**);

#endif
