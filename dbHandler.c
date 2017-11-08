#include "dbHandler.h"
#include "errorHandler.h"
#include "helper.h"
#include "enums.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define BUFFSIZE 1024
#define MAX_LINELENGTH 600 // = 75 octets --> iCalendar allows that much
#define ICALVERSION "2.0"

MYERRNO ics_load(const char* file, Calendar* cal) // loads ICS into RAM
{
    assert(file != NULL && cal != NULL);

    /* Count VEVENTs in file */
    int count = 0;
    switch (ICSVEventCounter(file, &count))
    {
        case FAIL_FILE_READ:
            warning("Failed to open iCalendar file %s for reading", file);
            return FAIL_FILE_EMPTY;

        case FAIL_FILE_EMPTY:
            warning("iCalendar file %s is empty --> ignoring", file);
        
        case FAIL_ICS_NOEND:
            warning("iCalendar file %s has no END:VCALENDAR! This should be fixed ASAP");
            break;

        case SUCCESS:
            break; // everything's in order, keep going

        default:
            warning("Something weird happened in ics_load function, please notify dev");
            return FAIL_UNKNOWN;
    }

    /* Then create and fill database with ics contents */
    FILE* fp;
    fp = fopen(file, "r");
    if (fp == NULL)
    {
        warning("Failed to open iCaelndar file %s for reading", file);
        return FAIL_FILE_READ;
    } // NOTE: no need to check for empty file as event counter has already done that

    // allocate memory for Calendar
    VEvent* tmp = malloc( count * sizeof(VEvent) );
    if (tmp == NULL)
    {
        warning("Failed to allocate memory for events");
        return FAIL_MALLOC;
    }
    
    cal->numberOfEntries = count;
    cal->vevents = tmp;

    char buff[BUFFSIZE];
    int i = -1; // index of current event; initialised to -1 because first event shall be at index 0
    int currentLine = 0;

    ++currentLine;
    fgets(buff, BUFFSIZE, fp);

    while (!hasICSTag(buff, "END:VCALENDAR"))
    {
        if (hasICSTag(buff, "BEGIN:VEVENT"))
        {
            ++i;
        }
        
        if (i != -1) // prevent processing lines outside BEGIN/END:VEVENT tags
        {
            if (hasICSTag(buff, "DTSTART"))
            {
                if (ICSTimeStampReader(buff + strlen("DTSTART:"), &cal->vevents[i].start) != SUCCESS)
                {
                    warning("Corrupt iCalendar file (invalid DTSTART at %s:%d)", file, currentLine);
                    return FAIL_FILE_CORRUPT;
                }
            }
            else if (hasICSTag(buff, "DTEND"))
            {
                if (ICSTimeStampReader(buff + strlen("DTEND:"), &cal->vevents[i].end) != SUCCESS)
                {
                    warning("Corrupt iCalendar file (invalid DTEND at %s:%d)", file, currentLine);
                    return FAIL_FILE_CORRUPT;
                }
            }
            else if (hasICSTag(buff, "SUMMARY"))
            {
                if (strlen(buff) > MAX_LINELENGTH)
                {
                    warning("iCalendar file contains SUMMARY line that exceeds limit (75 octets) at %s:%d\n\t=>Read line nevertheless, but clipped the overflowing characters", file, currentLine);
                }
                
                char* tmp = malloc( (strlen(buff) - strlen("SUMMARY:")) + 1 * sizeof(char) );
                if (tmp == NULL)
                {
                    warning("Could not allocate memory for SUMMARY at %s:%d", file, currentLine);
                    return FAIL_MALLOC;
                }
                
                cal->vevents[i].summary = tmp;
                strcpy( cal->vevents[i].summary, (buff + strlen("SUMMARY:")) );
                removeNewLineChar( cal->vevents[i].summary );
            }
            else if (hasICSTag(buff, "LOCATION"))
            {
                if (strlen(buff) > MAX_LINELENGTH)
                {
                    warning("iCalendar file contains LOCATION line that exceeds limit (75 octets) at %s:%d\n\t=>Read line nevertheless, but clipped the overflowing characters", file, currentLine);
                }
                
                char* tmp = malloc( (strlen(buff) - strlen("LOCATION:")) + 1 * sizeof(char) );
                if (tmp == NULL)
                {
                    warning("Could not allocate memory for LOCATION at %s:%d", file, currentLine);
                    return FAIL_MALLOC;
                }
                
                cal->vevents[i].location = tmp;
                strcpy( cal->vevents[i].location, (buff + strlen("LOCATION:")) );
                removeNewLineChar( cal->vevents[i].location );
            }
            else if (hasICSTag(buff, "DESCRIPTION"))
            {
                if (strlen(buff) > MAX_LINELENGTH)
                {
                    warning("iCalendar file contains DESCRIPTION line that exceeds limit (75 octets) at %s:%d\n\t=>Read line nevertheless, but clipped the overflowing characters", file, currentLine);
                }
                
                char* tmp = malloc( (strlen(buff) - strlen("DESCRIPTION:")) + 1 * sizeof(char) );
                if (tmp == NULL)
                {
                    warning("Could not allocate memory for DESCRIPTION at %s:%d", file, currentLine);
                    return FAIL_MALLOC;
                }
                
                cal->vevents[i].description = tmp;
                strcpy( cal->vevents[i].description, (buff + strlen("DESCRIPTION:")) );
                removeNewLineChar( cal->vevents[i].description );
            }
            else if (hasICSTag(buff, "PRIORITY"))
            {
                int priority;
                if (myatoi( (buff + strlen("PRIORITY:")) , &priority) == 0)
                {
                    warning("Corrupt ics file (invalid PRIORITY at %s:%d)", file, currentLine);
                }
                
                cal->vevents[i].priority = priority;
            }
        }
        
        // prevent infinite loop by also checking for EOF
        if (feof(fp))
        {
            warning("%s contains no %s! Last line read was\n\t%s", file, "END:VCALENDAR", buff);
            break;
        }
        
        // advance
        ++currentLine;
        fgets(buff, BUFFSIZE, fp);
    }

    fclose(fp);

    return SUCCESS;
}
    
MYERRNO ics_write(const Calendar* cal, char* file)
{
    assert(cal != NULL && file != NULL);
    
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
        return FAIL_FILE_WRITE;
    }
    
    /* Printing basic iCalendar stuff first */
    fprintf(fp, "BEGIN:VCALENDAR\n"); 
    fprintf(fp, "VERSION:%s\n", ICALVERSION);
    
    /* Printing every event, one by one */
    for (int i = 0; i < cal->numberOfEntries; ++i)
    {
        fprintf(fp, "BEGIN:VEVENT\n");
        fprintf
            (
                fp, "DTSTART:%04d%02d%02dT%02d%02d00\n",
                cal->vevents[i].start.date.year,
                cal->vevents[i].start.date.month,
                cal->vevents[i].start.date.day,
                cal->vevents[i].start.time.hour,
                cal->vevents[i].start.time.minute
            );
        fprintf
            (
                fp, "DTEND:%04d%02d%02dT%02d%02d00\n",
                cal->vevents[i].end.date.year,
                cal->vevents[i].end.date.month,
                cal->vevents[i].end.date.day,
                cal->vevents[i].end.time.hour,
                cal->vevents[i].end.time.minute
            );
        fprintf(fp, "SUMMARY:%s\n", cal->vevents[i].summary);
        fprintf(fp, "LOCATION:%s\n", cal->vevents[i].location);
        fprintf(fp, "DESCRIPTION:%s\n", cal->vevents[i].description);
        fprintf(fp, "PRIORITY:%d\n", cal->vevents[i].priority);
        fprintf(fp, "END:VEVENT\n");
    }

    /* Close whole iCalendar file */
    fprintf(fp, "END:VCALENDAR");

    fclose(fp);

    return SUCCESS;
}

MYERRNO VEvent_add(Calendar* cal, const VEvent ve)
{
    assert(cal != NULL);

    /* Reallocate memory for events array in cal, adding more space for another entry */
    VEvent* tmp_vevent = realloc( cal->vevents, (sizeof(VEvent) * cal->numberOfEntries) + sizeof(VEvent) );
    if (tmp_vevent == NULL)
    {
        return FAIL_REALLOC;
    }

    cal->vevents = tmp_vevent;

    /* Writing input event details into newly allocated extra space */

    // DTSTART & DTEND
    cal->vevents[cal->numberOfEntries].start = ve.start;
    cal->vevents[cal->numberOfEntries].end = ve.end;
    
    // SUMMARY
    char* tmp = malloc( (strlen(ve.summary) + 1) * sizeof(char) );
    if (tmp == NULL)
    {
        return FAIL_MALLOC;
    }

    cal->vevents[cal->numberOfEntries].summary = tmp;
    strcpy(cal->vevents[cal->numberOfEntries].summary, ve.summary);
    
    // LOCATION
    tmp = malloc( (strlen(ve.location) + 1) * sizeof(char) );
    if (tmp == NULL)
    {
        return FAIL_MALLOC;
    }

    cal->vevents[cal->numberOfEntries].location = tmp;
    strcpy(cal->vevents[cal->numberOfEntries].location, ve.location);

    // DESCRIPTION
    tmp = malloc( (strlen(ve.description) + 1) * sizeof(char) );
    if (tmp == NULL)
    {
        return FAIL_MALLOC;
    }

    cal->vevents[cal->numberOfEntries].description = tmp;
    strcpy(cal->vevents[cal->numberOfEntries].description, ve.description);

    // PRIORITY
    cal->vevents[cal->numberOfEntries].priority = ve.priority;

    /* Increase number of entries value in structure and return pointer */
    ++cal->numberOfEntries;
    
    return SUCCESS;
}
