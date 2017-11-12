#ifndef DBHANDLER_H__
#define DBHANDLER_H__

#include "structs.h"
#include "enums.h"

MYERRNO ICS_load(const char*, Calendar*);

MYERRNO ICS_write(const char*, const Calendar*, WriteMode);

MYERRNO VEvent_add(Calendar*, const VEvent);

MYERRNO Calendar_create(Calendar*);

MYERRNO Calendar_destroy(Calendar*);

MYERRNO Calendar_addVEvent(Calendar*, VEvent);

MYERRNO Calendar_deleteVEvent(Calendar*, VEvent);

MYERRNO VEvent_delete(VEvent*);

#endif
