// Pacman.cpp By: Chris Galletta

#include "stdafx.h"
#include <iostream>
#include "GL/glut.h"
#include "GL/GL.H"
#include <math.h>

using namespace std;

const int boardX = 20;// width of maze
const int boardY = 20;// height of maze
int pellets[boardX][boardY] = 
{
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // row 0
	{1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // row 1
	{1,0,1,1,0,1,1,1,1,0,1,0,1,0,1,1,1,1,0,1}, // row 2
	{1,0,1,1,0,1,1,1,1,0,1,0,1,0,1,1,1,1,0,1}, // row 3
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // row.. etc
	{1,0,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1},
	{1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
	{1,1,1,1,0,1,0,0,0,0,0,1,0,1,0,1,1,1,0,1},
	{1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,0,0,1},
	{1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1},
	{0,0,0,1,1,1,0,1,1,1,1,0,0,1,0,0,0,0,0,0},
	{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
	{1,0,1,1,0,1,0,1,1,0,0,0,0,1,0,1,1,1,1,1},
	{1,0,1,1,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1},
	{1,0,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
	{1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{1,0,0,0,0,1,0,0,1,0,1,1,0,1,1,0,0,0,0,1},
	{1,0,1,1,1,1,1,0,1,0,1,1,0,1,1,1,1,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

const int height = 780; // window height
const int width = 780; // window width
bool isGoingDown = true; // is moving down

void *font = GLUT_BITMAP_TIMES_ROMAN_24;
void *fonts[] =
{
	GLUT_BITMAP_9_BY_15,
	GLUT_BITMAP_8_BY_13,
	GLUT_BITMAP_TIMES_ROMAN_10,
	GLUT_BITMAP_TIMES_ROMAN_24
};

void displayFirst();// initial display of objects on screen
void display(); // translation display of objects on screen
void rotatePac(); // rotates objects
void init(); // initializes window
void drawButton(); // draws button on screen
void drawBoard(); // draw maze/game board and eatible pellets
void drawMouth(); // draw pac mouth
void handleButton(int button, int state, int x, int y); // handler for button actions
void drawPac();// draw pacman
void translate(); // move player
void processNormalKeys(unsigned char key, int mx, int my);
void processSpecialKeys(int key, int mx, int my);
void displayText( float x, float y, int r, int g, int b, const char *string ) ;
void checkPelletCollision(); // detect if player is colliding w/ pellet
bool checkCollision(int x, int y); // detect if player is colliding w/ wall
void initLevel();

static float PI = 3.1459; // approx for PI
static float radius = 75; // radius of circle
static float INC = (PI/30); // angle measure for circle definition
static GLfloat deltax = 0.0; // change in x
static GLfloat deltay = 0.0; // change in y
static GLfloat deltaz = .001;// change in z

// Pacman/Player variables
bool isMouthOpen = false;// is pacmans mouth open?
bool isLeft = false; // is pac moving left?
bool isRight = false; // is pac moving right?
bool isUp = false; // is pac moving up?
bool isDown = false; // is pac moving down?

int pacBoardX = 1;// pac x position on pellet[][] board
int pacBoardY = 1;// pac y position on pellet[][] board
static GLfloat pacX = 16.0 + (30 * pacBoardX); // player charcter position x
static GLfloat pacY = 16.0 + (30 * pacBoardY); // player charcter position y

int amtPellets = 0; // total amount of pellets a player can eat in the level
int score = 0;// total score player has accumulated

struct Pellet
{
	int x; // x position on screen
	int y; // y position on screen
	int boardX;// x position on game board
	int boardY;// y position on game board
	bool isVisible;// is it visible in the game?
};

Pellet pelletEntities[boardX*boardY]; // store pellets and their positions in level

void main(int argc, char** argv)
{
	glutInit(&argc, argv);// inits GLUT and needs to be called before any GLUT functions
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);// init glut lib files
	glutInitWindowPosition(0,0);// init size of viewing window
	glutCreateWindow("Pacman");// positions window at upper left hand corner

	glutDisplayFunc(displayFirst);// called each time there is a display call back
	glutMouseFunc(handleButton); // init handler for input
	glutKeyboardFunc(processNormalKeys); // init normal keyboard keys handler
	glutSpecialFunc(processSpecialKeys); // init special keyboard keys handler

	init();// init projection/camera
	initLevel(); // init game/level before start
	glutMainLoop();//should be last func called in main. Causes program to enter an infinite loop

	return ;
}

/*******************************************
	Init display func when game first starts
********************************************/
void displayFirst()
{
	glClear(GL_COLOR_BUFFER_BIT);// clears all buffers whose bits are set in mask	
		
	drawBoard(); // draw maze/level
	drawPac(); // draw player character Pac
	drawButton();// draw input button

	glutSwapBuffers(); // swap framebuffs for drawing current calls
	glFlush(); // force execution of OpenGL functions 

	return;
}

/**************************************************
	Draws the arrow button and score text to screen
***************************************************/
void drawButton()
{
	// Display current player score to screen
	displayText(200, 650, 1, 1, 1, "Score: ");
	char scoreStr[15];
	sprintf_s(scoreStr, "%d", score);// cast score to new char array variable
	displayText(268, 650, 1, 1, 1, scoreStr);

	// alt display control tip adjacent to arrow button
	displayText(96, 716, 1, 1, 1, "S");
	displayText(110, 620, 1, 1, 1, "W");
	displayText(30, 660, 1, 1, 1, "A");
	displayText(160, 660, 1, 1, 1, "D");

	// Down arrow
	glBegin(GL_POLYGON);
		glColor4f(0,1,0,0);
		glVertex2i(103, 695);
		glColor4f(0,1,0,.25);
		glVertex2i(113, 675);
		glColor4f(0, 1, 0, .5);
		glVertex2i(108, 675);
		glColor4f(0, 1, 0, 1);
		glVertex2i(108, 655);
		glColor4f(0, 1, 0, 1);
		glVertex2i(98, 655);
		glColor4f(0, 1, 0, .5);
		glVertex2i(98, 675);
		glColor4f(0, 1, 0, .25);
		glVertex2i(93, 675);
	glEnd();

	// Up arrow
	glBegin(GL_POLYGON);
		glColor4f(0, 1, 0, 0);
		glVertex2i(103, 605);
		glColor4f(0, 1, 0, .25);
		glVertex2i(113, 625);
		glColor4f(0, 1, 0, .5);
		glVertex2i(108, 625);
		glColor4f(0, 1, 0, 1);
		glVertex2i(108, 645);
		glColor4f(0, 1, 0, 1);
		glVertex2i(98, 645);
		glColor4f(0, 1, 0, .5);
		glVertex2i(98, 625);
		glColor4f(0, 1, 0, .25);
		glVertex2i(93, 625);
	glEnd();

	// Right arrow
	glBegin(GL_POLYGON);
		glColor4f(0, 1, 0, 0);
		glVertex2i(158, 650);
		glColor4f(0, 1, 0, .25);
		glVertex2i(138, 640);
		glColor4f(0, 1, 0, .5);
		glVertex2i(138, 645);
		glColor4f(0, 1, 0, 1);
		glVertex2i(118, 645);
		glColor4f(0, 1, 0, 1);
		glVertex2i(118, 655);
		glColor4f(0, 1, 0, .5);
		glVertex2i(138, 655);
		glColor4f(0, 1, 0, .25);
		glVertex2i(138, 660);
	glEnd();

	// draw left arrow
	glBegin(GL_POLYGON);
		glColor4f(0, 1, 0, 0);
		glVertex2i(48, 650);
		glColor4f(0, 1, 0, .25);
		glVertex2i(68, 640);
		glColor4f(0, 1, 0, .5);
		glVertex2i(68, 645);
		glColor4f(0, 1, 0, 1);
		glVertex2i(88, 645);
		glColor4f(0, 1, 0, 1);
		glVertex2i(88, 655);
		glColor4f(0, 1, 0, .5);
		glVertex2i(68, 655);
		glColor4f(0, 1, 0, .25);
		glVertex2i(68, 660);
	glEnd();
}

/***********************************************
 * Processes input for normal keyboard keys
 ***********************************************/
void processNormalKeys(unsigned char key, int mx, int my)
{
	 if(key == 'E')
	 {
		 exit(-1); 
	 }

	 switch(key)
	 {
		case 'a' : // left
			isLeft = true;
			isRight = false;
			isUp = false;
			isDown = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}

			if(!checkCollision(-3, 0))// check for collision 3 pixels to the left
			{
 				pacX -= 4;
				deltax -= 4;
				glutIdleFunc(translate);
			}

			break;

		case 'd' : // right

			isRight = true;
			isLeft = false;
			isUp = false;
			isDown = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}

			if(!checkCollision(30, 0))// check for collision 30 pixels to the right
			{
				pacX +=4;
				deltax +=4;
				glutIdleFunc(translate);
						

			}

			break;

		case 'w' : // up

			isUp = true;
			isDown = false;
			isLeft = false;
			isRight = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}

			if(!checkCollision(0, -6)) // check for collision 6 pixels above
			{
				pacY -= 4;
				deltay -=4;
				glutIdleFunc(translate);
			}
			break;

		case 's' : // down

			isDown = true;
			isUp = false;
			isLeft = false;
			isRight = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}
		
			if(!checkCollision(0, 30))// check for collision 30 pixels below
			{
				pacY += 4;
				deltay += 4;
				glutIdleFunc(translate);
			}	

			break;
		default: 
			break;
		}
}

/***********************************************
  Processes input for special keyboard keys
  key: ascii number associated with char  
 ***********************************************/
void processSpecialKeys(int key, int mx, int my)
{
	 switch(key)
	 {
		case GLUT_KEY_LEFT :

			isLeft = true;
			isRight = false;
			isUp = false;
			isDown = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}

			if(!checkCollision(-3, 0)) // check 3 pixels ahead of current x position, if no collision, move.
			{
 				pacX-=4;
				deltax-=4;
				glutIdleFunc(translate);
			}

			break;

		case GLUT_KEY_RIGHT :

			isRight = true;
			isLeft = false;
			isUp = false;
			isDown = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}

			if(!checkCollision(30, 0))// check for collision 30 pixels to the right of player
			{
				pacX +=4;
				deltax+=4;
				glutIdleFunc(translate);
			}

			break;

		case GLUT_KEY_UP :

			isUp = true;
			isDown = false;
			isLeft = false;
			isRight = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}

			if(!checkCollision(0, -6))
			{
				pacY -= 4;
				deltay -=4;
				glutIdleFunc(translate);
			}

			break;

		case GLUT_KEY_DOWN :

			isDown = true;
			isUp = false;
			isLeft = false;
			isRight = false;

			if(isMouthOpen)
			{
				isMouthOpen = false;
			}
			else
			{
				isMouthOpen = true;
			}
		
			if(!checkCollision(0, 30))// check for collision 30 pixels below the player
			{
				pacY += 4;
				deltay += 4;
				glutIdleFunc(translate);
			}	

			break;

		default: 
			break;
		}
}

/*******************************************************
	handles the button; returned to the mouse handler
********************************************************/
void handleButton(int button, int state, int x, int y)
{
	static int index = -1;

	// if button is left button
	if(button == GLUT_LEFT_BUTTON)
	{
		if(x >= 48 && x <= 88 && y >= 640 && y <= 660)// left arrow
		{
			if(state == GLUT_DOWN)
			{
				isLeft = true;
				isRight = false;
				isUp = false;
				isDown = false;

				if(isMouthOpen)
				{
					isMouthOpen = false;
				}
				else
				{
					isMouthOpen = true;
				}

				if(!checkCollision(-3, 0)) // check for collision 3 pixels to the left
				{
 					pacX -=6;
					deltax -=6;
					glutIdleFunc(translate);
				}
			}
			if (state == GLUT_UP)
			{
				glutIdleFunc(NULL);
			}
		}
	
		if( x >= 118 && x <= 158 && y >= 640 && y <= 660)// right
		{
			if(state == GLUT_DOWN)
			{
				isRight = true;
				isLeft = false;
				isUp = false;
				isDown = false;

				if(isMouthOpen)
				{
					isMouthOpen = false;
				}
				else
				{
					isMouthOpen = true;
				}

				if(!checkCollision(30, 0))// check for collision 30 pixels to the right
				{
					pacX += 6;
					deltax += 6;
					glutIdleFunc(translate);
				}
			}
			if (state == GLUT_UP)
			{
				glutIdleFunc(NULL);
			}
		}
	
			
		if(x >= 93 && x <= 113 && y >= 655 && y <= 695)// down arrow
		{
			if(state == GLUT_DOWN)
			{
				isLeft = false;
				isRight = false;
				isUp = false;
				isDown = true;

				if(isMouthOpen)
				{
					isMouthOpen = false;
				}
				else
				{
					isMouthOpen = true;
				}

				if(!checkCollision(0, 1))// check for collision 1 pixel below the player
				{
 					pacY+=6;
					deltay+=6;
					glutIdleFunc(translate);
				}
			}
			if (state == GLUT_UP)
			{
				glutIdleFunc(NULL);
			}
		}

		if(x >= 93 && x <= 113 && y >= 625 && y <= 645)// up arrow
		{
			if(state == GLUT_DOWN)
			{
				isLeft = false;
				isRight = false;
				isUp = true;
				isDown = false;

				if(isMouthOpen)
				{
					isMouthOpen = false;
				}
				else
				{
					isMouthOpen = true;
				}

				if(!checkCollision(0, -1)) // check for collision 1 pixel above player
				{
 					pacY-=6;
					deltay-=6;
					glutIdleFunc(translate);
				}
			}
			if (state == GLUT_UP)
			{
				glutIdleFunc(NULL);
			}
		}			
	}

	glutSwapBuffers();		
}

/*******************************************
	Initialize GL camera and perspective
********************************************/
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	// set up std orthogonal view
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);

	return;
}

/*****************************************
	Draws player/pacman character to screen
******************************************/
void drawPac()
{
	// draw circular yellow/ pac man body
	glColor4f( 1, 1, 0, 1);

	glBegin(GL_POLYGON);
		for(float t = 0; t <=  3.14*2; t += INC)
		{
			glVertex2f(pacX + 10* cos(t), pacY + 10 * sin(t));
		}
	glEnd();		
}

/************************************
	Draws the player characters mouth
*************************************/
void drawMouth()
{
	// draw black mouth
	if(isMouthOpen)
	{
		glColor3f( 0, 0, 0);// set color to same as background(black)
					
		// right side facing mouth
		glBegin(GL_POLYGON);
			glVertex2f(56 + deltax, 54 + deltay);
			glVertex2f(56 + deltax, 40 + deltay);
			glVertex2f(46 + deltax, 47 + deltay);
		glEnd();	
	}
}

/**************** 
 Move Player
*****************/
void translate()
{
	checkPelletCollision(); // did pac eat a pellet?
	glClear(GL_COLOR_BUFFER_BIT);// clears all buffers whose bits are set in mask
	
	drawBoard();

	// move player 
	glPushMatrix();
		glTranslatef( pacX, pacY , 0.0);
		glTranslatef(- pacX, - pacY , 0.0);
		drawPac();
	glPopMatrix();

	// draw mouth after body, 
	// pac mouth rotation and maze corner facing conditions
	if(isMouthOpen && isRight && pacBoardX != 18 || pacBoardX == 1 && pacBoardY == 1 || pacBoardX == 1 && pacBoardY == 18)
	{
		glPushMatrix();
			glTranslatef( pacX, pacY , 0.0);
			glRotatef(0, 0.0, 0.0, 1.0);
			glTranslatef(- pacX, - pacY , 0.0);
			drawMouth();
		glPopMatrix();
	}
	else if(isMouthOpen && isUp)
	{
		glPushMatrix();
			glTranslatef( pacX, pacY , 0.0);
			glRotatef(-90, 0.0, 0.0, 1.0);
			glTranslatef(- pacX, - pacY , 0.0);
			drawMouth();
		glPopMatrix();
	}
	else if(isMouthOpen && isDown)
	{
		glPushMatrix();
			glTranslatef( pacX, pacY , 0.0);
			glRotatef(90, 0.0, 0.0, 1.0);
			glTranslatef(- pacX, - pacY , 0.0);
			drawMouth();
		glPopMatrix();
	}
	else if(isMouthOpen && isLeft || pacBoardX == 18 && pacBoardY == 1 || pacBoardX == 18 && pacBoardY == 18)
	{
		glPushMatrix();
			glTranslatef( pacX, pacY , 0.0);
			glRotatef(180, 0.0, 0.0, 1.0);
			glTranslatef(- pacX, - pacY , 0.0);
			drawMouth();
		glPopMatrix();
	}

	drawButton();

	glutSwapBuffers();
}

/************************************************************************
	Displays text to screen
	x: x position on screen to be drawn
	y: y position on screen to be drawn
	r: red color value of text
	g: green color value of text
	b: blue volor value of text
	*string: collection of character that will make up the text
****************************************************************************/
void displayText( float x, float y, int r, int g, int b, const char *string ) 
{
	int i;
	int j = strlen(string);
 
	glColor3f(r, g, b);// set txt color
	glRasterPos2f( x, y );// set postion to be drawn
	// loop thru each charcter and display
	for(i = 0; i < j; i++ ) 
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, string[i] );
	}
}

/********************************************
	Draws the Maze/gameboard walls and pellets onto the screen
*********************************************/
void drawBoard()
{
	int i;
	float t; // theta
	int x, y;

	// draw maze/board each block starting from left to right
	for(y = 0; y < boardY; y++)
	{
		for(x = 0; x < boardX; x++)
		{
			if(pellets[y][x] == 1)
			{
				glColor3f(0,0,1);// set draw color to blue

				// draw individual 1 by 1 block at x and y position
				glBegin(GL_POLYGON);
					glVertex2f(0 + (30 * x), 0 + (30 * y));
					glVertex2f(30 + (30 * x), 0 + (30 * y));
					glVertex2f(30 + (30 * x), 30 + (30 * y));
					glVertex2f(0 + (30 * x), 30 + (30 * y));
				glEnd();
			}
		}
	}		

	// draw pellets in level
	for(i = 0; i < amtPellets; i++)
	{		
		glColor3f(1, 1, 1);// set color to white
		if(pelletEntities[i].isVisible)// is pellet to be visible in the level
		{
			glBegin(GL_POLYGON);
				for(t = 0.0; t <= 3.14*2; t += INC)
				{
					glVertex2f(pelletEntities[i].x + 3 * cos(t), pelletEntities[i].y + 3 * sin(t));
					glColor4f(1, 1, 1, .25);// set shading color to white
				}
			glEnd();
		}
	}			
}

/************************************************************
	Initializes the pellets in level before the game starts 
************************************************************/
void initLevel()
{
	int i = 0;
	int x, y;		
			
	// loop row to row and init pellets for level
	for(y = 0; y < boardY; y++)
	{
		for(x = 0; x < boardX; x++)
		{
			if(pellets[y][x] == 0)
			{
				//init pebble x/y positions
				pelletEntities[i].isVisible = true;	
				pelletEntities[i].x = 16 + 30 * x; 
				pelletEntities[i].y = 16 + 30 * y;
				pelletEntities[i].boardX = x; // set x pos on maze/board
				pelletEntities[i].boardY = y; // set y pos on maze/board
				i++;// inc index for pelletEntities	
				amtPellets += 1;// track/count new pellet
			}
		}
	}	
}

/**************************************************************************************************
	Checks for pellet collision, inc score and set pellet to not be render in level after collision. 
***************************************************************************************************/
void checkPelletCollision()
{
	// find cur pacBoard X & Y positions
	pacBoardX = (pacX - 16) / 30 ;
	pacBoardY = (pacY  - 16) / 30;

	// collision check
	for(int i = 0; i < amtPellets; i++)// loop thru all pellets in level
	{
		if(pacBoardX == pelletEntities[i].boardX && pacBoardY == pelletEntities[i].boardY && pelletEntities[i].isVisible )
		{
			// make pellet disapear since it has been eaten
			pelletEntities[i].isVisible = false;// no longer render pellet to screen
			score+=10; // add 10 pts for eating a pellet
		}
	}
}

/*************************************************************************
	Checks for collisions with maze.(-x values will check to the left of player and +x checks right)
	x: amt to check in the x direction
	y: amt to check in the y direction
****************************************************************************/
bool checkCollision(int x, int y)
{
	x += pacX;// add in cur player position
	y += pacY;// add in cur player position
	int pX = (( x - 16)) / 30;// find x pos in maze (0-19)
	int pY = (( y - 16)) / 30;// find y pos in maze (0-19)

	if(pacBoardX <= -1 && pacBoardY == 10)// if player collides w/ left warp pos, warp player to the right side.
	{
		// move player
		pacX =  16 + (30 * 20);
		deltax = (30 * 20)-30 ;
		pacBoardX = 19;
		isLeft = true; // face player left
		return false;
	}
	else if(pellets[pY][pX] == 0 || pacBoardX == 19 && pacBoardY == 10 || pacBoardX == 0 && pacBoardY == 10)// if player moves towards the right level warp, return no collision
	{
		return false;
	}
	else if(pacBoardX >= 20 && pacBoardY == 10 ) // if player collides right warp pos, warp player to the left side.
	{
		// move player
		pacX = 16;
		deltax = (30 * 0)-30;
		pacBoardX = 0;
		isRight = true; // face player right
		return false;
	}
	else if(pellets[pY][pX] == 1 )// if there is a block
	{
		return true;
	}	
}

