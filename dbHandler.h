#ifndef DBHANDLER_H__
#define DBHANDLER_H__

#include "structs.h"
#include "enums.h"

Calendar ics_load(char*);

MYERRNO ics_write(Calendar*, char*);

MYERRNO entry_add(Calendar*, const Event);

#endif
