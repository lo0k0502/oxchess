							/*Tic Tac Toe Game by Ishan Ankita*/
									/*Client*/ 

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

int play(char *username, char *password);
void showBoard(char playBoard[][3]);
void playerWinhandler(int signum);
int menu();
int pid;
int te = 0;

int main()
{
	char username[10], password[10];

	printf("Please enter username: ");
	scanf("%s", username);
	printf("Please enter password: ");
	scanf("%s", password);
	
	play(username, password);
}

int play(char *username, char *password)
{
	 
	printf("Username: %s, Password: %s\n", username, password);

	static struct sigaction act; 
	act.sa_handler = SIG_IGN; 
	sigfillset(&(act.sa_mask));
	sigaction(SIGTSTP, &act, 0);

	signal(SIGUSR1, playerWinhandler);
	signal(SIGUSR2, playerWinhandler);

	struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8001);

	int sockfd, i; 
	int row, column, choice;
	int iclientRead; 		
	char input; 
	char readBuffer[2][40];
	char writeBuffer[10];
	char buffer[10];
	char pid[4];
	char clientRead[3][3];
	char clientWrite[1];
	

	char numberBoard [3][3] = {
		{'1','2','3'},
		{'4','5','6'},
		{'7','8','9'}
	}; // to display positions to choose from

	char playBoard [3][3] = {
		{' ',' ',' '},
		{' ',' ',' '},
		{' ',' ',' '}
	}; // to display the actual game status


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket Call Failed");
		exit(1);
	}

	if (connect (sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) 
	{ 
		perror("Connect Call Failed");
		exit(1);
	}

	write(sockfd, username, sizeof(username));
	sleep(1);
	write(sockfd, password, sizeof(password));
	
	read(sockfd,readBuffer,sizeof(readBuffer));

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
			write(sockfd, pid, sizeof(pid));
			printf("Logged In!!\n");
		}
	}

	for (i = 0;;) {
		switch(menu()) {
			case 1: {
				i++;
				strcpy(writeBuffer, "1");
				write(sockfd, writeBuffer, sizeof(writeBuffer));
				break;
			}
			case 2: {
				printf("\nQuitting\n\n");
				strcpy(writeBuffer, "2");
				write(sockfd, writeBuffer, sizeof(writeBuffer));
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

	read(sockfd, readBuffer, sizeof(readBuffer));
	printf("Online Players:\n");
	printf("%s\n", readBuffer[0]);
	
	if (!strcmp(readBuffer[0], "No other players right now...")) {
		read(sockfd, readBuffer, sizeof(readBuffer));
		printf("Accept? (y/n) ");
		for (; strcmp(clientWrite, "y");) {
			scanf("%s", clientWrite);
		}
		write(sockfd, "y", sizeof("y"));
	} else {
		printf("Please enter a user's name: ");
		scanf("%s", buffer);
		write(sockfd, buffer, sizeof(buffer));
	}
	printf("End\n");

	read(sockfd, readBuffer, sizeof(readBuffer));
	printf("Begin: %s\n", readBuffer[0]);
	printf("Begin: %s\n", readBuffer[1]);
	
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

		write(sockfd, clientWrite, sizeof(clientWrite));
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
				close(sockfd);
				exit(0);
			} else if (te == 2) {
				printf("Player 2 Wins!!\n");
				close(sockfd);
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
					close(sockfd);
					exit(0);
				} else if (te == 2) {
					printf("Player 2 Wins!!\n");
					close(sockfd);
					exit(0);
				}
			}	
			
			write(sockfd, clientWrite, sizeof(clientWrite));
			system("clear");
			showBoard(playBoard);
			printf("\nCurrent Play Board\n\n");
			if (te == 1) {
				printf("Player 1 Wins!!\n");
				close(sockfd);
				exit(0);
			} else if (te == 2) {
				printf("Player 2 Wins!!\n");
				close(sockfd);
				exit(0);
   			}
		}

		if (read(sockfd, clientRead, sizeof(clientRead)) > 0) {
			system("clear");
			memcpy(playBoard, clientRead, sizeof(playBoard));	// copy the contents of the array received from server side in playBoard array
			showBoard(playBoard);
			printf("\nCurrent Play Board\n\n");
			input = '\n';
			if (te == 1) {
				printf("Player 1 Wins!!\n");
				close(sockfd);
				exit(0);
			} else if (te == 2) {
				printf("Player 2 Wins!!\n");
				close(sockfd);
				exit(0);
   			}
		} else {
			printf("You Win!! Thank You, Please Play Again :D\n");
			close(sockfd);
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
		printf("Player 1 Wins!!\n");
    }

    if (signum == SIGUSR2) {
		te = 2;
		printf("Player 2 Wins!!\n");
    }
	kill(pid, 9);
}

int menu() {
	int reply;

	printf("Enter 1 to Select Player.\n\n");
	printf("Enter 2 to Quit.\n\n");

	scanf("%d", &reply);

	return reply;
}