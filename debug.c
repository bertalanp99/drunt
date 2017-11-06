#include "debug.h"

#include <stdio.h>

void printRawCalendar(Calendar* cal)
{
    for (int i = 0; i < cal->numberOfEntries; ++i)
    {
        printRawEvent(cal->events[i]);
        printf("\n");
    }
}

void printRawEvent(Event e)
{
    printf("From: %d/%d/%d %d.%d\n", e.start.date.year, e.start.date.month, e.start.date.day, e.start.time.hour, e.start.time.minute);
    printf("To: %d/%d/%d %d.%d\n", e.end.date.year, e.end.date.month, e.end.date.day, e.end.time.hour, e.end.time.minute);
    printf("Name: %s\n", e.name);
    printf("Location: %s\n", e.location);
    printf("Description: %s\n", e.description);
    printf("Priority: %d\n", e.priority);
}
