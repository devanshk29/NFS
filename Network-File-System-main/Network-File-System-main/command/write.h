#ifndef WRITE_H
#define WRITE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../errorcode.h"
#include "../client.h"
#include "../naming_server.h"
#include "../storage_server.h"

void write_file(char *input);

#endif