#include <stdio.h>
#include <string.h>
#include "data.h" // Month lengths and names

#define SUCCESS 1
#define FAILURE 0

////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

int isLeapYear(int year)
{
    return ( (year % 4) == 0 && (year % 100) != 0 && (year % 1000) == 0 );
}

void printMonthTitle(Month m, int width, Format f)
{
    if (f == Short)
    {
        for (int i = 0; i < (width - strlen(MonthNames[m].shortName) - 2 ) / 2; ++i)
        {
            putchar('-');
        }
        putchar('|');
        printf((MonthNames[m].shortName)); // TODO use puts or similar
        putchar('|');
        for (int i = 0; i < (width - strlen(MonthNames[m].shortName) - 2 ) / 2; ++i)
        {
            putchar('-');
        }
    }
    else if (f == Long)
    {
        for (int i = 0; i < (width - strlen(MonthNames[m].longName) - 2) / 2 + ((strlen(MonthNames[m].longName) + 1)  % 2); ++i)
        {
            putchar('-');
        }
        putchar('|');
        printf((MonthNames[m].longName)); // TODO use puts or similar
        putchar('|');
        for (int i = 0; i < (width - strlen(MonthNames[m].longName) - 2 ) / 2; ++i)
        {
            putchar('-');
        }
    }
    else
    {
        printf("Error!"); // TODO error handling
    }
    return;
}

int readICS(char* path) 
{
    char buff[255]; // TODO better implementation?
    FILE* fp;
    fp = fopen(path, "r");
    
    int eventCounter = 0;
    
    fgets(buff, 255, (FILE*) fp);
    while(strcmp(buff, "END:VCALENDAR"))
    {
        // we have not yet reached the end
        if (!strcmp(buff, "BEGIN:VEVENT"))
        {
            eventCounter++;
        }

        // read next line
        fgets(buff, 255, (FILE*) fp);
    }

    // debug command
    printf("%d\n", eventCounter);
    
    fclose(fp);
    
    return 0;
}


int main(int argc, char **argv)
{
    // PRINT MONTH TITLES
    /*
    for (Month m = January; m <= December; ++m)
    {
        printMonthTitle(m, 21, Short);
        putchar('\n');
    }

    printf("\n\n\n");

    for (Month m = January; m <= December; ++m)
    {
        printMonthTitle(m, 21, Long);
        putchar('\n');
    }
    */

    // PRINT NUMBER OF EVENTS

    printf("%d\n", readICS("./cal.ics")); 

    return 0;
}

