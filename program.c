#include <stdio.h>
#include <string.h>
#include "data.h" // Month lengths and names

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

void printMonth(Month m, Format f)
{
    printMonthTitle(m, 21, f);
    puts(""); // new line
    for (int i =0; i < 70; ++i)
    {
        if (i % 7 == 0)
        {
            puts(""); // newline
        }
        // TODO
    }

    return;
}

int main(int argc, char **argv)
{
    /*for (int i = 0; i < 12; ++i)
    {
        printf("Long: %s\t| Short: %s\n", MonthNames[i].longName, MonthNames[i].shortName);
    }
    */

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
    
    return 0;
}

