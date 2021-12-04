#include "utility.h"

void showBoard(char playBoard[][3]) {
        printString("");
        printString("      |     |       ");
        printf("   %c  |  %c  |  %c   \n", playBoard[0][0], playBoard[0][1], playBoard[0][2]);
        printString(" _____|_____|_____ ");
        printString("      |     |      ");
        printf("   %c  |  %c  |  %c   \n", playBoard[1][0], playBoard[1][1], playBoard[1][2]);
        printString(" _____|_____|_____ ");
        printString("      |     |      ");
        printf("   %c  |  %c  |  %c   \n", playBoard[2][0], playBoard[2][1], playBoard[2][2]);
        printString("      |     |      ");
}

int menu() {
	int reply;

	printString("Enter 1 to Select Player.\n");
	printString("Enter 2 to Quit.\n");

	scanf("%d", &reply);

	return reply;
}

char check (char playBoard[][3]) {
	int i, j, flag = 0;
	char key = ' ';

	// Check Rows
	for (i = 0; i < 3; i++) {
		if (playBoard[i][0] == playBoard[i][1] && playBoard[i][0] == playBoard[i][2] && playBoard[i][0] != ' ') {
            key = playBoard[i][0];
        }
	}

	// check Columns
	for (i = 0; i < 3; i++) {
		if (playBoard [0][i] == playBoard [1][i] && playBoard[0][i] == playBoard[2][i] && playBoard[0][i] != ' ') {
            key = playBoard[0][i];
        }
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
		
		if (!flag) {
			key = 'T';
		}
	}

	return key;
}