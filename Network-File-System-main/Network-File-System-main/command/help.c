#include "help.h"

void help(char *command)
{
    char* cpy_command = (char*)malloc(strlen(command) + 1);
    strcpy(cpy_command, command);
    cpy_command[strlen(command)] = '\0';
    int ctr = 0;
    char* token = strtok(cpy_command, " ");
    while (token != NULL)
    {
        ctr++;
        token = strtok(NULL, " ");
    }
    if (ctr > 1)
    {
        printf("Usage: help\n");
        return;
    }
    printf("Commands:\n");
    printf("1. makedir <dirname> <path> : Create a directory with name <dirname> at <path>\n");
    printf("2. makefile <filename> <path> : Create a file with name <filename> at <path>\n");
    printf("3. read <filename(with path)> : Read the file <filename> from <path>\n");
    printf("4. write <filename(with path)> <data>(in quotes) : Write <data> to the file <filename> from <path> (overwrite if already exists)\n");
    printf("5. deletefile <filename(with path)> : Delete the file <filename> from <path>\n");
    printf("6. deletedir <dirname(with path)> : Delete the directory <dirname> from <path>\n");
    printf("7. list <path>: List all the files and directories in the specified directory in NFS\n");
    printf("8. info <filename(with path)> : Get the information of the file <filename> from <path>\n");
    printf("9. copydir <src> <dest> : Copy the directory <src> to <dest>\n");
    printf("10. copyfile <src> <dest> : Copy the file <src> to <dest>\n");
    printf("11. exit : Exit the client\n");

    return;
}
