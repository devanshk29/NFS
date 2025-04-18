#include "deletedir.h"

void deletedir(char *input)
{
    char *cpy_command = (char *)malloc(strlen(input) + 1);
    strcpy(cpy_command, input);
    cpy_command[strlen(input)] = '\0';

    char *command = strtok(cpy_command, " ");

    char *dirname = strtok(NULL, " ");
    if (dirname == NULL)
    {
        printf(MAGENTA "Usage: deletedir <dirname(with path)>\n" RESET);
        return;
    }

    char *token = strtok(NULL, " ");
    if (token != NULL)
    {
        printf(MAGENTA "Usage: deletedir <dirname(with path)>\n" RESET);
        return;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        printf(RED "Error in creating socket\n" RESET);
        return;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NM_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    int ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (ret < 0)
    {
        printf(RED "Error in connecting to naming server\n" RESET);
        return;
    }

    char *msg = (char *)malloc(sizeof(char) * 100);
    strcpy(msg, "deletedir ");
    strcat(msg, dirname);

    int len = strlen(msg);
    msg[len] = '\0';

    if (send(sockfd, msg, strlen(msg), 0) < 0)
    {
        printf(RED "Error in sending data to naming server\n" RESET);
        return;
    }

    int status;
    if (recv(sockfd, &status, sizeof(status), 0) < 0)
    {
        printf(RED "Error in receiving data from naming server\n" RESET);
        return;
    }

    if (status == SUCCESS)
    {
        printf(GREEN "Directory deleted successfully\n" RESET);
    }
    else if (status == DIR_NOT_FOUND)
    {
        printf(RED "Directory not found\n" RESET);
    }
    else if (status == PERMISSION_DENIED)
    {
        printf(RED "Permission Denied\n" RESET);
    }
    else if (status == INVALID_PATH)
    {
        printf(RED "Invalid Path\n" RESET);
    }
    else if (status == INVALID_DIRNAME)
    {
        printf(RED "Invalid Directory Name\n" RESET);
    }
    else
    {
        printf(RED "Error in deleting directory\n" RESET);
    }

    close(sockfd);

    return;
}