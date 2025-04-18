#ifndef NAMING_SERVER_H
#define NAMING_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <poll.h>
#include "uthash.h"
#include "errorcode.h"
#include "nmutils/makedir.h"
#include "nmutils/deletedir.h"
#include "nmutils/makefile.h"
#include "nmutils/deletefile.h"
#include "nmutils/trie.h"
#include "nmutils/copyfile.h"
#include "nmutils/copydir.h"
#include "nmutils/read.h"
#include "nmutils/write.h"
#include "nmutils/backup.h"
#include "nmutils/lru.h"
#include "nmutils/list.h"

#define CLIENT_PORT 51234
#define SS_PORT 5572

#define CHECK_ALIVE_PORT 5575

#define IP "127.0.0.1"
struct replica_info
{
    int original_ss_index;
    char original_ss_ip[20];
    int original_ss_port;
    int replica1_ss_index;
    int replica2_ss_index;
    int replica1_ss_port;
    int replica2_ss_port;
    char replica1_ss_ip[20];
    char replica2_ss_ip[20];
    int number_of_references;
};

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"
// int backupdir(ss_info sender_info1, ss_info receiver_info1, char *dir_path);
// int backup_file(ss_info sender_info1, ss_info reciever_info1, char *file_path);

#endif