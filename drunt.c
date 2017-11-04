#include "structs.h"
#include "dbHandler.h"
#include "debug.h"

#include <stdlib.h>

int main(int argc, char **argv)
{
    Calendar* mycalendar;
    ics_load("./cal.ics", mycalendar);

    printRawCalendar(*mycalendar);
    
    free(mycalendar);

    return 0;
}

