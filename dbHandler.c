#include "dbHandler.h"
#include "errorHandler.h"
#include "helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINELENGTH 256

// this function loads the default ics file into the array of events (in the RAM)
Calendar* ics_load(char* file, Calendar* cal)
{
    FILE* fp;
    fp = fopen(file, "r");

    // check whether file even exists
    if (fp == NULL)
    {
        die("ics file does not exist");
    }

    char buff[MAX_LINELENGTH];

    /* First, count how many events there are */
    int count = 0;

    // check whether file is empty
    if (fgets(buff, MAX_LINELENGTH, fp) == NULL)
    {
        die("ics file is empty");
    }

    // TODO implement other sorts of iCalendar entries (e.g. VTODO)
    // keep reading lines until END:VCALENDAR is encountered
    while (strncmp(buff, "END:VCALENDAR", strlen("END:VCALENDAR")) != 0)
    {
        if (strncmp(buff, "BEGIN:VEVENT", strlen("BEGIN:VEVENT")) == 0)
        {
            ++count;
        }
        fgets(buff, MAX_LINELENGTH, fp);
    }

    fclose(fp);

    // ---

    fp = fopen(file, "r"); // no need to check for NULL pointer, already done

    /* Then create and fill database with ics contents */
    // allocate memory for database
    cal = malloc( count * sizeof(Event) + count * sizeof(int));

    // check whether memeory allocation was successful
    if (cal == NULL)
    {
        die("Failed to allocate memory for database");
    }
    
    // add number of entries to database
    cal->numberOfEntries = count;

    int i = -1; // index of current event

    fgets(buff, MAX_LINELENGTH, fp); // no need to check for empty file, already done
    while (strncmp(buff, "END:VCALENDAR", strlen("END:VCALENDAR")) != 0)
    {
        if (strncmp(buff, "BEGIN:VEVENT", strlen("BEGIN:VEVENT")) == 0)
        {
            ++i;
        }
        else if (strncmp(buff, "DTSTART", strlen("DTSTART")) == 0 && i != -1) // From date (+time)
        {
            char* miniBuff = icsTagRemover(buff, "DTSTART");
            if (icsTimeStampReader(miniBuff, &cal->events[i].start) == 0)
            {
                die("Corrupt ics file (invalid DTSTART somewhere)");
            }
            free(miniBuff);
        }
        else if (strncmp(buff, "DTEND", strlen("DTEND")) == 0 && i != -1)  // To date (+time)
        {
            char* miniBuff = icsTagRemover(buff, "DTEND");
            if (icsTimeStampReader(miniBuff, &cal->events[i].start) == 0)
            {
                die("Corrupt ics file (invalid DTEND somewhere)");
            }
            free(miniBuff);
        }
        else if (strncmp(buff, "SUMMARY", strlen("SUMMARY")) == 0 && i != -1) // 'name' of event
        {
            char* miniBuff = icsTagRemover(buff, "SUMMARY");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid SUMMARY somewhere)");
            }
            strcpy(cal->events[i].name, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "LOCATION", strlen("LOCATION")) == 0 && i != -1) // location of event
        {
            char* miniBuff = icsTagRemover(buff, "SUMMARY");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid LOCATION somewhere)");
            }
            strcpy(cal->events[i].location, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "DESCRIPTION", strlen("DESCRIPTION")) == 0 && i != -1) // description of event
        {
            char* miniBuff = icsTagRemover(buff, "DESCRIPTION");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid DESCRIPTION somewhere)");
            }
            strcpy(cal->events[i].description, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "PRIORITY", strlen("PRIORITY")) == 0 && i != -1) // priority of event
        {
            char* miniBuff = icsTagRemover(buff, "PRIORITY");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid PRIORITY somewhere (empty))");
            }
            int priority;
            if (myatoi(miniBuff, &priority) == 0)
            {
                die("Corrupt ics file (invalid PRIORITY somewhere (unable to interpret))");
            }
            cal->events[i].priority = priority;
            free(miniBuff);
        }
    }

    fclose(fp);

    return cal;
}
    

