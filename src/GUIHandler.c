#include "GUIHandler.h"

void GUI(int argc, char** argv)
{
    /* Declaring non widget things */
    GtkTextBuffer* buff_details;

    /* Declaring GTK widgets */
    GtkWidget* mainWindow;
    GtkWidget* mainWindowHeader;
    GtkWidget* menuPopover;
    GtkWidget* menuButton;
    GtkWidget* mainMenu;
    GtkWidget* menuItem_quit;
    GtkWidget* mainBox;
    GtkWidget* calendar;
    GtkWidget* detailViewer;

    /* Initialise GTK */
    gtk_init(&argc, &argv);


    /* Set up GTK widgets */
    mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(mainWindow, "destroy", G_CALLBACK(destroy), NULL);

    mainBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); // args: orientation, spacing
    
    mainWindowHeader = gtk_header_bar_new();

    menuPopover = gtk_popover_new(mainMenu);

    menuButton = gtk_menu_button_new();

    mainMenu = gtk_menu_new();

    menuItem_quit = gtk_menu_item_new_with_label("Quit");

    calendar = gtk_calendar_new();

    detailViewer = gtk_text_view_new();

    
    /* Setting up non widget things */
    // text buffer
    buff_details = gtk_text_view_get_buffer(GTK_TEXT_VIEW(detailViewer));
    
    
    /* Set widget properties */
    // header bar
    gtk_header_bar_set_title(GTK_HEADER_BAR(mainWindowHeader), "drunt");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(mainWindowHeader), "an InfoC project by bertalanp99");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(mainWindowHeader), TRUE);
    gtk_header_bar_set_decoration_layout(GTK_HEADER_BAR(mainWindowHeader), "menu:close");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(mainWindowHeader), menuButton);

    // menu
    gtk_menu_attach(GTK_MENU(mainMenu), menuItem_quit, 0, 5, 0, 5);

    // menu button
    gtk_menu_button_set_popover(GTK_MENU_BUTTON(menuButton), menuPopover);

    // main box
    gtk_box_pack_start(GTK_BOX(mainBox), calendar, FALSE, FALSE, 3); // args: box, child, expand, fill, padding
    gtk_box_pack_end(GTK_BOX(mainBox), detailViewer, FALSE, FALSE, 3); // args: box, child, expand, fill, padding
    
    // main window
    gtk_window_set_titlebar(GTK_WINDOW(mainWindow), mainWindowHeader);


    /* Set other properties */
    // text buffer
    gtk_text_buffer_set_text(buff_details, "Hello, this is some text", -1); // -1 means string is NULL terminated
    
    /* Put widgets into containers */
    gtk_container_add(GTK_CONTAINER(mainWindow), mainBox);
    gtk_widget_show_all(mainWindow);

    /* Call GTK's main */
    gtk_main();

    return;
}

void destroy(GtkWidget* widget, gpointer data)
{
    gtk_main_quit();
}
