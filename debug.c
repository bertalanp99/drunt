#include "debug.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>


void printCalendar_raw(const Calendar* cal)
{
    VEventNode* traveller = cal->first->next;
    while (traveller->next != NULL)
    {
        printVEventNode_raw(traveller);
        printf("\n\n\n");
        traveller = traveller->next;
    }

    printf("Finished printing calendar!\n");
}

void printVEventNode_raw(const VEventNode* ven)
{
    printf("Starting DateTime: %d/%d/%d %d.%d\n",
        ven->ve.start.date.year, ven->ve.start.date.month, ven->ve.start.date.day,
        ven->ve.start.time.hour, ven->ve.start.time.minute);
    printf("Ending DateTime: %d/%d/%d %d.%d\n",
        ven->ve.end.date.year, ven->ve.end.date.month, ven->ve.end.date.day,
        ven->ve.end.time.hour, ven->ve.end.time.minute);
    printf("Summary: %s\n", ven->ve.summary);
    printf("Location: %s\n", ven->ve.location);
    printf("Description: %s\n", ven->ve.description);
    printf("Priority: %d\n", ven->ve.priority);
}
    
