#include <ctype.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <signal.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#include "server_utility.h"
#include "utility.h"

int check (char playBoard[][3], int server_fd, int pid[]);

int main(int argc, char **argv) { 
	int server_fd,
		status,
		port = 8001,
		pid[2],
		client_fds[2],
		userCount = 0,
		turnNumber = 1,
		row = 0,
		column = 0,
		choice=0,
		i,
		flag;
	char serverRead[1],
		ans[1] = "",
		readBuffer[10],
		writeBuffer[2][40],
		*users[2][3] = {
			{ "user1", "12345", "false" },
			{ "user2", "23456", "false" }
		},
    	playBoard [3][3] = {
			{' ',' ',' '},
			{' ',' ',' '},
			{' ',' ',' '}
		}; // to display the actual game status
	
	struct sockaddr_in server_addr, client_addr[2];
    socklen_t client_addr_len = sizeof(client_addr[0]);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
		perror("Socket Call Failed");
		exit(1);
	}

	serverBindandListen(server_fd, server_addr, port);

	printf("Waiting for Players to join...\n");

	while (userCount < 2) {
		printf("Number of Players who joined the game: %d\n", userCount);

		client_fds[userCount] = accept(server_fd, (struct sockaddr *) &client_addr[userCount], &client_addr_len);

		read(client_fds[userCount], readBuffer, sizeof(readBuffer));
		printf("Username: %s\n", readBuffer);
		for (i = 0; i < 2; i++) {
			if (!strcmp(readBuffer, users[i][0])) {
				break;
			}
		}
		if (i == 2) {
			printf("No such user\n");
			strcpy(writeBuffer[0], "No such user");
			strcpy(writeBuffer[1], "0");
			write(client_fds[userCount], writeBuffer, sizeof(writeBuffer));
			continue;
		}
		read(client_fds[userCount], readBuffer, sizeof(readBuffer));
		printf("Password: %s\n", readBuffer);
		if (strcmp(readBuffer, users[i][1])) {
			printf("Wrong password\n");
			strcpy(writeBuffer[0], "Wrong password");
			strcpy(writeBuffer[1], "0");
			write(client_fds[userCount], writeBuffer, sizeof(writeBuffer));
			continue;
		}
		if (!strcmp(users[i][2], "true")) {
			printf("User logged in\n");
			strcpy(writeBuffer[0], "User has logged in");
			strcpy(writeBuffer[1], "0");
			write(client_fds[userCount], writeBuffer, sizeof(writeBuffer));
			continue;
		}

		users[i][2] = "true";

		userCount++;

		if (userCount == 1) {
			strcpy(writeBuffer[0], "Logged in");
			strcpy(writeBuffer[1], "0");
			write(client_fds[0], writeBuffer, sizeof(writeBuffer));

			read(client_fds[0], readBuffer, sizeof(readBuffer));
			pid[0] = atoi(readBuffer);

			read(client_fds[0], readBuffer, sizeof(readBuffer));
			if (strcmp(readBuffer, "1")) {
				printf("%s\n", readBuffer);
				users[i][2] = "false";
				userCount--;
				continue;
			}

			strcpy(writeBuffer[0], "No other players right now...");
			strcpy(writeBuffer[1], "0");
			write(client_fds[0], writeBuffer, sizeof(writeBuffer));
		}
	
		if (userCount == 2) {
			strcpy(writeBuffer[0], "Logged in");
			strcpy(writeBuffer[1], "0");
			write(client_fds[1], writeBuffer, sizeof(writeBuffer));
			
			read(client_fds[1], readBuffer, sizeof(readBuffer));
			pid[1] = atoi(readBuffer);

			read(client_fds[1], readBuffer, sizeof(readBuffer));
			if (strcmp(readBuffer, "1")) {
				printf("%s\n", readBuffer);
				users[i][2] = "false";
				userCount--;
				continue;
			}

			strcpy(writeBuffer[0], "user1");
			strcpy(writeBuffer[1], "0");
			write(client_fds[1], writeBuffer, sizeof(writeBuffer));

			strcpy(readBuffer, "");
			read(client_fds[1], readBuffer, sizeof(readBuffer));

			strcpy(writeBuffer[0], "User2 game request");
			strcpy(writeBuffer[1], "0");
			write(client_fds[0], writeBuffer, sizeof(writeBuffer));

			strcpy(readBuffer, "");
			read(client_fds[0], readBuffer, sizeof(readBuffer));
			printf("%s\n", readBuffer);
			if (!strncmp(readBuffer, "y", 1)) {
				strcpy(writeBuffer[0], "Let's play Tic Tac Toe!!");
				strcpy(writeBuffer[1], "1");
				write(client_fds[0], writeBuffer, sizeof(writeBuffer));
				strcpy(writeBuffer[1], "2");
				write(client_fds[1], writeBuffer, sizeof(writeBuffer));
			}
		}
	}

	printf("Let's play Tic Tac Toe!!\n");
	
	if (fork() == 0) {
		int count = 0;
		
		while (count == 0) {
			read(client_fds[turnNumber], serverRead, sizeof(serverRead));
			choice = atoi(serverRead);
			printf("Server side the Integer received is: %d\n",choice);
			row = --choice/3;
			column = choice%3;
			playBoard[row][column] = (turnNumber == 0) ? 'X' : 'O';

			if(turnNumber == 1) {
				turnNumber = 0;
			} else {
				turnNumber = 1;
			}
			
			write(client_fds[turnNumber], playBoard, sizeof(playBoard));
			if (check(playBoard, server_fd, pid)) {
				count++;
			};
		}

		close(client_fds[0]);
		exit (0);
	}
	wait(&status);
	close(client_fds[1]);
}

int check (char playBoard[][3], int server_fd, int pid[])
{
	int i;
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
		return 0;
	}

	if (key == 'X') {
		printf("Player 1 Wins\n\n");
		kill(pid[0], SIGUSR1);
		kill(pid[1], SIGUSR1);
	}

	if (key == 'O') {
		printf("Player 2 Wins\n\n");
		kill(pid[0], SIGUSR2);
		kill(pid[1], SIGUSR2);
	}

	return 1;
}
