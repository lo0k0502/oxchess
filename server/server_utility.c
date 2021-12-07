
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

int check (char playBoard[][3]) {
	int i, j, flag = 0;
	char key = ' ';

	// Check Rows
	for (i = 0; i < 3; i++) {
		if (playBoard[i][0] == playBoard[i][1] && playBoard[i][0] == playBoard[i][2] && playBoard[i][0] != ' ') key = playBoard[i][0];	
	}

	// check Columns
	for (i = 0; i < 3; i++) {
		if (playBoard [0][i] == playBoard [1][i] && playBoard[0][i] == playBoard[2][i] && playBoard[0][i] != ' ') key = playBoard[0][i];
	}

	// Check Diagonals
	if (playBoard [0][0] == playBoard [1][1] && playBoard[1][1] == playBoard[2][2] && playBoard[1][1] != ' ') {
		key = playBoard[1][1];
	}
	if (playBoard [0][2] == playBoard [1][1] && playBoard[1][1] == playBoard[2][0] && playBoard[1][1] != ' ') {
		key = playBoard[1][1];
	}

	if (key == ' ') {
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				if (playBoard[i][j] == ' ') {
					flag++;
					break;
				}
			}
		}
		
		if (flag) {
			return 0;
		}

		key = 'T';
	}

	if (key == 'X') {
		printString("Player 1 Wins\n");
	} else if (key == 'O') {
		printString("Player 2 Wins\n");
	} else if (key == 'T') {
		printString("Tie\n");
	}

	return 1;
}

char *onlinePlayers(char *users[2][3], char *currentPlayer) {
	int i, j;
    char *result = malloc(sizeof(char) * 50);

	if (strcmp(currentPlayer, users[0][0]) && strcmp(users[0][2], "false")) {
		strcat(result, users[0][0]);
	}
	if (strcmp(currentPlayer, users[1][0]) && strcmp(users[1][2], "false")) {
		if (!strcmp(result, "")) {
			strcat(result, " ");
		}
		strcat(result, users[1][0]);
	}
	if (!strcmp(result, "")) {
		strcat(result, "No other players right now...");
	}

	return result;
}