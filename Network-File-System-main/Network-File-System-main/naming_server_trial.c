#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

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
};

typedef struct tree_node tree_node;
typedef struct tree_node *tree_node_ptr;
typedef struct tree_node *root_ptr;

struct tree_node *root;


void insert_into_tree(char *path, int access_permission,char *ip,int port)
{
    int ct_of_slash = 0;
    // printf("%s\n", path);
    // char *token = strtok(path, "/");
    for (int i = 0; i < strlen(path); i++)
    {
        if (path[i] == '/')
        {
            ct_of_slash++;
        }
    }
    char *path_arr[ct_of_slash];
    char *cp_of_path = (char *)malloc(sizeof(char) * strlen(path));
    strcpy(cp_of_path, path);
    char *token = strtok(cp_of_path, "/");
    int i = 0;
    while (token != NULL)
    {
        path_arr[i] = token;
        token = strtok(NULL, "/");
        i++;
    }
    tree_node_ptr temp = root;
    for (int i = 0; i < (ct_of_slash - 1); i++)
    {
        if (temp->first_child == NULL)
        {
            tree_node_ptr new_node = (tree_node_ptr)malloc(sizeof(struct tree_node));
            new_node->name = path_arr[i];
            new_node->access_permissiom = 0;
            new_node->first_child = NULL;
            new_node->next = NULL;
            new_node->dir_or_file = 1;
            temp->first_child = new_node;
            temp = new_node;
        }
        else
        {
            temp = temp->first_child;
            while (temp->next != NULL)
            {
                if (strcmp(temp->name, path_arr[i]) == 0)
                {
                    break;
                }
                temp = temp->next;
            }
            if (strcmp(temp->name, path_arr[i]) == 0)
            {
                continue;
            }
            else
            {
                tree_node_ptr new_node = (tree_node_ptr)malloc(sizeof(struct tree_node));
                new_node->name = path_arr[i];
                new_node->access_permissiom = 0;
                new_node->first_child = NULL;
                new_node->next = NULL;
                new_node->dir_or_file = 1;
                temp->next = new_node;
                temp = new_node;
            }
        }
    }
    if (temp->first_child == NULL)
    {
        tree_node_ptr new_node = (tree_node_ptr)malloc(sizeof(struct tree_node));
        new_node->name = path_arr[ct_of_slash - 1];
        new_node->access_permissiom = access_permission;
        new_node->first_child = NULL;
        new_node->next = NULL;
        new_node->dir_or_file = 2;
        new_node->ss_port=port;
        new_node->ss_ip=(char *)malloc(sizeof(char)*(strlen(ip)+1));
        strcpy(new_node->ss_ip,ip);
        temp->first_child = new_node;
        temp = new_node;
    }
    else
    {
        temp = temp->first_child;
        while (temp->next != NULL)
        {
            if (strcmp(temp->name, path_arr[ct_of_slash - 1]) == 0)
            {
                break;
            }
            temp = temp->next;
        }
        if (strcmp(temp->name, path_arr[ct_of_slash - 1]) == 0)
        {
            temp->access_permissiom = access_permission;
            temp->ss_port=port;
            temp->ss_ip=(char *)malloc(sizeof(char)*(strlen(ip)+1));
            strcpy(temp->ss_ip,ip);
        }
        else
        {
            tree_node_ptr new_node = (tree_node_ptr)malloc(sizeof(struct tree_node));
            new_node->name = path_arr[ct_of_slash - 1];
            new_node->access_permissiom = access_permission;
            new_node->first_child = NULL;
            new_node->next = NULL;
            new_node->dir_or_file = 2;
            new_node->ss_port=port;
            new_node->ss_ip=(char *)malloc(sizeof(char)*(strlen(ip)+1));
            strcpy(new_node->ss_ip,ip);
            temp->next = new_node;
            temp = new_node;
        }
    }
}
void print_tree(struct tree_node* curr)
{
    if (curr==NULL)
    {
        return;
    }
    else
    {
        printf("%s ",curr->name);
        print_tree(curr->first_child);
        print_tree(curr->next);
    }
}

void init_root()
{
    root=(struct tree_node*)malloc(sizeof(struct tree_node));
    root->access_permissiom=0;
    root->dir_or_file=1;
    root->first_child=NULL;
    root->name=(char *)malloc(sizeof(char)*(strlen("/")+1));
    strcpy(root->name,"/");
    root->name[strlen("/")]='\0';
    root->next=NULL;
    root->ss_ip=NULL;
    root->ss_port=0;
}

int main()
{
    init_root();
    insert_into_tree("/home/abc/def", 1,"127.0.0.1",50353);
    print_tree(root);
    return 0;
}