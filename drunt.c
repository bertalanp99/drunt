#include "structs.h"
#include "dbHandler.h"
#include "debug.h"
#include "errorHandler.h"
#include "helper.h"
#include "interactiveHandler.h"

#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{ 
    printf("DEBUG Argument count: %d\n", argc);
    for (int i = 1; i < argc; ++i)
    {
        printf("DEBUG argument %d: %s\n", i, argv[i]);
    }

    printf("\n");
    shell();
    return 0;
}

