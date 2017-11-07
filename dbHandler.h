#ifndef DBHANDLER_H__
#define DBHANDLER_H__

#include "structs.h"
#include "enums.h"

void ics_load(char*, Calendar*);

MYERRNO ics_write(Calendar*, char*);

MYERRNO entry_add(Calendar*, const Event);

#endif
