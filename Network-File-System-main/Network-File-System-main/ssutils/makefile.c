#include "makefile.h"
extern struct my_struct *curr_files;
void makefiless(char *filename, char *path, int client_soket)
{
    char *new_path = (char *)malloc(strlen(path) + strlen(filename) + 4);
    char *tmp = ".";
    strcpy(new_path, tmp);
    strcat(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, filename);
    printf("new_path: %s\n", new_path);

    int status;

    if (access(new_path, F_OK) == 0)
    {
        status = FILE_ALREADY_EXISTS;
        send(client_soket, &status, sizeof(status), 0);
        perror("Error in access() function call: ");
        return;
    }

    int fd = open(new_path, O_CREAT | O_RDWR, 0777);
    if (fd == -1)
    {
        status = CREATE_ERROR;
        send(client_soket, &status, sizeof(status), 0);
        perror("Error in open() function call: ");
        return;
    }
    printf("File %s created\n", filename);

    struct my_struct *s;
    // HASH_FIND_STR(curr_files, new_path, s);
    // if (s != NULL)
    // {
    //     s->being_written = 0;
    // }
    // else
    {
        s = (struct my_struct *)malloc(sizeof(struct my_struct));
        char *new_new_path = (char *)malloc(sizeof(char) * (strlen(new_path) + 10));
        for (int i = 1; i < strlen(new_path); i++)
        {
            new_new_path[i - 1] = new_path[i];
        }
        new_new_path[strlen(new_new_path)] = '\0';
        strcpy(s->name, new_new_path);
        s->being_written = 0;
        pthread_mutex_init(&(s->mutex), NULL);
        HASH_ADD_STR(curr_files, name, s);
    }
    status = SUCCESS;
    if (send(client_soket, &status, sizeof(status), 0) < 0)
    {
        printf("Error in sending data to naming server\n");
        return;
    }

    close(fd);
}