
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

#include "server_utility.h"
#include "utility.h"

extern int errno;

// bind the server and let it listen
void serverBindandListen(int server_fd, struct sockaddr_in server_addr, int port) {
    char confirm;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    while (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        if (errno == EADDRINUSE) {
            // if the error is caused by the port is in use, then ask if the user wants to use next port
            printf("Port %d is in use", port);
            printf(", would you like to use port %d?", ++port);
            changePrintColor("bold-cyan");
            printf(" (y/n) ");
            changePrintColor("white");

            while (confirm != 'y' && confirm != 'n') {
                // check whether y or n is pressed, and react immediately
                confirm = getch();
            }
            if (confirm == 'y') {
                // if yes, set the port to the alternate port and try to bind again
                printString("yes");
                server_addr.sin_port = htons(port);
                continue;
            } else {
                // if no, then exit without error
                printString("no");
                exit(0);
            }
        }
        perror("Server bind error\n");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 10) == -1) {
        perror("Server listen error\n");
        close(server_fd);
        exit(1);
    }

    printf("Server is listening on port %d\n", port);
}