#include "commandHandler.h"

#include <time.h>
#include <stdbool.h>

int (*runCommand[])(char**) = {
    &command_help,
    &command_exit,
    &command_load,
    &command_create,
    &command_modify,
    &command_delete,
    &command_list
};

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
                shell_say(DONE, "Successfully saved calendar! Farewell (• ε •)");
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
            
            if (!shell_handleError( shell_readString(&file, MAX_PATHLENGTH) , file, 0, NULL))
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
            }

            return 0; // EXIT
        }
    }
    else if ( !strcmp(args[1], "--discard") || !strcmp(args[1], "-d") )
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
        shell_say(ERROR, "Invalid argument: %s", args[1]);
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
        if (!shell_handleError( shell_readString(&path, MAX_PATHLENGTH), path, 0, NULL ))
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
    {   
        if
            (
                !shell_handleError( shell_readTimestamp(&ve.start, DTSTART),            NULL, 0, NULL ) ||
                !shell_handleError( shell_readTimestamp(&ve.end, DTEND),                NULL, 0, NULL ) ||
                !shell_handleError( shell_readString(&ve.summary, MAX_LINELENGTH),      NULL, 0, NULL ) ||
                !shell_handleError( shell_readString(&ve.location, MAX_LINELENGTH),     NULL, 0, NULL ) ||
                !shell_handleError( shell_readString(&ve.description, MAX_LINELENGTH),  NULL, 0, NULL ) ||
                !shell_handleError( shell_readNum(&ve.priority),                        NULL, 0, NULL )
            )
        {
            return 1;
        }
    }
    /* THERE IS AT LEAST ONE ARGUMENT PASSED */
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
                            /* We need to find where string actually ends */
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
                                for (i = firstWord; args[i] != NULL || !hasClosingApostrophe; ++i)
                                {
                                    if (args[i][ strlen(args[i]) - 1 ] == '\'')
                                    {
                                        hasClosingApostrophe = true;
                                    }
                                    numberOfCharacters += (strlen(args[i]) + 1); // +1 --> also count spaces
                                }

                                if (!hasClosingApostrophe)
                                {
                                    shell_say(ERROR, "String missing closing apostrophe! Strings have to be bounded by two apostrophes, like this 'my text'");
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
                            
                            char* target;
                            if (strcmp(options[j].opt, "--summary") == 0)
                            {
                                target = ve.summary;
                                //ve.summary = tmp;
                                //strcat(ve.summary, args[firstWord] + 1); // first word need not have apostrophe
                                //strcat(ve.summary, " "); // add space between words as intended
                                //for (int k = firstWord + 1; k < i; k++)
                                //{
                                //    strcat(ve.summary, args[k]);
                                //    strcat(ve.summary, " "); // add space between words as intended
                                //}
                                //ve.summary[strlen(ve.summary) - 2] = '\0'; // get rid of closing apostrophe and unnecessary space
                            }
                            else if (strcmp(options[j].opt, "--location") == 0)
                            {
                                target = ve.location;
                                //ve.location = tmp;
                                //strcat(ve.location, args[firstWord] + 1); // first word need not have apostrophe
                                //strcat(ve.location, " "); // add space between words as intended
                                //for (int k = firstWord + 1; k < i; k++)
                                //for (int k = firstWord; k < i; k++)
                                //{
                                //    strcat(ve.location, args[k]);
                                //    strcat(ve.location, " "); // add space between words as intended
                                //}
                                //ve.location[strlen(ve.location) - 2] = '\0'; // get rid of closing apostrophe and unnecessary space
                            }
                            else if (strcmp(options[j].opt, "--description") == 0)
                            {
                                target = ve.description;
                                //ve.description = tmp;
                                //strcat(ve.description, args[firstWord] + 1); // first word need not have apostrophe
                                //strcat(ve.description, " "); // add space between words as intended
                                //for (int k = firstWord; k < i; k++)
                                //{
                                //    strcat(ve.description, args[k]);
                                //    strcat(ve.description, " "); // add space between words as intended
                                //}
                                //ve.description[strlen(ve.description) - 2] = '\0'; // get rid of closing apostrophe and unnecessary space
                            }
                            
                            target = tmp;
                            strcat(target, args[firstWord] + 1); // +1 --> get rid of opening apostrophe
                            strcat(target, " "); // add space after first word
                            for (int k = firstWord + 1; k < i; k++)
                            {
                                strcat(target, args[k]);
                                strcat(target, " "); // add space after words
                            }
                            target[strlen(target) - 2] = '\0'; // -2 --> get rid of closing apostrophe (-1) and unnecessary space (-1)
                            
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
                            //if ( !shell_readTimestamp(&ve.start, DTSTART) )
                            //{
                            //    return 1;
                            //}
                        }
                        else if (strcmp(options[i].opt, "--end") == 0)
                        {
                            target = &ve.end;
                            tt = DTEND;
                            //if ( !shell_readTimestamp(&ve.end, DTEND) )
                            //{
                            //    return 1;
                            //}
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
                            shell_say(PROMPT, "Event summary:");
                            //if (!shell_readString(&ve.summary, MAX_LINELENGTH))
                            //{
                            //    return 1;
                            //}
                        }
                        else if (strcmp(options[i].opt, "--location") == 0)
                        {
                            target = &ve.location;
                            shell_say(PROMPT, "Event location:");
                            //if ( !shell_readString(&ve.location, MAX_LINELENGTH) )
                            //{
                            //    return 1;
                            //}
                        }
                        else if (strcmp(options[i].opt, "--description") == 0)
                        {
                            target = &ve.description;
                            shell_say(PROMPT, "Event description:");
                            //if ( !shell_readString(&ve.description, MAX_LINELENGTH) )
                            //{
                            //    return 1;
                            //}
                        }
                        
                        if (!shell_handleError( shell_readString(target, MAX_LINELENGTH), NULL, 0, NULL ))
                        {
                            return 1;
                        }
                        break;
                    }

                    case NUMBER:
                    { // note: drunt only handles priorities as numbers as of yet
                        int* target = &ve.priority;

                        shell_say(PROMPT, "Event priority:");
                        bool wasValid = true;
                        do
                        {
                            if (!wasValid)
                            {
                                shell_say(ERROR, "Priority entered was invalid. Please try again. See 'help' for details");
                            }

                            if (!shell_handleError( shell_readNum(target), NULL, 0, NULL ))
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
    printVEvent(ve);
    
    if (!promptYN("[ ?? ] Confirm?"))
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
    assert(args);
    shell_say(WARNING, "This command does nothing; it has not been implemented yet.");

    return 1;
}

int command_delete(char** args)
{
    assert(args);
    shell_say(WARNING, "This command does nothing; it has not been implemented yet.");
    
    return 1;
}

int command_list(char** args)
{
    if (!args[1])
    {
        shell_say(ERROR, "Command 'list' needs at least one argument! Check 'help list' for more information!");
        return 1;
    }
    
    if ( !strcmp(args[1], "year") && args[2] && !args[3] )
    {
        int year;
        if ( !myatoi(args[2], &year) )
        {
            shell_say(ERROR, "Failed to convert entered year to number. Expected input such as '2020'");
            return 1;
        }

        if ( !isValidYear(year) )
        {
            shell_say(ERROR, "The year you entered is invalid. Drunt expects it in YYYY format. Limited from 1950 to 2050!");
            return 1;
        }

        shell_say(PROGRESS, "Listing year %04d below...", year);

        // TODO need month lister!
        return 1;
    }
    else if ( !strcmp(args[1], "month") && args[2] && args[3] && !args[4] )
    {
        Date month;

        if ( !myatoi(args[2], &month.year) )
        {
            shell_say(ERROR, "Failed to convert entered year to number. Expected input such as '2020'");
            return 1;
        }

        if ( !isValidYear(month.year) )
        {
            shell_say(ERROR, "The year you entered is invalid. Drunt expects it in YYYY format. Limited from 1950 to 2050!");
            return 1;
        }

        if ( !myatoi(args[3], &month.month) )
        {
            shell_say(ERROR, "Failed to convert entered month to number. Expected input such as '05'");
            return 1;
        }

        if ( !isValidMonth(month.month) )
        {
            shell_say(ERROR, "The month you entered is invalid. Drunt expects it in MM format.");
            return 1;
        }

        month.day = 1;

        shell_say(PROGRESS, "Listing month %02d of year %04d below...", month.month, month.year);

        printMonth(month, &calendar);
        
        return 1;
    }
    else if ( !strcmp(args[1], "day") && args[2] && args[3] && args[4] && !args[5])
    {
        /* Process input */
        DateTime tmp;
        if ( !shell_handleError(parseDateTime(&tmp, args[2], args[3], args[4], "0", "0"), NULL, 0, NULL) )
        {
            return 1;
        }
        Date day = tmp.date;

        /* Traverse calendar to until given day is reached*/
        VEventNode* traveller = calendar.first->next;
        while( traveller != calendar.last && compareDate(traveller->ve.start.date, day) != SAME)
        {
            traveller = traveller->next;
        }

        /* Keep traversing, but also print out events and hour headers */
        printDayHeader(day);
        int count = 1;
        int hour = -1; // initialise to impossible value
        int isAtNewHour = 1;
        while ( traveller != calendar.last && compareDate(traveller->ve.start.date, day) == SAME)
        {
            if (traveller->ve.start.time.hour != hour)
            {
                hour = traveller->ve.start.time.hour;
                isAtNewHour = 1;
            }
            
            if (isAtNewHour)
            {
                printHourHeader(traveller->ve.start.time.hour);
                isAtNewHour = 0;
            }
            
            printVEventWCount(traveller->ve, count);
            traveller = traveller->next;
        }

    }
    else if ( !strcmp(args[1], "agenda") ) // TODO create separate functions
    {
        if ( !args[2] ) // no argument --> list this week
        {
            DateTime now = currentDateTime();
            DateTime until = addDaysToDateTime(now, 7);

            /* Traverse calendar until first event after 'now' is found */
            VEventNode* traveller = calendar.first->next;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, now) == BEFORE)
            {
                traveller = traveller->next;
            }

            /* Keep traversing, but also print events in given period */
            int count = 1;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, until) != AFTER)
            {
                printVEventWCount(traveller->ve, count);
                ++count;
                traveller = traveller->next;
            }

        }
        else if ( !args[3] ) // argument passed --> process
        {
            int period;
            if ( !myatoi(args[2], &period) )
            {
                shell_say(ERROR, "Number of days passed to 'list agenda' could not be converted to a number! Please check?");
                return 1;
            }

            DateTime now = currentDateTime();
            DateTime until = addDaysToDateTime(now, period);

            /* Traverse calendar until first event after 'now' is found */
            VEventNode* traveller = calendar.first->next;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, now) == BEFORE)
            {
                traveller = traveller->next;
            }

            /* Keep traversing, but also print events in given period */
            int count = 1;
            while (traveller != calendar.last && compareDateTime(traveller->ve.start, until) != AFTER)
            {
                printVEventWCount(traveller->ve, count);
                ++count;
                traveller = traveller->next;
            }
        }
        else
        {
            shell_say(ERROR, "Command 'list agenda' got too many arguments! Check 'help agenda' for available options!");
            return 1;
        }
    }
    else
    {
        shell_say(ERROR, "Invalid arguments to 'list'! Check 'help list' for available options!");
        return 1;
    }

    return 1;
}
