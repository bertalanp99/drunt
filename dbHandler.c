#include "dbHandler.h"
#include "errorHandler.h"
#include "helper.h"
#include "enums.h"

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
    if (fp == NULL)
    {
        die("Unable to open file for reading: %s", file);
    }

    char buff[MAX_LINELENGTH];

    /* First, count how many events there are */
    int count = 0;

    // check whether file is empty
    if (fgets(buff, MAX_LINELENGTH, fp) == NULL)
    {
        die("ics file (%s) is empty", file);
    }

    // TODO implement other sorts of iCalendar entries (e.g. VTODO)
    // keep reading lines until END:VCALENDAR is encountered
    while (strncmp(buff, "END:VCALENDAR", strlen("END:VCALENDAR")) != 0)
    {
        if (strncmp(buff, "BEGIN:VEVENT", strlen("BEGIN:VEVENT")) == 0)
        {
            ++count;
        }
        
        // prevent infinite loop by also checking for EOF
        if (feof(fp))
        {
            warning("%s contains no %s! Last line read was\n\t%s", file, "END:VCALENDAR", buff);
            break;
        }
        
        fgets(buff, MAX_LINELENGTH, fp);
    }

    fclose(fp);

    // ---
    
    fp = fopen(file, "r");
    if (fp == NULL)
    {
        die("Unable to open file for reading: %s", file);
    }

    /* Then create and fill database with ics contents */
    Event* tmp = malloc( count * sizeof(Event) );
    if (tmp == NULL)
    {
        die("Failed to allocate memory for database");
    }
    
    // initialise calendar
    Calendar cal = {.numberOfEntries = count, .events = tmp};

    int i = -1; // index of current event; initialised to -1 because first event shall be at index 0
    int currentLine = 0; // store number of currently read line so that it can be referenced in case of errors

    ++currentLine;
    fgets(buff, MAX_LINELENGTH, fp); // no need to check for empty file, already done
    while (strncmp(buff, "END:VCALENDAR", strlen("END:VCALENDAR")) != 0)
    {
        if (strncmp(buff, "BEGIN:VEVENT", strlen("BEGIN:VEVENT")) == 0)
        {
            ++i;
        }
        else if (strncmp(buff, "DTSTART", strlen("DTSTART")) == 0 && i != -1) // 'from' date (+time)
        {
            char* miniBuff = icsTagRemover(buff, "DTSTART");
            if (icsTimeStampReader(miniBuff, &cal.events[i].start) == 0)
            {
                die("Corrupt ics file (invalid %s at %s:%d)", "DTSTART", file, currentLine);
            }
            free(miniBuff);
        }
        else if (strncmp(buff, "DTEND", strlen("DTEND")) == 0 && i != -1)  // 'to' date (+time)
        {
            char* miniBuff = icsTagRemover(buff, "DTEND");
            if (icsTimeStampReader(miniBuff, &cal.events[i].end) == 0)
            {
                die("Corrupt ics file (invalid %s at %s:%d)", "DTEND", file, currentLine);
            }
            free(miniBuff);
        }
        else if (strncmp(buff, "SUMMARY", strlen("SUMMARY")) == 0 && i != -1) // 'name' of event
        {
            char* miniBuff = icsTagRemover(buff, "SUMMARY");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid %s at %s:%d)", "SUMMARY", file, currentLine);
            }
            strcpy(cal.events[i].name, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "LOCATION", strlen("LOCATION")) == 0 && i != -1) // location of event
        {
            char* miniBuff = icsTagRemover(buff, "LOCATION");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid %s at %s:%d)", "LOCATION", file, currentLine);
            }
            strcpy(cal.events[i].location, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "DESCRIPTION", strlen("DESCRIPTION")) == 0 && i != -1) // description of event
        {
            char* miniBuff = icsTagRemover(buff, "DESCRIPTION");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid %s at %s:%d)", "DESCRIPTION", file, currentLine);
            }
            strcpy(cal.events[i].description, miniBuff);
            free(miniBuff);
        }
        else if (strncmp(buff, "PRIORITY", strlen("PRIORITY")) == 0 && i != -1) // priority of event
        {
            char* miniBuff = icsTagRemover(buff, "PRIORITY");
            if (miniBuff == NULL)
            {
                die("Corrupt ics file (invalid %s at %s:%d)", "PRIORITY", file, currentLine);
            }
            int priority;
            if (myatoi(miniBuff, &priority) == 0)
            {
                die("Corrupt ics file (invalid %s at %s:%d)", "PRIORITY", file, currentLine);
            }
            cal.events[i].priority = priority;
            free(miniBuff);
        }
        
        // prevent infinite loop by also checking for EOF
        if (feof(fp))
        {
            warning("%s contains no %s! Last line read was\n\t%s", file, "END:VCALENDAR", buff);
            break;
        }
        
        // advance
        ++currentLine;
        fgets(buff, MAX_LINELENGTH, fp);
    }

    fclose(fp);

    return cal;
}
    
MYERRNO ics_write(Calendar* cal, char* file)
{
    /* First of all, check if file exists --- if it does, ask whether to overwrite */
    if (access(file, 0) == 0)
    {
        if (promptYN("File %s already exists. Overwrite?", file) == 0)
        {
            return FAIL_PROMPT;
        }
        else
        {
            if (remove(file) != 0)
            {
                return FAIL_REMOVE;
            }
        }
    }
    
    /* Open up the file for writing */
    FILE* fp;
    fp = fopen(file, "w");
    if (fp == NULL)
    {
        return FAIL_FOPEN;
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

    return SUCCESS;
}

MYERRNO entry_add(Calendar* cal, const Event e)
{
    /* First, make sure Calendar pointer wasn't NULL */
    if (cal == NULL)
    {
       return FAIL_NULLPASSED;
    }

    /* Reallocate memory for events array in cal, adding more space for another entry */
    Event* tmp = realloc( cal->events, (sizeof(Event) * cal->numberOfEntries) + sizeof(Event) );
    if (tmp != NULL)
    {
        cal->events = tmp;
    }
    else
    {
        return FAIL_REALLOC;
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
    
    return SUCCESS;
}

// TODO credit tutorial on http://www.zentut.com/c-tutorial/c-linked-list/

EventNode* EventNode_create(Event data, EventNode* next) // add node to linked list
{
    EventNode* new = malloc( sizeof(EventNode) );
    if (new == NULL)
    {
        die("Failed to allocate memory");
    }

    new->data = data;
    new->next = next;

    return new;
}

EventNode* EventNode_prepend(EventNode* head, Event data)
{
    EventNode* new = EventNode_create(data, head);
    head = new;
    return head;
}

typedef void (*callback)(EventNode* data);
void traverse(EventNode* head, callback f)
{
    EventNode* cursor = head;
    while (cursor != NULL)
    {
        f(cursor);
        cursor = cursor->next;
    }
}

// TODO CONTINUE FROM HERE

