#include "client.h"

int main()
{
    printf(CYAN "------------------- Welcome to the NFS Client -------------------\n" RESET);
    printf(CYAN "------------------- Type 'help' for list of commands --------------\n" RESET);

    int pid = fork();
    if (pid == -1)
    {
        printf("Error in forking\n");
        exit(1);
    }
    else if (pid == 0)
    {
        char input[100];
        memset(input, 0, 100);
        while (1)
        {
            printf(BLUE ">> " RESET);
            fgets(input, 100, stdin);
            input[strlen(input) - 1] = '\0';

            char *cpy_command = (char *)malloc(strlen(input) + 1);
            strcpy(cpy_command, input);
            cpy_command[strlen(input)] = '\0';

            char *command = strtok(cpy_command, " ");
            if (strcmp(command, "help") == 0)
            {
                help(input);
            }
            else if (strcmp(command, "makedir") == 0)
            {
                makedir(input);
            }
            else if (strcmp(command, "makefile") == 0)
            {
                makefile(input);
            }
            else if (strcmp(command, "read") == 0)
            {
                read_file(input);
            }
            else if (strcmp(command, "write") == 0)
            {
                write_file(input);
            }
            else if (strcmp(command, "deletefile") == 0)
            {
                deletefile(input);
            }
            else if (strcmp(command, "deletedir") == 0)
            {
                deletedir(input);
            }
            else if (strcmp(command, "list") == 0)
            {
                list(input);
            }
            else if (strcmp(command, "info") == 0)
            {
                info(input);
            }
            else if (strcmp(command, "copydir") == 0)
            {
                copydir(input);
            }
            else if (strcmp(command, "copyfile") == 0)
            {
                copyfile(input);
            }
            else if (strcmp(command, "exit") == 0)
            {
                exit(0);
            }
            else if (strcmp(command, "list") == 0)
            {
                list();
            }
            else if (strcmp(command, "info") == 0)
            {
                char *path = strtok(NULL, " ");
                info(path);
            }
            else
            {
                printf(MAGENTA "Command not found\n" RESET);
            }
        }
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);

        if (status == 0)
        {
            printf(GREEN "Client exited successfully\n" RESET);
        }
        else
        {
            printf(RED "Client exited with error\n" RESET);
        }
    }
}