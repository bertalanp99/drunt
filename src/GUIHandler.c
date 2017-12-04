#include "GUIHandler.h"
#include "dbHandler.h"
#include "interactiveHandler.h"
#include "drunt.h"
#include "structs.h"

#include <gtk/gtk.h>

////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

gpointer calendarWidgetData = &calendar;

/////////////////
/// FUNCTIONS ///
/////////////////

void GUI(int argc, char** argv)
{
    /* Widgets */
    GtkWidget* mainWindow; // main window
    GtkWidget* mainWindowHeader; // header of main window
    GtkWidget* menuButton; // menu button
    GtkWidget* mainMenu; // menu
    GtkWidget* menuItem_quit; // menu item that quits drunt
    GtkWidget* menuItem_new; // menu item that adds new calendar entry
    GtkWidget* mainBox; // main box (contains calendar widget and label widget (that shows details on selected day))
    GtkWidget* calendarWidget; // calendar
    GtkWidget* detailViewer; // label widget that shows details on day selected

    /* GTK init */
    gtk_init(&argc, &argv);

    /* ---------------------------------------------------------------------------------------------------------------------- */

    /* Header setup */
    mainWindowHeader = gtk_header_bar_new(); // define header bar
    menuButton = gtk_menu_button_new(); // define menu button
    mainMenu = gtk_menu_new(); // define menu
    menuItem_quit = gtk_menu_item_new_with_label("Quit"); // define quit menu item
    menuItem_new = gtk_menu_item_new_with_label("New"); // define 'new' menu item
    gtk_widget_show_all(menuItem_quit); // show menu items
    gtk_widget_show_all(menuItem_new); // show menu items
    
    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenu), menuItem_new); // add 'new event' menu item to menu
    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenu), menuItem_quit); // add quit menu item to menu
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(menuButton), mainMenu); // set relation of menu and its button
    g_signal_connect(menuItem_new, "activate", G_CALLBACK(on_activate_menuItem_new), NULL); // set action for quit menu item
    g_signal_connect(menuItem_quit, "activate", G_CALLBACK(destroy), NULL); // set action for quit menu item

    gtk_header_bar_set_title(GTK_HEADER_BAR(mainWindowHeader), "drunt"); // title
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(mainWindowHeader), "an InfoC project by bertalanp99"); // subtitle
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(mainWindowHeader), TRUE); // show close button
    gtk_header_bar_set_decoration_layout(GTK_HEADER_BAR(mainWindowHeader), "menu:close"); // show only close button
    gtk_header_bar_pack_start(GTK_HEADER_BAR(mainWindowHeader), menuButton); // add menu button to header bar

    /* Detail viewer setup */
    detailViewer = gtk_label_new("<i>Heya!\nSelect a day to see events\non that day here!</i>"); // define as label (argument means with no text initially)
    gtk_label_set_use_markup(GTK_LABEL(detailViewer), TRUE); // enable markup formatting
    gtk_label_set_width_chars(GTK_LABEL(detailViewer), 32); // set width
    gtk_label_set_max_width_chars(GTK_LABEL(detailViewer), 32);

    /* Calendar setup */
    calendarWidget = gtk_calendar_new(); // define as calendar
    calendarWidget_markToday(calendarWidget); // mark today date
    g_signal_connect(calendarWidget, "day_selected", G_CALLBACK(calendarWidget_day_selected), detailViewer); // SIGNAL day_selected --> callback function

    /* Main box setup */
    mainBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); // define as box
    gtk_box_pack_start(GTK_BOX(mainBox), calendarWidget, FALSE, FALSE, 3); // child: calendar
    gtk_box_pack_end(GTK_BOX(mainBox), detailViewer, FALSE, FALSE, 3); // child: detailViewer
    
    /* Main window setup */
    mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL); // define as window
    gtk_window_set_titlebar(GTK_WINDOW(mainWindow), mainWindowHeader); // set titlebar to custom header
    gtk_container_add(GTK_CONTAINER(mainWindow), mainBox); // add container (mainBox) to window
    g_signal_connect(mainWindow, "destroy", G_CALLBACK(destroy), NULL); // SIGNAL destroy --> callback function

    /* --------------------------------------------------------------------------------------------------------------------- */

    /* Create main window */
    mainWindow_create(mainWindow);

    /* GO */
    gtk_main();
}

void mainWindow_create(GtkWidget* window)
{
    gtk_widget_show_all(window);
}

void calendarWidget_create(GtkWidget* calendarWidget)
{
    /* Load ICS file */
    shell_say(PROGRESS, "Loading default iCalendar into RAM..."); // TODO only in debug mode?
    shell_handleError( Calendar_create(calendarWidgetData), NULL, 0, NULL );
    shell_handleError( ICS_load("calendar.ics", calendarWidgetData), "calendar.ics", 0, NULL );
}

void calendarWidget_markToday(GtkWidget* calendarWidget)
{
    GTimeVal sinceEpoch; // declare variable to store time in
    g_get_current_time(&sinceEpoch); // get time and store in variable
    GDate* now = g_date_new(); // define variable to store current date in
    g_date_clear(now, 1); // apparently, this is needed
    g_date_set_time_val(now, &sinceEpoch); // set value of current date

    gtk_calendar_mark_day(GTK_CALENDAR(calendarWidget), g_date_get_day(now)); // mark calculated date

    g_date_free(now);
}

void calendarWidget_day_selected(GtkWidget* calendarWidget, gpointer detailViewerAddress) // CALLBACK
{   
    /* Get selected date in our format */
    Date selectedDate = { 0 };
    gtk_calendar_get_date(GTK_CALENDAR(calendarWidget), &selectedDate.year, &selectedDate.month, &selectedDate.day);
    selectedDate.month += 1; // seems like this is required (function above returns prev month every time)

    /* Update detail viewer */
    detailViewer_update(detailViewerAddress, selectedDate);
}

void detailViewer_update(GtkWidget* detailViewer, Date d)
{
    /* Create buffer, fill buffer, set label */
    gchar buff[BUFSIZ] = { 0 }; // FIXME this does not fit any amount of events
    gchar tmpBuff[BUFSIZ] = { 0 };

    // borrowed from commandHandler.c (command_list())
    VEventNode* traveller = calendar.first->next;

    while( traveller != calendar.last && compareDate(traveller->ve.start.date, d) != SAME)
    {
        traveller = traveller->next;
    }

    // printDayHeader(d);
    int count = 1;
    int hour = -1;
    // bool isAtNewHour = true;
    while ( traveller != calendar.last && compareDate(traveller->ve.start.date, d) == SAME)
    {
        // printVEvent(traveller->ve, count);
        
        g_snprintf
            (
                buff, sizeof buff,
                "\n%s<b>From:</b> %02d/%02d/%02d %02d.%02d\n<b>To:</b> %02d/%02d/%02d %02d.%02d\n<b>Summary:</b> %s\n<b>Location:</b> %s\n<b>Description</b> %s\n<b>Priority:</b> %d\n",
                    tmpBuff,
                
                    traveller->ve.start.date.year,
                    traveller->ve.start.date.month,
                    traveller->ve.start.date.day,
                    traveller->ve.start.time.hour,
                    traveller->ve.start.time.minute,
                    
                    traveller->ve.end.date.year,
                    traveller->ve.end.date.month,
                    traveller->ve.end.date.day,
                    traveller->ve.end.time.hour,
                    traveller->ve.end.time.minute,

                    traveller->ve.summary,
                    traveller->ve.location,
                    traveller->ve.description,
                    traveller->ve.priority
            );
        
       g_snprintf(tmpBuff, sizeof tmpBuff, "%s\n", buff); 
        
        traveller = traveller->next;
    }

    gtk_label_set_markup(GTK_LABEL(detailViewer), buff);
}

void on_activate_menuItem_new(GtkWidget* widget, gpointer data)
{
    /* Widgets */
    GtkWidget* newEventWindow;
    GtkWidget* layout;
    GtkWidget* label_from;
    GtkWidget* label_to;
    GtkWidget* label_summary;
    GtkWidget* label_location;
    GtkWidget* label_description;
    GtkWidget* label_priority;
    GtkWidget* comboBox_yearFrom;
    GtkWidget* comboBox_yearTo;
    GtkWidget* comboBox_monthFrom;
    GtkWidget* comboBox_monthTo;
    GtkWidget* comboBox_dayFrom;
    GtkWidget* comboBox_dayTo;
    GtkWidget* comboBox_hourFrom;
    GtkWidget* comboBox_hourTo;
    GtkWidget* comboBox_minuteFrom;
    GtkWidget* comboBox_minuteTo;
    GtkWidget* entry_summary;
    GtkWidget* entry_location;
    GtkWidget* entry_description;
    GtkWidget* comboBox_priority;
    GtkWidget* button_confirm;
    GtkWidget* button_cancel;

    /* Label setup */
    label_from = gtk_label_new("Starting date:"); // define as labels with text
    label_to = gtk_label_new("Ending date:");
    label_summary = gtk_label_new("Summary:");
    label_location = gtk_label_new("Location:");
    label_description = gtk_label_new("Description");
    label_priority = gtk_label_new("Priority:");

    /* Combobox setup */
    comboBox_yearFrom = gtk_combo_box_text_new_with_entry(); // define as text comboboxes with entry
    comboBox_yearTo = gtk_combo_box_text_new_with_entry();
    comboBox_monthFrom = gtk_combo_box_text_new_with_entry();
    comboBox_monthTo = gtk_combo_box_text_new_with_entry();
    comboBox_dayFrom = gtk_combo_box_text_new_with_entry();
    comboBox_dayTo = gtk_combo_box_text_new_with_entry();
    comboBox_hourFrom = gtk_combo_box_text_new_with_entry();
    comboBox_hourTo = gtk_combo_box_text_new_with_entry();
    comboBox_minuteFrom = gtk_combo_box_text_new_with_entry();
    comboBox_minuteTo = gtk_combo_box_text_new_with_entry();
    comboBox_priority = gtk_combo_box_text_new_with_entry();
    
    gchar buff[5] = { 0 }; // temporary buffer
    for (int i = YEAR_MIN; i <= YEAR_MAX; ++i) // fill comboboxes via loops
    {
        g_snprintf(buff, sizeof buff, "%04d", i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_yearFrom), buff);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_yearTo), buff);
    }
    for (int i = 1; i <= 12; ++i)
    {
        g_snprintf(buff, sizeof buff, "%02d", i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_monthFrom), buff);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_monthTo), buff);
    }
    for (int i = 1; i <= 31; ++i)
    {
        g_snprintf(buff, sizeof buff, "%02d", i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_dayFrom), buff);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_dayTo), buff);
    }
    for (int i = 1; i <= 59; ++i)
    {
        g_snprintf(buff, sizeof buff, "%02d", i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_hourFrom), buff);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_minuteFrom), buff);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_hourTo), buff);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_minuteTo), buff);
    }
    for (int i = 1; i <= 10; ++i)
    {
        g_snprintf(buff, sizeof buff, "%d", i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox_priority), buff);
    }
        

    /* Entry setup */
    entry_summary = gtk_entry_new(); // define as entry fields
    entry_location = gtk_entry_new();
    entry_description = gtk_entry_new();

    /* Getting props */
    newEvent_inputData dataSources = {
        comboBox_yearFrom,
        comboBox_yearTo,
        comboBox_monthFrom,
        comboBox_monthTo,
        comboBox_dayFrom,
        comboBox_dayTo,
        comboBox_hourFrom,
        comboBox_hourTo,
        comboBox_minuteFrom,
        comboBox_minuteTo,
        entry_summary,
        entry_location,
        entry_description,
        comboBox_priority
    };
    
    /* Button setup */
    button_confirm = gtk_button_new_with_label("Confirm"); // define as buttons
    button_cancel = gtk_button_new_with_label("Cancel");
    g_signal_connect(GTK_BUTTON(button_confirm), "clicked", G_CALLBACK(newEventConfirm), &dataSources); // TODO get working FIXME
    g_signal_connect(GTK_BUTTON(button_cancel), "clicked", G_CALLBACK(closeWindow), newEventWindow);

    
    /* Grid setup */
    layout = gtk_grid_new(); // define as grid
    gtk_grid_set_row_homogeneous(GTK_GRID(layout), TRUE); // enable same cell dimensions
    gtk_grid_set_column_homogeneous(GTK_GRID(layout), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 3); // set spacing
    gtk_grid_set_column_spacing(GTK_GRID(layout), 1);

    gtk_grid_attach(layout, label_from,             0, 0, 1, 1); // attach children
    gtk_grid_attach(layout, comboBox_yearFrom,      1, 0, 1, 1);
    gtk_grid_attach(layout, comboBox_monthFrom,     2, 0, 1, 1);
    gtk_grid_attach(layout, comboBox_dayFrom,       3, 0, 1, 1);
    gtk_grid_attach(layout, comboBox_hourFrom,      4, 0, 1, 1);
    gtk_grid_attach(layout, comboBox_minuteFrom,    5, 0, 1, 1);
    
    gtk_grid_attach(layout, label_to,               0, 1, 1, 1);
    gtk_grid_attach(layout, comboBox_yearTo,        1, 1, 1, 1);
    gtk_grid_attach(layout, comboBox_monthTo,       2, 1, 1, 1);
    gtk_grid_attach(layout, comboBox_dayTo,         3, 1, 1, 1);
    gtk_grid_attach(layout, comboBox_hourTo,        4, 1, 1, 1);
    gtk_grid_attach(layout, comboBox_minuteTo,      5, 1, 1, 1);
    
    gtk_grid_attach(layout, label_summary,          0, 2, 1, 1);
    gtk_grid_attach(layout, entry_summary,          1, 2, 5, 1);

    gtk_grid_attach(layout, label_location,         0, 3, 1, 1);
    gtk_grid_attach(layout, entry_location,         1, 3, 5, 1);
    
    gtk_grid_attach(layout, label_description,      0, 4, 1, 1);
    gtk_grid_attach(layout, entry_description,      1, 4, 5, 1);
    
    gtk_grid_attach(layout, label_priority,         0, 5, 1, 1);
    gtk_grid_attach(layout, comboBox_priority,      1, 5, 1, 1);

    gtk_grid_attach(layout, button_cancel,          0, 6, 2, 1);
    gtk_grid_attach(layout, button_confirm,         4, 6, 2, 1);


    /* Window setup */
    newEventWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL); // define as window
    gtk_window_set_title (GTK_WINDOW(newEventWindow), "New event"); // set window title
    gtk_container_add(GTK_CONTAINER(newEventWindow), layout); // add container (grid) to window
    gtk_widget_show_all (newEventWindow); // show window
}

void newEventConfirm(GtkWidget* widget, gpointer data) // CALLBACK
{
    newEvent_inputData* input = data;

    char DTSTART[16];
    char DTEND[16];
    char* summary;
    char* location;
    char* description;
    char* priority;

    g_snprintf
        (
            DTSTART, sizeof DTSTART, "%s%s%sT%sd%sd00Z",
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->yearFrom)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->monthFrom)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->dayFrom)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->hourFrom)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->minuteFrom))
        );
    
    g_snprintf
        (
            DTEND, sizeof DTEND, "%s%s%sT%sd%sd00Z",
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->yearTo)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->monthTo)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->dayTo)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->hourTo)),
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->minuteTo))
        );

    summary = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->summary));
    location = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->location));
    description = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->description));
    priority = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input->description));

    char* tempArgs[] = { "create", "-b", DTSTART, "-e", DTEND, "-s", summary, "-l", location, "-d", description, "-p", priority, NULL };
    shell_handleError( command_create(tempArgs), NULL, 0, NULL );
}

void closeWindow(GtkWidget* window, gpointer data) // CALLBACK
{
    gtk_window_close(GTK_WINDOW(window));
}

void destroy(GtkWidget* widget, gpointer data) // CALLBACK
{
    gtk_main_quit();
}

