#include "debug.h"

#include <stdio.h>
#include <assert.h>

void printRawCalendar(Calendar cal)
{
    for (int i = 0; i < cal.numberOfEntries; ++i)
    {
        printRawVEvent(cal.vevents[i]);
        printf("\n");
    }
}

void printRawVEvent(VEvent ve)
{
    printf("From: %d/%d/%d %d.%d\n", ve.start.date.year, ve.start.date.month, ve.start.date.day, ve.start.time.hour, ve.start.time.minute);
    printf("To: %d/%d/%d %d.%d\n", ve.end.date.year, ve.end.date.month, ve.end.date.day, ve.end.time.hour, ve.end.time.minute);
    assert(ve.summary != NULL);
    printf("Summary: %s\n", ve.summary);
    printf("Location: %s\n", ve.location);
    printf("Description: %s\n", ve.description);
    printf("Priority: %d\n", ve.priority);
}
