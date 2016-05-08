#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include "cmd.h"

const CMD_TBL cmd_tbl[] =
{

    {"help", show_help},
    {"?", show_help},
    {0, 0}
};


