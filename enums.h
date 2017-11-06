#ifndef ENUMS_H__
#define ENUMS_H__

// Long month names
typedef enum {
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
} LongMonth; // TODO unused as of yet. Do I even need this?

// Short month names
typedef enum {
    jan,
    feb,
    mar,
    apr,
    may,
    jun,
    jul,
    aug,
    sep,
    oct,
    nov,
    dec
} Month; // TODO use this

// ---------------------

typedef enum {
    Short,
    Long
} Format;

/* Function ERRNOs */

typedef enum {
    FAIL_PROMPT,
    FAIL_REMOVE,
    FAIL_FOPEN,
    FAIL_NULLPASSED,
    FAIL_REALLOC,
    SUCCESS
} MYERRNO;

#endif
