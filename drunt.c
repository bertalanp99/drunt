#include "structs.h"
#include "dbHandler.h"
#include "debug.h"
#include "errorHandler.h"
#include "helper.h"

#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{ 
    printf("Reading ICS...\n");
    Calendar cal;
    Calendar_create(&cal);
    ICS_load("neptun.ics", &cal);
    
    printf("Printing calendar raw below\n\n\n"); 

    printCalendar_raw(&cal);

    return 0;
}

