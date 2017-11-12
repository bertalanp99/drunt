#include "interactiveHandler.h"

///////////////////////////////////////////////////////////////////////////////////////////
// This was made following tutorial on https://brennan.io/2015/01/16/write-a-shell-in-c/ //
///////////////////////////////////////////////////////////////////////////////////////////

void shell(void)
{
    /* Initialise empty calendar */
    Calendar* cal = &calendar;
    Calendar_create(cal);

    /* Set default file */
    char* tmps = malloc( sizeof(char) * (strlen(DEFAULTFILE) + 1) );
    if (!tmps)
    {
        die("Failed to allocate memory for default filename (%s)", DEFAULTFILE);
    }
    strcpy(tmps, DEFAULTFILE);
    file = tmps;
    
    char* line;
    char** args;
    int status;

    shell_say(STATUS, "Welcome to drunt! Please enter 'help' if you are new around here :-)");
    
    do
    {
        printf("\n> ");
        
        /* Basic cycle is read --> interpret --> execute --> repeat */
        line = shell_readline();
        args = shell_splitline(line);
        status = shell_execute(args);

        /* Freeing dynamic arrays every time */
        free(line);
        free(args);
    }
    while (status);

    Calendar_destroy(cal);
}

char* shell_readline(void)
{
    char buff[BUFFSIZE];
    
    do
    {
        fgets(buff, sizeof buff, stdin);
        if (strlen(buff) >= MAX_COMMANDLENGTH)
        {
            shell_say(ERROR, "Command entered is too long. Maximum %d characters allowed.", MAX_COMMANDLENGTH);
        }
    }
    while (strlen(buff) >= MAX_COMMANDLENGTH);

    char* line = malloc( sizeof(char) * (strlen(buff) + 1) );
    if (!line)
    {
        shell_say(ERROR, "Fatal error: failed to allocate memory");
        die("Failed to allocate memory for command string");
    }

    strcpy(line, buff);
    
    return line;
}

char** shell_splitline(char* line)
{   
    assert(line != NULL);
    
    /* Allocate memory for command arguments */
    char** tokens = malloc( sizeof(char*) * MAX_TOKEN_NUMBER );
    if (!tokens)
    {
        shell_say(ERROR, "Failed to allocate memory for command arguments!");
        return NULL;
    }

    /* Allocate memory for one token */
    char* token = malloc( sizeof(char) * TOKEN_BUFFSIZE );
    if (!token)
    {
        shell_say(ERROR, "Failed to allocate memory for command arguments!");
        return NULL;
    }

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

int shell_execute(char** args)
{
    /* First, handle empty command */
    if (args[0] == NULL)
    {
        return 1;
    }

    /* Handle other commands */
    for (int i = 0; i < (int) ( sizeof commands / sizeof(char*) ); ++i)
    {
        if ( !strcmp(args[0], commands[i]) )
        {
            return (*runCommand[i])(args);
        }
    }

    shell_say(ERROR, "Command not found: %s", args[0]);
    return 1;
}

void shell_say(ShellSays ss, const char* message, ...)
{
    /* Handle parameters */
    char buff[BUFFSIZE];
    va_list args;
    va_start(args, message);
    int rc = vsnprintf(buff, sizeof buff, message, args);
    va_end(args);

    /* Output */
    if (rc == -1)
    {
        printf("[shell_say has failed to format message --> unformatted message follows\n%s\n", message);
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

