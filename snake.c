/* 
 * coded by hatchet - 4th May 2024
 *
 * this is a step towards writing a game of snake.
 * At the moment this creates a snake of 10 characters that you can move around with the arrow keys
 *
 * todo:
 * 	* done * create collission detection, if the snake hits itself, or if it hits the edge of the screen
 * 	remove the bug where the unused elements of the snake show up at location 0,0 (the top left of the screen)
 * 		need to do a loop to update the snake array so the snake is visable at the start of the game
 * 		rather than just the first character
 * 	add fruit for the snake to eat, when it eats a fruit it gets longer
 * 	* done * add a timer so the snake keeps moving on its own
 * 	add colour, the fruit at least should be coloured
 * 	add an intro screen
 * 	* done * add a game over screen
 * 	add sound
 * 	* done * maybe be able to change the size of the screen, ie 100x100 or something like that, maybe a bit smaller
 * 	
 */

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SNAKE_START_LEN 5

// the colors that will be used through the game for different items
#define SNAKE_COLOR	1
#define BORDER_COLOR	2
#define SCORE_COLOR	3
#define FRUIT_COLOR	4 

#define SCREEN_WIDTH	15
#define SCREEN_HEIGHT	30

// this structure is our snake
// we will add a few more details here, ie: has there been a collission maybe...
typedef struct {
	int x;
	int y;

}snakestr;


// this function does two things
// 	it adds an extra element to the snake
// 	it also clears the screen, calls drawborder(), draws the snake and then calls refresh()
int printsnake(snakestr *snake, int snakelen, snakestr *fruit, snakestr tempa, snakestr tempb);

// this function draws the border
int drawborder();

/// returns an integer that can be used to identify the collission type
//      returns 0 if the game is not yet over
//      returns 1 for collission with the snake
//      returns 2 for a collission with the border
//      returns 3 for having eaten a fruit
int collision(snakestr *snake, snakestr *fruit, int snakelen);

// this writes the next location of the fruit into &fruit
int getnextfruit(snakestr *snake, snakestr *fruit, int snakelen);

// prints the game over screen and return 1 if the player wants to play again
// or returns 0 if they choose not to play again
int gameover(int code, int snakelen);

// prints the intro screen
int introscreen();

int main(int argc, char *argv[]){

	int inputchar;
	int lastmove;

	// this variable is used to save the type of collission when the player 
	// hits the wall, bits themself or eats a fruit
	int coltype;	

	// this variable keeps track of the length of the snake
	int snakelen = SNAKE_START_LEN;

	// these are both used in the loop when extending the snake 
	snakestr tempa;
	snakestr tempb;


	// this structure is used with the nanosleep() function
	// change the second element to change the refresh time for the game
	struct timespec timer = {0, 500000000};

	// this holds the location of the next fruit
	snakestr fruit;

	// the array of the snake
	// this needs to be of a size that would let you fill the screen with the snake
	// the lenght of the array is to allow the snake to fill the screen
	snakestr snake[(SCREEN_WIDTH - 2) * (SCREEN_HEIGHT - 2)];
	
	initscr();

	// this sets the size of the screen
	resize_term(SCREEN_WIDTH, SCREEN_HEIGHT);

	start_color();
	init_pair(SNAKE_COLOR, COLOR_BLUE, COLOR_BLACK);
	init_pair(BORDER_COLOR, COLOR_YELLOW, COLOR_BLACK);
	init_pair(FRUIT_COLOR, COLOR_GREEN, COLOR_BLACK);
	init_pair(SCORE_COLOR, COLOR_RED, COLOR_BLACK);
	
	// turn off echoing characters when they are typed
	noecho();

	// this removes the flashing curser you normally have when typing
	curs_set(0);

	// this allows the keypad to be used
	keypad(stdscr, TRUE);

	introscreen();

	// this goto is used for when the player dies and chooses to play again
startgame:

	// this makes getch() not wait for input
	// it will check if a character has been input and if there has been a key hit then
	// it is return, otherwise it is null 
	nodelay(stdscr, TRUE);

	// set both arrays to zero
	memset(&fruit, 0, sizeof(fruit));
	memset(snake, 0, sizeof(snake));

	// this needs to be initialised again for when the player dies and plays again
	snakelen = SNAKE_START_LEN;

	// draw the border around the screen
	drawborder();

	// we set the snake starting point to be in the middle of the screen
	snake[0].x = LINES / 2;
	snake[0].y = COLS / 2;
	

	// temp a is used to keep a track of the head of the snake
	tempa.x = snake[0].x;
	tempa.y = snake[0].y;


	// ensure that we start with something on the screen
	// this places an x at the centre of the screen which is were the game starts
	attron(COLOR_PAIR(SNAKE_COLOR));
	mvprintw(snake[0].x, snake[0].y, "X");
	attroff(COLOR_PAIR(SNAKE_COLOR));
	refresh();

	// get the first fruit location
	getnextfruit(snake, &fruit, snakelen);

	// when the game starts, the first move is up
	inputchar = KEY_UP;

	while(1){
		
		// the reason for this is to ensure that we will always make a move on every move
		// we only update last move if the character entered has been an arrow
		// otherwise the snake will stop moving
		switch(inputchar){
			case KEY_DOWN:
			case KEY_UP:
			case KEY_LEFT:
			case KEY_RIGHT:
				lastmove = inputchar;
				nanosleep(&timer, NULL);
				break;
			default:
				break;
		}

		inputchar = getch();

		// if no key has been pressed then getch returns ERR
		// if that happens then we copy the last move to inputchar
		// this ensures that the snake will always move in the last direction that was used
		if (inputchar == ERR)
			inputchar = lastmove;

			// the switch is to recognise which keys have been pressed
			switch(inputchar){
				case KEY_DOWN:

					// this is done to ensure that tempa is already holding what will be the next value for the head of the snake
					tempa.x++;

					printsnake(snake, snakelen, &fruit, tempa, tempb);
					// mvprintw(0, 0, "%d", fruit.x);
					// mvprintw(0, 3, "%d", fruit.y);
					refresh();
				
					break;

				case KEY_UP:

					tempa.x--;

					printsnake(snake, snakelen, &fruit, tempa, tempb);
					// mvprintw(0, 0, "%d", fruit.x);
					// mvprintw(0, 3, "%d", fruit.y);
					refresh();
					
					break;

				case KEY_LEFT:

					tempa.y--;

					printsnake(snake, snakelen, &fruit, tempa, tempb);
					// mvprintw(0, 0, "%d", fruit.x);
					// mvprintw(0, 3, "%d", fruit.y);
					refresh();
		
					break;

				case KEY_RIGHT:


					tempa.y++;

					printsnake(snake, snakelen, &fruit, tempa, tempb);
					// mvprintw(0, 0, "%d", fruit.x);
					// mvprintw(0, 3, "%d", fruit.y);
					refresh();

					break;
				
				// hitting 'q' ends the program
				case 'q':
					endwin();
					return 0;

				case 'm':
					//snakelen++;
						break;

				default:
					break;

			}

			// check if there has been a collision
			coltype = collision(snake, &fruit, snakelen);


			// end the game if appropriate
			switch(coltype){
				// game over if we have bitten ourself
				case 1:
					if(gameover(coltype, snakelen))
						goto startgame;

					endwin();
					return -1;
				// game over if the wall gets hit
				case 2:
					if(gameover(coltype, snakelen))
						goto startgame;

					endwin();
					return -1;

				// not game over, a fruit has been eaten
				case 3:
					getnextfruit(snake, &fruit, snakelen);
					snakelen++;

				// nothing happened, no collission has been made
				case 0:
					break;
			}
			
	}

	endwin();
	return 0;
}


int printsnake(snakestr *snake, int snakelen, snakestr *fruit, snakestr tempa, snakestr tempb){
	int counter = 0;

	clear();

	// print the fruit 
	attron(COLOR_PAIR(FRUIT_COLOR));
	mvprintw(fruit->x, fruit->y, "O");
	attroff(COLOR_PAIR(FRUIT_COLOR));


	while(counter < snakelen){
		attron(COLOR_PAIR(SNAKE_COLOR));
		mvprintw(tempa.x, tempa.y, "X");
		attroff(COLOR_PAIR(SNAKE_COLOR));


		// the next 6 lines are to do the following:
		// save the first element in th snake
		tempb.x = snake[counter].x;
		tempb.y = snake[counter].y;

		
		// copy the saved element to the start of the snake
		snake[counter].x = tempa.x;
		snake[counter].y = tempa.y;


		// have both of the temp structures equal each other
		tempa.x = tempb.x;
		tempa.y = tempb.y;

		counter++;
	}	

	drawborder();

	attron(COLOR_PAIR(SCORE_COLOR));

	// print the score at the top of the screen
	mvprintw(0, 3, "  Score %d   ", snakelen - SNAKE_START_LEN);
	attroff(COLOR_PAIR(SCORE_COLOR));


	refresh();
		
	return 0;
}

int drawborder(){

	int counter;

	attron(COLOR_PAIR(BORDER_COLOR));

	// this draws the line across the top and then the line on the bottom
	for(counter = 0; counter < COLS; counter++){
		mvprintw(0, counter, "*");
		mvprintw(LINES - 1, counter, "*");
	}

	// this draws the line on the left hand side and then the line on the right hand border
	for(counter = 0; counter < LINES; counter++){
		mvprintw(counter, 0, "*");
		mvprintw(counter, COLS - 1, "*");
	}

	attroff(COLOR_PAIR(BORDER_COLOR));

	return 0;
}

// returns an integer that can be used to identify the collission type
//      returns 0 if the game is not yet over
//      returns 1 for collission with the snake
//      returns 2 for a collission with the border
//      returns 3 for having eaten a fruit
int collision(snakestr *snake, snakestr *fruit, int snakelen){
        int counter;


	// this loop checks to see if the snake[0] is the same as any other element in the snake[0] array
	// this is to check if the snake has bitten itself
        for(counter = 1; counter < snakelen; counter++){
                if((snake[0].x == snake[counter].x) && (snake[0].y == snake[counter].y))
                        return 1;
        }

	// these if staments check to see if the snake has hit the border
        if((snake[0].x == 0) || (snake[0].x == LINES - 1))
                return 2;
        if((snake[0].y == 0) || (snake[0].y == COLS - 1))
                return 2;

	// this if statement is to check if a fruit has been eaten
	if((snake[0].x == fruit->x) && (snake[0].y == fruit->y))
		return 3;

	// return 0 if there has been no collission
        return 0;
}


// this function does finds the location of the next fruit to be eaten
// the use of the int randomseed is to ensure that we always get a different set of random numbers to work with
// otherwise we may end up in a loop for a second wating for another random number, this would make the game hang
int getnextfruit(snakestr *snake, snakestr *fruit, int snakelen){
	int counter;
	int randomseed = snakelen;

	while (1){
	jumpline:
		// seed the random number generator
		// the use of randomseed is to ensure that we always get a different number
		
		srand(time(NULL) + randomseed++);

		// get a random location for x
		// minus two to remove the borders
		// then add 1 so we have a random number on the line between the borders
		fruit->x = rand() % (LINES- 2);
		fruit->x++;

		srand(time(NULL) + randomseed++);

		// same as above for y
		fruit->y = rand() % (COLS - 2);
		fruit->y++;

		// this loop checks that there is not a random number which is on the snake 
		for(counter = 0; counter < snakelen; counter++){
			if((fruit->x == snake[counter].x) && (fruit->y == snake[counter].y)){
				goto jumpline;
			}
		}

		return 0;
	}
}

// this function displays a message at game over and gives a chance to play again
// if the function returns 0 then you don't want to play again
// when it returns 1 then another game has been requested
int gameover(int code, int snakelen){
	int input;


	switch(code){
		// the player has bitten themself
		case 1:
			clear();
			drawborder();

			attron(COLOR_PAIR(FRUIT_COLOR));
			mvprintw(2, 2, "       Game Over");
      			mvprintw(3, 2, "    You bit yourself");
      			mvprintw(5, 2, "   Were you couragous?");
			attroff(COLOR_PAIR(FRUIT_COLOR));
			
			attron(COLOR_PAIR(SCORE_COLOR));
      			mvprintw(7, 2, "   You scored %d points", snakelen - SNAKE_START_LEN);
			attroff(COLOR_PAIR(SCORE_COLOR));

			attron(COLOR_PAIR(FRUIT_COLOR));
			mvprintw(9, 2, "   Want to play again?");
			mvprintw(10, 2, "     Hit 'Y' or 'N'");
			attroff(COLOR_PAIR(FRUIT_COLOR));

			refresh();

			while(1){
	 			nodelay(stdscr, FALSE);
				input = getch();

				switch(input){
					case 'y':
					case 'Y':
						clear();
						return 1;
						
					case 'n':
					case 'N':
						endwin();
						return 0;
				}
			}

			break;
			
		// the player has hit the wall
		case 2:
			clear();
			drawborder();

			attron(COLOR_PAIR(FRUIT_COLOR));
			mvprintw(2, 2, "       Game Over");
      			mvprintw(3, 2, "    You hit the wall");
      			mvprintw(5, 2, "   Were you couragous?");
			attroff(COLOR_PAIR(FRUIT_COLOR));
			
			attron(COLOR_PAIR(SCORE_COLOR));
      			mvprintw(7, 2, "   You scored %d points", snakelen - SNAKE_START_LEN);
			attroff(COLOR_PAIR(SCORE_COLOR));

			attron(COLOR_PAIR(FRUIT_COLOR));
			mvprintw(9, 2, "   Want to play again?");
			mvprintw(10, 2, "     Hit 'Y' or 'N'");
			attroff(COLOR_PAIR(FRUIT_COLOR));

			refresh();

			while(1){
	 			nodelay(stdscr, FALSE);
				input = getch();

				switch(input){
					case 'y':
					case 'Y':
						clear();
						return 1;
						
					case 'n':
					case 'N':
						endwin();
						return 0;
				}
			}


		// this should never be called
		default:
			return 0;
	}

	return 0;
}

int introscreen(){

	clear();
	drawborder();


	attron(COLOR_PAIR(FRUIT_COLOR));
	mvprintw(2, 2, "    Welcome to Snake");
      	mvprintw(3, 2, " A game for the couragous");
      	mvprintw(5, 2, "  Hit any key to start");
	mvprintw(7, 2, "    coded by Hatchet");
	mvprintw(8, 2, "        May 2024");


	attroff(COLOR_PAIR(FRUIT_COLOR));

	// turn off nodelay before calling getch
	// to ensure that the play has to hit a key and we have the delay
	nodelay(stdscr, FALSE);
	getch();
	nodelay(stdscr, TRUE);
	
	clear();
	return 0;
}
