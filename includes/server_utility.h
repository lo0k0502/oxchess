#ifndef SERVER_UTILITY_H
#define SERVER_UTILITY_H

void serverBindandListen(int server_fd, struct sockaddr_in server_addr, int port);
int check(char playBoard[][3]);
char *onlinePlayers(char *users[2][3], char *currentPlayer);

#endif