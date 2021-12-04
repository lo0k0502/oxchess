#include <ctype.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "client_utility.h"
#include "utility.h"

int main(int argc, char **argv) {
	char username[10], password[10];

	printf("Please enter username: ");
	scanf("%s", username);
	printf("Please enter password: ");
	scanf("%s", password);

	printf("Username: %s, Password: %s\n", username, password);

	static struct sigaction action;
	action.sa_handler = SIG_IGN;
	sigfillset(&(action.sa_mask));
	sigaction(SIGTSTP, &action, 0);

	struct sockaddr_in server_addr;
	int client_fd,
		port = 8001,
		row,
		column,
		choice,
		i;
	char input,
		readBuffer[2][40],
		writeBuffer[10],
		buffer[10],
		pid[4],
		clientRead[3][3],
		clientWrite[1],
		c,
		numberBoard [3][3] = {
			{'1','2','3'},
			{'4','5','6'},
			{'7','8','9'}
		}, // to display positions to choose from
		playBoard [3][3] = {
			{' ',' ',' '},
			{' ',' ',' '},
			{' ',' ',' '}
		}; // to display the actual game status

	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket Call Failed");
		exit(1);
	}

	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "--port")) {
			port = atoi(argv[++i]);
		}
	}

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

	if (connect(client_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) {
		perror("Connect Call Failed");
		exit(1);
	}

	write(client_fd, username, sizeof(username));
	sleep(1);
	write(client_fd, password, sizeof(password));
	
	read(client_fd,readBuffer,sizeof(readBuffer));

	if (!strcmp(readBuffer[1], "0")) {
		if (!strcmp(readBuffer[0], "No such user")) {
			printString(readBuffer[0]);
			exit(0);
		} else if (!strcmp(readBuffer[0], "Wrong password")) {
			printString(readBuffer[0]);
			exit(0);
		} else if (!strcmp(readBuffer[0], "User has logged in")) {
			printString(readBuffer[0]);
			exit(0);
		} else {
			int num1 = getpid();
			sprintf(pid, "%d", num1);
			write(client_fd, pid, sizeof(pid));
			changePrintColor("bold-green");
			printString("\nLogged In!!\n");
			changePrintColor("white");
		}
	}

	for (i = 0;;) {
		switch(menu()) {
			case 1: {
				i++;
				strcpy(writeBuffer, "1");
				write(client_fd, writeBuffer, sizeof(writeBuffer));
				break;
			}
			case 2: {
				printString("\nQuitting\n");
				strcpy(writeBuffer, "2");
				write(client_fd, writeBuffer, sizeof(writeBuffer));
				exit(0);
				break;
			}
			default: {
				printString("\nInvalid choice\n");
				break;
			}
		}
		if (i) {
			break;
		}
	}

	read(client_fd, readBuffer, sizeof(readBuffer));
	changePrintColor("bold-cyan");
	printString("\nOnline Players:");
	changePrintColor("white");
	printString(readBuffer[0]);
	
	if (!strcmp(readBuffer[0], "No other players right now...")) {
		read(client_fd, readBuffer, sizeof(readBuffer));
		printf("\nGame request from user2! Accept?");
		changePrintColor("bold-cyan");
		printf(" (y/n) ");
		changePrintColor("white");
		for (; c != 'y';) {
			c = getch();
		}
		printString("yes");
		write(client_fd, "y", sizeof("y"));
	} else {
		printf("\nPlease enter a user's name to request a game: ");
		scanf("%s", buffer);
		write(client_fd, buffer, sizeof(buffer));
		printString("Waiting for response...");
	}

	read(client_fd, readBuffer, sizeof(readBuffer));
	changePrintColor("bold-yellow");
	printString(readBuffer[0]);
	changePrintColor("white");
	
	if (strcmp(readBuffer[1], "1"))
	{
		showBoard(numberBoard);	
		printString("\n    Number Board\n");
		for(;;)
			{
			printf("\nPlease enter the number of the square where you want to place your '%c': \n", (strcmp(readBuffer[1], "1") == 0) ? 'X' : 'O');
			scanf("%s", clientWrite);

			choice = atoi(clientWrite);
			row = --choice / 3;
			column = choice % 3;
		
			if (choice < 0 || choice > 9 || playBoard[row][column] > '9'|| playBoard[row][column] == 'X' || playBoard[row][column] == 'O') {
				printString("Invalid Input. Please Enter again.\n");
			} else {
				playBoard[row][column] = (strcmp(readBuffer[1], "1") == 0) ? 'X' : 'O';					
				break;
			}
		}

		write(client_fd, clientWrite, sizeof(clientWrite));
		system("clear");
		showBoard(playBoard);
		printString("\nCurrent Play Board\n");
	}

	for (input = 'x';;) { 	
		if (input == '\n') {
			showBoard(numberBoard);
			printString("\n    Number Board\n");

			for (;;) {
				printf("\nNow your turn .. Please enter the number of the square where you want to place your '%c': \n", (strcmp(readBuffer[1], "1") == 0) ? 'X' : 'O');
				scanf("%s", clientWrite);
		
				choice = atoi(clientWrite);
				row = --choice/3;
				column = choice%3;
		
				if(choice < 0 || choice > 9 || playBoard[row][column] > '9' || playBoard[row][column] == 'X' || playBoard[row][column] == 'O') {
					printString("Invalid Input. Please Enter again.\n");
				} else {
					playBoard[row][column] = (!strcmp(readBuffer[1], "1")) ? 'X' : 'O';
					break;
				}
			}	
			
			write(client_fd, clientWrite, sizeof(clientWrite));
			system("clear");
			showBoard(playBoard);
			printString("\nCurrent Play Board\n");
		}

		if (read(client_fd, clientRead, sizeof(clientRead)) > 0) {
			system("clear");
			memcpy(playBoard, clientRead, sizeof(playBoard)); // copy the contents of the array received from server side in playBoard array
			showBoard(playBoard);
			printString("\nCurrent Play Board\n");
			input = '\n';
		}

		if (check(playBoard) == ((!strcmp(readBuffer[1], "1")) ? 'X' : 'O')) {
			printString("You Win!! Thank You for playing!!");
			close(client_fd);
			exit(0);
		} else if (check(playBoard) == ((!strcmp(readBuffer[1], "1")) ? 'O' : 'X')) {
			printString("You lose... Thank You for playing!!");
			close(client_fd);
			exit(0);
		} else if (check(playBoard) == 'T') {
			printString("Tie!! Thank You for playing!!");
			close(client_fd);
			exit(0);
		}
	}
}
