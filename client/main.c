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

void showBoard(char playBoard[][3]);
void playerWinhandler(int signum);
int menu();
int pid;
int te = 0;

int main(int argc, char **argv) {
	
	printf("%d %s\n", argc, *argv);

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

	signal(SIGUSR1, playerWinhandler);
	signal(SIGUSR2, playerWinhandler);

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
			printf("%s\n", readBuffer[0]);
			exit(0);
		} else if (!strcmp(readBuffer[0], "Wrong password")) {
			printf("%s\n", readBuffer[0]);
			exit(0);
		} else if (!strcmp(readBuffer[0], "User has logged in")) {
			printf("%s\n", readBuffer[0]);
			exit(0);
		} else {
			int num1 = getpid();
			sprintf(pid, "%d", num1);
			write(client_fd, pid, sizeof(pid));
			printf("Logged In!!\n");
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
				printf("\nQuitting\n\n");
				strcpy(writeBuffer, "2");
				write(client_fd, writeBuffer, sizeof(writeBuffer));
				exit(0);
				break;
			}
			default: {
				printf("\nInvalid choice\n\n");
				break;
			}
		}
		if (i) {
			break;
		}
	}

	read(client_fd, readBuffer, sizeof(readBuffer));
	printf("Online Players:\n");
	printf("%s\n", readBuffer[0]);
	
	if (!strcmp(readBuffer[0], "No other players right now...")) {
		read(client_fd, readBuffer, sizeof(readBuffer));
		printf("Game request from user2! Accept? (y/n) ");
		for (; strcmp(clientWrite, "y");) {
			scanf("%s", clientWrite);
		}
		write(client_fd, "y", sizeof("y"));
	} else {
		printf("Please enter a user's name: ");
		scanf("%s", buffer);
		write(client_fd, buffer, sizeof(buffer));
		printf("Waiting for response...\n");
	}

	read(client_fd, readBuffer, sizeof(readBuffer));
	printf("%s\n", readBuffer[0]);
	
	if (strcmp(readBuffer[1], "1"))
	{
		showBoard(numberBoard);	
		printf("\n    Number Board\n\n");
		for(;;)
			{
			printf("\nPlayer %d,Please enter the number of the square where you want to place your '%c': \n",
				(strcmp(readBuffer[1], "1") == 0) ? 1 : 2,
				(strcmp(readBuffer[1], "1") == 0) ? 'X' : 'O');
			scanf("%s", clientWrite);

			choice = atoi(clientWrite);
			row = --choice/3;
			column = choice%3;
		
			if (choice < 0 || choice > 9 || playBoard[row][column] > '9'|| playBoard[row][column] == 'X' || playBoard[row][column] == 'O') {
				printf("Invalid Input. Please Enter again.\n\n");
			} else {
				playBoard[row][column] = (strcmp(readBuffer[1], "1") == 0) ? 'X' : 'O';					
				break;
			}
		}

		write(client_fd, clientWrite, sizeof(clientWrite));
		system("clear");
		showBoard(playBoard);
		printf("\nCurrent Play Board\n\n");
	}

	for (input = 'x';;) { 	
		if (input == '\n') {
			if (te == 0) {
				showBoard(numberBoard);
				printf("\n    Number Board\n\n");
			} else if (te == 1) {
				printf("Player 1 Wins!!\n");
				close(client_fd);
				exit(0);
			} else if (te == 2) {
				printf("Player 2 Wins!!\n");
				close(client_fd);
				exit(0);
			}
			for(;;)
			{	
				if (te == 0)
				{
					printf("\nPlayer %d, Now your turn .. Please enter the number of the square where you want to place your '%c': \n",(strcmp(readBuffer[1], "1")==0)?1:2,(strcmp(readBuffer[1], "1")==0)?'X':'O');
					scanf("%s",clientWrite);
			
					choice = atoi(clientWrite);
					row = --choice/3;
					column = choice%3;
			
					if(choice < 0 || choice > 9 || playBoard[row][column] > '9' || playBoard[row][column] == 'X' || playBoard[row][column] == 'O') {
						printf("Invalid Input. Please Enter again.\n\n");
					} else {
						playBoard[row][column] = (!strcmp(readBuffer[1], "1")) ? 'X' : 'O';
						break;
					}
				} else if (te == 1) {
					printf("Player 1 Wins!!\n");
					close(client_fd);
					exit(0);
				} else if (te == 2) {
					printf("Player 2 Wins!!\n");
					close(client_fd);
					exit(0);
				}
			}	
			
			write(client_fd, clientWrite, sizeof(clientWrite));
			system("clear");
			showBoard(playBoard);
			printf("\nCurrent Play Board\n\n");
			if (te == 1) {
				printf("Player 1 Wins!!\n");
				close(client_fd);
				exit(0);
			} else if (te == 2) {
				printf("Player 2 Wins!!\n");
				close(client_fd);
				exit(0);
   			}
		}

		if (read(client_fd, clientRead, sizeof(clientRead)) > 0) {
			system("clear");
			memcpy(playBoard, clientRead, sizeof(playBoard));	// copy the contents of the array received from server side in playBoard array
			showBoard(playBoard);
			printf("\nCurrent Play Board\n\n");
			input = '\n';
			if (te == 1) {
				printf("Player 1 Wins!!\n");
				close(client_fd);
				exit(0);
			} else if (te == 2) {
				printf("Player 2 Wins!!\n");
				close(client_fd);
				exit(0);
   			}
		} else {
			printf("You Win!! Thank You, Please Play Again :D\n");
			close(client_fd);
			exit(0);
		}
	}
}

void showBoard(char playBoard[][3])
{
        printf("\n");
        printf("      |     |       \n");
        printf("   %c  |  %c  |  %c   \n", playBoard[0][0], playBoard[0][1], playBoard[0][2]);
        printf(" _____|_____|_____ \n");
        printf("      |     |      \n");
        printf("   %c  |  %c  |  %c   \n", playBoard[1][0], playBoard[1][1], playBoard[1][2]);
        printf(" _____|_____|_____ \n");
        printf("      |     |      \n");
        printf("   %c  |  %c  |  %c   \n", playBoard[2][0], playBoard[2][1], playBoard[2][2]);
        printf("      |     |      \n");
}

void playerWinhandler(int signum) {
    if (signum == SIGUSR1) {
        te = 1;
    } else if (signum == SIGUSR2) {
		te = 2;
    }
}

int menu() {
	int reply;

	printf("Enter 1 to Select Player.\n\n");
	printf("Enter 2 to Quit.\n\n");

	scanf("%d", &reply);

	return reply;
}