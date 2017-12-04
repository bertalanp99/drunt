#ifndef STRUCTS_H__
#define STRUCTS_H__

#include "enums.h"

#include <gtk/gtk.h>

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

/* Databases */

typedef struct {
    DateTime start;
    DateTime end;
    char* summary;
    char* location;
    char* description;
    int priority;
} VEvent;

typedef struct VEventNode_s {
    VEvent ve;
    struct VEventNode_s* prev;
    struct VEventNode_s* next;
} VEventNode;

typedef struct {
    int numberOfEntries;
    VEventNode* first;
    VEventNode* last;
} Calendar;

/* GTK */

typedef struct {
    GtkWidget* yearFrom;
    GtkWidget* yearTo;
    GtkWidget* monthFrom;
    GtkWidget* monthTo;
    GtkWidget* dayFrom;
    GtkWidget* dayTo;
    GtkWidget* hourFrom;
    GtkWidget* hourTo;
    GtkWidget* minuteFrom;
    GtkWidget* minuteTo;
    GtkWidget* summary;
    GtkWidget* location;
    GtkWidget* description;
    GtkWidget* priority;
} newEvent_inputData;

/* Misc */

typedef struct {
    char* tag;
    ICSDataType type;
    int flag;
} ICSTag;

#endif
