#include "drunt.h"
#include "interactiveHandler.h"

#define YEAR_MIN 1950
#define YEAR_MAX 2050

//////////////////////////////////////////////////////////////////////////////////////////////////
// The 'shell' was made following tutorial on https://brennan.io/2015/01/16/write-a-shell-in-c/ //
//////////////////////////////////////////////////////////////////////////////////////////////////

void shell(void)
{
    putchar('\n');
    shell_say(STATUS, "Welcome to drunt! Please enter 'help' if you are new (^ v ^)");
    
    /* Shell handling */
    char* line;
    char** args;
    bool status;
    
    do
    {
        printf("\n[ -> ] ");
        
        // basic cycle: read --> interpret --> execute --> repeat
        line = shell_readline();
        args = shell_splitline(line);
        status = shell_execute(args);

        // freeing dynamic arrays each time
        free(line);
        free(args);
    }
    while (status);

    shell_say(NEUTRAL, "Good-bye :-{");
    return;
}

char* shell_readline(void)
{
    char* line = NULL;
    shell_handleError(shell_readString(NULL, &line, MAX_COMMANDLENGTH), NULL, 0, line);
    
    return line;
}

char** shell_splitline(char* line)
{
    assert(line != NULL);
    
    /* Allocate memory for command arguments */
    char** tokens = malloc( sizeof(char*) * MAX_TOKEN_NUMBER );
    if (tokens == NULL)
    {
        shell_say(ERROR, "Failed to allocate memory for command arguments!");
        return NULL;
    }

    char* token;

    /* Use strtok() to cut command into chunks */
    int i = 0;
    token = strtok(line, TOKEN_DELIMITER);
    while (token != NULL)
    {
        tokens[i++] = token;
        if (i >= TOKEN_BUFFSIZE)
        {
            shell_say(ERROR, "Too many arguments entered! Maximum %d arguments are allowed.", MAX_TOKEN_NUMBER - 1);
            return NULL;
        }

        token = strtok(NULL, TOKEN_DELIMITER);
    }

    /* Indicate end of argument array with a NULL */
    tokens[i] = NULL;

    return tokens;
}

bool shell_execute(char** args)
{
    assert(args != NULL);

    /* First, handle empty command */
    if (args[0] == NULL)
    {
        return true;
    }

    /* Handle other commands */
    size_t numbereOfCommands = ( sizeof commands / sizeof(char*) );
    for (int i = 0; i < numberOfCommands; ++i)
    {
        if ( strcmp(args[0], commands[i]) == 0 )
        {
            return (*runCommand[i])(args);
        }
    }

    shell_say(ERROR, "Command not found: %s", args[0]);
    return true;
}

void shell_say(ShellSays ss, const char* message, ...)
{
    assert(message != NULL);

    /* Handle parameters */
    char buff[BUFSIZ];
    va_list args;
    va_start(args, message);
    int rc = vsnprintf(buff, sizeof buff, message, args);
    va_end(args);

    /* Output */
    if (rc == -1)
    {
        printf("[ xx ] drunt has failed to format message --> unformatted message follows\n\t%s\n", message);
    }
    else
    {
        switch (ss)
        {
            case NEUTRAL:
                printf("[ -- ] %s\n", buff);
                break;

            case WARNING:
                printf("[ !! ] %s\n", buff);
                break;

            case PROMPT:
                printf("[ ?? ] %s ", buff);
                break;

            case STATUS:
                printf("[ ++ ] %s\n", buff);
                break;

            case PROGRESS:
                printf("[ >> ] %s\n", buff);
                break;

            case DONE:
                printf("[ oo ] %s\n", buff);
                break;

            case ERROR:
                printf("[ xx ] %s\n", buff);
                break;
        }
    }
}

MYERRNO shell_readTimestamp(DateTime* dt, TimestampType tt)
{
    assert(dt != NULL);

    char* type = (tt == DTSTART) ? "STARTING" : "ENDING";
    char buff[BUFSIZ];
    bool done = true;

    do
    {
        do
        {
            shell_say(PROMPT, "%s date:", type);
            if ( fgets(buff, sizeof buff, stdin) == NULL)
            {
                putchar('\n');
                return FAIL_EOF;
            }

            if (
                    sscanf(buff, "%04d %02d %02d %02d %02d",
                        &dt->date.year, &dt->date.month, &dt->date.day,
                        &dt->time.hour, &dt->time.minute)
                    < 5
                )
            {
                shell_say(ERROR, "Insufficient number of details entered. Perhaps you forgot something?");
                done = false;
            }
            else
            { // TODO generic
                if (!isValidYear(dt->date.year))
                {
                    shell_say(ERROR, "Invalid input for year: %04d", dt->date.year);
                }
                
                if (!isValidMonth(dt->date.month))
                {
                    shell_say(ERROR, "Invalid input for month: %02d", dt->date.month);
                }

                if (!isValidDay(dt->date.day, dt->date.month, dt->date.year))
                {
                    shell_say(ERROR, "Invalid input for day: %02d", dt->date.day);
                }

                if (!isValidHour(dt->time.hour))
                {
                    shell_say(ERROR, "Invalid input for hour: %02d", dt->time.hour);
                }

                if (!isValidMinute(dt->time.minute))
                {
                    shell_say(ERROR, "Invalid input for minute: %02d", dt->time.minute);
                }

                done = true;
            }
        }
        while (!isValidDateTime(*dt));
    }
    while (!done);

    return SUCCESS;
}

MYERRNO shell_readString(const char* message, char** str, const size_t maxLength)
{
    assert (str != NULL && maxLength != 0);

    char buff[BUFSIZ];

    do
    {
        if (message != NULL)
        {
            shell_say(PROMPT, "%s:", message);
        }

        if (fgets(buff, sizeof buff, stdin) == NULL)
        {
            putchar('\n');
            return FAIL_EOF;
        }
        
        removeNewLineChar(buff);
        if (strlen(buff) > maxLength)
        {
            shell_say(ERROR, "Entered string is too long. Please enter a maximum of %d characters.", maxLength);
        }
    }
    while (strlen(buff)  > maxLength);

    char* tmp = malloc( sizeof(char) * (strlen(buff) + 1) );
    if (tmp == NULL)
    {
        shell_say(ERROR, "Failed to allocate memory for string!");
        return FAIL_MALLOC;
    }
    *str = tmp;

    strcpy(*str, buff);

    return SUCCESS;
}

MYERRNO shell_readNum(const char* message, int* n)
{
    assert(n != NULL);

    char buff[BUFSIZ];

    do
    {
        if (message != NULL)
        {
            shell_say(PROMPT, "%s:", message);
        }
        
        if ( fgets(buff, sizeof buff, stdin) == NULL)
        {
            putchar('\n');
            shell_say(WARNING, "Received EOF, cancelling...");
            return FAIL_EOF;
        }

        removeNewLineChar(buff);
    }
    while(sscanf(buff, "%d", n) == EOF);
        
    return SUCCESS;
}

MYERRNO shell_readNumWithCriterion(const char* message, int* n, bool (*criterion)(int))
{
    assert(n != NULL);

    char buff[BUFSIZ];

    do
    {
        if (message != NULL)
        {
            shell_say(PROMPT, "%s:", message);
        }
        
        if ( fgets(buff, sizeof buff, stdin) == NULL)
        {
            putchar('\n');
            shell_say(WARNING, "Received EOF, cancelling...");
            return FAIL_EOF;
        }

        removeNewLineChar(buff);
    }
    while(sscanf(buff, "%d", n) == EOF && !criterion(*n));
        
    return SUCCESS;
}
    
bool shell_promptYN(char* message, ...)
{
    assert(message != NULL);

    /* Handle parameters */
    char buff[BUFSIZ];
    va_list args;
    va_start(args, message);
    int rc = vsnprintf(buff, sizeof buff, message, args);
    va_end(args);

    char response;

    do
    {
        if (rc == 0)
        {
            shell_say(WARNING, "Failed to format prompt's message! Unformatted message follows\n");
            shell_say(PROMPT, "%s [y/n] ", message);
        }
        else
        {
            shell_say(PROMPT, "%s [y/n] ", buff);
        }

        response = getchar();
        
        switch (response)
        {
            case 'y':
            case 'Y':
                getchar(); // consume newline
                return true;

            case 'n':
            case 'N':
                getchar(); // consume newline
                return false;

            default:
                shell_say(ERROR, (response == '\n') ? "Please enter 'y' or 'n'\n" : "Invalid choice '%c'. Please enter 'y' or 'n'\n", response);
                while ((response = getchar()) != '\n')
                {
                    ;
                }
                break;
        }
    }
    while (response != 'y' && response != 'Y' && response != 'n' && response != 'N');

    return false; // theoretically impossible to get here
}

bool shell_handleError(const MYERRNO error, const char* file, const int line, const char* content)
{
    // no assertion --> this function can receive NULL pointers on purpose
    switch (error)
    {
        case FAIL_UNDERFLOW:
            shell_say(ERROR, (file == NULL || line <= 0 ) ? "Underflow occured" : "Underflow occured at %s:%d", file, line);
            return false;

        case FAIL_OVERFLOW:
            shell_say(ERROR, (file == NULL || line <= 0) ? "Overflow occured" : "Overflow occured at %s:%d", file, line);
            return false;

        case FAIL_PROMPT:
            shell_say(WARNING, "Prompt response was 'no'");
            return false;

        case FAIL_FILE_REMOVE:
            shell_say(ERROR, (file == NULL) ? "Failed to remove file" : "Failed to remove file %s", file);
            return false;

        case FAIL_FILE_READ:
            shell_say(ERROR, (file == NULL) ? "Failed to open file for reading" : "Failed to open file %s for reading", file);
            return false;

        case FAIL_FILE_WRITE:
            shell_say(ERROR, (file == NULL) ? "Failed to open file for writing" : "Failed to open file %s for writing", file);
            return false;

        case FAIL_FILE_EMPTY:
            shell_say(ERROR, (file == NULL) ? "File is empty" : "File %s is empty", file);
            return false;

        case FAIL_FILE_CORRUPT:
            shell_say(ERROR, (file == NULL) ? "File seems to be corrupt / does not exist" : "File %s seems to be corrupt / does not exist", file);
            return false;

        case FAIL_FILE_EXISTS:
            shell_say(ERROR, (file == NULL) ? "File already exists" : "File %s already exists", file);
            return false;

        case FAIL_ICS_NOEND:
            shell_say(ERROR, (file == NULL) ? "iCalendar/vCalendar file %s has no END:VCALENDAR" : "iCalendar/vCalendar file has no END:VCALENDAR", file);
            return false;

        case FAIL_MALLOC:
            shell_say(ERROR, "Memory allocation failure");
            return false;

        case FAIL_REALLOC:
            shell_say(ERROR, "Memory reallocation failure");
            return false;

        case FAIL_MYATOI:
            shell_say(ERROR, "Failed to parse number as integer");
            return false;

        case FAIL_TIMESTAMP_CORRUPT:
            shell_say
                (
                    ERROR,
                    (file == NULL || line <= 0 || content == NULL) ?
                        "Timestamp seems to be corrupt"
                        :
                        "Timestamp at %s:%d seems to be corrupt. See below:\n[ -- ] \t%s",
                    file, file, content
                );
            return false;

        case FAIL_INVALID_YEAR:
            if (content == NULL)
            {
                shell_say(ERROR, "Invalid year entered. Please note that drunt only processes years in range %d -- %d", YEAR_MIN, YEAR_MAX);
            }
            else
            {
                shell_say(ERROR, "Invalid year '%s' entered. Please note that drunt only processes years in range %d -- %d", content, YEAR_MIN, YEAR_MAX);
            }
            return false;

        case FAIL_INVALID_MONTH:
            shell_say(ERROR, (content == NULL) ? "Invalid month entered" : "Invalid month '%s' entered", content);
            return false;

        case FAIL_INVALID_DAY:
            shell_say(ERROR, (content == NULL) ? "Invalid day entered" : "Invalid day '%s' entered", content);
            return false;

        case FAIL_INVALID_HOUR:
            shell_say(ERROR, (content == NULL) ? "Invalid hour entered" : "Invalid hour '%s' entered", content);
            return false;

        case FAIL_INVALID_MINUTE:
            shell_say(ERROR, (content == NULL) ? "Invalid minute entered" : "Invalid minute '%s' entered", content);
            return false;

        case FAIL_VEVENT_NOT_FOUND:
            shell_say(ERROR, "Event not found");
            return false;

        case FAIL_UNKNOWN:
            shell_say(ERROR, "Unknown failure");
            return false;

        case FAIL_EOF:
            shell_say(WARNING, "Unexpected EOF reached");
            return false;

        case SUCCESS:
            return true;

        default:
            return 0; // theoretically impossible case
    }
}
