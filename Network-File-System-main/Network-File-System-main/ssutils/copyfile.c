#include "copyfile.h"
extern struct my_struct *curr_files;
void copyfilereceive(char *dest, ss_info *ss_to_receive, int client_socket_nm)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("Socket creation failed\n");
        return;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ss_to_receive->s2s_port);
    server_addr.sin_addr.s_addr = inet_addr(ss_to_receive->ss_ip);
    printf("IP: %s\n", ss_to_receive->ss_ip);
    printf("Port: %d\n", ss_to_receive->s2s_port);

    int connect_status = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_status < 0)
    {
        printf("Connection failed\n");
        return;
    }

    char *temp_path = (char *)malloc(sizeof(char) * 100);
    strcpy(temp_path, ".");
    strcat(temp_path, dest);
    temp_path[strlen(temp_path)] = '\0';

    // store the current directory
    char current_directory[100];
    getcwd(current_directory, sizeof(current_directory));

    int status = chdir(temp_path);
    if (status < 0)
    {
        printf("Error in changing directory\n");
        close(server_fd);
        return;
    }

    char file_name[100];
    read(server_fd, file_name, sizeof(file_name));
    printf("File name recieved is %s\n", file_name);

    int file_fd = open(file_name, O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC, 0666);
    int number_of_bytes_to_recieve;
    read(server_fd, &number_of_bytes_to_recieve, sizeof(number_of_bytes_to_recieve));

    char buffer[100];
    int number_of_bytes_recieved = 0;
    while (1)
    {
        if (number_of_bytes_recieved >= number_of_bytes_to_recieve)
        {
            break;
        }
        int number_of_bytes_recieved_now = read(server_fd, buffer, sizeof(buffer));
        if (number_of_bytes_recieved_now == 0)
        {
            break;
        }

        write(file_fd, buffer, number_of_bytes_recieved_now);
        number_of_bytes_recieved += number_of_bytes_recieved_now;
    }

    printf("File recieved\n");

    // change back to the original directory
    status = chdir(current_directory);
    if (status < 0)
    {
        printf("Error in changing directory\n");
        close(server_fd);
        return;
    }
    char *new_file_path = (char *)malloc(sizeof(char) * (strlen(temp_path) + strlen(file_name) + 20));
    int curr_index = 0;
    for (curr_index = 1; curr_index < strlen(temp_path); curr_index++)
    {
        new_file_path[curr_index - 1] = temp_path[curr_index];
    }
    curr_index--;
    if (new_file_path[curr_index - 1] != '/')
    {
        new_file_path[curr_index] = '/';
        curr_index++;
    }
    // new_file_path[curr_index] = '/';
    // curr_index++;
    for (int i = 0; i < strlen(file_name); i++)
    {
        new_file_path[curr_index] = file_name[i];
        curr_index++;
    }
    new_file_path[strlen(new_file_path)] = '\0';
    printf("new_file_path11: %s\n", new_file_path);
    struct my_struct *s;
    // HASH_FIND_STR(curr_files, new_file_path, s);
    // if (s != NULL)
    // {
    //     s->being_written = 0;
    // }
    // else
    {
        s = (struct my_struct *)malloc(sizeof(struct my_struct));
        strcpy(s->name, new_file_path);
        s->being_written = 0;
        pthread_mutex_init(&(s->mutex), NULL);
        HASH_ADD_STR(curr_files, name, s);
    }
    int status1 = SUCCESS;
    if (send(client_socket_nm, &status1, sizeof(status1), 0) == -1)
    {
        perror("Error in send() function call: ");
        return;
    }
    close(server_fd);
    return;
}

void copyfiless(char *src, ss_info *ss_to_send, int client_socket_nm, int s2s_port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("Socket creation failed\n");
        return;
    }

    // use SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        printf("Error in setsockopt\n");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(s2s_port);
    server_addr.sin_addr.s_addr = inet_addr(SS_IP);
    int bind_status = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_status < 0)
    {
        printf("Binding failed\n");
        return;
    }
    int listen_status = listen(server_fd, 10);
    if (listen_status < 0)
    {
        printf("Listening failed\n");
        return;
    }

    printf("Listening\n");

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0)
    {
        printf("Accepting failed\n");
        close(server_fd);
        return;
    }

    printf("Accepted\n");

    char *temp_path = (char *)malloc(sizeof(char) * 100);
    strcpy(temp_path, ".");
    strcat(temp_path, src);
    temp_path[strlen(temp_path)] = '\0';

    struct stat file_stat;
    int file_status = stat(temp_path, &file_stat);
    if (file_status < 0)
    {
        printf("File does not exist\n");
        close(server_fd);
        return;
    }
    if (!(file_stat.st_mode & S_IRUSR))
    {
        printf("File does not have read permission\n");
        close(server_fd);
        return;
    }
    int send_size = file_stat.st_size;

    char file_name[100];
    int i = strlen(temp_path) - 1;
    while (i >= 0 && temp_path[i] != '/')
    {
        i--;
    }
    i++;
    int j = 0;
    while (i < strlen(temp_path))
    {
        file_name[j] = temp_path[i];
        i++;
        j++;
    }
    file_name[j] = '\0';
    write(client_fd, file_name, sizeof(file_name));
    write(client_fd, &send_size, sizeof(send_size));
    int number_of_bytes_sent = 0;
    int number_of_bytes_to_send = send_size;
    int file_fd = open(temp_path, O_RDONLY);
    char buffer[100];
    while (1)
    {
        if (number_of_bytes_sent >= number_of_bytes_to_send)
        {
            break;
        }
        int number_of_bytes_to_send_now = read(file_fd, buffer, sizeof(buffer));
        if (number_of_bytes_to_send_now == 0)
        {
            break;
        }
        int sent_bytes = write(client_fd, buffer, number_of_bytes_to_send_now);
        if (sent_bytes < 0)
        {
            printf("Error in sending file\n");
            close(server_fd);
            return;
        }
        number_of_bytes_sent += sent_bytes;
    }

    printf("sending done\n");

    int status = SUCCESS;
    if (send(client_socket_nm, &status, sizeof(status), 0) == -1)
    {
        perror("Error in send() function call: ");
        return;
    }

    close(server_fd);
    return;
}

void copyfiless_same(char *src, char *dest, int client_socket_nm, int s2s_port)
{
    printf("IDHAR HU MEIN\n");
    char *temp_src = (char *)malloc(sizeof(char) * 100);
    strcpy(temp_src, ".");
    strcat(temp_src, src);
    temp_src[strlen(temp_src)] = '\0';

    char *temp_token = (char *)malloc(sizeof(char) * 100);
    strcpy(temp_token, src);
    temp_token[strlen(temp_token)] = '\0';

    char *filename = (char *)malloc(sizeof(char) * 100);

    char *token = strtok(temp_token, "/");
    while (token != NULL)
    {
        memset(filename, 0, sizeof(filename));
        strcpy(filename, token);
        filename[strlen(filename)] = '\0';

        token = strtok(NULL, "/");
    }

    char *temp_dest = (char *)malloc(sizeof(char) * 100);
    strcpy(temp_dest, ".");
    strcat(temp_dest, dest);
    strcat(temp_dest, "/");
    strcat(temp_dest, filename);
    temp_dest[strlen(temp_dest)] = '\0';

    struct stat file_stat;
    int file_status = stat(temp_src, &file_stat);
    if (file_status < 0)
    {
        printf("File does not exist\n");
        return;
    }

    if (!(file_stat.st_mode & S_IRUSR))
    {
        printf("File does not have read permission\n");
        return;
    }

    int src_fd = open(temp_src, O_RDONLY);
    int dest_fd = open(temp_dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    char buffer[100];
    while (1)
    {
        int number_of_bytes_read = read(src_fd, buffer, sizeof(buffer));
        if (number_of_bytes_read <= 0)
        {
            break;
        }
        write(dest_fd, buffer, number_of_bytes_read);
    }

    int status = SUCCESS;
    char *temp_path_new = (char *)malloc(sizeof(char) * (strlen(temp_dest) + strlen(filename) + 10));
    int curr_index = 0;
    for (curr_index = 1; curr_index < strlen(temp_dest); curr_index++)
    {
        temp_path_new[curr_index - 1] = temp_dest[curr_index];
    }
    curr_index--;
    temp_path_new[strlen(temp_path_new)] = '\0';
    printf("temp_path_new: %s\n", temp_path_new);
    struct my_struct *s;
    HASH_FIND_STR(curr_files, temp_path_new, s);
    if (s != NULL)
    {
        s->being_written = 0;
    }
    else
    {
        s = (struct my_struct *)malloc(sizeof(struct my_struct));
        strcpy(s->name, temp_path_new);
        s->name[strlen(s->name)] = '\0';
        s->being_written = 0;
        pthread_mutex_init(&(s->mutex), NULL);
        HASH_ADD_STR(curr_files, name, s);
    }

    if (send(client_socket_nm, &status, sizeof(status), 0) == -1)
    {
        perror("Error in send() function call: ");
        return;
    }
    return;
}