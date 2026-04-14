#include "pipe_utils.h"
#include <stdio.h>
#include <stdlib.h>

void die(const char *msg)
{
    perror(msg);
    exit(1);
}