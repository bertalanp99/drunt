#include "structs.h"
#include "dbHandler.h"
#include "debug.h"
#include "errorHandler.h"

#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-l") == 0)
    {
        Calendar* mycalendar = ics_load("./cal.ics");
        printRawCalendar(mycalendar);
        free(mycalendar);
    }
    else if (strcmp(argv[1], "--add") == 0 || strcmp(argv[1], "-a") == 0)
    {
        // TODO event adder
    }
    else if (strcmp(argv[1], "-temp") == 0)
    {
        Calendar* mycalendar = ics_load("./cal.ics");
        Event myevent = {
            .start ={   .date = {   .year = 2017,
                                    .month = 12,
                                    .day = 15
                                }, 
                        .time = {   .hour = 11,
                                    .minute = 0
                                }
                    },
            .end =  {   .date = {   .year = 2017,
                                    .month = 12,
                                    .day = 15
                                }, 
                        .time = {   .hour = 12,
                                    .minute = 0
                                }
                    },
            .name = "Tüntetés a kacsák egyenjogúsága ellen",
            .location= "New York",
            .description = "Sálálálá",
            .priority = 10
        };
        
        mycalendar = entry_add(mycalendar, myevent);
        
        if (mycalendar == NULL)
        {
            die("Fuck! Failed to allocate memory!");
        }

        if(ics_write(mycalendar, "./newCal.ics") == 0)
        {
            die("File already exists");
        }

        free(mycalendar);
    }
    else if (strcmp(argv[1], "-temp2") == 0)
    {
        Calendar* mycalendar = ics_load("./cal.ics");
        if (ics_write(mycalendar, "./newCal.ics") == 0)
        {
            die("File already exists");
        }

        free(mycalendar);
    }

    return 0;
}

