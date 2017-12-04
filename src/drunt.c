#include "structs.h"
#include "dbHandler.h"
#include "errorHandler.h"
#include "helper.h"
#include "interactiveHandler.h"
#include "GUIHandler.h"

#include <stdlib.h>
#include <string.h>

////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

Calendar calendar; // TODO more calendars simultaneously?
char* file; // store currently opened calendar globally

////////////
/// MAIN ///
////////////

int main(int argc, char** argv)
{
    char* defaultMode = "INTERACTIVE"; // TODO config
    char* defaultFile = "calendar.ics";
    
    /* Initialise calendar */
    Calendar_create(&calendar);

    /* Set default file */
    char* tmps = malloc( sizeof(char) * (strlen(defaultFile) + 1) );
    if (!tmps)
    {
        die("Failed to allocate memory for default filename (%s)", defaultFile);
    }
    strcpy(tmps, defaultFile);
    file = tmps;
    
    /* Startup actions */
    shell_say(PROGRESS, "Loading default iCalendar (%s)...", defaultFile);
    if (!shell_handleError(ICS_load(defaultFile, &calendar), defaultFile, 0, NULL))
    {
        shell_say(ERROR, "Failed to load default iCalendar file. You are supposed to manually load a calendar file with 'load'. Please check 'help load' for details");
        file = NULL;
    }

    /* ------------------------------------------------------------------------------------------------------------ */

    /* Process arguments */
    if (argc == 1)
    {
        shell_say(NEUTRAL, "No arguments passed, launching %s mode by default", defaultMode);
        if (strcmp(defaultMode, "INTERACTIVE") == 0)
        {
            shell();
        }
        else
        {
            GUI(argc, argv);
        }
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-i") == 0)
        {
            shell();
        }
        else// if (strcmp(argv[1], "-g") == 0)
        {
            GUI(argc, argv);
        }
    }
    else
    {
        shell_say(ERROR, "Something went wrong");
    }
    
    // destroy calendar
    Calendar_destroy(&calendar);

    putchar('\n');
    return 0;
}

