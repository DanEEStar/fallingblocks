/******************************************************************************
 * name: game.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 26.07.03
 * description: This file has the definitions of the game itself, that
 * means the board and the block etc.
 ******************************************************************************/

#ifndef GAME_H
#define GAME_H

#define BOARDHEIGHT	23
#define BOARDWIDTH	16

/* this structure holds the variables for a block type */
typedef struct Block_type
{
	int x, y;	// the position in the board
	float screeny;	// the exact y-position of the block
	int rot;	// the rotation status
	int data[4][4][4];	// the block structure data[rot][x][y]
} Block;

/* this is the game structure */
typedef struct Game_type
{
	Block current_block;	// the state of the current block
	Block next_block;	// the state of the next block
	int board[BOARDWIDTH][BOARDHEIGHT];	// the main game board
	int lines;		// the current number of lines
	int level;		// the current level
	int score;		// the current score
} Game;

/* This structure holds the game input */
typedef struct GameInput_type
{
	/* some simple key-states */
	int bRotateLeft, bRotateRight;
	int bDown, bLeft, bRight;
	
	// the value of the keys
	int key_left, key_right, key_down;
	int key_rotate_left, key_rotate_right;
} GameInput;

/* a structure for holding the actual game options */
typedef struct GameOptions_type
{
	int startLevel;
	int playSound;
	int playMusic;
	int musicStarted;
} GameOptions;

// this function will run the game
// returns 0 if the game is over and 1 if all went right
int UpdateGame( Game *current_game, GameInput *input, int ms);

/* this function initilize a new game */
void InitGame( Game *current_game, int level );

/* this function init the inputs */
void ResetGameInput( GameInput *input );

/* These function get the key inputs */
void GetGameInput( GameInput *input, Uint8 *keystate );

/* this function gives a new block */
void GetBlock( Block *new_block );

/* this function copies a block */
void CopyBlock( Block *next, Block *current );

/* try to rotate a block */
void RotateBlockLeft( int board[BOARDWIDTH][BOARDHEIGHT], Block *b );
void RotateBlockRight( int board[BOARDWIDTH][BOARDHEIGHT], Block *b );

/* try to move a block */
void MoveBlockLeft( int board[BOARDWIDTH][BOARDHEIGHT], Block *b );
void MoveBlockRight( int board[BOARDWIDTH][BOARDHEIGHT], Block *b );

/* try to move down a block. Returns 1 if succeded, 0 otherwise */
int MoveBlockDown( int board[BOARDWIDTH][BOARDHEIGHT], Block *b, float pixeldown );

/* try to get a new block
 * returns 1 if pssible and succeded, 0 otherwise (wich means game over) */
int NewBlock( Game *current_game );

/* Adds the current block to the board */
void AddBlockToBoard( int board[BOARDWIDTH][BOARDHEIGHT], Block *b );

/* check if a block can be here.
 * returns 1 if position is possible, 0 otherwise */
int BlockPossible( int board[BOARDWIDTH][BOARDHEIGHT], Block *b, int newx, int newy, int newrot );

/* check if there are lines and removes them if there are any *
 * returns 0 if there are no lines and the number of lines removed if there were any */
int CheckLines( int board[BOARDWIDTH][BOARDHEIGHT] );

/* draws the board and the block with score and levels */
void DrawGame( Game *current_game );

/* only for test purposes */
int MoveBlockUp( int board[BOARDWIDTH][BOARDHEIGHT], Block *b );

#endif
