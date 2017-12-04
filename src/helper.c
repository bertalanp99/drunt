#include "helper.h"
#include "errorHandler.h"
#include "enums.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#define YEAR_MIN 1950
#define YEAR_MAX 2050

#define MAX_LINELENGTH 600 // 75 octets
#define LONGESTPROPERTY "Description"

////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

static int monthLength[12] = {
    31, // jan
    28, // feb
    31, // mar
    30, // apr
    31, // may
    30, // jun
    31, // jul
    31, // aug
    30, // sep
    31, // oct
    30, // nov
    31  // dec
};

static char* monthTitles[12] = {
    "JAN",
    "FEB",
    "MAR",
    "APR",
    "MAY",
    "JUN",
    "JUL",
    "AUG",
    "SEP",
    "OCT",
    "NOV",
    "DEC"
};

static char dayLetters[7] = {
    'M',
    'T',
    'W',
    'T',
    'F',
    'S',
    'S'
};

/////////////////
/// FUNCTIONS ///
/////////////////

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
    char buff[BUFSIZ];
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

bool isValidICS(const char* file)
{
    assert(file != NULL);
    
    if (isNonEmptyFile(file) != SUCCESS)
    {
        return false; // empty file
    }

    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        return false; // unable to read
    }

    /* Go through the file line by line; if a line starting with BEGIN is found, look
     * for a closing END at heads of lines. If no END is found or another BEGIN of same
     * type is found, the ICS file is corrupt */

    char buff[BUFSIZ];
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
        return false; // first line is not BEGIN:VCALENDAR
    }

    // check other lines
    while (fgets(buff, sizeof buff, fp) != NULL)
    {
        ++currentLine;
        removeNewLineChar(buff);
        if (endCalendarFlag)
        {
            warning("Content found after END:VCALENDAR at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
            return false;
        }
        else if ( !strcmp(buff, "BEGIN:VCALENDAR") )
        {
            warning("Nested VCALENDAR at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
            return false;
        }
        else if ( !strcmp(buff, "END:VCALENDAR") )
        {
            if (beginVEventFlag)
            {
                warning("BEGIN:VEVENT closed by END:VCALENDAR at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                return false;
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
                return false;
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
                return false;
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
                    return false;
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
                        return false;
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
                                    return false;
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
                                    return false;
                                }
                                else
                                {
                                    tags[i].flag = 1;
                                }
                                break;
                            }

                            case NUMBER: // note: drunt only processes priority as NUMER
                            {
                                int n;
                                if ( !myatoi(buff + strlen(tags[i].tag), &n) )
                                {
                                    warning("Failed to convert priority to integer at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                                    return false;
                                }
                                else
                                {
                                    if ( !isValidPriority(n) )
                                    {
                                        warning("Invalid priority at %s:%d\n[ x> ]\t%s", file, currentLine, buff);
                                        return false;
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
        return false;
    }
    else
    {
        return true;
    }

    fclose(fp);
}

bool isValidPriority(int p)
{
    return (p >= 0 && p < 10);
}

bool isValidICSTimeStamp(const char* timestamp)
{
    assert(timestamp != NULL && strcmp(timestamp, ""));
   
    // temporarily myatoi() everything and also check their validity
    char buff[8]; // 4+1 would be enough though
    int year;
    int month;
    int day;
    int hour;
    int minute;

    return
        (
            myatoi(mystrncpy(buff, timestamp, 4), &year)         && isValidYear(year)               &&
            myatoi(mystrncpy(buff, timestamp + 4, 2), &month)    && isValidMonth(month)             &&
            myatoi(mystrncpy(buff, timestamp + 6, 2), &day)      && isValidDay(day, month, year)    &&
            myatoi(mystrncpy(buff, timestamp + 9, 2), &hour)     && isValidHour(hour)               &&
            myatoi(mystrncpy(buff, timestamp + 11, 2), &minute)  && isValidMinute(minute)           &&
            timestamp[15] == 'Z'
        );
}

bool hasNewLineChar(const char* str)
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

bool isValidYear(const int year)
{
    return (year >= 1950 && year <= 2050);
}

bool isValidMonth(const int month)
{
    return (month >= 1 && month <= 12);
}

bool isValidDay(const int day, const int month, const int year)
{
    if (isLeapYear(year) && (month == 2))
    {
        return (day >= 1 && day <= (monthLength[month - 1] + 1));
    }
    else
    {
        return (day >=1 && day <= monthLength[month - 1]);
    }
}

bool isValidHour(const int hour)
{
    return (hour >= 0 && hour < 24);
}

bool isValidMinute(const int minute)
{
    return (minute >= 0 && minute < 60);
}

bool isValidDate(const Date d)
{
    return ( isValidYear(d.year) && isValidMonth(d.month) && isValidDay(d.day, d.month, d.year) );
}

bool isValidTime(const Time t)
{
    return ( isValidHour(t.hour) && isValidMinute(t.minute) );
}

bool isValidDateTime(const DateTime dt)
{
    return ( isValidDate(dt.date) && isValidTime(dt.time) );
}

bool isValidVEvent(const VEvent ve)
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

RelativeDate compareDateTime(const DateTime dt1, const DateTime dt2)
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

RelativeDate compareDate(const Date d1, const Date d2)
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

RelativeDate compareTime(const Time t1, const Time t2)
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

bool promptYN(char* message, ...) // TODO is this needed?
{
    assert(message != NULL);

    /* Handle parameters */
    char buff[BUFSIZ];
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

bool compareVEvent(const VEvent ve1, const VEvent ve2)
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

DateTime currentDateTime(void)
{
    DateTime dt;

    char* months[] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };
    
    time_t sinceEpoch = time(NULL);
    char* formatted = ctime(&sinceEpoch);

    /* ctime output format: 'Www Mmm dd hh mm ss yyyy' | ex gr 'Tue May 26 21:51:03 2015'
                                                                012345678901234567890123  */

    /* Copy stuff by characters */
    
    int position = 0;
    position = 4; // first thing we're interested in is the month -> unfortunately we have to decode it
    for (int i = 0; i < 12; ++i)
    {
        if ( !strncmp(months[i], formatted + position, 3) )
        {
            dt.date.month = i + 1;
            break;
        }
    }

    position = 8;
    char day[3] = { formatted[position], formatted[position + 1], '\0' };
    myatoi(day, &dt.date.day);

    position = 11;
    char hour[3] = { formatted[position], formatted[position + 1], '\0' };
    myatoi(hour, &dt.time.hour);

    position = 14;
    char min[3]= { formatted[position], formatted[position + 1], '\0' };
    myatoi(min, &dt.time.minute);

    position = 20;
    myatoi(formatted + position, &dt.date.year);

    // check programmer error
    assert(isValidDateTime(dt));

    return dt;
}

DateTime addDaysToDateTime(const DateTime from, const int days)
{
    assert(isValidDateTime(from));
    
    DateTime dt = from;
    for (int i = 0; i < days; ++i)
    {
        if (monthOverflows(dt.date.month))
        {
            ++dt.date.year;
            dt.date.month = 1;
        }

        if (dayOverflows(dt.date.day, dt.date.month, dt.date.year))
        {
            ++dt.date.month;
            dt.date.day = 1;
        }

        ++dt.date.day;
    }
        
    if (monthOverflows(dt.date.month))
    {
        ++dt.date.year;
        dt.date.month = 1;
        dt.date.day = 1;
    }

    if (dayOverflows(dt.date.day, dt.date.month, dt.date.year))
    {
        ++dt.date.month;
        dt.date.day = 1;
    }
    
    // check for programmer error
    assert(isValidDateTime(dt));

    return dt;
}

bool monthOverflows(const int month)
{
    return (month > 12);
}

bool dayOverflows(const int day, const int month, const int year)
{
    if (isLeapYear(year))
    {
        if (month == 2)
        {
            if (day > 29)
            {
                return true;
            }
        }
    }

    return (day > monthLength[month - 1]);
}

void printVEvent(const VEvent ve, const int count)
{
    int width = (count) ? (strlen(LONGESTPROPERTY) + 1) : strlen(LONGESTPROPERTY); 
    
    printf((count) ? "\n| VEVENT %02d |\n" : "\n| VEVENT |\n", count);
    printf("%*s %04d/%02d/%02d %02d.%02d [Z]\n",
            width, "Start:",
            ve.start.date.year, ve.start.date.month, ve.start.date.day,
            ve.start.time.hour, ve.start.time.minute);
    printf("%*s %04d/%02d/%02d %02d.%02d [Z]\n",
            width, "End:",
            ve.end.date.year, ve.end.date.month, ve.end.date.day,
            ve.end.time.hour, ve.end.time.minute);
    printf("%*s %s\n", width, "Summary:", ve.summary);
    printf("%*s %s\n", width, "Location:", ve.location);
    printf("%*s %s\n", width, "Description", ve.description);
    printf("%*s %d\n", width, "Priority:", ve.priority);
    printf("\n");
}

MYERRNO parseDateTime(DateTime* dest, const char* year, const char* month, const char* day, const char* hour, const char* minute)
{
    assert( year && month && day && hour && minute );
    if
        (
            !myatoi(year, &dest->date.year)      ||
            !myatoi(month, &dest->date.month)    ||
            !myatoi(day, &dest->date.day)        ||
            !myatoi(hour, &dest->time.hour)      ||
            !myatoi(minute, &dest->time.minute)
        )
    {
        return FAIL_MYATOI;
    }

    if ( !isValidYear(dest->date.year) )
    {
        return FAIL_INVALID_YEAR;
    }
    else if ( !isValidMonth(dest->date.month) )
    {
        return FAIL_INVALID_MONTH;
    }
    else if ( !isValidDay(dest->date.day, dest->date.month, dest->date.day) )
    {
        return FAIL_INVALID_DAY;
    }
    else if ( !isValidHour(dest->time.hour) )
    {
        return FAIL_INVALID_HOUR;
    }
    else if ( !isValidMinute(dest->time.minute) )
    {
        return FAIL_INVALID_MINUTE;
    }

    return SUCCESS;
}

void printDayHeader(const Date day)
{
    assert(isValidDate(day));

    printf("\n");
    printf("<-------------------->\n");
    printf("<---< %04d/%02d/%02d >--->\n", day.year, day.month, day.day);
    printf("<-------------------->\n");
    printf("\n");
}

void printMonthHeader(const Date month) // TODO generic to all these
{
    assert(isValidDate(month));

    printf("\n");
    printf("<----------------->\n");
    printf("<---< %04d/%02d >--->\n", month.year, month.month);
    printf("<----------------->\n");
    printf("\n");
}

void printMonthTitle(const int month, const int width, const char borderChar)
{
    assert(month > 0 && month <= 12);

    int mid = width / 2;

    putchar(borderChar);
    printSep(' ', ((mid - 2) - (mid % 2)), false);
    printf("%s", monthTitles[month - 1]);
    printSep(' ', (mid - 2), false);
    putchar(borderChar);
}
    

void printHourHeader(const int hour)
{
    assert(isValidHour(hour));

    printf("<--- %02d h --->\n", hour);
}

void printSep(const char sepChar, const int n, bool lineBreak)
{
    for (int i = 0; i < n; ++i)
    {
        putchar(sepChar);
    }
    
    if (lineBreak)
    {
        putchar('\n');
    }
}

int zellerToISO(const Date d)
{
    // check for incorrect parameters
    assert(isValidDate(d));

    /* Zeller's original congruence:
     * h = ( q + FLOOR((13(m+1))/(5)) + K + FLOOR(K/4) + FLOOR(J/4) - 2J ) % 7
     * 
     * Zeller's congruence modified:
     * 
     * h = ( q + FLOOR((13(m+1))/(5)) + K + FLOOR(K/4) + FLOOR(J/4) + 5J ) % 7
     *
     * where...
     *  h = day of week (0 -> SAT, 1 -> SUN, ..., 6 -> FRI)
     *  q = day of month
     *  m = month (3 -> MAR, 4 -> APR, ..., 14 -> FEB)
     *  K = year % 100
     *  J = FLOOR(year / 100)
     *
     * to convert h to an ISO week date where 1 means MON and 7 means SUN:
     *  d = ( (h + 5) % 7 ) + 1
     */

    int year = d.year;
    int month = d.month;
    int day = d.day;

    int rv; // future return value
    int h; // zeller value in original 'format'
    int q = day;
    int m; // gets value below
    if (month > 2)
    {
        m = month;
    }
    else
    {
        m = (month + 12);
        --year; // month trated as extra month of previous year
    }
    int K = (year % 100);
    int J = (year / 100);
    
    h = ((q + (13*(m+1))/(5) + K + K/4 + J/4 + 5*J ) % 7);
    rv = (( (h+5) % 7) + 1);
    
    return rv;
}

void printMonth(Date month, const Calendar* cal)
{
    assert(isValidDate(month));
    
    /* Make sure we start on the first day */
    month.day = 1;
    
    /* Print header */
    printMonthHeader(month);

    /* Find out what day the first day is (MON, TUE... represented as int) */
    int firstDay = zellerToISO(month);

    /* Set number of leading spaces in first row (days of previous month) */
    int leadingSpaces = firstDay - 1;

    /* Print day letters (stored in global array) */
    for (int i = 0; i < 7; ++i)
    {
        printf("%2c", days[i]);
        printf(" ");
    }

    /* Print separator line */
    printf("\n");
    printSep( '-', ((7*2) + 7 - 1),  true); // two for each day + 7 for each space -1 for better looks
    
    /* Print leading spaces */
    for (int i = 0; i < leadingSpaces; ++i)
    {
        printf("  "); // leave day blank
        printf(" "); // space between days
    }

    /* Traverse calendar until month is reached */
    VEventNode* traveller = cal->first->next;
    while ((traveller != cal->last) && (compareDate(traveller->ve.start.date, month) == BEFORE))
    {
        traveller = traveller->next;
    }
    
    /* Print actual table and every time there's a line break, print event indicators in next line */
    int nOfDays;
    if (isLeapYear(month.year))
    {
        nOfDays = 29;
    }
    else
    {
        nOfDays = monthLength[month.month - 1];
    }
    for (int i = 1, j = 1; i <= nOfDays; ++i)
    {
        printf("%2d", i); // print day no
        printf(" "); // space between days
        
        if ((i + leadingSpaces) % 7 == 0) // line break condition
        {
            printf("\n");
            while (traveller != cal->last && j <= i) // traverse calendar until last day printed (in line above)
            {
                if ((traveller->ve.start.date.month == month.month) && (traveller->ve.start.date.day == j)) // event indicator condition
                {
                    printf("%2c", '\''); // print event indicator
                    printf(" "); // print space between indicators
                    // find next event in calendar, but not on current day
                    while (traveller->ve.start.date.day == j)
                    {
                        traveller = traveller->next;
                    }
                }
                else
                {
                    printf("  "); // leave space blank
                    printf(" "); //space between indicators
                }
                ++j;
            }
            printf("\n");
        }
    }

    printf("\n");
}

int nthDigit(int num, int n)
{
    return ( (num % (int) pow(10, n)) / (int) pow(10, n-1) );
}

void printYearHeader(const int year, const int spacing, const int layoutWidth, const char thickChar, const char thinChar) // layout width = number of months in a row
{
    int length = ((layoutWidth * ((7*2) + 6 + 2)) + ((layoutWidth - 1) * spacing));
    int mid = length / 2;

    putchar('\n');
    printSep(thickChar, length, true);
    for (int i = 0; i < (mid - 4); ++i)
    {
        putchar(thinChar);
    }
    printf(" %d %d %d %d ", nthDigit(year, 4), nthDigit(year, 3), nthDigit(year, 2), nthDigit(year, 1));
    for (int i = 0; i < (mid - 4 - !(length % 2)); ++i)
    {
        putchar(thinChar);
    }
    putchar('\n');
    printSep(thickChar, length, false);
    putchar('\n');
}

void printYear(Date date, int layoutWidth)
{
    assert(isValidDate(date) && (layoutWidth >= 1) && (layoutWidth <= 4));

    int year = date.year;
    int layoutHeight = 12 / layoutWidth;

    /* Unfortunately, the only viable way is to create a temporary
     * struct type (monthTable_s) and an array of these (monthTables)
     * Then using multiple nested loops, we print these tables row by row
     */

    /* Defining temporary structure and creating an array of it */
    struct monthTable_s {
        int month;
        int table[6][7];
    };

    struct monthTable_s monthTables[12] = { 0 }; // initialise to 0 so that we are able to skip filling zeroes

    /* Filling array of tables (same algorithm as in printMonth) */
    for (int i = 1; i <= 12; ++i)
    {
        // n-th month
        monthTables[i-1].month = i;
        // filling table using algorithm
        int firstDay = zellerToISO((Date) {.year = year, .month = i, .day = 1}); // compound literal (C99)
        int leadingSpaces = (firstDay - 1);
        int vloc = 0; // vertical cursor position
        int hloc = 0; // horizontal cursor position
        // leading spaces are now represented as 0's
        for (vloc = 0; vloc < leadingSpaces; ++vloc)
        {
            ; // empty loop
        }

        int nOfDays;
        if (isLeapYear(year) && (i == 2))
        {
            nOfDays = 29;
        }
        else
        {
            nOfDays = monthLength[i-1];
        }

        for (int j = 1; j <= nOfDays; ++j)
        {
            monthTables[i-1].table[hloc][vloc++] = j;

            if ((j + leadingSpaces % 7) == 0)
            {
                ++hloc;
                hloc = 0;
            }
        }
    }

    /* --- */

    /* Tables created; Print things */
    int monthWidth = ((7*2) + 6 + 2); // (7*2): 2 characters for each day; (6): spaces; (2): borders
    int spacing = 5;
    int monthCount_title = 1;
    int monthCount_table = 0;
    
    char thinChar = '-';
    char thickChar = '=';
    char borderChar = '|';
    char space = ' ';

    // header
    putchar('\n');
    printYearHeader(year, spacing, layoutWidth, thickChar, thinChar);
    putchar('\n');
    
    // ugly nested loops (they work though  ^^)
    for (int i = 0; i < layoutHeight; ++i)
    {
        for (int j = 0; j < layoutWidth; ++j)
        {
            printSep(thinChar, monthWidth, false);
            printSep(space, spacing, false);
        }
        putchar('\n');
        for (int j = 0; j < layoutWidth; ++j)
        {
            printMonthTitle(monthCount_title++, monthWidth, borderChar);
            printSep(space, spacing, false);
        }
        putchar('\n');
        for (int j = 0; j < layoutWidth; ++j)
        {
            putchar(borderChar);
            printSep(thickChar, (monthWidth - 2), false);
            putchar(borderChar);
            printSep(space, spacing, false);
        }
        putchar('\n');
        for (int j = 0; j < layoutWidth; ++j)
        {
            putchar(borderChar);
            for (int k = 0; k < 7; ++k)
            {
                printf("%2c", dayLetters[k]);
                if (k != 6)
                {
                    putchar(space);
                }
            }
            putchar(borderChar);
            printSep(space, spacing, false);
        }
        putchar('\n');
        for (int j = 0; j < 6; ++j)
        {
            for (int k = 0; k < layoutWidth; ++k)
            {
                putchar(borderChar);
                for (int l = 0; l < 7; ++l)
                {
                    if (monthTables[monthCount_table].table[j][l] == 0)
                    {
                        printf("%2c", space);
                    }
                    else
                    {
                        printf("%2d", monthTables[monthCount_table].table[j][l]);
                    }
                    
                    if (l != 6)
                    {
                        putchar(space);
                    }
                }
                putchar(borderChar);
                printSep(space, spacing, false);
                monthCount_table += 1;
            }
            if (j != 5)
            {
                putchar('\n');
            }
            monthCount_table -= layoutWidth;
        }
        putchar('\n');
        for (int j = 0; j < layoutWidth; ++j)
        {
            printSep(thinChar, monthWidth, false);
            printSep(space, spacing, false);
        }

        monthCount_table += layoutWidth;
        putchar('\n');
        putchar('\n');
    }
    putchar('\n');
}

int countVEvents(const Calendar* cal, const DateTime from, const DateTime to)
{
    assert(cal != NULL);

    int count = 0;

    /* Traverse calendar until first VEvent after $from */
    VEventNode* traveller = cal->first->next;
    while (traveller != cal->last && compareDateTime(traveller->ve.start, from) == BEFORE)
    {
        traveller = traveller->next;
    }

    /* Keep traversing until last VEvent before $to and count */
    while (traveller != cal->last && compareDateTime(traveller->ve.start, to) == BEFORE)
    {
        ++count;
        traveller = traveller->next;
    }

    return count;
}

bool isValidModifyOption(const int c)
{
    return (c == 'b' || c == 'e' || c == 's' || c == 'l' || c == 'd' || c == 'p');
}

