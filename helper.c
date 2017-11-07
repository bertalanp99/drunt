#include "helper.h"
#include "errorHandler.h"
#include "arrays.h"
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

int isLeapYear(const int year)
{
    if (year < YEAR_MIN || year > YEAR_MAX)
    {
        return -1;
    }
    
    return ( (year % 4 == 0) && (year % 100 != 0) && (year % 400 == 0) );
}

int hasICSTag(const char* str, const char* tag)
{
    assert(str != NULL && tag != NULL);
    return !strncmp(str, tag, strlen(tag));
}

MYERRNO ICSVEventCounter(FILE* fp, int* count)
{
    assert(fp != NULL && count != NULL);

    char buff[BUFFSIZE];
    
    *count = 0;
    int currentLine = 0;
    int noend = 0;

    /* Read first line and check whether file is empty */
    ++currentLine;
    if (fgets(buff, BUFFSIZE, fp) == NULL)
    {
        return FAIL_FILE_EMPTY;
    }

    /* Read other lines and count occurrences of VEVENTs */
    while(!hasICSTag(buff, "END:VCALENDAR"))
    {
        ++ currentLine;

        if (hasICSTag(buff, "BEGIN:VEVENT"))
        {
            ++*count;
        }

        // prevent infinite loop by also checking for EOF
        if (feof(fp))
        {
            noend = 1;
            break;
        }

        fgets(buff, BUFFSIZE, fp);
    }

    return (noend) ? SUCCESS : FAIL_ICS_NOEND;
}

MYERRNO ICSTimeStampReader(const char* str, DateTime* dt)
{
    /* Format of iCalendar timestamp: YYYYMMDDTHHMMSS --- ex gr 20171112T090000 = 2017/11/12 09.00.00 (we are not going to use seconds for anything) */

    assert( str != NULL && dt != NULL && !strcmp(str, "") );

    int year;
    int month;
    int day;
    
    int hour;
    int minute;
    
    /* Interpret the input string and check for errors each time (myatoi()'s return comes in handy) */
    char buff[4]; // the longest string part is the year, which is 4 characters long

    // year
    strncpy(buff, str, 4);
    if (myatoi(buff, &year) == 0)
    {
        return FAIL_MYATOI;
    }

    // month
    strncpy(buff, (str + 4), 2);
    if (myatoi(buff, &month) == 0)
    {
        return FAIL_MYATOI;
    }

    // day
    strncpy(buff, (str + 6), 2);
    if (myatoi(buff, &day) == 0)
    {
        return FAIL_MYATOI;
    }

    // hour
    strncpy(buff, (str + 9), 2);
    if (myatoi(buff, &hour) == 0)
    {
        return FAIL_MYATOI;
    }

    // minute
    strncpy(buff, (str + 11), 2);
    if (myatoi(buff, &minute) == 0)
    {
        return FAIL_MYATOI;
    }

    /* Seemingly, no errors were encountered --- set values */
    dt->date.year = year;
    dt->date.month = month;
    dt->date.day = day;
    dt->time.hour = hour;
    dt->time.minute = minute;

    return SUCCESS;
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

void icsTagRemover(char* original, char* new)
{
    assert
        (   
            original != NULL      && new != NULL        &&
            !strcmp(original, "") && !strcmp(new, "")
        );

    /* Look for iCalendar tag in input string */
    for (int i = 0; ICSTags[i] != NULL; ++i)
    {
        if ( !strncmp(original, ICSTags[i], strlen(ICSTags[i])) )
        {
            new = (original + strlen(ICSTags[i])); // found tag --> store it in new and return
            return;
        }
    }
    
    new = NULL; // could not find tag, return NULL to indicate
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

    char response;
    scanf(" %c", &response);
    switch (response)
    {
        case 'y':
            return 1;
            break;

        case 'n':
            return 0;
            break;

        default:
            printf("Invalid choice: %c", response);
            return 0;
    }
}
