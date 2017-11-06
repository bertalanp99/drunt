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
    dev
} Month; // TODO use this

// ---------------------

typedef enum {
    Short,
    Long
} Format;

typedef enum {
    DIE_PROMPT,
    DIE_REMOVEFAILURE,
    DIE_FOPENFAILURE,
    SUCCESS
} ICS_WRITE_ERRNO;

#endif
