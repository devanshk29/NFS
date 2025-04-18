#ifndef COPYFILESS_H
#define COPYFILESS_H

#include "../storage_server.h"

void copyfiless(char *src, ss_info *ss_to_send, int client_socket_nm,int s2s_port);
void copyfilereceive(char *dest, ss_info *ss_to_receive, int client_socket_nm);
void copyfiless_same(char *src, char *dest, int client_socket_nm, int s2s_port);

#endif