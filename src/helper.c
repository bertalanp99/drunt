#include "helper.h"
#include "errorHandler.h"
#include "enums.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#define YEAR_MIN 1950
#define YEAR_MAX 2050

#define BUFFSIZE 1024
#define MAX_LINELENGTH 600 // 75 octets

int isLeapYear(const int year)
{
    if (year < YEAR_MIN || year > YEAR_MAX)
    {
        return -1;
    }
    
    return ( ( (year % 4 == 0) && (year % 100 != 0) ) || (year % 400 == 0) );
}

MYERRNO ICSVEventCounter(const char* file, int* n)
{
    assert(file != NULL && n != NULL);

    if (!isValidICS(file))
    {
        return FAIL_FILE_CORRUPT;
    }

    /* Reset counter to zero and attempt to open file for reading */
    *n = 0;

    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        return FAIL_FILE_READ;
    }

    /* Counting only BEGIN:VEVENT lines is safe because ICS validity has already been checked */
    char buff[BUFFSIZE];
    while(fgets(buff, sizeof buff, fp) != NULL)
    {
        if ( !strcmp(buff, "BEGIN:VEVENT\n") || !strcmp(buff, "BEGIN:VEVENT\r\n") )
        {
            ++*n;
        }
    }

    fclose(fp);

    return SUCCESS;
}

int isValidICS(const char* file)
{
    assert(file != NULL);
    
    if (isNonEmptyFile(file) != SUCCESS)
    {
        return 0; // empty file
    }

    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        return 0; // unable to read
    }

    /* Go through the file line by line; if a line starting with BEGIN is found, look
     * for a closing END at heads of lines. If no END is found or another BEGIN of same
     * type is found, the ICS file is corrupt */

    char buff[BUFFSIZE];
    int currentLine = -1;
 
    ICSTag tags[] = {
        { .tag = "DTSTART:",        .type = TIMESTAMP,  .flag = 0 },
        { .tag = "DTEND:",          .type = TIMESTAMP,  .flag = 0 },
        { .tag = "SUMMARY:",        .type = STRING,     .flag = 0 },
        { .tag = "LOCATION:",       .type = STRING,     .flag = 0 },
        { .tag = "DESCRIPTION:",    .type = STRING,     .flag = 0 },
        { .tag = "PRIORITY:",       .type = NUMBER,     .flag = 0 }
    };

    int endCalendarFlag = 0;
    int beginVEventFlag = 0;

    // check first line first
    fgets(buff, sizeof buff, fp);
    ++currentLine;
    removeNewLineChar(buff);
    if ( strcmp(buff, "BEGIN:VCALENDAR") )
    {
        return 0; // first line is not BEGIN:VCALENDAR
    }

    // check other lines
    while (fgets(buff, sizeof buff, fp) != NULL)
    {
        ++currentLine;
        removeNewLineChar(buff);
        if (endCalendarFlag)
        {
            warning("Content found after END:VCALENDAR at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
            return 0;
        }
        else if ( !strcmp(buff, "BEGIN:VCALENDAR") )
        {
            warning("Nested VCALENDAR at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
            return 0;
        }
        else if ( !strcmp(buff, "END:VCALENDAR") )
        {
            if (beginVEventFlag)
            {
                warning("BEGIN:VCALENDAR closed by END:VCALENDAR at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                return 0;
            }
            else
            {
                endCalendarFlag = 1;
            }
        }
        else if ( !strcmp(buff, "BEGIN:VEVENT") )
        {   
            if (beginVEventFlag)
            {
                warning("Nested VEVENT at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                return 0;
            }
            else
            {
                beginVEventFlag = 1;
            }
        }
        else if ( !strcmp(buff, "END:VEVENT") )
        {
            if (!beginVEventFlag)
            {
                warning("END:VEVENT found without preceding BEGIN:VEVENT at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                return 0;
            }
            else
            {
                int hasAllRequiredTags = 1;
                for (int i = 0; i < (int) ( sizeof tags / sizeof(ICSTag) ); ++i)
                {
                    if ( !tags[i].flag )
                    {
                        hasAllRequiredTags = 0;
                        break;
                    }
                }
                
                if (hasAllRequiredTags)
                {
                    beginVEventFlag = 0;
                    for (int i = 0; i < (int) ( sizeof tags / sizeof(ICSTag) ); ++i)
                    {
                        tags[i].flag = 0;
                    }
                }
                else
                {
                    warning("END:VEVENT found without required VEVENT detauls preceding at %d\n[ x> ]\t%s", file, currentLine, buff);
                    return 0;
                }
            }
        }
        else
        {
            for (int i = 0; i < (int) ( sizeof tags / sizeof(ICSTag) ); ++i)
            {
                if ( !strncmp(buff, tags[i].tag, strlen(tags[i].tag)) )
                {
                    if ( !beginVEventFlag )
                    {
                        warning("Found '%s' outside VEVENT block at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                        return 0;
                    }
                    else
                    {
                        switch (tags[i].type)
                        {
                            case TIMESTAMP:
                            {
                                if ( !isValidICSTimeStamp(buff + strlen(tags[i].tag)) )
                                {
                                    warning("Found invalid timestamp at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                                    return 0;
                                }
                                else
                                {
                                    tags[i].flag = 1;
                                }
                                break;
                            }

                            case STRING:
                            {
                                if ( strlen(buff) > MAX_LINELENGTH )
                                {
                                    warning("Summary exceeds iCalendar limit (75 octets) at %s:%d", file, currentLine);
                                    return 0;
                                }
                                else
                                {
                                    tags[i].flag = 1;
                                }
                                break;
                            }

                            case NUMBER: // TODO drunt only processes priority as NUMER
                            {
                                int n;
                                if ( !myatoi(buff + strlen(tags[i].tag), &n) )
                                {
                                    warning("Failed to convert priority to integer at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                                    return 0;
                                }
                                else
                                {
                                    if ( !isValidPriority(n) )
                                    {
                                        warning("Invalid priority at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                                        return 0;
                                    }
                                    else
                                    {
                                        tags[i].flag = 1;
                                    }
                                }
                                break;
                            }
                        }       
                    }
                }
            }
        }    
    }

    if ( !endCalendarFlag )
    {
        warning("No END:VCALENDAR found in '%s'! Last line read:\n[ x> ]\t%s", file, buff);
        return 0;
    }
    else
    {
        return 1;
    }

    fclose(fp);
}

int isValidPriority(int p)
{
    return (p >= 0 && p < 10);
}

int isValidICSTimeStamp(const char* timestamp)
{
    assert(timestamp != NULL && strcmp(timestamp, ""));
   
    // temporarily myatoi() everything and also check their validity
    char buff[8]; // 4+1 would be enough though
    int tmp;

    return
        (
            myatoi(mystrncpy(buff, timestamp, 4), &tmp)       && isValidYear(tmp)     &&
            myatoi(mystrncpy(buff, timestamp + 4, 2), &tmp)   && isValidMonth(tmp)    &&
            myatoi(mystrncpy(buff, timestamp + 6, 2), &tmp)   && isValidDay(tmp)      &&
            myatoi(mystrncpy(buff, timestamp + 9, 2), &tmp)   && isValidHour(tmp)     &&
            myatoi(mystrncpy(buff, timestamp + 11, 2), &tmp)  && isValidMinute(tmp)   &&
            // TODO deal with seconds?
            timestamp[15] == 'Z' // TODO only ZULU timestamps supported as of yet
        );
}

int hasNewLineChar(const char* str)
{
    return ( str[strlen(str) - 1] == '\n' || strcmp(str + strlen(str) - 2, "\r\n") );
}

char* mystrncpy(char* dest, const char* src, const int len)
{
    strncpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

MYERRNO isNonEmptyFile(const char* file)
{
    assert(file != NULL);

    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        perror(file);
        return FAIL_FILE_READ;
    }
    
    /* Go to end of file and check ftell() */
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) != 0)
    {
        return SUCCESS;
    }
    else
    {
        return FAIL_FILE_EMPTY;
    }

    fclose(fp);
}

MYERRNO ICSTimeStampReader(const char* ts, DateTime* dt)
{
    /* Format of iCalendar timestamp: YYYYMMDDTHHMMSS
     * ex gr 20171112T090000 = 2017/11/12 09.00.00
     * (we are not going to use seconds for anything) */

    assert( ts != NULL && dt != NULL);

    if ( !isValidICSTimeStamp(ts) )
    {
        return FAIL_TIMESTAMP_CORRUPT;
    }

    /* No need to check for anything, isValidICSTimeStamp() has already done that */
    char buff[8]; // 4+1 would be sufficient though

    myatoi(mystrncpy(buff, ts, 4), &dt->date.year);
    myatoi(mystrncpy(buff, ts + 4, 2), &dt->date.month);
    myatoi(mystrncpy(buff, ts + 6, 2), &dt->date.day);
    myatoi(mystrncpy(buff, ts + 9, 2), &dt->time.hour);
    myatoi(mystrncpy(buff, ts + 11, 2), &dt->time.minute);

    return SUCCESS;
}

int isValidYear(const int year)
{
    return (year >= 1950 && year <= 2050);
}

int isValidMonth(const int month)
{
    return (month >= 1 && month <= 12);
}

int isValidDay(const int day)
{
    return (day >=1 && day <= 31);
}

int isValidHour(const int hour)
{
    return (hour >= 0 && hour < 24);
}

int isValidMinute(const int minute)
{
    return (minute >= 0 && minute < 60);
}

int isValidDate(const Date d)
{
    return ( isValidYear(d.year) && isValidMonth(d.month) && isValidDay(d.day) );
}

int isValidTime(const Time t)
{
    return ( isValidHour(t.hour) && isValidMinute(t.minute) );
}

int isValidDateTime(const DateTime dt)
{
    return ( isValidDate(dt.date) && isValidTime(dt.time) );
}

int isValidVEvent(const VEvent ve)
{
    return
        ( 
            isValidDateTime(ve.start)   &&
            isValidDateTime(ve.end)     &&
            ve.summary != NULL          &&
            ve.location != NULL         &&
            ve.description != NULL
        );
}

RELATIVEDATE compareDateTime(const DateTime dt1, const DateTime dt2)
{
    if ( !isValidDateTime(dt1) || !isValidDateTime(dt2) )
    {
        return RELATIVEDATE_ERROR;
    }
    
    if (compareDate(dt1.date, dt2.date) == BEFORE)
    {
        return BEFORE;
    }
    else if (compareDate(dt1.date, dt2.date) == AFTER)
    {
        return AFTER;
    }
    else // dt1 & dt2 are on the same day, check time
    {
        if (compareTime(dt1.time, dt2.time) == BEFORE)
        {
            return BEFORE;
        }
        else if (compareTime(dt1.time, dt2.time) == AFTER)
        {
            return AFTER;
        }
        else // both time and date match
        {
            return SAME;
        }
    }
}

RELATIVEDATE compareDate(const Date d1, const Date d2)
{
   if (!isValidDate(d1) || !isValidDate(d2))
   {
       return RELATIVEDATE_ERROR;
   }

   if (d1.year < d2.year)
   {
       return BEFORE;
   }
   else if (d1.year > d2.year)
   {
       return AFTER;
   }
   else // same year, check month
   {
       if (d1.month < d2.month)
       {
            return BEFORE;
       }
       else if (d1.month > d2.month)
       {
           return AFTER;
       }
       else // same month too, check day
       {
           if (d1.day < d2.day)
           {
               return BEFORE;
           }
           else if (d1.day > d2.day)
           {
               return AFTER;
           }
           else // same year, month, day, meaning two dates match
           {
               return SAME;
           }
       }
   }
}

RELATIVEDATE compareTime(const Time t1, const Time t2)
{
    if (!isValidTime(t1) || !isValidTime(t2))
    {
        return RELATIVEDATE_ERROR;
    }

    if (t1.hour < t2.hour)
    {
        return BEFORE;
    }
    else if (t1.hour > t2.hour)
    {
        return AFTER;
    }
    else // same hour, check minute
    {
        if (t1.minute < t2.minute)
        {
            return BEFORE;
        }
        else if (t1.minute > t2.minute)
        {
            return AFTER;
        }
        else // same hour and same minute, meaning two 'times' must match
        {
            return SAME;
        }
    }
}
    
void removeNewLineChar( char* str )
{
    assert(str != NULL && hasNewLineChar(str) );

    if ( str[strlen(str) - 2] == '\r' )
    {
        str[strlen(str) - 2] = '\0';
    }
    else
    {
        str[strlen(str) - 1] = '\0';
    }
}

int myatoi(const char* str, int* out) // TODO CREDIT THIS FUNCTION TO "https://stackoverflow.com/a/26083517/6860707"
{
    /* First, check for various errors with strtol (under-/overflow, no digits, or sg else) */
    char* endp = NULL; // pointer to remaining characters
    long l = 0; // strtol's output goes here

    errno = 0;
    l = strtol(str, &endp, 10);

    if
        (
        str == endp                         ||
        (errno == ERANGE && l == LONG_MIN)  || 
        (errno == ERANGE && l == LONG_MAX)  || 
        errno == EINVAL                     || 
        (errno != 0 && l == 0)
        )
    {
        return 0; // conversion failure, caller should know
    }
    
    /* Now using atoi seems safe */
    *out = atoi(str);
    
    return 1;
}

int promptYN(char* message, ...)
{
    assert(message != NULL);

    /* Handle parameters */
    char buff[BUFFSIZE];
    va_list args;
    va_start(args, message);
    int rc = vsnprintf(buff, sizeof buff, message, args);
    va_end(args);

    if (rc == 0)
    {
        warning("Function promptYN() failed to format string! Unformatted message follows\n");
        printf("%s [y/n] ", message);
    }
    else
    {
        printf("%s [y/n] ", buff);
    }

    char response = getchar();
    switch (response)
    {
        case 'y':
            return 1;
            break;

        case 'n':
            return 0;
            break;

        default:
            printf("Invalid choice: %c\n", response);
            return 0;
    }
}

int compareVEvent(const VEvent ve1, const VEvent ve2)
{
    return
        (
            compareDateTime(ve1.start, ve2.start) == SAME   &&
            compareDateTime(ve1.end, ve2.end) == SAME       &&
            strcmp(ve1.summary, ve2.summary) == 0           &&
            strcmp(ve1.location, ve2.location) == 0         &&
            strcmp(ve1.description, ve2.description) == 0   &&
            ve1.priority == ve2.priority
        );
}

