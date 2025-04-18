#ifndef COPYDIRSSUTILS_H
#define COPYDIRSSUTILS_H

#include "../storage_server.h"

void copydirreceive(char *dest, ss_info *ss_to_receive, int client_socket_nm);
void copydirss(char *src, ss_info *ss_to_send, int client_socket_nm, int s2s_conn_port);
void copydirss_same(char *src, char *dest, int client_socket_nm, int s2s_conn_port);

#endif