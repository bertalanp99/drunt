#ifndef ARRAYS_H__
#define ARRAYS_H__

#include "enums.h"

#include <stdlib.h>

/*int monthLength[12] = {
    31, // January
    28, // February
    31, // March
    30, // April
    31, // May
    30, // June
    31, // July
    31, // August
    30, // September
    31, // October
    30, // November
    31  // December
};*/ // TODO unused

/*extern char* ICSTags[] = {
    "DTSTART:",
    "DTEND:",
    "SUMMARY:",
    "LOCATION:",
    "DESCRIPTION:",
    "PRIORITY:",
    NULL
}*/ // TODO unused

static char* commands[] = {
    "help",
    "exit",
    "open",
    "create",
    "modify",
    "delete",
    "list"
};

#endif

