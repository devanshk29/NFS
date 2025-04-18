#include "makedir.h"

void makedir(char *input)
{
    char *cpy_input = (char *)malloc(strlen(input) + 1);
    strcpy(cpy_input, input);
    cpy_input[strlen(input)] = '\0';

    char *command = strtok(cpy_input, " ");
    char *dirname = strtok(NULL, " ");
    if (dirname == NULL)
    {
        printf("Usage: makedir <dirname> <path>\n");
        return;
    }
    char *path = strtok(NULL, " ");
    if (path == NULL)
    {
        printf("Usage: makedir <dirname> <path>\n");
        return;
    }
    char *tmp = strtok(NULL, " ");
    if (tmp != NULL)
    {
        printf("Usage: makedir <dirname> <path>\n");
        return;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error in creating socket\n");
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
        printf("Error in connecting to naming server\n");
        return;
    }

    char *msg = (char *)malloc(sizeof(char) * 100);
    strcpy(msg, "makedir ");
    strcat(msg, dirname);
    strcat(msg, " ");
    strcat(msg, path);

    int len = strlen(msg);
    msg[len] = '\0';

    if (send(sockfd, msg, strlen(msg), 0) < 0)
    {
        printf("Error in sending data to naming server\n");
        return;
    }

    int status;

    if (recv(sockfd, &status, sizeof(status), 0) < 0)
    {
        printf("Error in receiving data from naming server\n");
        return;
    }

    if (status == SUCCESS)
    {
        printf("Directory created successfully\n");
    }
    else if (status == DIR_ALREADY_EXISTS)
    {
        printf("Directory already exists\n");
    }
    else if (status == INVALID_PATH)
    {
        printf("Given path is invalid\n");
    }
    else if (status == PERMISSION_DENIED)
    {
        printf("Permission Denied\n");
    }
    else if (status == INVALID_DIRNAME)
    {
        printf("Given directory name is invalid\n");
    }
    else if (status == CREATE_ERROR)
    {
        printf("Error in creating new directory\n");
    }
    else
    {
        printf("Unknown Error occured while creating directory\n");
    }

    close(sockfd);

    return;

}