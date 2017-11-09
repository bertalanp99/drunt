#ifndef DBHANDLER_H__
#define DBHANDLER_H__

#include "structs.h"
#include "enums.h"

MYERRNO ics_load(const char*, Calendar*);

MYERRNO ics_write(const Calendar*, char*);

MYERRNO VEvent_add(Calendar*, const VEvent);

MYERRNO Calendar_create(Calendar*);

MYERRNO Calendar_addVEvent(Calendar*, VEvent);

MYERRNO Calendar_deleteVEvent(Calendar*, VEvent);

MYERRNO VEvent_delete(VEvent*);

#endif
