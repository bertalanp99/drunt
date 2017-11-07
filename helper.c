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

#define BUFFSIZE 256

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

MYERRNO icsTimeStampReader(const char* str, DateTime* dt)
{
    /* Format of iCalendar timestamp: YYYYMMDDTHHMMSS --- ex gr 20171112T090000 = 2017/11/12 09.00.00 (we are not going to use seconds for anything) */

    assert(str != NULL && dt != NULL);

    int year;
    int month;
    int day;
    
    int hour;
    int minute;
    
    /* Interpret the input string and check for errors each time (myatoi()'s return comes in handy) */
    char buff[4]; // the longest string part is the year, which is 4 characters long
    
    // TODO better way to copy certain parts of string into buffer?
    int i = 0;
    int j = 0;
    
    // year
    for (i = 0; i < 4; ++i)
    {
        buff[j++] = str[i];
    }
    buff[j] = '\0';
    if (myatoi(buff, &year) == 0)
    {
        return 0;
    }

    // month
    j = 0;
    for (i = 4; i < 6; ++i)
    {
        buff[j++] = str[i];
    }
    buff[j] = '\0';
    if (myatoi(buff, &month) == 0)
    {
        return 0;
    }

    // day
    j = 0;
    for (i = 6; i < 8; ++i)
    {
        buff[j++] = str[i];
    }
    buff[j] = '\0';
    if (myatoi(buff, &day) == 0)
    {
        return 0;
    }

    // hour
    j = 0;
    for (i = 9; i < 11; ++i)
    {
        buff[j++] = str[i];
    }
    buff[j] = '\0';
    if (myatoi(buff, &hour) == 0)
    {
        return 0;
    }

    // minute
    j = 0;
    for (i = 11; i < 13; ++i)
    {
        buff[j++] = str[i];
    }
    buff[j] = '\0';
    if (myatoi(buff, &minute) == 0)
    {
        return 0;
    }

    /* Seemingly, no errors were encountered. Set values at input location as asked */
    dt->date.year = year;
    dt->date.month = month;
    dt->date.day = day;
    dt->time.hour = hour;
    dt->time.minute = minute;

    return 1;
}

int myatoi(const char* str, int* out) // TODO CREDIT THIS FUNCTION TO "https://stackoverflow.com/a/26083517/6860707"
{
    /* First, check for various errors with strtol (under-/overflow, no digits, or sg else) */
    char* endp = NULL; // pointer to remaining characters
    long l = 0; // strtol's output goes here

    errno = 0; // reset errno
    l = strtol(str, &endp, 10);

    if (str == endp || (errno == ERANGE && l == LONG_MIN) || (errno == ERANGE && l == LONG_MAX) || errno == EINVAL || (errno != 0 && l == 0))
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
            original != NULL        &&
            new != NULL             &&
            !strcmp(original, "")   &&
            !strcmp(new, "")
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
