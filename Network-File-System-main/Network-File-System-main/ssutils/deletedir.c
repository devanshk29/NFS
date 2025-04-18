#include "deletedir.h"

int deletedirectory(char *path)
{
    DIR* dir = opendir(path);
    struct dirent *entry;
    entry = readdir(dir);
    while(entry != NULL)
    {
        char *filename = entry->d_name;
        if(strcmp(filename,".") == 0 || strcmp(filename,"..") == 0)
        {
            entry = readdir(dir);
            continue;
        }
        char *new_path = (char *)malloc(strlen(path) + strlen(filename) + 2);
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, filename);
        if(entry->d_type == DT_DIR)
        {
            deletedirectory(new_path);
        }
        else
        {
            // remove(new_path);
            int err_check = remove(new_path);
            if (err_check == -1)
            {
                perror("Error in remove() function call: ");
                return -1;
            }
        }
        entry = readdir(dir);

    }
    closedir(dir);

    int err_check = rmdir(path);
    if (err_check == -1)
    {
        perror("Error in rmdir() function call: ");
        return -1;
    }
    
    return 0;

}

void deletedirss(char* path,int socket)
{
    int status;
    char *new_path = (char *)malloc(strlen(path) + 4);
    char *tmp = ".";
    strcpy(new_path, tmp);
    strcat(new_path, path);

    if (access(new_path, F_OK) == -1)
    {
        status = DIR_NOT_FOUND;
        send(socket, &status, sizeof(status), 0);
        perror("Error in access() function call: ");
        return;
    }
    // create directory
    // int err_check = rmdir(new_path);
    // if (err_check == -1)
    // {
    //     status = DELETE_ERROR;
    //     send(socket, &status, sizeof(status), 0);
    //     perror("Error in rmdir() function call: ");
    //     return;
    // }
    int err_check = deletedirectory(new_path);
    if (err_check == -1)
    {
        status = DELETE_ERROR;
        send(socket, &status, sizeof(status), 0);
        perror("Error in rmdir() function call: ");
        return;
    }
    printf("Directory %s deleted\n",path);

    status = SUCCESS;
    if (send(socket, &status, sizeof(status), 0) < 0)
    {
        printf("Error in sending data to naming server\n");
        return;
    }
}