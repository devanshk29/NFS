#include "copydir.h"

void copydir(char *input)
{
    char *cpy_command = (char *)malloc(strlen(input) + 1);
    strcpy(cpy_command, input);
    cpy_command[strlen(input)] = '\0';

    char *command = strtok(cpy_command, " ");
    char *src = strtok(NULL, " ");
    if (src == NULL)
    {
        printf(MAGENTA "Usage: copydir <src> <dest>\n" RESET);
        return;
    }

    char *dest = strtok(NULL, " ");
    if (dest == NULL)
    {
        printf(MAGENTA "Usage: copydir <src> <dest>\n" RESET);
        return;
    }

    char *tmp = strtok(NULL, " ");
    if (tmp != NULL)
    {
        printf(MAGENTA "Usage: copydir <src> <dest>\n" RESET);
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
    strcpy(msg, "copydir ");
    strcat(msg, src);
    strcat(msg, " ");
    strcat(msg, dest);

    int len = strlen(msg);
    msg[len] = '\0';

    if (send(sockfd, msg, strlen(msg), 0) < 0)
    {
        printf(RED "Error in sending message to naming server\n" RESET);
        return;
    }

    int status;
    if (recv(sockfd, &status, sizeof(status), 0) < 0)
    {
        printf(RED "Error in recieving status from naming server\n" RESET);
        return;
    }

    if(status == SUCCESS)
    {
        printf(GREEN "Directory copied successfully\n" RESET);
    }
    else if(status == SRC_NOT_FOUND)
    {
        printf(RED "Source directory not found\n" RESET);
    }
    else if (status == DEST_NOT_FOUND)
    {
        printf(RED "Destination directory not found\n" RESET);
    }
    else if(status == SRC_IS_FILE)
    {
        printf(RED "Source is a file\n" RESET);
    }
    else if(status == DEST_IS_FILE)
    {
        printf(RED "Destination is a file\n" RESET);
    }
    else if(status == SRC_IS_DIR)
    {
        printf(RED "Source is a directory\n" RESET);
    }
    else if(status == DEST_IS_DIR)
    {
        printf(RED "Destination is a directory\n" RESET);
    }
    else if(status == COPY_ERROR)
    {
        printf(RED "Error in copying directory\n" RESET);
    }
    else
    {
        printf(RED "Unknown error\n" RESET);
    }

    return;
}