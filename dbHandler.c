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

    /* Process file */
    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        return FAIL_FILE_READ;
    }

    char buff[BUFFSIZE];
    int currentLine = 0;
    
    ICSTag tags[] = {
        { .tag = "DTSTART:",        .type = TIMESTAMP,  .flag = 0 },
        { .tag = "DTEND:",          .type = TIMESTAMP,  .flag = 0 },
        { .tag = "SUMMARY:",        .type = STRING,     .flag = 0 },
        { .tag = "LOCATION:",       .type = STRING,     .flag = 0 },
        { .tag = "DESCRIPTION:",    .type = STRING,     .flag = 0 },
        { .tag = "PRIORITY:",       .type = NUMBER,     .flag = 0 }
    };

    size_t numberOfTags = ( sizeof tags / sizeof(ICSTag) );

    while (fgets(buff, sizeof buff, fp) != NULL)
    {
        ++currentLine;
        removeNewLineChar(buff);
        if ( !strcmp(buff, "BEGIN:VEVENT") )
        {
            VEvent ve;
            while ( strcmp(buff, "END:VEVENT") )
            {
                for (int i = 0; i < numberOfTags; ++i)
                {
                    if ( !strncmp(buff, tags[i].tag, strlen(tags[i].tag)) )
                    {
                        switch(tags[i].type)
                        {
                            case TIMESTAMP:
                            {
                                if ( !strcmp(tags[i].tag, "DTSTART:") )
                                {
                                    ICSTimeStampReader(buff + strlen(tags[i].tag), &ve.start);
                                }
                                else if ( !strcmp(tags[i].tag, "DTEND:") )
                                {
                                    ICSTimeStampReader(buff + strlen(tags[i].tag), &ve.end);
                                }
                                else
                                {
                                    ; // TODO no other TIMESTAMP properties implemented yet
                                }
                            }
                            break;

                            case STRING:
                            {
                                if ( !strcmp(tags[i].tag, "SUMMARY:") )
                                {
                                    ve.summary = malloc( sizeof(char) * (strlen(buff) + 1) );
                                    if ( !ve.summary )
                                    {
                                        warning("Failed to allocate memory for an event summary");
                                        return FAIL_MALLOC;
                                    }
                                    strcpy(ve.summary, buff + strlen(tags[i].tag));
                                }
                                else if ( !strcmp(tags[i].tag, "LOCATION:") )
                                {
                                    ve.location = malloc( sizeof(char) * (strlen(buff) + 1) );
                                    if ( !ve.location )
                                    {
                                        warning("Failed to allocate memory for an event location");
                                        return FAIL_MALLOC;
                                    }
                                    strcpy(ve.location, buff + strlen(tags[i].tag));
                                }
                                else if ( !strcmp(tags[i].tag, "DESCRIPTION:") )
                                {
                                    ve.description = malloc( sizeof(char) * (strlen(buff) + 1) );
                                    if ( !ve.description )
                                    {
                                        warning("Failed to allocate memory for an event description");
                                        return FAIL_MALLOC;
                                    }
                                    strcpy(ve.description, buff + strlen(tags[i].tag));
                                }
                                break;
                            }

                            case NUMBER:
                            { // TODO we can only handle priority as number
                                myatoi(buff + strlen(tags[i].tag), &ve.priority);
                                break;
                            }
                        }
                    }
                }
                
                // proceed
                fgets(buff, sizeof buff, fp);
                removeNewLineChar(buff);
                ++currentLine;
            }
            
            Calendar_addVEvent(cal, ve);
        }
    }
    
    return SUCCESS;
}

MYERRNO ICS_write(const char* file, const Calendar* cal, WriteMode wm)
{
    assert(file != NULL && cal != NULL);

    switch (wm)
    {
        case NEW:
            switch ( isNonEmptyFile(file) )
            {
                case FAIL_FILE_READ:
                    break;

                default:
                    warning("File seems to already exist but drunt was asked to create new. Ignoring...\n");
                    return FAIL_FILE_EXISTS;
            }
            break;

        case OVERWRITE:
            break;
    }             

    FILE* fp = fopen(file, "w");
    if (fp == NULL)
    {
        return FAIL_FILE_WRITE;
    }
    
    /* First, print basic things */
    fprintf(fp, "BEGIN:VCALENDAR\n");
    fprintf(fp, "VERSION:%s\n", ICALVERSION);

    /* Traverse Calendar and print VEvents into file */
    VEventNode* traveller = cal->first->next;

    while( traveller != cal->last )
    {
        fprintf(fp, "BEGIN:VEVENT\n");
        fprintf(fp, "DTSTART:%04d%02d%02dT%02d%02d00Z\n",
                traveller->ve.start.date.year, traveller->ve.start.date.month, traveller->ve.start.date.day,
                traveller->ve.start.time.hour, traveller->ve.start.time.minute);
        fprintf(fp, "DTEND:%04d%02d%02dT%02d%02d00Z\n",
                traveller->ve.end.date.year, traveller->ve.end.date.month, traveller->ve.end.date.day,
                traveller->ve.end.time.hour, traveller->ve.end.time.minute);
        fprintf(fp, "SUMMARY:%s\n", traveller->ve.summary);
        fprintf(fp, "LOCATION:%s\n", traveller->ve.location);
        fprintf(fp, "DESCRIPTION:%s\n", traveller->ve.description);
        fprintf(fp, "PRIORITY:%d\n", traveller->ve.priority);
        fprintf(fp, "END:VEVENT\n");

        traveller = traveller->next;
    }

    /* Close VCALENDAR in file */
    fprintf(fp, "END:VCALENDAR\n");

    fclose(fp);

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

MYERRNO Calendar_destroy(Calendar* cal)
{
    assert(cal != NULL);

    /* Traverse Calendar and destroy all VEvents between sentinels */
    VEventNode* traveller = cal->first->next;

    while( traveller != cal->last )
    {
        VEvent_delete(&traveller->ve);
        traveller = traveller->next;
    }

    /* Only sentinels remain, destroy them too */
    free(cal->first);
    free(cal->last);

    return SUCCESS;
}

MYERRNO Calendar_addVEvent(Calendar* cal, VEvent ve)
{
    assert(cal != NULL && isValidVEvent(ve));

    VEventNode* traveller = cal->first;
    /* Traverse Calendar to find last entry */
    while
        (
            traveller->next != cal->last && 
                (
                    compareDateTime(ve.start, traveller->next->ve.start) == AFTER  ||
                    compareDateTime(ve.end, traveller->next->ve.start) == SAME
                )
        )
    {
        traveller = traveller->next;
    }
    
    /* Allocate memory for new entry and set it's pointers */
    VEventNode* tmpven = malloc( sizeof(VEventNode) );
    if ( !tmpven )
    {
        return FAIL_MALLOC;
    }
    tmpven->next = traveller->next;
    tmpven->prev = traveller;
    traveller->next = tmpven;
    
    /* Move on to new entry and fill its data with ve */
    traveller = traveller->next;
    traveller->ve = ve;

    ++cal->numberOfEntries;

    return SUCCESS;
}

// TODO VEvent finder?

MYERRNO Calendar_deleteVEvent(Calendar* cal, VEvent ve)
{
    assert(cal != NULL && isValidVEvent(ve));

    VEventNode* traveller = cal->first->next; // start from first real VEvent
    
    /* Traverse the Calendar to find specified VEvent */
    while (traveller->next != cal->last && !compareVEvent(traveller->ve, ve))
    {
        traveller = traveller->next;
    }
    
    if (traveller == cal->last)
    {
        return FAIL_VEVENT_NOT_FOUND;
    }
    
    VEvent_delete(&traveller->ve);
    --cal->numberOfEntries;
    
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
