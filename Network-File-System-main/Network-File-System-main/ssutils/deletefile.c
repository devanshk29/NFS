#include "deletefile.h"

void deletefiless(char *path , int client_soket)
{
    char *new_path = (char *)malloc(strlen(path) + 4);
    char *tmp = ".";
    strcpy(new_path, tmp);
    strcat(new_path, path);
    printf("new_path: %s\n", new_path);

    int status;

    if(access(new_path,F_OK)==-1)
    {
        status = FILE_NOT_FOUND;
        send(client_soket,&status,sizeof(status),0);
        perror("Error in access() function call: ");
        return;
    }

    int err_check = remove(new_path);
    if(err_check==-1)
    {
        status = DELETE_ERROR;
        send(client_soket,&status,sizeof(status),0);
        perror("Error in remove() function call: ");
        return;
    }
    printf("File %s deleted\n",path);

    status = SUCCESS;
    if(send(client_soket,&status,sizeof(status),0)<0)
    {
        printf("Error in sending data to naming server\n");
        return;
    }

    
}