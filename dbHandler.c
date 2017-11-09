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

MYERRNO ICS_load(const char* file, Calendar* cal)
{
    assert(file != NULL && cal != NULL);

    if (!isValidICS(file))
    {
        return FAIL_FILE_CORRUPT;
    }

    cal->numberOfEntries = 0;
    switch (ICSVEventCounter(file, &cal->numberOfEntries))
    {
        case FAIL_FILE_CORRUPT:
            return FAIL_FILE_CORRUPT;
        
        case FAIL_FILE_READ:
            return FAIL_FILE_READ;

        default:
            break;
    }

    /* Process file */
    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        return FAIL_FILE_READ;
    }

    char buff[BUFFSIZE];
    int currentLine = 0;

    while (fgets(buff, sizeof buff, fp) != NULL)
    {
        ++currentLine;
        if ( !strcmp(buff, "BEGIN:VEVENT") )
        {
            VEvent ve;
            while ( strcmp(buff, "END:VEVENT") )
            {
                if ( !strncmp(buff, "DTSTART:", strlen("DTSTART:")) )
                {
                    if ( ICSTimeStampReader(buff + strlen("DTSTART"), &ve.start) != SUCCESS )
                    {
                        warning("Corrupt timestamp at %s:%d\n=>\t%s", file, currentLine, buff);
                        return FAIL_TIMESTAMP_CORRUPT;
                    }
                }
                else if ( !strncmp(buff, "DTEND:", strlen("DTEND:")) )
                {
                    if ( ICSTimeStampReader(buff + strlen("DTEND:"), &ve.end) != SUCCESS )
                    {
                        warning("Corrupt timestamp at %s:%d\n=>\t%s", file, currentLine, buff);
                        return FAIL_TIMESTAMP_CORRUPT;
                    }
                }
                else if ( !strncmp(buff, "SUMMARY:", strlen("SUMMARY:")) )
                {
                    char* tmp = malloc( sizeof(char) * strlen(buff + strlen("SUMMARY:")) );
                    if (tmp == NULL)
                    {
                        warning("Failed to allocate memory for SUMMARY at %s:%d", file, currentLine);
                        return FAIL_MALLOC;
                    }
                    
                    if (strlen(tmp) > MAX_LINELENGTH)
                    {
                        warning("SUMMARY too long at %s:%d", file, currentLine);
                        return FAIL_OVERFLOW;
                    }

                    ve.summary = tmp;
                }
                else if ( !strncmp(buff, "LOCATION:", strlen("LOCATION:")) )
                {
                    char* tmp = malloc( sizeof(char) * strlen(buff + strlen("LOCATION:")) );
                    if (tmp == NULL)
                    {
                        warning("Failed to allocate memory for LOCATION at %s:%d", file, currentLine);
                        return FAIL_MALLOC;
                    }
                    
                    if (strlen(tmp) > MAX_LINELENGTH)
                    {
                        warning("LOCATION too long at %s:%d", file, currentLine);
                        return FAIL_OVERFLOW;
                    }

                    ve.location = tmp;
                }
                else if ( !strncmp(buff, "DESCRIPTION:", strlen("DESCRIPTION:")) )
                {
                    char* tmp = malloc( sizeof(char) * strlen(buff + strlen("DESCRIPTION:")) );
                    if (tmp == NULL)
                    {
                        warning("Failed to allocate memory for DESCRIPTION at %s:%d", file, currentLine);
                        return FAIL_MALLOC;
                    }
                    
                    if (strlen(tmp) > 600)
                    {
                        warning("DESCRIPTION too long at %s:%d", file, currentLine);
                        return FAIL_OVERFLOW;
                    }

                    ve.description = tmp;
                }
                else if ( !strncmp(buff, "PRIORITY:", strlen("PRIORITY:")) )
                {
                    myatoi(buff + strlen("PRIORITY:"), &ve.priority);
                }
                ++currentLine;
            }
            if ( isValidVEvent(ve) )
            {
                Calendar_addVEvent(cal, ve);
            }
        }
    }           
    
    return SUCCESS;
}

MYERRNO Calendar_create(Calendar* cal)
{
    assert(cal != NULL);
    
    // set number of entries to 0 since new Calendar contains no entries
    cal->numberOfEntries = 0;
    
    VEventNode* tmpven;
    /* Allocate memory for sentinels and also set cal's pointers to them */
    tmpven = malloc( sizeof(VEventNode) );
    if (tmpven == NULL)
    {
        return FAIL_MALLOC;
    }
    cal->first = tmpven;
    
    tmpven = malloc( sizeof(VEventNode) );
    if (tmpven == NULL)
    {
        return FAIL_MALLOC;
    }
    cal->last = tmpven;
    
    /* Set pointers of sentinels to correct values (NULL at endpoints) */
    cal->first->prev = NULL;
    cal->first->next = cal->last;
    
    cal->last->prev = cal->first;
    cal->last->next = NULL;

    return SUCCESS;
}

MYERRNO Calendar_addVEvent(Calendar* cal, VEvent ve)
{
    assert(cal != NULL && isValidVEvent(ve));

    // increase number of entries
    ++cal->numberOfEntries;

    VEventNode* traveller = cal->first;
    /* Traverse Calendar to find last entry */
    while
        (
            traveller->next != cal->last && 
                (
                    compareDateTime(ve.start, traveller->next->ve.start) == BEFORE  ||
                    compareDateTime(ve.end, traveller->next->ve.start) == SAME
                )
        )
    {
        traveller = traveller->next;
    }
    
    /* Allocate memory for new entry and set it's pointers */
    VEventNode* tmpven = malloc( sizeof(VEventNode) );
    if (tmpven == NULL)
    {
        return FAIL_MALLOC;
    }
    tmpven->next = traveller->next;
    tmpven->prev = traveller;
    traveller->next = tmpven;
    
    /* Move on to new entry and fill its data with ve */
    traveller = traveller->next;
    traveller->ve = ve;

    return SUCCESS;
}

// TODO VEvent finder?

MYERRNO Calendar_deleteVEvent(Calendar* cal, VEvent ve)
{
    assert(cal != NULL && isValidVEvent(ve));

    VEventNode* traveller = cal->first->next; // start from first real VEvent
    /* Traverse the Calendar to find specified VEvent */
    while (traveller->next != cal->last && compareVEvent(traveller->ve, ve))
    {
        traveller = traveller->next;
    }
    
    if (traveller == cal->last)
    {
        return FAIL_VEVENT_NOT_FOUND;
    }
    
    VEvent_delete(&traveller->ve);
    
    // set pointers (tear out link at deletion point)
    traveller->prev->next = traveller->next;
    traveller->next->prev = traveller->prev;

    free(traveller);

    return SUCCESS;
}

MYERRNO VEvent_delete(VEvent* ve)
{
    assert(ve != NULL);

    free(ve->summary);
    free(ve->location);
    free(ve->description);

    return SUCCESS;
}
