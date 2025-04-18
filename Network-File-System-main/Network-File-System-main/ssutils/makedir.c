#include "makedir.h"

void makedirss(char* dirname, char* path, int client_socket_nm)
{
    int status;

    char *new_path = (char *)malloc(strlen(path) + strlen(dirname) + 4);
    char *tmp = ".";
    strcpy(new_path, tmp);
    strcat(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, dirname);
    printf("new_path: %s\n", new_path);

    if (access(new_path, F_OK) == 0)
    {
        status = DIR_ALREADY_EXISTS;
        send(client_socket_nm, &status, sizeof(status), 0);
        perror("Error in access() function call: ");
        return;
    }
    // create directory
    int err_check = mkdir(new_path, 0777);
    if (err_check == -1)
    {
        status = CREATE_ERROR;
        send(client_socket_nm, &status, sizeof(status), 0);
        perror("Error in mkdir() function call: ");
        return;
    }
    printf("Directory %s created\n",dirname);

    status = SUCCESS;
    if (send(client_socket_nm, &status, sizeof(status), 0) < 0)
    {
        printf("Error in sending data to naming server\n");
        return;
    }
}