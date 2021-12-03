							/*Tic Tac Toe Game by Ishan Ankita*/
									/*Server*/ 
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

int check (char playBoard[][3], int server_fd, int pid[]);

int main(void) 
{ 
	int server_fd,
		status,
		pid[2],
		client_fds[2],
		userCount=0,
		ctr=1,
		row = 0,
		column = 0,
		choice=0,
		i,
		flag;
	char serverRead[1],
		ans[1],
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

	strcpy(ans,"");

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
		perror("Socket Call Failed");
		exit(1);
	}

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8001);

	if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) { 
		perror("Bind Call Failed");
		exit(1);
	}
	
	printf("Waiting for Players to join in TicTacToe Club.. \n");

	strcpy(writeBuffer[0],"Waiting for the other Player to join\n");

	if (listen(server_fd, 5) == -1) {
		perror("Listen Call Failed\n");
		exit(1);
	}

	while (userCount < 2) {
		client_fds[userCount] = accept(server_fd, (struct sockaddr *) &client_addr[userCount], &client_addr_len);

		read(client_fds[userCount], readBuffer, sizeof(readBuffer));
		printf("Username: %s\n", readBuffer);
		for (i = 0; i < 2; i++) {
			if (!strcmp(readBuffer, users[i][0])) {
				break;
			}
		}
		if (i == 2) {
			strcpy(writeBuffer[0], "No such user");
			strcpy(writeBuffer[1], "0");
			write(client_fds[userCount], writeBuffer, sizeof(writeBuffer));
			continue;
		}
		read(client_fds[userCount], readBuffer, sizeof(readBuffer));
		printf("Password: %s\n", readBuffer);
		if (strcmp(readBuffer, users[i][1])) {
			strcpy(writeBuffer[0], "Wrong password");
			strcpy(writeBuffer[1], "0");
			write(client_fds[userCount], writeBuffer, sizeof(writeBuffer));
			continue;
		}
		if (!strcmp(users[i][2], "true")) {
			strcpy(writeBuffer[0], "User has logged in");
			strcpy(writeBuffer[1], "0");
			write(client_fds[userCount], writeBuffer, sizeof(writeBuffer));
			continue;
		}

		users[i][2] = "true";
		userCount++;

		printf("Number of Players who joined the game: %d\n", userCount);

		if (userCount == 1) {
			strcpy(writeBuffer[1],"0");
			write(client_fds[0], writeBuffer, sizeof(writeBuffer));
			read(client_fds[0], readBuffer, sizeof(readBuffer));
			pid[0] = atoi(readBuffer);
		}
	
		if (userCount == 2) {
			strcpy(writeBuffer[0], "Let's play Tic Tac Toe!!");
			strcpy(writeBuffer[1], "1");
			write(client_fds[0], writeBuffer, sizeof(writeBuffer));
			strcpy(writeBuffer[1], "2");
			write(client_fds[1], writeBuffer, sizeof(writeBuffer));

			read(client_fds[1], readBuffer, sizeof(readBuffer));
			pid[1] = atoi(readBuffer);
		}
	}
	
	if (fork() == 0) 
	{
		int count=0; 		
		
		while (count==0) 
		{ 	
		
			read(client_fds[ctr], serverRead, sizeof(serverRead));			
			choice = atoi(serverRead);
			printf("Server side the Integer received is: %d\n",choice);
			row = --choice/3;
				column = choice%3;
			playBoard[row][column] = (ctr==0)?'X':'O';
			
			//check(playBoard);				
			if(ctr == 1)
				ctr = 0;
			else
				ctr = 1;
			
			write(client_fds[ctr],playBoard,sizeof(playBoard)); 
			if (check(playBoard, server_fd, pid)) {
				break;
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
	for (i=0; i<3;i++)
	if (playBoard [i][0] == playBoard [i][1] && playBoard [i][0] == playBoard [i][2] && playBoard [i][0] != ' ') key = playBoard [i][0];	
	// check Columns
	for (i=0; i<3;i++)
	if (playBoard [0][i] == playBoard [1][i] && playBoard [0][i] == playBoard [2][i] && playBoard [0][i] != ' ') key = playBoard [0][i];
	// Check Diagonals
	if (playBoard [0][0] == playBoard [1][1] && playBoard [1][1] == playBoard [2][2] && playBoard [1][1] != ' ') key = playBoard [1][1];
	if (playBoard [0][2] == playBoard [1][1] && playBoard [1][1] == playBoard [2][0] && playBoard [1][1] != ' ') key = playBoard [1][1];

	if (key == ' ') {
		return 0;
	}

	close(server_fd);

	if (key == 'X')
	{
		printf("Player 1 Wins\n\n");
		kill(pid[0], SIGUSR1);
		kill(pid[1], SIGUSR1);
	}

	if (key == 'O')
	{
		printf("Player 2 Wins\n\n");
		kill(pid[0], SIGUSR2);
		kill(pid[1], SIGUSR2);
	}

	return 1;
}
