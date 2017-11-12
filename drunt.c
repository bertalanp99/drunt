#include "structs.h"
#include "dbHandler.h"
#include "debug.h"
#include "errorHandler.h"
#include "helper.h"
#include "interactiveHandler.h"

#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{ 
    /*char* source = "cal.ics";
    printf("Reading %s...\n", source);
    Calendar cal;
    Calendar_create(&cal);
    
    switch ( ICS_load(source, &cal) )
    {
        case FAIL_FILE_CORRUPT:
            die("ICS_load failed saying file is corrupt\n");
            break;

        case FAIL_FILE_READ:
            die("ICS_load was unable to open the file for reading\n");
            break;

        case FAIL_TIMESTAMP_CORRUPT:
            die("ICS_load found corrupt timestamp in file\n");
            break;

        case FAIL_MALLOC:
            die("Memory allocation failure in ICS_load\n");
            break;

        case FAIL_OVERFLOW:
            die("Overflow occured in ICS_load\n");
            break;

        default:
            break;
    }

    VEvent myvevent = {
        .start = {
            .date = { .year = 2018, .month = 2, .day = 13 },
            .time = { .hour = 0, .minute = 0 } },
        .end = {
            .date = { .year = 2018, .month = 2, .day = 13},
            .time = { .hour = 0, .minute = 0 } },
        .summary = "It's my birthday!",
        .location = "Everywhere",
        .description = "It's not like it counts or anything",
        .priority = 10
    };

    printf("Event validity: %d\n", isValidVEvent(myvevent));

    printf("Adding VEvent to Calendar in RAM...\n");
    
    switch( Calendar_addVEvent(&cal, myvevent) )
    {
        case FAIL_MALLOC:
            die("Calendar_addVEvent memory allocation failure\n");
            break;

        default:
            break;
    }

    char* dest = "newcal.ics";
    printf("Writing updated calendar to %s...\n", dest);

    switch ( ICS_write(dest, &cal) )
    {
        case FAIL_FILE_WRITE:
            die("ICS_write was unable to open file for writing\n");
            break;

        default:
            break;
    }

    printf("Successfully written file %s!\n", dest);

    printf("Destroying Calendar in ram...\n");

    Calendar_destroy(&cal);

    printf("I'll try to print the calendar raw now (UB):\n\n\n");
    printCalendar_raw(&cal);

    printf("Everything done. Bye!\n");

    return 0;
    */

    shell();
}

