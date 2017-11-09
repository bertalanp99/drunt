#include "structs.h"
#include "dbHandler.h"
#include "debug.h"
#include "errorHandler.h"
#include "helper.h"

#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE* fp = fopen("cal.ics", "r");
    if (fp == NULL)
    {
        printf("Fuck! Could not open %s\n", "cal.ics");
    }
    else
    {
        printf("Hmm.. I seem to be able to open %s\n", "cal.ics");
    }
    fclose(fp);
    
    printf("Reading ICS...\n");
    Calendar cal;
    Calendar_create(&cal);
    ICS_load("neptun.ics", &cal);
    
    printf("Printing calendar raw below (PLEASE!!!)\n\n\n"); 

    printCalendar_raw(&cal);

    return 0;
}

