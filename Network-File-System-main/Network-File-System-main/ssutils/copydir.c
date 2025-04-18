#include "copydir.h"
extern struct my_struct *curr_files;
extern char home_dir[500];

void rec_file(int server_fd)
{
    char file_name[100];
    memset(file_name, 0, sizeof(file_name));
    read(server_fd, file_name, sizeof(file_name));
    // printf("File name recieved is %s\n", file_name);
    // int file_fd = open(file_name, O_RDONLY);
    // create
    int file_fd = open(file_name, O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd == -1)
    {
        perror("Error in open() function call: ");
        exit(1);
    }
    int number_of_bytes_to_recieve;
    read(server_fd, &number_of_bytes_to_recieve, sizeof(number_of_bytes_to_recieve));
    printf("Number of bytes to recieve is %d\n", number_of_bytes_to_recieve);
    // char buffer[100];
    // memset(buffer, 0, sizeof(buffer));
    int number_of_bytes_recieved = 0;
    while (1)
    {
        if (number_of_bytes_recieved >= number_of_bytes_to_recieve)
        {
            break;
        }
        // memset(buffer, 0, sizeof(buffer));
        int left = number_of_bytes_to_recieve - number_of_bytes_recieved;
        int size_of_buffer;
        if (left > 100)
        {
            size_of_buffer = 100;
        }
        else
        {
            size_of_buffer = left;
        }
        char new_buffer[size_of_buffer];
        int number_of_bytes_recieved_now = read(server_fd, new_buffer, sizeof(new_buffer));
        if (number_of_bytes_recieved_now == 0)
        {
            /* code */
            break;
        }
        write(file_fd, new_buffer, number_of_bytes_recieved_now);
        number_of_bytes_recieved += number_of_bytes_recieved_now;
    }
    char *relative_path = (char *)malloc(sizeof(char) * 500);
    memset(relative_path, 0, sizeof(relative_path));
    strcpy(relative_path, home_dir);
    strcat(relative_path, "/");
    strcat(relative_path, file_name);
    relative_path = strstr(relative_path, home_dir);
    relative_path[strlen(relative_path)] = '\0';
    // printf("relative_path: %s\n", relative_path);
    int index_of_dot = -1;
    for (int i = 0; i < strlen(relative_path); i++)
    {
        if (relative_path[i] == '.')
        {
            index_of_dot = i;
            break;
        }
    }
    char *path = (char *)malloc(sizeof(char) * 500);
    memset(path, 0, sizeof(path));
    for (int i = index_of_dot + 1; i < strlen(relative_path); i++)
    {
        path[i - index_of_dot - 1] = relative_path[i];
    }
    path[strlen(path)] = '\0';
    // printf("path: %s\n", path);

    struct my_struct *s;
    HASH_FIND_STR(curr_files, path, s);
    if (s != NULL)
    {
        s->being_written = 0;
    }
    else
    {
        s = (struct my_struct *)malloc(sizeof(struct my_struct));
        strcpy(s->name, path);
        s->being_written = 0;
        pthread_mutex_init(&(s->mutex), NULL);
        HASH_ADD_STR(curr_files, name, s);
    }
    // printf("File recieved\n");
}

void rec_dir(int server_fd)
{
    char dir_name[100];
    memset(dir_name, 0, sizeof(dir_name));
    read(server_fd, dir_name, sizeof(dir_name));
    mkdir(dir_name, 0777);
}

void copydirreceive(char *dest, ss_info *ss_to_receive, int client_socket_nm)
{

    char *original = (char *)malloc(sizeof(char) * 100);
    // get current working directory
    getcwd(original, 100);

    char *temp = (char *)malloc(sizeof(char) * 100);
    strcpy(temp, ".");
    strcat(temp, dest);

    int status = chdir(temp);
    if (status == -1)
    {
        perror("Error in chdir() function call: ");
        return;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Error in socket() function call: ");
        chdir(original);
        return;
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(ss_to_receive->s2s_port);
    server_address.sin_addr.s_addr = inet_addr(ss_to_receive->ss_ip);
    usleep(100000);
    int connect_success = connect(server_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connect_success == -1)
    {
        perror("Error in connect() function call: ");
        chdir(original);
        return;
    }
    printf("Connected\n");
    int number_of_paths;
    read(server_fd, &number_of_paths, sizeof(number_of_paths));
    while (number_of_paths--)
    {
        int dir_or_file;
        dir_or_file = 0;
        read(server_fd, &dir_or_file, sizeof(dir_or_file));
        if (dir_or_file == 0)
        {
            rec_file(server_fd);
        }
        else if (dir_or_file == 1)
        {
            rec_dir(server_fd);
        }
    }

    int status1 = SUCCESS;
    if (send(client_socket_nm, &status1, sizeof(status1), 0) == -1)
    {
        perror("Error in send() function call: ");
        chdir(original);
        return;
    }

    status = chdir(original);
    if (status == -1)
    {
        perror("Error in chdir() function call: ");
        return;
    }
    close(server_fd);
    return;
}

int count_no_of_paths(char src[])
{
    char path[100];
    memset(path, 0, sizeof(path));
    strcpy(path, src);
    path[strlen(path)] = '\0';
    int dir_or_file = 1;
    char dir_name[100];
    memset(dir_name, 0, sizeof(dir_name));
    strcpy(dir_name, path);
    dir_name[strlen(dir_name)] = '\0';
    DIR *dir = opendir(dir_name);
    struct dirent *curr_elem_of_dir;
    curr_elem_of_dir = readdir(dir);
    int count = 1;
    while (curr_elem_of_dir != NULL)
    {
        if (strcmp(curr_elem_of_dir->d_name, ".") != 0 && strcmp(curr_elem_of_dir->d_name, "..") != 0)
        {
            memset(path, 0, sizeof(path));
            strcpy(path, dir_name);
            strcat(path, "/");
            strcat(path, curr_elem_of_dir->d_name);
            path[strlen(path)] = '\0';
            struct stat path_stat;
            stat(path, &path_stat);
            if (S_ISDIR(path_stat.st_mode))
            {
                dir_or_file = 1;
            }
            else
            {
                dir_or_file = 0;
            }
            if (dir_or_file == 1)
            {
                count += count_no_of_paths(path);
            }
            else
            {
                count++;
            }
        }
        curr_elem_of_dir = readdir(dir);
    }
    return count;
}

int send_file(char path[], int client_fd, char send_path[])
{
    char temp_path[100];
    memset(temp_path, 0, sizeof(temp_path));
    strcpy(temp_path, send_path);
    temp_path[strlen(temp_path)] = '\0';

    char file_path[100];
    memset(file_path, 0, sizeof(file_path));
    strcpy(file_path, path);
    file_path[strlen(file_path)] = '\0';

    struct stat path_stat;
    int status = stat(file_path, &path_stat);
    if (status == -1)
    {
        perror("Error in stat() function call: ");
        return -1;
    }

    if (!(path_stat.st_mode & S_IRUSR))
    {
        perror("Error in stat() function call: ");
        return -1;
    }
    int size_of_file = path_stat.st_size;

    char file_name[100];
    memset(file_name, 0, sizeof(file_name));
    strcpy(file_name, file_path);
    file_name[strlen(file_name)] = '\0';
    int dir_or_file = 0;
    write(client_fd, &dir_or_file, sizeof(dir_or_file));
    write(client_fd, temp_path, sizeof(temp_path));
    write(client_fd, &size_of_file, sizeof(size_of_file));
    int file_fd = open(file_path, O_RDONLY);
    int number_of_bytes_to_send = size_of_file;
    int number_of_bytes_sent = 0;
    while (1)
    {
        if (number_of_bytes_sent >= number_of_bytes_to_send)
        {
            break;
        }
        int left = number_of_bytes_to_send - number_of_bytes_sent;
        int size_of_buffer;
        if (left > 100)
        {
            size_of_buffer = 100;
        }
        else
        {
            size_of_buffer = left;
        }
        char buffer[size_of_buffer];
        memset(buffer, 0, sizeof(buffer));
        int number_of_bytes_read_now = read(file_fd, buffer, sizeof(buffer));
        if (number_of_bytes_read_now == 0)
        {
            break;
        }
        if (number_of_bytes_read_now < 0)
        {
            perror("Error in read() function call: ");
            return -1;
        }
        write(client_fd, buffer, number_of_bytes_read_now);
        number_of_bytes_sent += number_of_bytes_read_now;
    }
}

int send_dir(char src[], int client_fd, char send_path[])
{
    char temp_path[100];
    memset(temp_path, 0, sizeof(temp_path));
    strcpy(temp_path, send_path);
    temp_path[strlen(temp_path)] = '\0';

    char path[100];
    memset(path, 0, sizeof(path));
    strcpy(path, src);
    path[strlen(path)] = '\0';
    int dir_or_file = 1;
    char dir_name[100];
    memset(dir_name, 0, sizeof(dir_name));
    strcpy(dir_name, path);
    dir_name[strlen(dir_name)] = '\0';
    write(client_fd, &dir_or_file, sizeof(dir_or_file));
    write(client_fd, temp_path, sizeof(temp_path));
    DIR *dir = opendir(dir_name);
    struct dirent *curr_elem_of_dir;
    curr_elem_of_dir = readdir(dir);
    while (curr_elem_of_dir != NULL)
    {
        if (strcmp(curr_elem_of_dir->d_name, ".") != 0 && strcmp(curr_elem_of_dir->d_name, "..") != 0)
        {
            memset(path, 0, sizeof(path));
            strcpy(path, dir_name);
            strcat(path, "/");
            strcat(path, curr_elem_of_dir->d_name);
            path[strlen(path)] = '\0';

            memset(temp_path, 0, sizeof(temp_path));
            strcpy(temp_path, send_path);
            strcat(temp_path, "/");
            strcat(temp_path, curr_elem_of_dir->d_name);

            struct stat path_stat;
            stat(path, &path_stat);
            if (S_ISDIR(path_stat.st_mode))
            {
                dir_or_file = 1;
            }
            else
            {
                dir_or_file = 0;
            }
            if (dir_or_file == 1)
            {
                send_dir(path, client_fd, temp_path);
            }
            else
            {
                send_file(path, client_fd, temp_path);
            }
        }
        curr_elem_of_dir = readdir(dir);
    }

    return 0;
}

void copydirss(char *src, ss_info *ss_to_send, int client_socket_nm, int s2s_conn_port)
{
    int sock_ss = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ss == -1)
    {
        perror("Error in socket() function call: ");
        return;
    }

    // use SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sock_ss, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Error in setsockopt() function call: ");
        return;
    }

    struct sockaddr_in server_address_ss;
    memset(&server_address_ss, 0, sizeof(server_address_ss));
    server_address_ss.sin_family = AF_INET;
    server_address_ss.sin_port = htons(s2s_conn_port);
    server_address_ss.sin_addr.s_addr = inet_addr(SS_IP);

    usleep(10000);
    int bind_success = bind(sock_ss, (struct sockaddr *)&server_address_ss, sizeof(server_address_ss));
    if (bind_success == -1)
    {
        perror("Error in bind() function call: ");
        close(sock_ss);
        return;
    }

    int listen_success = listen(sock_ss, 10);
    if (listen_success == -1)
    {
        perror("Error in listen() function call: ");
        close(sock_ss);
        return;
    }

    int client_socket_ss;
    struct sockaddr_in client_address_ss;
    int client_address_length_ss = sizeof(client_address_ss);
    client_socket_ss = accept(sock_ss, (struct sockaddr *)&client_address_ss, &client_address_length_ss);
    if (client_socket_ss < 0)
    {
        perror("Error in accept() function call: ");
        close(sock_ss);
        return;
    }

    char temp2[100];
    memset(temp2, 0, sizeof(temp2));
    strcpy(temp2, ".");
    strcat(temp2, src);

    char *token_temp2 = (char *)malloc(sizeof(char) * 100);
    strcpy(token_temp2, temp2);

    char *token = strtok(token_temp2, "/");
    while (token != NULL)
    {
        memset(temp2, 0, sizeof(temp2));
        strcpy(temp2, "./");
        strcat(temp2, token);
        temp2[strlen(temp2)] = '\0';
        token = strtok(NULL, "/");
    }

    char dir_name[100];
    memset(dir_name, 0, sizeof(dir_name));
    strcpy(dir_name, ".");
    strcat(dir_name, src);
    dir_name[strlen(dir_name)] = '\0';
    int ct = count_no_of_paths(dir_name);
    write(client_socket_ss, &ct, sizeof(ct));
    send_dir(dir_name, client_socket_ss, temp2);

    close(sock_ss);
    close(client_socket_ss);

    int status = SUCCESS;
    if (send(client_socket_nm, &status, sizeof(status), 0) == -1)
    {
        perror("Error in send() function call: ");
        return;
    }
    return;
}

void copysamefcn(char *src, char *dest)
{
    DIR *dir = opendir(src);
    if (dir == NULL)
    {
        perror("Error in opendir() function call: ");
        return;
    }

    struct dirent *curr_elem_of_dir;
    while ((curr_elem_of_dir = readdir(dir)) != NULL)
    {
        if (strcmp(curr_elem_of_dir->d_name, ".") == 0 || strcmp(curr_elem_of_dir->d_name, "..") == 0)
            continue;

        char *new_src = (char *)malloc(100 * sizeof(char));
        memset(new_src, 0, 100);
        char *new_dest = (char *)malloc(100 * sizeof(char));
        memset(new_dest, 0, 100);
        strcpy(new_src, src);
        strcat(new_src, "/");
        strcat(new_src, curr_elem_of_dir->d_name);
        new_src[strlen(new_src)] = '\0';
        strcpy(new_dest, dest);
        strcat(new_dest, "/");
        strcat(new_dest, curr_elem_of_dir->d_name);
        new_dest[strlen(new_dest)] = '\0';

        struct stat path_stat;
        stat(new_src, &path_stat);

        if (S_ISDIR(path_stat.st_mode))
        {
            int status = mkdir(new_dest, 0777);
            if (status == -1)
            {
                perror("Error in mkdir() function call: ");
                return;
            }
            copysamefcn(new_src, new_dest);
        }
        else
        {
            FILE *fp1 = fopen(new_src, "r");
            if (fp1 == NULL)
            {
                perror("Error in fopen() function call: ");
                return;
            }

            FILE *fp2 = fopen(new_dest, "w");
            if (fp2 == NULL)
            {
                perror("Error in fopen() function call: ");
                return;
            }

            char ch;
            while ((ch = fgetc(fp1)) != EOF)
            {
                fputc(ch, fp2);
            }
            printf("%s \n", new_src);
            printf("%s \n", new_dest);
            char *temp_dest = (char *)malloc(sizeof(char) * (strlen(new_dest) + 10));
            int curr_index = 0;
            while (new_dest[curr_index] == home_dir[curr_index])
            {
                // temp_dest[curr_index] = new_dest[curr_index];
                curr_index++;
            }
            // temp_dest[curr_index] = '\0';
            for (int i = curr_index; i < strlen(new_dest); i++)
            {
                /* code */
                temp_dest[i - curr_index] = new_dest[i];
            }
            temp_dest[strlen(temp_dest)] = '\0';
            printf("temp_dest: %s\n", temp_dest);
            struct my_struct *s;
            HASH_FIND_STR(curr_files, temp_dest, s);
            if (s != NULL)
            {
                s->being_written = 0;
            }
            else
            {
                s = (struct my_struct *)malloc(sizeof(struct my_struct));
                strcpy(s->name, temp_dest);
                s->being_written = 0;
                pthread_mutex_init(&(s->mutex), NULL);
                HASH_ADD_STR(curr_files, name, s);
            }
            fclose(fp1);
            fclose(fp2);
        }
    }
    closedir(dir);

    return;
}
void copydirss_same(char *src, char *dest, int client_socket_nm, int s2s_conn_port)
{
    char *original = (char *)malloc(sizeof(char) * 100);
    getcwd(original, 100);

    char *new_src = (char *)malloc(sizeof(char) * 100);
    memset(new_src, 0, 100);
    char *new_dest = (char *)malloc(sizeof(char) * 100);
    memset(new_dest, 0, 100);
    strcpy(new_src, original);
    strcat(new_src, src);
    new_src[strlen(new_src)] = '\0';
    strcpy(new_dest, original);
    strcat(new_dest, dest);
    new_dest[strlen(new_dest)] = '\0';

    int status = chdir(new_dest);
    if (status == -1)
    {
        perror("Error in chdir() function call: ");
        return;
    }

    char *dirname = (char *)malloc(sizeof(char) * 100);
    memset(dirname, 0, 100);

    char *temp = (char *)malloc(sizeof(char) * 100);
    memset(temp, 0, 100);
    strcpy(temp, src);

    char *token = strtok(temp, "/");
    while (token != NULL)
    {
        memset(dirname, 0, 100);
        strcpy(dirname, token);
        dirname[strlen(dirname)] = '\0';
        token = strtok(NULL, "/");
    }

    int status1 = mkdir(dirname, 0777);
    if (status1 == -1)
    {
        perror("Error in mkdir() function call: ");
        return;
    }

    strcat(new_dest, "/");
    strcat(new_dest, dirname);
    new_dest[strlen(new_dest)] = '\0';

    copysamefcn(new_src, new_dest);

    status = chdir(original);
    if (status == -1)
    {
        perror("Error in chdir() function call: ");
        return;
    }

    int status2 = SUCCESS;
    if (send(client_socket_nm, &status2, sizeof(status2), 0) == -1)
    {
        perror("Error in send() function call: ");
        return;
    }

    return;
}