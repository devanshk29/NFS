#ifndef TRIE_H
#define TRIE_H

#include "../naming_server.h"

struct tree_node
{
    char *name;
    int number_of_children;
    struct tree_node *first_child;
    struct tree_node *next;
    int dir_or_file;
    int access_permissiom;
    char *ss_ip;
    int ss_port;
    int client_port;
    int s2s_port;
};

typedef struct tree_node tree_node;
typedef struct tree_node *tree_node_ptr;
typedef struct tree_node *root_ptr;

struct ss_info
{
    char ss_ip[50];
    int ss_port;
    int client_port;
    int s2s_port;
    int dir_or_file;
};

typedef struct ss_info ss_info;


typedef struct lru_entry
{
    char *key;
    ss_info value;
    UT_hash_handle hh;
} lru_entry;

#define IS_DIR 1
#define IS_FILE 0

void insert_into_tree_new(char *path, int access_permission, char *ip, int port,int client_port1,int s2s_port1,int dir_or_file1);
void init_root();
ss_info search_path_in_trie(char *path);
void print_tree(struct tree_node *curr);
int delete_from_trie(char *path);
void delete_subtree(struct tree_node *curr);

int backupdir(ss_info sender_info1, ss_info receiver_info1, char *dir_path);
int backup_file(ss_info sender_info1, ss_info reciever_info1, char *file_path);

ss_info find_in_cache(char *key);
void add_to_cache(char *key, ss_info value);

extern struct tree_node *root;
#endif