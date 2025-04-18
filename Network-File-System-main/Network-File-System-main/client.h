#ifndef CLIENT_H
#define CLIENT_H

#include "naming_server.h"
#include "errorcode.h"
#include "command/help.h"
#include "command/makedir.h"
#include "command/makefile.h"
#include "command/deletefile.h"
#include "command/deletedir.h"
#include "command/list.h"
#include "command/read.h"
#include "command/write.h"
#include "command/info.h"
#include "command/copyfile.h"
#include "command/copydir.h"
#include <wait.h>

#define NM_PORT 51234
#define ip "127.0.0.1"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

#endif