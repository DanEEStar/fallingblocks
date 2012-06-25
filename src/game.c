/******************************************************************************
 * name: game.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 26.07.03
 * description: This file has the definitions of the game itself, that
 * means the board and the block etc.
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "defs.h"
#include "game.h"

#define LONG_DELAY	140
#define SHORT_DELAY	40

// globals
extern SDL_Surface *screen;
extern SDL_Surface *game_back, *blocks;

extern Mix_Chunk *rotwav;
extern Mix_Chunk *fanfare;
extern Mix_Chunk *down;
extern Mix_Chunk *line;
extern Mix_Chunk *nextlevel;

// information about the font
extern SFont_FontInfo blue_font;

// this globals holds information about the fading blocks
extern FadingBlockLine fading;
extern int fadingtime;

extern GameOptions options;

// the number of fading lines
static int fadinglines = 0;

/******************************************************************************/
int UpdateGame( Game *current_game, GameInput *input, int ms)
{
	static int move_delay = 0;	// amount of time the input should be delayed
	static int down_delay = 0;	// amount of time the down input should be delayed
	static int rotated = 0;	// the block should rotate only once per key-press
	static float pixeldown = 0;	// the nuber of pixels the block should move down
	static int first_move = 1;	// only the first move should long be delayed
	
	// update the move delay
	move_delay -= ms;
	if(move_delay < 0)
		move_delay = 0;
	
	// update down delay
	down_delay -= ms;
	if(down_delay < 0)
		down_delay = 0;
	
	// handle the left-, right-input
	if(input->bLeft)
	{
		if(!move_delay)
		{
			MoveBlockLeft(current_game->board, &current_game->current_block);
			if(first_move)
			{
				move_delay = LONG_DELAY;
				first_move = 0;
			}
			else
			{
				move_delay = SHORT_DELAY;
			}
		}
	}
	else if(input->bRight)
	{
		if(!move_delay)
		{
			MoveBlockRight(current_game->board, &current_game->current_block);
			if(first_move)
			{
				move_delay = LONG_DELAY;
				first_move = 0;
			}
			else
			{
				move_delay = SHORT_DELAY;
			}
		}
	}
	else
	{
		// update first move
		first_move = 1;
	}
	
	// handle down move
	if(input->bDown)
	{
		if(!down_delay)
		{
			MoveBlockDown(current_game->board, &current_game->current_block, 25);
			down_delay = SHORT_DELAY;
		}
	}
	
	// handle rotation, you can only rotate once per key press
	if(input->bRotateRight)
	{
		if(!rotated)
		{
			RotateBlockRight(current_game->board, &current_game->current_block);
			rotated = 1;
		}
	}
	else if(input->bRotateLeft)
	{
		if(!rotated)
		{
			RotateBlockLeft(current_game->board, &current_game->current_block);
			rotated = 1;
		}
	}
	else
	{
		// reset Rotation
		rotated = 0;
	}
	
	// handle the move down
	pixeldown = ms * (0.035f + (float)current_game->level * 0.030f);
	
	// lock the down-coming-speed (there are obscure errors if you don't do that
	if(pixeldown > BLOCKSIZE)
		pixeldown = BLOCKSIZE;
	
	// finally move the block down
	if( MoveBlockDown( current_game->board, &current_game->current_block, pixeldown ) == 0 )
	{
		// the block hit the ground try to get new block
		// play a sound first
		PlaySound( down, options.playSound );
		if( NewBlock( current_game ) == 0 )
		{
			// no new block possible -> game-over
			return 0;
		}
		move_delay = LONG_DELAY;
		down_delay = LONG_DELAY;
	}
	
	// Draw the game
	DrawGame( current_game );
	
	// draw the fading blocks
	if( fadingtime )
	{
		DrawFadingBlocks( ms, fadinglines );
	}
	
	// one frame complete!
	return 1;
}

/******************************************************************************/
void InitGame( Game *current_game, int level )
{
	int i, j;
	
	/* initialise the board */
	for(i = 0; i < BOARDWIDTH; i++)
	{
		for(j = 0; j < BOARDHEIGHT; j++)
		{
			// the board will get a boarder to make the
			// bound checking easier
			if((i < BOARDWIDTHSTART || i >= BOARDWIDTHEND) || j >= BOARDHEIGHTREAL)
				current_game->board[i][j] = 1;
			else
				current_game->board[i][j] = 0;
		}
	}
	
	/* get new blocks */
	GetBlock(&current_game->current_block);
	GetBlock(&current_game->next_block);
	
	/* initialise the other variables */
	current_game->score = 0;
	current_game->level = level;
	current_game->lines = 0;
	
	return;
}

/******************************************************************************/
void ResetGameInput( GameInput *input )
{
	input->key_left = SDLK_LEFT;
	input->key_right = SDLK_RIGHT;
	input->key_rotate_right = SDLK_UP;
	input->key_rotate_left = SDLK_SPACE;
	input->key_down = SDLK_DOWN;
	
	return;
}

/******************************************************************************/
void GetGameInput( GameInput *input, Uint8 *keystate )
{
	// left key
	input->bLeft = keystate[input->key_left];
	
	// right key
	input->bRight = keystate[input->key_right];
	
	// rotate left
	input->bRotateRight = keystate[input->key_rotate_right];
		
	// rotate right
	input->bRotateLeft = keystate[input->key_rotate_left];
	
	// key down
	input->bDown = keystate[input->key_down];
	
	return;
}

/******************************************************************************/
void CopyBlock( Block *next, Block *current )
{
	int rot, x, y;
	
	/* copy the every element from next to current */
	for(rot = 0; rot < 4; rot++)
		for(x = 0; x < 4; x++)
			for(y = 0; y < 4; y++)
				current->data[rot][x][y] = next->data[rot][x][y];
	
	/* copy the other items from next to current */
	current->rot = next->rot;
	current->x = next->x;
	current->y = next->y;
	current->screeny = next->screeny;
	
	return;
}

/******************************************************************************/
int BlockPossible( int board[BOARDWIDTH][BOARDHEIGHT], Block *b, int newx, int newy, int newrot )
{
	int i, j;
	
	/* check every single entry in the board */
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if( board[i + newx][j + newy] && b->data[newrot][i][j])
		    	{
				/* here is already a block */
				return 0;
			}
	
	/* this position is good */
	return 1;
}

/******************************************************************************/
void RotateBlockLeft( int board[BOARDWIDTH][BOARDHEIGHT], Block *b )
{
	int newrot;
	
	/* the rotation status has to be between 0 and 3 */
	if(b->rot == 0)
		newrot = 3;
	else
		newrot = b->rot - 1;
	
	/* can the block be there */
	if(BlockPossible(board, b, b->x, b->y, newrot))
	{
		/* update the position */
		b->rot = newrot;
		
		// Play a sound
		PlaySound( rotwav, options.playSound );
	}
	
	// do nothing otherwise
	return;
}

/******************************************************************************/
void RotateBlockRight( int board[BOARDWIDTH][BOARDHEIGHT], Block *b )
{
	int newrot;
	
	/* the rotation status has to be between 0 and 3 */
	if(b->rot == 3)
		newrot = 0;
	else
		newrot = b->rot + 1;
	
	/* can the block be there */
	if(BlockPossible(board, b, b->x, b->y, newrot))
	{
		/* update the position */
		b->rot = newrot;
		
		// Play a sound
		PlaySound( rotwav, options.playSound );
	}
	
	// do nothing otherwise
	return;
}

/******************************************************************************/
void MoveBlockLeft( int board[BOARDWIDTH][BOARDHEIGHT], Block *b )
{
	int newx = b->x - 1;
	
	/* check if the position is possible */
	if(BlockPossible(board, b, newx, b->y, b->rot))
	{
		/* update the position */
		b->x = newx;
	}
	
	return;
}

/******************************************************************************/
void MoveBlockRight( int board[BOARDWIDTH][BOARDHEIGHT], Block *b )
{
	int newx = b->x + 1;
	
	/* check if the position is possible */
	if(BlockPossible(board, b, newx, b->y, b->rot))
	{
		/* update the position */
		b->x = newx;
	}
	
	return;
}

/******************************************************************************/
int MoveBlockDown( int board[BOARDWIDTH][BOARDHEIGHT], Block *b, float pixeldown )
{
	int newy = (int)((b->screeny + pixeldown) / BLOCKSIZE);
	
	/* check if the position is possible */
	if(BlockPossible(board, b, b->x, newy, b->rot))
	{
		/* update the position and return succes */
		b->y = newy;
		b->screeny += pixeldown;
		return 1;
	}
	
	// return failure
	return 0;
}

/******************************************************************************/
int NewBlock( Game *current_game )
{
	int new_lines;
	/* add the current block to the board and check for lines */
	AddBlockToBoard(current_game->board, &current_game->current_block);
	new_lines = CheckLines(current_game->board);
	
	/* if there were new lines update score and level */
	if(new_lines > 0)
	{
		current_game->lines += new_lines;
		
		// check if the new level would be below the current level
		if( current_game->lines / 10 > current_game->level )
		{
			// check for a level change
			if( current_game->level < current_game->lines / 10 )
			{
				// we reached a new level
				PlaySound( nextlevel, options.playSound );
				current_game->level = current_game->lines / 10;
			}
		}
		
		/* the score is computed like the original GB- and NES-Tetris scheme */
		switch(new_lines)
		{
			case 1:
			{
				PlaySound( line, options.playSound );
				current_game->score += (current_game->level + 1) * 40;
				break;
			}
			case 2:
			{
				PlaySound( line, options.playSound );
				current_game->score += (current_game->level + 1) * 100;
				break;
			}
			case 3:
			{
				PlaySound( line, options.playSound );
				current_game->score += (current_game->level + 1) * 300;
				break;
			}
			case 4:
			{
				current_game->score += (current_game->level + 1) * 1200;
				// play a sound
				PlaySound( fanfare, options.playSound );
				break;
			}
		}
	}
		
	/* check if the next block has enough place on the board */
	if(BlockPossible(current_game->board,
		         &current_game->next_block,
		         current_game->next_block.x,
			 current_game->next_block.y,
			 current_game->next_block.rot))
	{
		/* copy block to board change pieces, get a new piece and return succes */
		CopyBlock(&current_game->next_block, &current_game->current_block);
		GetBlock(&current_game->next_block);
		return 1;
	}
	
	/* return failure */
	return 0;
}

/******************************************************************************/
void AddBlockToBoard( int board[BOARDWIDTH][BOARDHEIGHT], Block *b )
{
	int i, j;
	
	/* Add every single element to the board */
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if(b->data[b->rot][i][j])
				board[i + b->x][j + b->y] = b->data[b->rot][i][j];
	
	return;
}

/******************************************************************************/
int CheckLines( int board[BOARDWIDTH][BOARDHEIGHT] )
{
	int lines = 0;		// number of lines
	int maxlines = 0;	// the max number of still possible lines
	int flag;		// indicate if there is a line
	int y, x, i;		// wheel-counters
	
	/* looks if there are full lines, begin with the most down */
	for(y = BOARDHEIGHTREAL - 1; y >= maxlines; y--)
	{
		flag = 1;
		for(x = BOARDWIDTHSTART; x < BOARDWIDTHEND; x++)
		{
			// if there is one empty cell then break
			if(board[x][y] == 0)
			{
				flag = 0;
			}
		}
		
		// if flag is still one there is a line
		if(flag)
		{
			/* initialize the fading line */
			InitFadingBlocks( board, y, lines );
			fadingtime = FADING_TIME;
			
			/* remove the line */
			for(x = BOARDWIDTHSTART; x < BOARDWIDTHEND; x++)
				board[x][y] = 0;
			
			/* move the other lines down */
			for(i = y; i > 0; i--)
				for(x = BOARDWIDTHSTART; x < BOARDWIDTHEND; x++)
					board[x][i] = board[x][i-1];
			
			/* fill the uppermost line with zeros */
			for(x = BOARDWIDTHSTART; x < BOARDWIDTHEND; x++)
				board[x][0] = 0;
			
			lines++;
			maxlines = y - 3;
			/* set the counter 1 up */
			y++;
		}
	}
	
	fadinglines = lines;
	return lines;
}

/******************************************************************************/
void GetBlock( Block *new_block )
{
	int random;
	int rot, x, y;
	
	/* set all data elements to 0 */
	for(rot = 0; rot < 4; rot++)
		for(x = 0; x < 4; x++)
			for(y = 0; y < 4; y++)
				new_block->data[rot][x][y] = 0;
	
	/* set the position of the block */
	new_block->x = BLOCKXSTART;
	new_block->y = 0;
	new_block->screeny = 0;
	
	/* get a random rotation */
	new_block->rot = rand() % 4;
	
	/* get a new random block */
	random = rand() % 7;
	
	/* set the data to the specific blocks */
	switch(random)
	{
		/* the hardcoded things isn't yellow of the egg... */
		case 0:	// S-Block
		{
			new_block->data[0][1][0] = 1;	// 0100
			new_block->data[0][1][1] = 1;	// 0110
			new_block->data[0][2][1] = 1;	// 0010
			new_block->data[0][2][2] = 1;	// 0000
			new_block->data[1][0][1] = 1;
			new_block->data[1][1][0] = 1;
			new_block->data[1][1][1] = 1;
			new_block->data[1][2][0] = 1;
			new_block->data[2][1][0] = 1;
			new_block->data[2][1][1] = 1;
			new_block->data[2][2][1] = 1;
			new_block->data[2][2][2] = 1;
			new_block->data[3][0][1] = 1;
			new_block->data[3][1][0] = 1;
			new_block->data[3][1][1] = 1;
			new_block->data[3][2][0] = 1;
			break;
		}
		case 1:	// Z-Block
		{
			new_block->data[0][1][1] = 2;	// 0010
			new_block->data[0][1][2] = 2;	// 0110
			new_block->data[0][2][0] = 2;	// 0100
			new_block->data[0][2][1] = 2;
			new_block->data[1][1][0] = 2;
			new_block->data[1][2][0] = 2;
			new_block->data[1][2][1] = 2;
			new_block->data[1][3][1] = 2;
			new_block->data[2][1][1] = 2;
			new_block->data[2][1][2] = 2;
			new_block->data[2][2][0] = 2;
			new_block->data[2][2][1] = 2;
			new_block->data[3][1][0] = 2;
			new_block->data[3][2][0] = 2;
			new_block->data[3][2][1] = 2;
			new_block->data[3][3][1] = 2;
			break;
		}
		case 2:	// Middle-Finger-Block
		{
			new_block->data[0][1][1] = 3;	// 0000
			new_block->data[0][2][0] = 3;	// 0010
			new_block->data[0][2][1] = 3;	// 0111
			new_block->data[0][3][1] = 3;	// 0000
			new_block->data[1][2][0] = 3;
			new_block->data[1][2][1] = 3;
			new_block->data[1][2][2] = 3;
			new_block->data[1][3][1] = 3;
			new_block->data[2][1][1] = 3;
			new_block->data[2][2][1] = 3;
			new_block->data[2][2][2] = 3;
			new_block->data[2][3][1] = 3;
			new_block->data[3][1][1] = 3;
			new_block->data[3][2][0] = 3;
			new_block->data[3][2][1] = 3;
			new_block->data[3][2][2] = 3;
			break;
		}
		case 3:	// O-Block
		{
			new_block->data[0][1][0] = 4;	// 0110
			new_block->data[0][1][1] = 4;	// 0110
			new_block->data[0][2][0] = 4;	// 0000
			new_block->data[0][2][1] = 4;
			new_block->data[1][1][0] = 4;
			new_block->data[1][1][1] = 4;
			new_block->data[1][2][0] = 4;
			new_block->data[1][2][1] = 4;
			new_block->data[2][1][0] = 4;
			new_block->data[2][1][1] = 4;
			new_block->data[2][2][0] = 4;
			new_block->data[2][2][1] = 4;
			new_block->data[3][1][0] = 4;
			new_block->data[3][1][1] = 4;
			new_block->data[3][2][0] = 4;
			new_block->data[3][2][1] = 4;
			break;
		}
		case 4:	// J-Block
		{
			new_block->data[0][1][0] = 5;	// 0000
			new_block->data[0][1][1] = 5;	// 0110
			new_block->data[0][1][2] = 5;	// 0100
			new_block->data[0][2][0] = 5;	// 0100
			new_block->data[1][1][0] = 5;
			new_block->data[1][2][0] = 5;
			new_block->data[1][3][0] = 5;
			new_block->data[1][3][1] = 5;
			new_block->data[2][1][2] = 5;
			new_block->data[2][2][0] = 5;
			new_block->data[2][2][1] = 5;
			new_block->data[2][2][2] = 5;
			new_block->data[3][1][0] = 5;
			new_block->data[3][1][1] = 5;
			new_block->data[3][2][1] = 5;
			new_block->data[3][3][1] = 5;
			break;
		}
		case 5:	// L-Block
		{
			new_block->data[0][1][0] = 6;	// 0000
			new_block->data[0][2][0] = 6;	// 0110
			new_block->data[0][2][1] = 6;	// 0010
			new_block->data[0][2][2] = 6;	// 0010
			new_block->data[1][1][1] = 6;
			new_block->data[1][2][1] = 6;
			new_block->data[1][3][0] = 6;
			new_block->data[1][3][1] = 6;
			new_block->data[2][1][0] = 6;
			new_block->data[2][1][1] = 6;
			new_block->data[2][1][2] = 6;
			new_block->data[2][2][2] = 6;
			new_block->data[3][1][0] = 6;
			new_block->data[3][1][1] = 6;
			new_block->data[3][2][0] = 6;
			new_block->data[3][3][0] = 6;
			break;
		}
		case 6:	// I-Block
		{
			new_block->data[0][1][0] = 7;	// 0100
			new_block->data[0][1][1] = 7;	// 0100
			new_block->data[0][1][2] = 7;	// 0100
			new_block->data[0][1][3] = 7;	// 0100
			new_block->data[1][0][1] = 7;
			new_block->data[1][1][1] = 7;
			new_block->data[1][2][1] = 7;
			new_block->data[1][3][1] = 7;
			new_block->data[2][2][0] = 7;
			new_block->data[2][2][1] = 7;
			new_block->data[2][2][2] = 7;
			new_block->data[2][2][3] = 7;
			new_block->data[3][0][1] = 7;
			new_block->data[3][1][1] = 7;
			new_block->data[3][2][1] = 7;
			new_block->data[3][3][1] = 7;
			break;
		}
		default:
		{
			// that shouldn't happen
			printf("Error in switch\n");
		}
	}
	
	return;
}

void DrawGame( Game *current_game )
{
	int i, j;
	int current_rot = current_game->current_block.rot;
	int next_rot = current_game->next_block.rot;
	char tmp[255];	// holds the text
	SDL_Rect src, dest;
	
	/* Draw background */
	SDL_BlitSurface(game_back, NULL, screen, NULL);
	
	/* draw board */
	for(i = BOARDWIDTHSTART; i < BOARDWIDTHEND; i++)
	{
		for(j = 0; j < BOARDHEIGHTREAL; j++)
		{
			if(current_game->board[i][j])
			{
				src.x = (current_game->board[i][j] - 1) * BLOCKSIZE;
				src.y = 0;
				src.w = BLOCKSIZE;
				src.h = BLOCKSIZE;
				dest.y = j * BLOCKSIZE + BOARD_DRAW_STARTY;
				dest.x = i * BLOCKSIZE + BOARD_DRAW_STARTX;
				SDL_BlitSurface(blocks, &src, screen, &dest);
			}
		}
	}
	
	/* draw current block */
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(current_game->current_block.data[current_rot][i][j])
			{
				src.x = (current_game->current_block.data[current_rot][i][j] - 1) * BLOCKSIZE;
				src.y = 0;
				src.w = BLOCKSIZE;
				src.h = BLOCKSIZE;
				dest.x = (current_game->current_block.x + i) * BLOCKSIZE + BOARD_DRAW_STARTX;
				dest.y = (current_game->current_block.y + j) * BLOCKSIZE + BOARD_DRAW_STARTY;
				SDL_BlitSurface(blocks, &src, screen, &dest);
			}
		}
	}
	
	/* draw next block */
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(current_game->next_block.data[next_rot][i][j])
			{
				src.x = (current_game->next_block.data[next_rot][i][j] - 1) * BLOCKSIZE;
				src.y = 0;
				src.w = BLOCKSIZE;
				src.h = BLOCKSIZE;
				dest.x = i * BLOCKSIZE + NEXT_BLOCK_DRAWX;
				dest.y = j * BLOCKSIZE + NEXT_BLOCK_DRAWY;
				SDL_BlitSurface(blocks, &src, screen, &dest);
			}
		}
	}
	
	// draw game information
	sprintf(tmp, "%d", current_game->score);
	PutString2(screen, &blue_font, SCORE_DRAW_X, SCORE_DRAW_Y, tmp);
	sprintf(tmp, "%d", current_game->level);
	PutString2(screen, &blue_font, LEVEL_DRAW_X, LEVEL_DRAW_Y, tmp);
	sprintf(tmp, "%d", current_game->lines);
	PutString2(screen, &blue_font, LINES_DRAW_X, LINES_DRAW_Y, tmp);
	return;
}

int MoveBlockUp( int board[BOARDWIDTH][BOARDHEIGHT], Block *b )
{
	int newy = b->y - 1;
	
	/* check if the position is possible */
	if(BlockPossible(board, b, b->x, newy, b->rot))
	{
		/* update the position and return succes */
		b->y = newy;
		return 1;
	}
	
	// return failure
	return 0;
}

