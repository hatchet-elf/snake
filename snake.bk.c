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

#define SNAKE_START_LEN 100

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

int getnextfruit(snakestr *snake, snakestr *fruit, int snakelen);

int main(int argc, char *argv[]){

	int inputchar;
	int lastmove;
	int gameover;

	// we start with the snake 5 characters long
	int snakelen = SNAKE_START_LEN;

	// these are both used in the loop when extending the snake 
	snakestr tempa;
	snakestr tempb;

	snakestr fruit;
	memset(&fruit, 0, sizeof(fruit));

	// the array of the snake
	// this needs to be of a size that would let you fill the screen with the snake
	snakestr snake[1000];
	memset(snake, 0, sizeof(snake));

	initscr();

	// this sets the size of the screen
	resize_term(20, 30);

/*
	getnextfruit(snake, &fruit, snakelen);
	printw("fruit.x %d", fruit.x);
	printw("fruit.y %d", fruit.y);
	getch();
	endwin();
	return 0;
*/
	// draw the border around the screen
	drawborder();

	// we set the snake starting point to be in the middle of the screen
	snake[0].x = LINES / 2;
	snake[0].y = COLS / 2;
	

	// temp a is used to keep a track of the head of the snake
	tempa.x = snake[0].x;
	tempa.y = snake[0].y;

	// this allows the keypad to be used
	keypad(stdscr, TRUE);

	// this removes the flashing curser you normally have when typing
	curs_set(0);

	// turn off echoing characters when they are typed
	noecho();

	// this makes getch() not wait for input
	// it will check if a character has been input and if there has been a key hit then
	// it is return, otherwise it is null 
	nodelay(stdscr, TRUE);

	// ensure that we start with something on the screen
	// this places an x at the centre of the screen which is were the game starts
	mvprintw(snake[0].x, snake[0].y, "X");
	refresh();

	// get the first fruit location
	getnextfruit(snake, &fruit, snakelen);

	// when the game starts, the first move is up
	inputchar = KEY_UP;

	// this structure is used with the nanosleep() function
	// change the second element to change the refresh time for the game
	struct timespec timer = {0, 500000000};

	

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
			gameover = collision(snake, &fruit, snakelen);

			// end the game if there has been a collision
			// neaten the screen and display a message
			switch(gameover){
				// game over if we have bitten ourself
				case 1:
					clear();
					drawborder();
					mvprintw(2, 2, "Game Over");
				       	mvprintw(3, 2, "you bit yourself");
					nodelay(stdscr, FALSE);
					getch();
					endwin();
					return -1;
				// game over if the wall gets hit
				case 2:
					clear();
					drawborder();
					mvprintw(2, 2, "Game Over");
				       	mvprintw(3, 2, "you hit the wall");
					nodelay(stdscr, FALSE);
					getch();
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
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	attron
	mvprintw(fruit->x, fruit->y, "O");

	while(counter < snakelen){
		mvprintw(tempa.x, tempa.y, "X");
		drawborder();


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

	// print the score at the top of the screen
	mvprintw(0, 3, "  Score %d   ", snakelen - SNAKE_START_LEN);
	refresh();
		
	return 0;
}

int drawborder(){

	int counter;

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
