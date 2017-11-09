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

/* Misc */

typedef enum {
    FAIL_UNDERFLOW,
    FAIL_OVERFLOW,
    FAIL_PROMPT,
    FAIL_REMOVE,
    FAIL_FILE_READ,
    FAIL_FILE_WRITE,
    FAIL_FILE_EMPTY,
    FAIL_FILE_CORRUPT,
    FAIL_ICS_NOEND,
    FAIL_MALLOC,
    FAIL_REALLOC,
    FAIL_MYATOI,
    FAIL_TIMESTAMP_CORRUPT,
    FAIL_VEVENT_NOT_FOUND,
    FAIL_UNKNOWN,
    SUCCESS
} MYERRNO;

typedef enum {
    BEFORE,
    SAME,
    AFTER,
    ERROR
} RELATIVEDATE;
#endif
