#include "dbHandler.h"
#include "errorHandler.h"
#include "helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINELENGTH 256 // ics allows 75 octets
#define ICALVERSION "2.0"

Calendar ics_load(char* file)
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
    // allocate memory for events and database
    Event* tmp = malloc( count * sizeof(Event) );
    // check whether memeory allocation was successful
    if (tmp == NULL)
    {
        die("Failed to allocate memory for database");
    }
    Calendar cal = {.numberOfEntries = count, .events = tmp};

    int i = -1; // index of current event

    fgets(buff, MAX_LINELENGTH, fp); // no need to check for empty file, already done
    while (strncmp(buff, "END:VCALENDAR", strlen("END:VCALENDAR")) != 0)
    {
        if (strncmp(buff, "BEGIN:VEVENT", strlen("BEGIN:VEVENT")) == 0) // TODO IMPROVE THESE WITH ENUM
        {
            ++i;
        }
        else if (strncmp(buff, "DTSTART", strlen("DTSTART")) == 0 && i != -1) // From date (+time)
        {
            char* miniBuff = icsTagRemover(buff, "DTSTART");
            if (icsTimeStampReader(miniBuff, &cal.events[i].start) == 0)
            {
                die("Corrupt ics file (invalid DTSTART somewhere)");
            }
            free(miniBuff);
        }
        else if (strncmp(buff, "DTEND", strlen("DTEND")) == 0 && i != -1)  // To date (+time)
        {
            char* miniBuff = icsTagRemover(buff, "DTEND");
            if (icsTimeStampReader(miniBuff, &cal.events[i].end) == 0)
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
            strcpy(cal.events[i].name, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "LOCATION", strlen("LOCATION")) == 0 && i != -1) // location of event
        {
            char* miniBuff = icsTagRemover(buff, "LOCATION");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid LOCATION somewhere)");
            }
            strcpy(cal.events[i].location, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "DESCRIPTION", strlen("DESCRIPTION")) == 0 && i != -1) // description of event
        {
            char* miniBuff = icsTagRemover(buff, "DESCRIPTION");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid DESCRIPTION somewhere)");
            }
            strcpy(cal.events[i].description, miniBuff);
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
            cal.events[i].priority = priority;
            free(miniBuff);
        }
     
        fgets(buff, MAX_LINELENGTH, fp); // next line
    }

    fclose(fp);

    return cal;
}
    
int ics_write(Calendar* cal, char* file)
{
    /* First of all, check if file exists by any chance */
    if (access(file, 0) == 0)
    {
        return 0; // caller should check for this
    }
    
    /* Open up the file for writing */
    FILE* fp;
    fp = fopen(file, "w");
    if (fp == NULL)
    {
        return 0;
    }
    
    /* Printing basic iCalendar stuff first */
    fprintf(fp, "BEGIN:VCALENDAR\n"); 
    fprintf(fp, "VERSION:%s\n", ICALVERSION);
    
    /* Printing every event, one by one */
    for (int i = 0; i < cal->numberOfEntries; ++i)
    { // TODO write stuff only if they exist
        fprintf(fp, "BEGIN:VEVENT\n");
        fprintf
            (
                fp, "DTSTART:%04d%02d%02dT%02d%02d00\n",
                cal->events[i].start.date.year,
                cal->events[i].start.date.month,
                cal->events[i].start.date.day,
                cal->events[i].start.time.hour,
                cal->events[i].start.time.minute
            );
        fprintf
            (
                fp, "DTEND:%04d%02d%02dT%02d%02d00\n",
                cal->events[i].end.date.year,
                cal->events[i].end.date.month,
                cal->events[i].end.date.day,
                cal->events[i].end.time.hour,
                cal->events[i].end.time.minute
            );
        fprintf(fp, "SUMMARY:%s\n", cal->events[i].name);
        fprintf(fp, "LOCATION:%s\n", cal->events[i].location);
        fprintf(fp, "DESCRIPTION:%s\n", cal->events[i].description);
        fprintf(fp, "PRIORITY:%d\n", cal->events[i].priority);
        fprintf(fp, "END:VEVENT\n");
    }

    /* Close whole iCalendar file */
    fprintf(fp, "END:VCALENDAR");

    fclose(fp);

    return 1;
}

int entry_add(Calendar* cal, const Event e) // TODO update fnctn info
{
    /* First, make sure Calendar pointer wasn't NULL */
    if (cal == NULL)
    {
       return 0; // caller should check for this
    }

    /* Reallocate memory for events in cal, adding more space for another entry */
    Event* tmp = realloc( cal->events, (sizeof(*(cal->events)) + sizeof(Event)) );
    // only reassign pointer to cal if reallocating has been successful (so as to prevent losing pointer upon unsuccessful reallocation)
    if (tmp != NULL)
    {
        cal->events = tmp;
    }
    else
    {
        return 0; // caller should check for this
    }

    /* Writing input event details into newly allocated extra space */
    cal->events[cal->numberOfEntries].start = e.start;
    cal->events[cal->numberOfEntries].end = e.end;
    
    strcpy(cal->events[cal->numberOfEntries].name, e.name);
    strcpy(cal->events[cal->numberOfEntries].location, e.location);
    strcpy(cal->events[cal->numberOfEntries].description, e.description);

    cal->events[cal->numberOfEntries].priority = e.priority;

    /* Increase number of entries value in structure and return pointer */
    ++cal->numberOfEntries;
    
    return 1;
}
