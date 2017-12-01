#ifndef ENUMS_H__
#define ENUMS_H__

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
} Month; 

// ---------------------

typedef enum {
    Short,
    Long
} Format;

/* Misc */

typedef enum {
    SUCCESS,
    FAIL_UNDERFLOW,
    FAIL_OVERFLOW,
    FAIL_PROMPT,
    FAIL_FILE_REMOVE,
    FAIL_FILE_READ,
    FAIL_FILE_WRITE,
    FAIL_FILE_EMPTY,
    FAIL_FILE_CORRUPT,
    FAIL_FILE_EXISTS,
    FAIL_ICS_NOEND,
    FAIL_MALLOC,
    FAIL_REALLOC,
    FAIL_MYATOI,
    FAIL_TIMESTAMP_CORRUPT,
    FAIL_INVALID_YEAR,
    FAIL_INVALID_MONTH,
    FAIL_INVALID_DAY,
    FAIL_INVALID_HOUR,
    FAIL_INVALID_MINUTE,
    FAIL_VEVENT_NOT_FOUND,
    FAIL_EOF,
    FAIL_DNE,
    FAIL_UNKNOWN
} MYERRNO;

typedef enum {
    NEW,
    OVERWRITE
} WriteMode;

typedef enum {
    NEUTRAL,
    WARNING,
    PROMPT,
    STATUS,
    PROGRESS,
    DONE,
    ERROR
} ShellSays;

typedef enum {
    DTSTART,
    DTEND
} TimestampType;

typedef enum {
    BEFORE,
    SAME,
    AFTER,
    RELATIVEDATE_ERROR
} RelativeDate;

typedef enum {
    NUMBER,
    STRING,
    TIMESTAMP
} ICSDataType;

#endif
