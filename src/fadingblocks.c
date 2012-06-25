/******************************************************************************
 * name: fadingblocks.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 30.07.03
 * description: holds functions to draw fading blocks
 ******************************************************************************/

#include <stdlib.h>
#include "fadingblocks.h"
#include "defs.h"

extern SDL_Surface *screen;
extern SDL_Surface *fadingblocks;

FadingBlockLine fading;
int fadingtime = 0;

void InitFadingBlocks( int board[BOARDWIDTH][BOARDHEIGHT], int line_number, int fading_line )
{
	static int fading_type = 0;
	int i;
	
	// initialize a new fading-type if we have to
	if(fading_line == 0)
	{
		fading_type = rand() % 2;
	}
	
	// init the line, x and y-positions and get random velocities
	for(i = 0; i < 10; i++)
	{
		fading.blocks[fading_line][i] = board[i+BOARDWIDTHSTART][line_number];
		fading.x[fading_line][i] = i * BLOCKSIZE + REAL_BOARD_DRAW_STARTX;
		fading.y[fading_line][i] = (line_number - fading_line) * BLOCKSIZE + BOARD_DRAW_STARTY;
		
		// initialize the velocitys according to the type
		switch(fading_type)
		{
			case 0:
			{
				// move up and down
				fading.vy[fading_line][i] = 150 + fading_line * 10;
				fading.vx[fading_line][i] = (i - 5) * 25;
				fading.ay[fading_line][i] = 220;
				break;
			}
			case 1:
			{
				// mv left up highest
				fading.vy[fading_line][i] = (i * 20) + fading_line * 10;
				fading.vx[fading_line][i] = 0;
				fading.ay[fading_line][i] = 300;
				break;
			}
		}
	}
	
	fading.alpha = 128;
	
	return;
}

void DrawFadingBlocks( int ms, int num_lines )
{
	int i, k;
	float time = (float)ms / 1000;
	SDL_Rect src, dest;
	
	// update the fading time
	fadingtime -= ms;
	if( fadingtime < 0 )
		fadingtime = 0;
	
	// update the alpha value
	fading.alpha -= (int)(time / (8 * FADING_TIME / 1000));
	SDL_SetAlpha(fadingblocks, SDL_SRCALPHA, fading.alpha);
	
	for(k = 0; k < num_lines; k++)
	{
		// update x, ang y-positions
		for(i = 0; i < 10; i++)
		{
			fading.x[k][i] += fading.vx[k][i] * time;
			fading.y[k][i] -= fading.vy[k][i] * time;
			fading.vy[k][i] -= fading.ay[k][i] * time;
		}
		
		// draw the blocks
		for(i = 0; i < 10; i++)
		{
			src.x = (fading.blocks[k][i] - 1) * BLOCKSIZE;
			src.y = 0;
			src.w = src.h = BLOCKSIZE;
			
			dest.x = (int)fading.x[k][i];
			dest.y = (int)fading.y[k][i];
			
			SDL_BlitSurface(fadingblocks, &src, screen, &dest);
		}
	}
	
	return;
}
