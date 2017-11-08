#ifndef STRUCTS_H__
#define STRUCTS_H__

/* Date & Time */

typedef struct {
    int year; 
    int month;
    int day;
} Date;

typedef struct {
    int hour;
    int minute;
} Time;

typedef struct {
    Date date;
    Time time;
} DateTime;

/* Events */
typedef struct {
    DateTime start;
    DateTime end;
    char* summary;
    char* location;
    char* description;
    int priority;
} VEvent;

typedef struct {
    char longName[10];
    char shortName[4];
} MonthName; // TODO do I really need this?

/* Databases */
typedef struct {
    int numberOfEntries;
    VEvent* vevents;
} Calendar;

#endif