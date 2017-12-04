#ifndef GUIHANDLER_H__
#define GUIHANDLER_H__

#include "structs.h"

#include <gtk/gtk.h>

////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

extern gpointer calendarWidgetData;

/////////////////
/// FUNCTIONS ///
/////////////////


void GUI(int, char**);

void mainWindow_create(GtkWidget*);

void calendarWidget_create(GtkWidget*);

void calendarWidget_markToday(GtkWidget*);

void calendarWidget_day_selected(GtkWidget*, gpointer); // CALLBACK

void detailViewer_update(GtkWidget*, Date d);

void on_activate_menuItem_new(GtkWidget*, gpointer);

void newEventConfirm(GtkWidget*, gpointer); // CALLBACK

void closeWindow(GtkWidget*, gpointer); // CALLBACK

void destroy(GtkWidget*, gpointer); // CALLBACK

#endif
