#ifndef STORAGE_SERVER_H
#define STORAGE_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "errorcode.h"
#include <semaphore.h>
#include "ssutils/makedir.h"
#include "ssutils/deletedir.h"
#include "ssutils/makefile.h"
#include "ssutils/deletefile.h"
#include "nmutils/trie.h"
#include "ssutils/copyfile.h"
#include "ssutils/copydir.h"
#include "ssutils/read.h"
#include "ssutils/write.h"
#include "ssutils/info.h"
#include "uthash.h"

#define MAX_PATHS 1000
#define NM_INIT_PORT 5572
#define NM_CONN_IP "127.0.0.1"
#define SS_IP "127.0.0.1"

#define CHECK_ALIVE_PORT 5575


struct details_of_path
{
    char path[400];
    int permissions;
    int dir_or_file;
    int backup_pending[2];
    int recovery_pending[2];
};

struct data_of_ss
{
    int port_number;
    int number_of_paths;
    int s2s_port;
    int client_port;
    struct details_of_path paths[MAX_PATHS];
    char ip[20];
};
struct my_struct
{
    char name[400];
    int being_written;
    pthread_mutex_t mutex;
    UT_hash_handle hh;
};

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

#endif