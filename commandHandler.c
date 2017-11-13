#include "commandHandler.h"

int (*runCommand[])(char**) = {
    &command_help,
    &command_exit,
    &command_open,
    &command_create,
    &command_modify,
    &command_delete,
    &command_list
};

int command_help(char** args)
{   
    if (!args[1])
    {
        shell_say(NEUTRAL, "You are currently in interactive mode. The following commands are at your disposal:");
    
        for (int i = 0; i < numberOfCommands; ++i)
        {
            shell_say(STATUS, "\t%s", commands[i]);
        }

        shell_say(NEUTRAL, "Type 'help <command>' to know more about a certain command");
    }
    else if (args[1] && !args[2])
    {
        if ( !strcmp(args[1], "help") )
        {
            shell_say(NEUTRAL, "\tSyntax:\thelp [commnand]");
            shell_say(NEUTRAL, "\tType this command without any arguments to get a list of available commands.");
            shell_say(NEUTRAL, "\tType 'help <command>' to get to know more about <command>. Note that you've just done that");
            return 1;
        }
        else if ( !strcmp(args[1], "exit") )
        {
            shell_say(NEUTRAL, "\tSyntax:\texit [options]");
            shell_say(NEUTRAL, "\tType this command to exit interactive mode.");
            shell_say(NEUTRAL, "\tDrunt overwrites your last opened iCalendar file upon exiting by default. To change this behaviur, please type 'exit --discard' or 'exit -d'");
            shell_say(NEUTRAL, "\t\tIn this case, you'll be prompted to specify file to write to. You can also skip saving, effectively throwing away your changes");
        }
        else if ( !strcmp(args[1], "open") )
        {
            shell_say(NEUTRAL, "\tSyntax:\topen <path>");
            shell_say(NEUTRAL, "\tOpen iCalendar/vCalendar files with this option. You will have to specify which calendar to load .ics file into.");
        }
        else if ( !strcmp(args[1], "create") )
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
        else if ( !strcmp(args[1], "modify") )
        {
            shell_say(NEUTRAL, "\tSyntax:\tmodify [date]");
            shell_say(NEUTRAL, "\tAs of yet, you can only specify events by their starting date. You can enter date as an argument, but if you don't, you'll be prompted for it anyway");
            shell_say(NEUTRAL, "\t\tPlease note that if you do choose to enter the date, you have to follow iCalendar/vCalendar timestamp format. Check 'help create' for detailed instructions, or read the manual");
        }
        else if ( !strcmp(args[1], "delete") )
        {
            shell_say(NEUTRAL, "\tSyntax:\tdelete [date]");
            shell_say(NEUTRAL, "\tThis command lets you delete events. As of yet, events can only be specified by their starting date. You can either enter date as an argument or give no argument and let interactive mode prompt you.");
            shell_say(NEUTRAL, "\t\tPlease note that if you do choose to enter the date, you have to follow iCalendar/vCalendar timestamp format. Check 'help create' for detailed instructions, or read the manual");
        }
        else if ( !strcmp(args[1], "list") )
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
    if ( !args[1] )
    {
        shell_say(PROGRESS, "Saving you calendar into file '%s'...", file);
        if (file)
        {
            switch( ICS_write(file, &calendar, OVERWRITE) )
            {
                case FAIL_FILE_WRITE:
                    shell_say(ERROR, "Failed to open file %s for writing. Changes will be lost. Sorry!", file);
                    break;

                default:
                    break;
            }
            shell_say(DONE, "Successfully saved calendar! Farewell (• ε •)");
            return 0;
        }
        else // filename is NULL
        {
            shell_say(STATUS, "For some odd reason, file has been set to a NULL pointer. Where would you like to save your calendar?");
            
            char buff[BUFFSIZE];

            do
            {
                shell_say(PROMPT, "Please enter path to file to save into:");
                fgets(buff, sizeof buff, stdin);
                if (strlen(buff) >= MAX_PATHLENGTH)
                {
                    shell_say(ERROR, "Path is too long. Maxium %d characters allowed. Please try again", MAX_PATHLENGTH);
                }
            }
            while (strlen(buff) >= MAX_PATHLENGTH);

            switch( ICS_write(file, &calendar, OVERWRITE) )
            {
                case FAIL_FILE_WRITE:
                    shell_say(ERROR, "Failed to open file %s for writing. Changes will be lost. Sorry!", file);
                    break;

                default:
                    break;
            }

            return 0;
        }
    }
    else if ( !strcmp(args[1], "--discard") || !strcmp(args[1], "-d") )
    {
        shell_say(WARNING, "About to exit drunt without saving changes! Are you sure?");
        if (!promptYN("[ ?? ] Exit?"))
        {
            shell_say(STATUS, "Cancelled exit");
            return 1;
        }
        else
        {
            shell_say(PROGRESS, "Exiting drunt and ditching changes...");
            return 0;
        }
    }
    else
    {
        shell_say(ERROR, "Invalid argument: %s", args[1]);
        return 1;
    }
}
    
int command_open(char** args)
{
    if ( !args[1] )
    {
        char buff[BUFFSIZE];

        shell_say(PROMPT, "Please enter path to file:");
        fgets(buff, sizeof buff, stdin);
        removeNewLineChar(buff);
        if (strlen(buff) >= MAX_PATHLENGTH)
        {
            shell_say(ERROR, "Path is too long. Can't open that. You could try moving your file somewhere else");
            return 1;
        }

        if ( file )
        {
            shell_say(PROGRESS, "Saving currently open iCalendar '%s' to disk...", file);
            switch ( ICS_write(file, &calendar, OVERWRITE) )
            {
                case FAIL_FILE_WRITE:
                {
                    shell_say(ERROR, "Failed to save file!");
                    if ( !promptYN("[ ?? ] Close without saving?") )
                    {
                        shell_say(STATUS, "Cancelled closing file and opening new");
                        return 1;
                    }
                    else
                    {
                        break;
                    }
                }

                default:
                {
                    shell_say(STATUS, "Successfully saved open calendar to file '%s'. Moving on to opening other file", file);
                    break;
                }
            }
        }

        // reinit calendar
        Calendar_destroy(&calendar);
        Calendar_create(&calendar);

        shell_say(PROGRESS, "Loading iCalendar file %s into RAM...", buff);

        switch( ICS_load(buff, &calendar) )
        {
            case FAIL_FILE_CORRUPT:
                shell_say(ERROR, "File %s is corrupt/invalid or simply does not exist. The file has not been loaded.", buff);
                return 1;
                break;
            
            case FAIL_FILE_READ:
                shell_say(ERROR, "Unable to open file %s for reading. The file has not been loaded.", buff);
                return 1;
                break;
            
            case FAIL_TIMESTAMP_CORRUPT:
                shell_say(ERROR, "File %s contains corrupt timestamp. The file has not been loaded.", buff);
                return 1;
                break;

            case FAIL_MALLOC:
                shell_say(ERROR, "Fatal error: failed to allocate memory");
                die("Failed to allocate memory for calendar. Quitting...");
                break;

            case FAIL_OVERFLOW:
                shell_say(ERROR, "File %s contains overflow. The file has not been loaded.", buff);
                return 1;
                break;

            default:
                break;
        }

        char* tmp = realloc(file, sizeof(char) * (strlen(buff) + 1) );
        if (!tmp)
        {
            shell_say(ERROR, "Failed to allocate memory for new filename! This can lead to all sorts of errors. It is STROGLY advised to exit and try again");
            return 1;
        }
        file = tmp;
        strcpy(file, buff);
        shell_say(DONE, "File %s has been successfully loaded!", buff);
        return 1;
    }
    else if ( args[1] && !args[2] )
    {
        if ( file )
        {
            shell_say(PROGRESS, "Saving currently open iCalendar '%s' to disk...", file);
            switch ( ICS_write(file, &calendar, OVERWRITE) )
            {
                case FAIL_FILE_WRITE:
                {
                    shell_say(ERROR, "Failed to save file!");
                    if ( !promptYN("[ ?? ] Close without saving?") )
                    {
                        shell_say(STATUS, "Cancelled closing file and opening new");
                        return 1;
                    }
                    else
                    {
                        break;
                    }
                }

                default:
                {
                    shell_say(STATUS, "Successfully saved open calendar to file '%s'. Moving on to opening other file", file);
                    break;
                }
            }
        }

        // reinit calendar
        Calendar_destroy(&calendar);
        Calendar_create(&calendar);
        
        shell_say(PROGRESS, "Loading iCalendar file %s into RAM...", args[1]);

        switch( ICS_load(args[1], &calendar) )
        {
            case FAIL_FILE_CORRUPT:
                shell_say(ERROR, "File %s is corrupt/invalid or simply just does not exist. The file has not been loaded.", args[1]);
                return 1;
                break;
            
            case FAIL_FILE_READ:
                shell_say(ERROR, "Unable to open file %s for reading. The file has not been loaded.", args[1]);
                return 1;
                break;
            
            case FAIL_TIMESTAMP_CORRUPT:
                shell_say(ERROR, "File %s contains corrupt timestamp. The file has not been loaded.", args[1]);
                return 1;
                break;

            case FAIL_MALLOC:
                shell_say(ERROR, "Fatal error: failed to allocate memory");
                die("Failed to allocate memory for calendar. Quitting...");
                break;

            case FAIL_OVERFLOW:
                shell_say(ERROR, "File %s contains overflow. The file has not been loaded.", args[1]);
                return 1;
                break;

            default:
                break;
        }

        char* tmp = realloc(file, sizeof(char) * (strlen(args[1]) + 1) );
        if (!tmp)
        {
            shell_say(ERROR, "Failed to allocate memory for new filename! This can lead to all sorts of errors. It is STROGLY advised to exit and try again");
            return 1;
        }
        file = tmp;
        strcpy(file, args[1]);
        
        shell_say(DONE, "File %s has been successfully loaded!", args[1]);
        return 1;
    }
    else
    {
        shell_say(ERROR, "Too many arguments passed!");
        return 1;
    }
}

int command_create(char** args)
{
    if (!file)
    {
        shell_say(WARNING, "No file has been opened yet; you are editing an empty calendar.");
        if (!promptYN("[ ?? ] Continue?")) // TODO better
        {
            shell_say(STATUS, "No event created");
            return 1;
        }
    }
    
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
    
    if (!args[1])
    {
        /* Read event details one by one */
        
        if ( !shell_readTimeStamp(&ve.start, DTSTART) )
        {
            return 1;
        }
         
        if ( !shell_readTimeStamp(&ve.end, DTEND) )
        {
            return 1;
        }
        
        shell_say(PROMPT, "Event summary:");
        if ( !shell_readString(&ve.summary) )
        {
            return 1;
        }
         
        shell_say(PROMPT, "Event location:");
        if ( !shell_readString(&ve.location) )
        {
            return 1;
        }
        
        shell_say(PROMPT, "Event description:");
        if ( !shell_readString(&ve.location) )
        {
            return 1;
        }

        shell_say(PROMPT, "Event priority:");
        
        char buff[BUFFSIZE];

        do
        {
            fgets(buff, sizeof buff, stdin);
            sscanf(buff, "%d", &ve.priority);
        }
        while ( !isValidPriority(ve.priority) );
        
        /* Confirm event with user */

        shell_say(NEUTRAL, "Event to be created:");
        // TODO event lister
        
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
    else
    {
        typedef struct {
            char* opt;
            char* opt_short;
            ICSDATATYPE type;
            int flag;
        } Option;
        
        Option options[] = {
            { .opt = "--begin",         .opt_short = "-b", .type = TIMESTAMP,   .flag = 0 },
            { .opt = "--end",           .opt_short = "-e", .type = TIMESTAMP,   .flag = 0 },
            { .opt = "--summary",       .opt_short = "-s", .type = STRING,      .flag = 0 },
            { .opt = "--location",      .opt_short = "-l", .type = STRING,      .flag = 0 },
            { .opt = "--description",   .opt_short = "-d", .type = STRING,      .flag = 0 },
            { .opt = "--priority",      .opt_short = "-p", .type = NUMBER,      .flag = 0 }
        };
        
        size_t numberOfOptions = ( sizeof options / sizeof(Option) );
        
        for (int i = 0; args[i] != NULL; ++i)
        {
            for (int j = 0; j < numberOfOptions; ++j)
            {
                if ( !strcmp(args[i], options[j].opt) || !strcmp(args[i], options[j].opt_short) )
                {
                    ; // TODO
                }
            }
        }
        
        //for (int i = 1; args[i] != NULL; ++i)
        //{
        //    for (int j = 0; j < numberOfOptions; j += 2)
        //    {
        //        if ( !strcmp(args[i], options[j]) || !strcmp(args[i], options[j+1]) )
        //        { // TODO improve !!!
        //            if (j == 0)
        //            {
        //                if ( !args[i + 1] )
        //                {
        //                    shell_say(ERROR, "Option --begin / -b missing argument (timestamp)");
        //                    return 1;
        //                }
        //                else
        //                {
        //                    if ( !isValidICSTimeStamp(args[i + 1]) )
        //                    {
        //                        shell_say(ERROR, "Option --begin / -b received invalid timestamp! Check 'help create' for details!");
        //                        return 1;
        //                    }
        //                    else
        //                    {
        //                        ICSTimeStampReader(args[i + 1], &ve.start);
        //                        DTSTARTFlag = 1;
        //                    }
        //                }
        //            }
        //            else if (j == 2)
        //            {
        //                if ( !args[i + 1] )
        //                {
        //                    shell_say(ERROR, "Option --end / -e missing argument (timestamp)");
        //                    return 1;
        //                }
        //                else
        //                {
        //                    if ( !isValidICSTimeStamp(args[i + 1]) )
        //                    {
        //                        shell_say(ERROR, "Option --end / -e received invalid timestamp! Check 'help create' for details!");
        //                        return 1;
        //                    }
        //                    else
        //                    {
        //                        ICSTimeStampReader(args[i + 1], &ve.end);
        //                        DTENDFlag = 1;
        //                    }
        //                }
        //            }
        //            else if (j == 4)
        //            {
        //                if ( !args[i + 1] )
        //                {
        //                    shell_say(ERROR, "Option --summary / -s missing argument (text)");
        //                    return 1;
        //                }
        //                else
        //                {
        //                    ve.summary = malloc( sizeof(char) * (strlen(args[i + 1]) + 1) );
        //                    if (!ve.summary)
        //                    {
        //                        shell_say(ERROR, "Failed to allocate memory for summary field!");
        //                        return 1;
        //                    }
        //                    strcpy(ve.summary, args[i+1]);
        //                    summaryFlag = 1;
        //                }
        //            }
        //            else if (j == 6)
        //            {
        //                if ( !args[i + 1] )
        //                {
        //                    shell_say(ERROR, "Option --location / -l missing argument (text)");
        //                    return 1;
        //                }
        //                else
        //                {
        //                    ve.location = malloc( sizeof(char) * (strlen(args[i + 1]) + 1) );
        //                    if (!ve.location)
        //                    {
        //                        shell_say(ERROR, "Failed to allocate memory for location field!");
        //                        return 1;
        //                    }
        //                    strcpy(ve.location, args[i+1]);
        //                    locationFlag = 1;
        //                }
        //            }
        //            else if (j == 8)
        //            {
        //                if ( !args[i + 1] )
        //                {
        //                    shell_say(ERROR, "Option --description / -d missing argument (text)");
        //                    return 1;
        //                }
        //                else
        //                {
        //                    ve.description = malloc( sizeof(char) * (strlen(args[i + 1]) + 1) );
        //                    if (!ve.description)
        //                    {
        //                        shell_say(ERROR, "Failed to allocate memory for description field!");
        //                        return 1;
        //                    }
        //                    strcpy(ve.description, args[i+1]);
        //                    descriptionFlag = 1;
        //                }
        //            }
        //            else if (j == 10)
        //            {
        //                if ( !args[i + 1] )
        //                {
        //                    shell_say(ERROR, "Option --priority / -p missing argument (number 0-9)"); // TODO VALID
        //                    return 1;
        //                }
        //                else
        //                {
        //                    if ( !myatoi(args[i + 1], &ve.priority) )
        //                    {
        //                        shell_say(ERROR, "Failed to interpret priority value entered!");
        //                        return 1;
        //                    }
        //                    else
        //                    {
        //                        priorityFlag = 1;
        //                    }
        //                }
        //            }
        //        }
        //        else
        //        {
        //            shell_say(ERROR, "Invalid option '%s'!", args[i]);
        //            return 1;
        //        }
        //    }
        //}

        //if (!DTSTARTFlag)
        //{
        //    // TODO READER
        //}

        return 1;
    }
    return 1;
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
    assert(args);
    shell_say(WARNING, "This command does nothing; it has not been implemented yet.");

    return 1;
}

