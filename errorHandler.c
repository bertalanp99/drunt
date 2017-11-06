#include "errorHandler.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#define BUFFSIZE 256

void die(const char* message, ...)
{
    /* Handle parameters */
    char buff[BUFFSIZE];
    va_list args;
    va_start(args, message);
    int rc = vsnprintf(buff, sizeof buff, message, args);
    va_end(args);
    
    /* Output errors and bail programme */
    if (errno)
    {
        perror(buff);
    }
    else if (rc == -1)
    {
        printf("[errorHandler has failed to format error message --> unformatted message follows] %s\n", message);
    }
    else
    {
        printf("ERROR: %s\n", buff);
    }

    exit(1);
}

void warning(const char* message, ...)
{
    /* Handle parameters */
    char buff[BUFFSIZE];
    va_list args;
    va_start(args, message);
    int rc = vsnprintf(buff, sizeof buff, message, args);
    va_end(args);
    
    /* Output warnings */
    if (rc == -1)
    {
        printf("[errorHandler has failed to format warning message --> unformatted message follows] %s\n", message);
    }
    else
    {
        printf("WARNING: %s\n", buff);
    }
}
