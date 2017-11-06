#ifndef DBHANDLER_H__
#define DBHANDLER_H__

#include "structs.h"

Calendar* ics_load(char*);

int ics_write(Calendar*, char*);

Calendar* entry_add(Calendar*, const Event);

#endif
