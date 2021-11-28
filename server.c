#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h> 
#include <string.h>

#define SIZE sizeof(struct sockaddr_in) 

void check (char playBoard[][3]);
void catcher(int sig); 
int newsockfd[2]; 
int pid[2];

int main(void) { 
	int sockfd[2], 
		usr=0,
		ctr=1,
		row = 0,
		column = 0,
		choice=0,
		status;
	char serverRead[1],
		ans[1],
		x[4],
		a[2][40],
    	playBoard[3][3] = {							// to display the actual game status
			{' ',' ',' '},
			{' ',' ',' '},
			{' ',' ',' '}
		};
	
	struct sockaddr_in server = {AF_INET, 8001, INADDR_ANY}; 
	strcpy(ans,"");
	static struct sigaction action; 
	action.sa_handler = catcher; 
	sigfillset(&(action.sa_mask)); 
	sigaction(SIGPIPE, &action, 0); 
	sigaction(SIGTSTP, &action, 0);
	sigaction(SIGINT, &action, 0);

	if ((sockfd[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
		perror("Socket Call Failed"); 
		exit(1); 
	}

	if (bind(sockfd[0], (struct sockaddr *)&server, SIZE) == -1) { 
		perror("Bind Call Failed"); 
		exit(1); 
	}
	
	printf("Waiting for Players to join in TicTacToe Club.. \n");

	strcpy(a[0],"Waiting for the other Player to join\n");

	while(usr < 2)
	{
		if (listen(sockfd[0], 5) == -1) { 
			perror("Listen Call Failed\n"); 
			exit(1) ; 
		}
		
		newsockfd[usr] = accept(sockfd[0], 0, 0);
		usr++;

		if (usr == 1) {	
			strcpy(a[1],"0");
			write(newsockfd[0],a,sizeof(a));
			read(newsockfd[0],x,sizeof(x));
			pid[0]=atoi(x);
		}

		printf("No. of Players who joined the Club: %d\n",usr);
	
		if (usr == 2) {
			strcpy(a[0],"Let's play Tic Tac Toe!!");
			strcpy(a[1],"1");
			write(newsockfd[0],a,sizeof(a));
			strcpy(a[1],"2");
			write(newsockfd[1],a,sizeof(a));
			read(newsockfd[1],x,sizeof(x));
			pid[1]=atoi(x);
		} 	
	}	
	
	if (!fork()) {
		int count=0; 		
		
		while (count==0) { 	
			read(newsockfd[ctr], serverRead, sizeof(serverRead));			
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
			
			write(newsockfd[ctr],playBoard,sizeof(playBoard)); 
			check(playBoard);
		}
		
		close(newsockfd[0]);
		exit(0);
	} 	
	wait(&status);
	close(newsockfd[1]);
}

void check (char playBoard[][3]) {
	int i;
	char key = ' ';

	// Check Rows
	for (i = 0; i < 3; i++)
	if (playBoard [i][0] == playBoard [i][1] && playBoard [i][0] == playBoard [i][2] && playBoard [i][0] != ' ') key = playBoard [i][0];	
	// check Columns
	for (i = 0; i < 3; i++)
	if (playBoard [0][i] == playBoard [1][i] && playBoard [0][i] == playBoard [2][i] && playBoard [0][i] != ' ') key = playBoard [0][i];
	// Check Diagonals
	if (playBoard [0][0] == playBoard [1][1] && playBoard [1][1] == playBoard [2][2] && playBoard [1][1] != ' ') key = playBoard [1][1];
	if (playBoard [0][2] == playBoard [1][1] && playBoard [1][1] == playBoard [2][0] && playBoard [1][1] != ' ') key = playBoard [1][1];

	if (key == 'X') {
		printf("Player 1 Wins\n\n");
			kill(pid[0], SIGUSR1);
		kill(pid[1], SIGUSR1);
		exit (0); 
	}

	if (key == 'O') {
		printf("Player 2 Wins\n\n");
		kill(pid[0], SIGUSR2);
		kill(pid[1], SIGUSR2);
		exit (0);
	}
	
}

void catcher(int sig) { 
	printf("Quitting\n");
}

