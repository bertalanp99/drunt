#include "helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define YEAR_MIN 1950
#define YEAR_MAX 2050

int isLeapYear(const int year)
{
    if (year < YEAR_MIN || year > YEAR_MAX)
    {
        return -1;
    }
    
    return ( (year % 4 == 0) && (year % 100 != 0) && (year % 400 == 0) );
}

int icsTimeStampReader(const char* str, DateTime* dt)
{
    /* Format of iCalendar timestamp: YYYYMMDDTHHMMSS --- ex gr 20171112T090000 = 2017/11/12 09.00.00 (we are not going to use seconds for anything) */

    /* First, check for erroneous input */
    if (str == NULL)
    {
        return 0;
    }

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

    /* Seemingly, no errors were encountered. Let's set values at input location as asked */
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

char* icsTagRemover(char* line, char* tag)
{
    /* First, check for erroneous arguments */
    if (line == NULL || tag == NULL || (strcmp(line, "") == 0) || (strcmp(tag, "") == 0))
    {
        return NULL;
    }
    
    char* out = malloc( (strlen(line) - strlen(tag) - 2 + 1) * sizeof(char) ); // -2: colon between tag and value and \n at end; +1: closing 0
    int i = 0; // itetator for out
    for (int j = strlen(tag) + 1; line[j] != '\n'; ++j) 
    {
        out[i++] = line[j];
    }
    out[i] = '\0';
    return out;
}
    
int promptYN(char* message)
{
    char response;
    printf("%s [y/n] ", message);
    scanf("%c", &response);
    switch (response)
    {
        case 'y':
            return 1;
            break;

        case 'n':
            return 0;
            break;

        default:
            return -1; // invalid response --- caller should check for this
            break;
    }
}
