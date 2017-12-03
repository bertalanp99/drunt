#include "structs.h"
#include "dbHandler.h"
#include "debug.h"
#include "errorHandler.h"
#include "helper.h"
#include "interactiveHandler.h"
#include "GUIHandler.h"

#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        shell_say(NEUTRAL, "No arguments passed, launching interactive mode by default");
        shell();
        return 0;
    }

    if (argc > 2)
    {
        shell_say(ERROR, "Too many arguments passed to main. Drunt is not capable of processing more than one argument at this stage!");
        return 1;
    }

    if (strcmp(argv[1], "-i") == 0)
    {
        shell();
        return 0;
    }
    else if (strcmp(argv[1], "-g") == 0)
    {
        GUI(argc, argv);
        return 0;
    }
    
    putchar('\n');
    shell();
    return 0;
}

