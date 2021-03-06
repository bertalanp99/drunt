#include "commandHandler.h"
#include "drunt.h"

#include <time.h>
#include <stdbool.h>

#define DEFAULT_YEARLISTWIDTH 3
#define MAX_LINELENGTH 600

/////////////////////////
/// GLOBAL VARIABLES ///
////////////////////////

/*char* commands[8] = {
    "help",
    "exit",
    "load",
    "create",
    "modify",
    "delete",
    "list",
    "find"
};*/

int (*runCommand[])(char**) = {
    &command_help,
    &command_exit,
    &command_load,
    &command_create,
    &command_modify,
    &command_delete,
    &command_list,
    &command_find
};

/////////////////
/// FUNCTIONS ///
/////////////////

int command_help(char** args) // there isn't really a way to shorten this, sorry
{   
    if (args[1] == NULL)
    {
        shell_say(NEUTRAL, "The following commands are at your disposal:");
    
        for (int i = 0; i < numberOfCommands; ++i)
        {
            shell_say(STATUS, "\t%s", commands[i]);
        }

        shell_say(NEUTRAL, "Type 'help <command>' to know more about a certain command");
    }
    else if (args[2] == NULL)
    {
        if (strcmp(args[1], "help") == 0)
        {
            shell_say(NEUTRAL, "\tSyntax:\thelp [commnand]");
            shell_say(NEUTRAL, "\tType this command without any arguments to get a list of available commands.");
            shell_say(NEUTRAL, "\tType 'help <command>' to get to know more about <command>. Note that you've just done that");
            return 1;
        }
        else if (strcmp(args[1], "exit") == 0)
        {
            shell_say(NEUTRAL, "\tSyntax:\texit [options]");
            shell_say(NEUTRAL, "\tType this command to exit interactive mode.");
            shell_say(NEUTRAL, "\tDrunt overwrites your last opened iCalendar file upon exiting by default. To change this behaviur, please type 'exit --discard' or 'exit -d'");
            shell_say(NEUTRAL, "\t\tIn this case, you'll be prompted to specify file to write to. You can also skip saving, effectively throwing away your changes");
        }
        else if (strcmp(args[1], "load") == 0)
        {
            shell_say(NEUTRAL, "\tSyntax:\tload <path>");
            shell_say(NEUTRAL, "\tLoad (open) iCalendar/vCalendar files with this option.");
        }
        else if (strcmp(args[1], "create") == 0)
        {
            shell_say(NEUTRAL, "\tSyntax:\tcreate [details]");
            shell_say(NEUTRAL, "\tWith this function, you can add a new event to your calendar.");
            shell_say(NEUTRAL, "\tPossible options when creating event:");
            putchar('\n');
            shell_say(STATUS, "\t\t--begin | -b <timestamp>");
            shell_say(NEUTRAL, "\t\t\tSpecify starting date of new event. Timestamp format is iCalendar/vCalendar standard:");
            shell_say(NEUTRAL, "\t\t\t\tYYYYMMDDTHHMMSSZ\tFor example: 2018/02/13 12.00 is 20180213T120000Z");
            shell_say(NEUTRAL, "\t\t\tPlease note that seconds shall be ignored and you MUST add the Z at the end (as drunt only works with ZULU timestamps)");
            putchar('\n');
            shell_say(STATUS, "\t\t--end | -e <timestamp>");
            shell_say(NEUTRAL, "\t\t\tSpecify ending date of new event. See above for formatting.");
            putchar('\n');
            shell_say(STATUS, "\t\t--summary | -s <text>");
            shell_say(NEUTRAL, "\t\t\tSpecify short summary of event. Maximum 600 characters. This acts like a 'title.'");
            shell_say(NEUTRAL, "\t\t\tPlease note that if you invoke this argument, you MUST enter something. Otherwise you have the option to leave this blank (which is stronly disencouraged though).");
            putchar('\n');
            shell_say(STATUS, "\t\t--location | -l <text>");
            shell_say(NEUTRAL, "\t\t\tSpecify location where new event takes place. Works the same way as 'summary,' check above!");
            putchar('\n');
            shell_say(STATUS, "\t\t--description | -d <text>");
            shell_say(NEUTRAL, "\t\t\tGive new event a description. Works the same way as 'summary' and 'location,' check above!");
            putchar('\n');
            shell_say(STATUS, "\t\t--priority | -p <text>");
            shell_say(NEUTRAL, "\t\t\tSet prioriy of new event. Priority ranges from 0 to 9. 0 means most important and 9 means least important");
        }
        else if (strcmp(args[1], "modify") == 0)
        {
            shell_say(NEUTRAL, "\tSyntax:\tmodify [date]");
            shell_say(NEUTRAL, "\tAs of yet, you can only specify events by their starting date. You can enter date as an argument, but if you don't, you'll be prompted for it anyway");
            shell_say(NEUTRAL, "\t\tPlease note that if you do choose to enter the date, you have to follow iCalendar/vCalendar timestamp format. Check 'help create' for detailed instructions, or read the manual");
        }
        else if (strcmp(args[1], "delete") == 0)
        {
            shell_say(NEUTRAL, "\tSyntax:\tdelete [date]");
            shell_say(NEUTRAL, "\tThis command lets you delete events. As of yet, events can only be specified by their starting date. You can either enter date as an argument or give no argument and let interactive mode prompt you.");
            shell_say(NEUTRAL, "\t\tPlease note that if you do choose to enter the date, you have to follow iCalendar/vCalendar timestamp format. Check 'help create' for detailed instructions, or read the manual");
        }
        else if (strcmp(args[1], "list") == 0)
        {
            shell_say(NEUTRAL, "\tSyntax:\tlist <year | month | day | agenda> [year | month | day | period]");
            shell_say(NEUTRAL, "\tThis is probably the command you are going to use the most. It is used to list events.");
            shell_say(NEUTRAL, "\tYou need to specify what to list: enter (as an argument) explicitly 'year', 'month', 'day', or 'agenda'");
            shell_say(NEUTRAL, "\t\tAfter 'year', 'month', and 'day', you can specify which year, month or day. If you omit this, drunt will prompt you.");
            shell_say(NEUTRAL, "\t\t\tPlease note though, that in case you want to list 'year,' you have to enter it like YYYY. In case you want to list a 'month,' you have to enter YYYY/MM and finnaly, in case you want to list events of a 'day,' enter YYYY/MM/DD!");
            shell_say(NEUTRAL, "\tThe option 'agenda' lists events upcoming in the next 7 days by default, but you can also specify 'period' by entering the number of days you want to list (from today of course)");
        }
        else
        {
            shell_say(ERROR, "Cannot show help for command '%s'. Command does not exist.", args[1]);
            return 1;
        }
    }
    else
    {
        shell_say(ERROR, "Too many arguments passed to 'help'");
    }

    return 1;
}

int command_exit(char** args)
{   
    if (args[1] == NULL)
    {
        shell_say(PROGRESS, "Saving you calendar into file '%s'...", file);
        if (file != NULL)
        {
            if ( shell_handleError( ICS_write(file, &calendar, OVERWRITE), file, 0, NULL ) )
            {
                shell_say(DONE, "Successfully saved calendar! Farewell; take care! :-{");
                return 0; // EXIT
            }
            else
            {
                shell_say(ERROR, "Failed to safely exit. Please use option '-d' to forcefully exit drunt and discard any changes");
                return 1;
            }
        }
        else
        {
            shell_say(STATUS, "For some odd reason, file has been set to a NULL pointer. Where would you like to save your calendar?");
            
            if (!shell_handleError( shell_readString("Path", &file, MAX_PATHLENGTH) , file, 0, NULL))
            {
                shell_say(ERROR, "Failed to read path to savefile");
                return 1;
            }
            else
            {
                shell_say(PROGRESS, "Saving to file %s ...", file);
            }

            if (!shell_handleError( ICS_write(file, &calendar, OVERWRITE), file, 0, NULL ))
            {
                shell_say(ERROR, "Failed to safely exit. Please use option '-d' to forcefully exit drunt and discard any changes");
                return 1;
            }

            return 0; // EXIT
        }
    }
    else if ( ((strcmp(args[1], "--discard") == 0) || (strcmp(args[1], "-d") == 0)) && args[2] == NULL )
    {
        shell_say(WARNING, "About to exit drunt without saving changes! Are you sure?");
        if (!shell_promptYN("Sure you want to exit?"))
        {
            shell_say(STATUS, "Cancelled exit");
            return 1;
        }
        else
        {
            shell_say(PROGRESS, "Exiting drunt and ditching changes...");
            return 0; // EXIT
        }
    }
    else
    {
        shell_say(ERROR, "Too many arguments or argument '%s' is invalid", args[1]);
        return 1;
    }
}

int command_load(char** args)
{
    /* Check if there's an already loaded file */
    if (file)
    {
        shell_say(PROGRESS, "Saving currently open iCalendar '%s' to disk...", file);
        if (!shell_handleError( ICS_write(file, &calendar, OVERWRITE), file, 0, NULL ))
        {
            if ( !shell_promptYN("Failed to save. Close without saving?") )
            {
                shell_say(STATUS, "Cancelled operation");
                return 1;
            }
        }
        else
        {
            shell_say(STATUS, "Successfully saved open calendar to file '%s'. Moving on to loading other file", file);
        }
    }

    char* path; // path to file to load

    /* NO ARGUMENTS PASSED --> attempt to query user */
    if (args[1] == NULL)
    {
        char* path;
        if (!shell_handleError( shell_readString("Path to file", &path, MAX_PATHLENGTH), path, 0, NULL ))
        {
            return 1;
        }
    }
    /* ONE ARGUMENT PASSED --> use path passed as argument */
    else if (args[2] == NULL)
    {
        path = args[1];
    }
    /* TOO MANY ARGUMENTS PASSED --> cancel */
    else
    {
        shell_say(ERROR, "Too many arguments passed!");
        return 1;
    }
    
    /* Reinit calendar */
    Calendar_destroy(&calendar);
    Calendar_create(&calendar);

    /* Load ICS from file */
    shell_say(PROGRESS, "Loading iCalendar file %s into RAM...", path);
    if (!shell_handleError( ICS_load(path, &calendar), path, 0, NULL ))
    {
        shell_say(ERROR, "An error has occured while loading file. Will attempt to reload previous file (%s) now", file);
        if (!shell_handleError( ICS_load(file, &calendar), file, 0, NULL))
        {
            shell_say(ERROR, "An error has occrured while reloading previus file. Will set file path to NULL. Please note that you are basically in demo mode until you load a file");
            file = NULL;
            return 1;
        }
        else
        {
            shell_say(STATUS, "Successfully reloaded file %s", file);
        }
    }

    shell_say(DONE, "Succesfully loaded file");
    
    /* Set file path to new path */
    char* tmp = realloc(file, sizeof(char) * (strlen(path) + 1) );
    if (tmp == NULL)
    {
        shell_say(ERROR, "Failed to allocate memory for new filename! Will set file path to NULL to prevent overwriting files by accident. Please note that you are basically in demo mode from now on, until you load a file");
        file = NULL;
        return 1;
    }
    file = tmp;
    strcpy(file, path);

    free(path);
    return 1;
}

int command_create(char** args)
{
    /* Check file path */
    if (file == NULL)
    {
        shell_say(WARNING, "It seems like no file has been opened yet: you are editing an empty calendar.");
        if (!shell_promptYN("Continue?"))
        {
            shell_say(STATUS, "Cancelled creating event");
            return 1;
        }
    }
    
    /* Create empty VEvent to fill */
    VEvent ve = {
        .start = {
            .date = { .year = 0, .month = 0, .day = 0 },
            .time = { .hour = 0, .minute = 0 } },
        .end = {
            .date = { .year = 0, .month = 0, .day = 0 },
            .time = { .hour = 0, .minute = 0 } },
        .summary = NULL,
        .location = NULL,
        .description = NULL,
        .priority = 10
    };
    
    /* NO ARGUMENTS PASSED --> Query user for each detail */
    if (args[1] == NULL)
    {   // TODO better method (hotfix: checking for priority validity here)
        if
            (
                !shell_handleError( shell_readTimestamp(&ve.start, DTSTART),                                            NULL, 0, NULL ) ||
                !shell_handleError( shell_readTimestamp(&ve.end, DTEND),                                                NULL, 0, NULL ) ||
                !shell_handleError( shell_readString("Short summary", &ve.summary, MAX_LINELENGTH),                     NULL, 0, NULL ) ||
                !shell_handleError( shell_readString("Location", &ve.location, MAX_LINELENGTH),                         NULL, 0, NULL ) ||
                !shell_handleError( shell_readString("Description", &ve.description, MAX_LINELENGTH),                   NULL, 0, NULL ) ||
                !shell_handleError( shell_readNumWithCriterion("Priority", &ve.priority, isValidPriority), NULL, 0, NULL )
            )
        {
            if (!isValidPriority(ve.priority))
            {
                shell_say(ERROR, "Invalid priority entered: '%d'. Please check 'help create' for details");
            }
            return 1;
        }
    }
    /* AT LEAST ONE ARGUMENT PASSED */
    else
    {
        // temporary structure
        typedef struct {
            char* opt;
            char* opt_short;
            ICSDataType type;
            bool flag;
        } Option;
        
        // temporary array of option structure
        Option options[] = {
            { .opt = "--begin",         .opt_short = "-b", .type = TIMESTAMP,   .flag = false },
            { .opt = "--end",           .opt_short = "-e", .type = TIMESTAMP,   .flag = false },
            { .opt = "--summary",       .opt_short = "-s", .type = STRING,      .flag = false },
            { .opt = "--location",      .opt_short = "-l", .type = STRING,      .flag = false },
            { .opt = "--description",   .opt_short = "-d", .type = STRING,      .flag = false },
            { .opt = "--priority",      .opt_short = "-p", .type = NUMBER,      .flag = false }
        };
        size_t numberOfOptions = ( sizeof options / sizeof(Option) );
        
        /* Iterate through all arguments */
        for (int i = 1; args[i] != NULL; ++i)
        {
            bool wasValidOption = true;

            // for each argument, iterate through options and compare
            for (int j = 0; j < numberOfOptions; ++j)
            {
                if ( (strcmp(args[i], options[j].opt) == 0) || (strcmp(args[i], options[j].opt_short) == 0) )
                {
                    /* To prevent SEGFAULT's, check whether there's actually something after the option */
                    if (args[i+1] == NULL)
                    {
                        shell_say(ERROR, "Option '%s' missing argument. Please check 'help create' for more information", args[i]);
                        return 1;
                    }

                    switch (options[j].type)
                    {
                        case TIMESTAMP:
                        {
                            DateTime* target;
                            if (strcmp(options[j].opt, "--begin") == 0)
                            {
                                target = &ve.start;
                            }
                            else if (strcmp(options[j].opt, "--end") == 0)
                            {
                                target = &ve.end;
                            }

                            if (!shell_handleError( ICSTimeStampReader(args[i+1], target), NULL, 0, args[i+1] ))
                            {
                                return 1;
                            }
                            break;
                        }

                        case STRING:
                        {
                            /* Find string boundaries (apostrophes) */
                            int numberOfCharacters = 0;
                            int firstWord = i + 1;
                            if (args[firstWord][0] != '\'')
                            {
                                shell_say(ERROR, "Strings need to be bounded by apostrophes! See 'help create' for more information");
                                return 1;
                            }
                            else
                            {
                                bool hasClosingApostrophe = false;
                                for (i = firstWord; args[i] != NULL; ++i)
                                {
                                    if (args[i][ strlen(args[i]) - 1 ] == '\'')
                                    {
                                        hasClosingApostrophe = true;
                                        break;
                                    }
                                    numberOfCharacters += (strlen(args[i]) + 1); // +1 --> also count spaces
                                }

                                if (!hasClosingApostrophe)
                                {
                                    shell_say(ERROR, "String missing closing apostrophe! Strings have to be bounded by two apostrophes, like this: 'my text'");
                                    return 1;
                                }
                            }

                            /* Attempt to allocate memory for string */
                            char* tmp = malloc( sizeof(char) * (numberOfCharacters + 1) );
                            if (tmp == NULL)
                            {
                                shell_say(ERROR, "Failed to allocate memory for event %s!", options[j].opt + 2);
                                return 1;
                            }
                            
                            char** target;
                            if (strcmp(options[j].opt, "--summary") == 0)
                            {
                                target = &ve.summary;
                            }
                            else if (strcmp(options[j].opt, "--location") == 0)
                            {
                                target = &ve.location;
                            }
                            else if (strcmp(options[j].opt, "--description") == 0)
                            {
                                target = &ve.description;
                            }
                            
                            *target = tmp;
                            strcat(*target, args[firstWord] + 1); // +1 --> get rid of opening apostrophe
                            strcat(*target, " "); // add space after first word
                            for (int k = firstWord + 1; k < i; k++)
                            {
                                strcat(*target, args[k]);
                                strcat(*target, " "); // add space after words
                            }
                            (*target)[strlen(*target) - 2] = '\0'; // -2 --> get rid of closing apostrophe (-1) and unnecessary space (-1)
                            
                            break;
                        }

                        case NUMBER:
                        { // note: drunt only handles priorities as numbers as of yet
                            if (myatoi(args[i+1], &ve.priority) == 0)
                            {
                                shell_say(ERROR, "Failed to convert event priority '%s' to a number!", args[i+1]);
                                return 1;
                            }

                            if (!isValidPriority(ve.priority))
                            {
                                shell_say(ERROR, "Invalid priority value '%d'! Please check 'help' for more information", ve.priority);
                                return 1;
                            }
                            break;
                        }
                    }
                    options[j].flag = true;
                    break;
                }
                else if (j == numberOfOptions)
                {
                    wasValidOption = false;
                }
            }

            if (!wasValidOption)
            {   
                shell_say(ERROR, "Invalid option '%s'. See 'help create' for a list of available options", args[i]);
                return 1;
            }
        }
        
        /* Prompt for omitted details */
        for (int i = 0; i < numberOfOptions; ++i)
        {
            if (!options[i].flag)
            {
                switch (options[i].type)
                {
                    case TIMESTAMP:
                    {
                        DateTime* target;
                        TimestampType tt;
                        if (strcmp(options[i].opt, "--begin") == 0)
                        {
                            target = &ve.start;
                            tt = DTSTART;
                        }
                        else if (strcmp(options[i].opt, "--end") == 0)
                        {
                            target = &ve.end;
                            tt = DTEND;
                        }
                        
                        if (!shell_handleError( shell_readTimestamp(target, tt), NULL, 0, NULL ))
                        {
                            return 1;
                        }
                        break;
                    }

                    case STRING:
                    {
                        char** target; 
                        if (strcmp(options[i].opt, "--summary") == 0)
                        {
                            target = &ve.summary;
                            shell_say(PROMPT, "Short summary:");
                        }
                        else if (strcmp(options[i].opt, "--location") == 0)
                        {
                            target = &ve.location;
                            shell_say(PROMPT, "Location:");
                        }
                        else if (strcmp(options[i].opt, "--description") == 0)
                        {
                            target = &ve.description;
                            shell_say(PROMPT, "Description:");
                        }
                        
                        if (!shell_handleError( shell_readString(NULL, target, MAX_LINELENGTH), NULL, 0, NULL ))
                        {
                            return 1;
                        }
                        break;
                    }

                    case NUMBER:
                    { // note: drunt only handles priorities as numbers as of yet
                        int* target = &ve.priority;

                        //shell_say(PROMPT, "Event priority:");
                        bool wasValid = true;
                        do
                        {
                            if (!wasValid)
                            {
                                shell_say(ERROR, "Priority entered was invalid. Please try again. See 'help' for details");
                            }

                            if (!shell_handleError( shell_readNum("Priority:", target), NULL, 0, NULL ))
                            {
                                return 1;
                            }

                            wasValid = isValidPriority(*target);
                        }
                        while (!wasValid);
                        
                        break;
                    }
                }
            }
        }
    }

    /* Confirm event with user */
    shell_say(NEUTRAL, "Event to be created:");
    printVEvent(ve, 0);
    
    if (!shell_promptYN("Confirm?"))
    {
        shell_say(STATUS, "No event created");
        return 1;
    }
    else
    {
        shell_say(PROGRESS, "Adding new event to calendar...");
        Calendar_addVEvent(&calendar, ve);
        shell_say(DONE, "Successfully added new event!");
        return 1;
    }
}

int command_modify(char** args)
{
    assert(args != NULL);
    
    if ((args[1] == NULL) || (args[2] == NULL) || (args[3] == NULL))
    {
        shell_say(ERROR, "Insufficient arguments passed to 'modify'. Please check 'help modify' for more information");
        return 1;
    }
    else
    {
        if (args[4] != NULL)
        {
            shell_say(ERROR, "Too many arguments passed to 'modify'. Please check 'help modify' for more information");
            return 1;
        }
        
        /* Check number of VEvents on specified day */
        DateTime from;
        DateTime to;
        if
            (
                !shell_handleError(parseDateTime(&from, args[1], args[2], args[3], "00", "00"), NULL, 0, NULL) ||
                !shell_handleError(parseDateTime(&to, args[1], args[2], args[3], "23", "59"), NULL, 0, NULL)
            )
        {
            return 1;
        }       

        int count = countVEvents(&calendar, from, to);
        int chosenOne = -1; // initialise to impossible value

        if (count == 0)
        {
            shell_say(STATUS, "No events found on day specified: nothing to delete");
        }
        else if (count == 1)
        {
            char* tempArgs[] = { "list", "day", args[1], args[2], args[3], NULL };
            command_list(tempArgs);
            shell_say(STATUS, "Only one event found on day specified. You can see it above.");
            if (!shell_promptYN("Would you like to modify this event?"))
            {
                shell_say(STATUS, "Cancelled operation");
                return 1;
            }
            chosenOne = 1;
        }
        else
        {
            char* tempArgs[] = { "list", "day", args[1], args[2], args[3], NULL };
            command_list(tempArgs);
            shell_say(STATUS, "Multiple events found on day specified. They are printed above with numbers as indices");
            if (!shell_handleError(shell_readNum("Please enter index representing event to modify", &chosenOne), NULL, 0, 0))
            {
                return 1;
            }
            
            if (chosenOne <= 0 || chosenOne > count)
            {
                shell_say(ERROR, "The number you have entered is either less than 0, equal to 0 or more than events on day specified. Cancelling operation");
                return 1;
            }
        }

        /* There are possibly better ways to do this, but the most robust and simple solution is to
         * temporarily get the event structure, modify it, put it into the calendar as the new event
         * and delete the old node from the calendar (if it really is different, Calendar_deleteVEvent() function
         * will delete the appropriate entry) */
        
        /* Traverse calendar, find VEvent */
        VEventNode* traveller = calendar.first->next;
        while (traveller != calendar.last && compareDateTime(traveller->ve.start, from) == BEFORE)
        {
            traveller = traveller->next;
        }
        for (int i = 1; i < chosenOne; ++i)
        {
            traveller = traveller->next;
        }
        
        VEvent old = traveller->ve;
        VEvent new = old;
        bool finished = false;

        char* menuOptions[] = {
            "(1) Begin (starting date)",
            "(2) End (ending date)",
            "(3) Summary",
            "(4) Location",
            "(5) Description",
            "(6) Priority"
        };
        size_t nOfMenuOptions = (sizeof menuOptions / sizeof(char*));
       
        do
        {
            shell_say(NEUTRAL, "Available options:");
            for (int i = 0; i < nOfMenuOptions; ++i)
            {
                shell_say(NEUTRAL, "\t%s", menuOptions[i]);
            }
            
            int choice;
            if (!shell_handleError( shell_readNumWithCriterion("Please enter your choice", &choice, isValidModifyOption), NULL, 0, NULL))
            {
                return 1;
            }

            switch (choice) // TODO find a solution not so primitive
            {
                case 1:
                {
                    if (!shell_handleError( shell_readTimestamp(&new.start, DTSTART), NULL, 0, NULL ))
                    {
                        return 1;
                    }
                    break;
                }

                case 2:
                {
                    if (!shell_handleError( shell_readTimestamp(&new.end, DTEND), NULL, 0, NULL ))
                    {
                        return 1;
                    }
                    break;
                }

                case 3:
                {
                    if (!shell_handleError( shell_readString("New summary", &new.summary, MAX_LINELENGTH), NULL, 0, NULL))
                    {
                        return 1;
                    }
                    break;
                }
                
                case 4:
                {
                    if (!shell_handleError( shell_readString("New location", &new.location, MAX_LINELENGTH), NULL, 0, NULL))
                    {
                        return 1;
                    }
                    break;
                }
                
                case 5:
                {
                    if (!shell_handleError( shell_readString("New description", &new.description, MAX_LINELENGTH), NULL, 0, NULL))
                    {
                        return 1;
                    }
                    break;
                }
                
                case 6:
                {
                    if (!shell_handleError( shell_readNumWithCriterion("New priority", &new.priority, isValidPriority), NULL, 0, NULL))
                    {
                        return 1;
                    }
                    break;
                }

                default:
                {
                    shell_say(ERROR, "Something went wrong (this may be serious, report this phenomenon!)");
                    return 1;
                }
            }

            finished = shell_promptYN("Are you finished with editing this event?");
        }
        while (!finished);

        shell_say(NEUTRAL, "Please check if you want to keep these modifications!");
        shell_say(STATUS, "ORIGINAL EVENT BELOW:");
        printVEvent(old, 0);
        shell_say(STATUS, "NEW EVENT BELOW:");
        printVEvent(new, 0);

        bool accept = shell_promptYN("Confirm these changes and overwrite?");
        if (!accept)
        {
            shell_say(STATUS, "Rolling back changes...");
            return 1;
        }

        if (compareVEvent(new, old))
        {
            return 1;
        }

        bool createrc;
        bool deleterc;
        bool undorc;
        deleterc = shell_handleError( Calendar_deleteVEvent(&calendar, old), NULL, 0, NULL );
        if (deleterc)
        {
            createrc = shell_handleError( Calendar_addVEvent(&calendar, new), NULL, 0, NULL );
        }

        if (!deleterc)
        {
            shell_say(ERROR, "Failed to overwrite event. Nothing happened");
            return 1;
        }
        else
        {
            if (!createrc)
            {
                shell_say(ERROR, "Successfully removed old event, but failed to add modified one.");
                shell_say(STATUS, "Attempting to add original event again");
                undorc = shell_handleError( Calendar_addVEvent(&calendar, old), NULL, 0, NULL );
                if (!undorc)
                {
                    shell_say(ERROR, "Failed to add original event. Sorry, but it is gone and so is your modified one.");
                    return 1;
                }
                else
                {
                    shell_say(STATUS, "At least original event could have been restored. Essentially, nothing happened");
                    return 1;
                }
            }
            else
            {
                shell_say(DONE, "Successfully overwritten event!");
                return 1;
            }
        }
    } 

}

int command_delete(char** args)
{
    assert(args != NULL);
    
    if ((args[1] == NULL) || (args[2] == NULL) || (args[3] == NULL))
    {
        shell_say(ERROR, "Insufficient arguments passed to 'delete'. Please check 'help delete' for more information");
        return 1;
    }
    else
    {
        if (args[4] != NULL)
        {
            shell_say(ERROR, "Too many arguments passed to 'delete'. Please check 'help delete' for more information");
            return 1;
        }
        
        /* Check number of VEvents on specified day */
        DateTime from;
        DateTime to;
        if
            (
                !shell_handleError(parseDateTime(&from, args[1], args[2], args[3], "00", "00"), NULL, 0, NULL) ||
                !shell_handleError(parseDateTime(&to, args[1], args[2], args[3], "23", "59"), NULL, 0, NULL)
            )
        {
            return 1;
        }       

        int count = countVEvents(&calendar, from, to);
        int chosenOne = -1; // initialise to impossible value

        if (count == 0)
        {
            shell_say(STATUS, "No events found on day specified: nothing to delete");
        }
        else if (count == 1)
        {
            char* tempArgs[] = { "list", "day", args[1], args[2], args[3], NULL };
            command_list(tempArgs);
            shell_say(STATUS, "Only one event found on day specified. You can see it above.");
            if (!shell_promptYN("Sure you want to delete that event?"))
            {
                shell_say(STATUS, "Cancelled deleting event");
                return 1;
            }
            chosenOne = 1;
        }
        else
        {
            char* tempArgs[] = { "list", "day", args[1], args[2], args[3], NULL };
            command_list(tempArgs);
            shell_say(STATUS, "Multiple events found on day specified. They are printed above with numbers as indices");
            if (!shell_handleError(shell_readNum("Please enter index representing event to delete", &chosenOne), NULL, 0, 0))
            {
                return 1;
            }
            
            if (chosenOne <= 0 || chosenOne > count)
            {
                shell_say(ERROR, "The number you have entered is either less than 0, equal to 0 or more than events on day specified. Cancelling operation");
                return 1;
            }
        }
        
        shell_say(PROGRESS, "Deleting event...");

        /* Traverse calendar, find VEvent and make it suffer ;) (sorry, I'm getting a bit annoyed) */
        VEventNode* traveller = calendar.first->next;
        while (traveller != calendar.last && compareDateTime(traveller->ve.start, from) == BEFORE)
        {
            traveller = traveller->next;
        }
        for (int i = 1; i < chosenOne; ++i)
        {
            traveller = traveller->next;
        }
        if (!shell_handleError(Calendar_deleteVEvent(&calendar, traveller->ve), NULL, 0, NULL))
        {
            return 1;
        }
        
        shell_say(SUCCESS, "Successfully deleted event! (Just so you know, there's no way to bring it back)");
        return 1;
    } 
}

int command_list(char** args)
{
    /* NO ARGUMENTS PASSED --> do nothing */
    if (args[1] == NULL)
    {
        shell_say(ERROR, "Command 'list' needs at least one argument! Check 'help list' for more information!");
        return 1;
    }
    
    /* ARGUMENT#1 == year --> check other args */
    if ( (strcmp(args[1], "year") == 0))
    {
        if (args[2] == NULL)
        {
            shell_say(ERROR, "Insufficient arguments passed to 'list year'. Pleas check 'help list' for more information");
            return 1;
        }    

        DateTime year;
        int layoutWidth = DEFAULT_YEARLISTWIDTH;

        if ( (strcmp(args[2], "--width") == 0) || (strcmp(args[2], "-w") == 0) )
        {
            if (args[3] == NULL)
            {
                shell_say(ERROR, "Width option has been passed but didn't get argument. Check 'help list' for more information");
                return 1;
            }

            if (myatoi(args[3], &layoutWidth) == 0)
            {
                shell_say(ERROR, "Failed to convert requested layout width '%s' to an integer", args[3]);
                return 1;
            }
            
            if (layoutWidth < 1 || layoutWidth > 4)
            {
                shell_say(ERROR, "Invalid year listing width specified '%d'. Valid value range: 1 -- 4", layoutWidth);
                return 1;
            }

            shell_say(STATUS, "Will print using width %d", layoutWidth);
            
            if (args[4] == NULL)
            {
                shell_say(ERROR, "Insufficient arguments to 'list year'. Please check 'help list' for more information");
                return 1;
            }

            if ( !shell_handleError(parseDateTime(&year, args[4], "01", "01", "00", "00"), NULL, 0, NULL) )
            {
                return 1;
            }
        }
        else if ( !shell_handleError(parseDateTime(&year, args[2], "01", "01", "00", "00"), NULL, 0, NULL) )
        {
            return 1;
        }
        
        shell_say(PROGRESS, "Listing year %04d below...", year.date.year);
        printYear(year.date, layoutWidth); 

        return 1;
    }
    /* ARGUMENT#1 == month && ARGUMENT#2 AND ARGUMENT#3 PASSED --> list by month ARGUMENT#3 of year ARGUMENT#2 */
    else if ( (strcmp(args[1], "month") == 0) && (args[4] == NULL) )
    {
        if (args[2] == NULL || args[3] == NULL)
        {
            shell_say(ERROR, "Insufficient arguments passed to 'list month'. Pleas check 'help list' for more information");
            return 1;
        }

        /* Process arguments */
        DateTime month;
        if ( !shell_handleError(parseDateTime(&month, args[2], args[3], "01", "00", "00"), NULL, 0, NULL) )
        {
            return 1;
        }

        /* List by month */
        shell_say(PROGRESS, "Listing month %02d of year %04d below...", month.date.month, month.date.year);
        printMonth(month.date, &calendar);
        
        return 1;
    }
    /* ARGUMENT#1 == day && ARGUMENT#2 AND ARGUMENT#3 AND ARGUMENT#4 PASSED --> list by day ARGUMENT#4 of month ARGUMENT#3 or year ARGUMENT#2 */
    else if ( (strcmp(args[1], "day") == 0) && (args[5] == NULL) )
    {
        if (args[2] == NULL || args[3] == NULL || args[3] == NULL)
        {
            shell_say(ERROR, "Insufficient arguments passed to 'list day'. Please check 'help list' for more information");
            return 1;
        }

        /* Process arguments */
        DateTime day;
        if ( !shell_handleError(parseDateTime(&day, args[2], args[3], args[4], "00", "00"), NULL, 0, NULL) )
        {
            return 1;
        }

        /* Traverse calendar until day passed as argument is reached*/
        VEventNode* traveller = calendar.first->next;
        while( traveller != calendar.last && compareDate(traveller->ve.start.date, day.date) != SAME)
        {
            traveller = traveller->next;
        }

        /* Keep traversing, but also print out events and hour headers */
        printDayHeader(day.date);
        int count = 1;
        int hour = -1; // initialise to impossible value
        bool isAtNewHour = true;
        while ( traveller != calendar.last && compareDate(traveller->ve.start.date, day.date) == SAME)
        {
            if (traveller->ve.start.time.hour != hour)
            {
                hour = traveller->ve.start.time.hour;
                isAtNewHour = true;
            }
            
            if (isAtNewHour)
            {
                printHourHeader(traveller->ve.start.time.hour);
                isAtNewHour = false;
            }
            
            printVEvent(traveller->ve, count++);
            traveller = traveller->next;
        }

    }
    /* ARGUMENT#1 == agenda --> check whether there is another argument */
    else if (strcmp(args[1], "agenda") == 0)
    {
        /* NO ARGUMENT#2 --> list next week (default) */
        if (args[2] == NULL)
        {
            /* Get current date and calculate date in 7 days */
            DateTime now = currentDateTime();
            DateTime until = addDaysToDateTime(now, 7);

            /* Traverse calendar until first future event is found */
            VEventNode* traveller = calendar.first->next;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, now) == BEFORE)
            {
                traveller = traveller->next;
            }

            /* Keep traversing, but also print events of next 7 days */
            int count = 1;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, until) != AFTER)
            {
                printVEvent(traveller->ve, count);
                ++count;
                traveller = traveller->next;
            }

        }
        /* ARGUMENT#2 PASSED --> list next ARGUMENT#2 days */
        else if (args[3] == NULL)
        {
            /* Process argument */
            int period;
            if (myatoi(args[2], &period) == 0)
            {
                shell_say(ERROR, "Argument passed ('%s') could not be converted to a number! Expected a number representing days in a reasonable range", args[2]);
                return 1;
            }

            /* Get current date and calculate date in ARGUMENT#2 days */
            DateTime now = currentDateTime();
            DateTime until = addDaysToDateTime(now, period);

            /* Traverse calendar until first future event is found */
            VEventNode* traveller = calendar.first->next;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, now) == BEFORE)
            {
                traveller = traveller->next;
            }

            /* Keep traversing, but also print events in given period */
            int count = 1;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, until) != AFTER)
            {
                printVEvent(traveller->ve, count);
                ++count;
                traveller = traveller->next;
            }
        }
        /* TOO MANY ARGUMENTS PASSED --> cancel */
        else
        {
            shell_say(ERROR, "Received too many arguments! Check 'help agenda' for more information!");
            return 1;
        }
    }
    /* ARGUMENT#1 PASSED BUT INVALID --> cancel */
    else if (args[2] == NULL)
    {
        shell_say(ERROR, "Invalid argument '%s' received! Check 'help list' for more information!");
        return 1;
    }
    /* TOO MANY ARGUMENTS PASSED --> cancel */
    else
    {
        shell_say(ERROR, "Too many arguments passed! Check 'help list' for more information!");
        return 1;
    }

    return 1;
}

int command_find(char** args) // TODO multiple word search
{
    assert(args != NULL);

    if (args[1] == NULL)
    {
        shell_say(ERROR, "Insufficient arguments passed to 'find'. Please check 'help find' for more information");
        return 1;
    }
    
    int count = 1;
    char pattern[BUFSIZ] = "";
    while (args[count] != NULL)
    {
        strncat(pattern, args[count], strlen(args[count]));
        strncat(pattern, " ", 1); // spaces
        ++count;
    }
    if (pattern[strlen(pattern) - 1] == ' ') // remove extra space
    {
        pattern[strlen(pattern) - 1] = '\0';
    }

    if (count != 1)
    {
        if (pattern[0] != '\'' || pattern[strlen(pattern) - 1] != '\'')
        {
            shell_say(ERROR, "Argument not bounded by apostrophes. Please enter argument like this: 'my arg'");
            return 1;
        }
        else
        { // get rid of apostrophes
            memmove(pattern, (pattern + 1), strlen(pattern));
            pattern[strlen(pattern) - 1] = '\0';
        }
    }
    else
    {
        strncpy(pattern, args[1], strlen(args[1]));
    }

    count = 0; // reset and use for something else
    VEvent** matches = malloc( sizeof(VEvent*) );
    if (matches == NULL)
    {
        shell_say(ERROR, "Failed to allocate memory for array that would have contained matches!");
        return 1;
    }

    int patternLength = strlen(pattern);
    
    VEventNode* traveller = calendar.first->next;
    while (traveller != calendar.last)
    {
        int summaryLength = strlen(traveller->ve.summary);
        int locationLength = strlen(traveller->ve.location);
        int descriptionLength = strlen(traveller->ve.description);

        for (int i = 0; i < summaryLength; ++i)
        {
            if (patternLength <= summaryLength && strncmp((traveller->ve.summary + i), pattern, patternLength) == 0)
            {
                if (count == 0)
                {
                    matches[count] = &(traveller->ve);
                    ++count;
                }
                else
                {
                    ++count;
                    matches = realloc( matches, (count * sizeof(VEvent*)) );
                    if (matches == NULL)
                    {
                        shell_say(ERROR, "Failed to reallocate memory for array that would have contained matches!");
                        return 1;
                    }
                    matches[count-1] = &(traveller->ve);
                }
            }
        }
        for (int i = 0; i < locationLength; ++i)
        {
            if (patternLength < locationLength && strncmp((traveller->ve.location + i), pattern, patternLength) == 0)
            {
                if (count == 0)
                {
                    matches[count] = &(traveller->ve);
                    ++count;
                }
                else
                {
                    ++count;
                    matches = realloc( matches, (count * sizeof(VEvent*)) );
                    if (matches == NULL)
                    {
                        shell_say(ERROR, "Failed to reallocate memory for array that would have contained matches!");
                        return 1;
                    }
                    matches[count-1] = &(traveller->ve);
                }
            }
        }
        for (int i = 0; i < descriptionLength; ++i)
        {
            if (patternLength < descriptionLength && strncmp((traveller->ve.description + i), pattern, patternLength) == 0)
            {
                if (count == 0)
                {
                    matches[count] = &(traveller->ve);
                    ++count;
                }
                else
                {
                    ++count;
                    matches = realloc( matches, (count * sizeof(VEvent*)) );
                    if (matches == NULL)
                    {
                        shell_say(ERROR, "Failed to reallocate memory for array that would have contained matches!");
                        return 1;
                    }
                    matches[count-1] = &(traveller->ve);
                }
            }
        }

        traveller = traveller->next;
    }

    if (count == 0)
    {
        shell_say(DONE, "No results for '%s'", pattern);
        return 1;
    }
    
    shell_say(PROGRESS, "Printing results for '%s'...", pattern);
    for (int i = 0; i < count; ++i)
    {
        printVEvent(*(matches[i]), (i + 1)); 
    }

    free(matches);

    shell_say(DONE, "Finished printing results");
    return 1;
}
