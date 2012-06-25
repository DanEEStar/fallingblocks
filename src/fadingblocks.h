/******************************************************************************
 * name: fadingblocks.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 30.07.03
 * description: holds functions to draw fading blocks
 ******************************************************************************/
#ifndef FADINGBLOCKS_H
#define FADINGBLOCKS_H

#include "defs.h"

/* this structure holds the fading (= to removing) blocks */
typedef struct FadingBlockLine_type
{
	int blocks[4][10];		// the line
	float x[4][10], y[4][10];	// the position on the screen
	float vx[4][10], vy[4][10];	// the velocity against the axis
	float ay[4][10];		// the acceleration against the y axis
	int alpha;			// the transparancy of the surface
} FadingBlockLine;

// initialize the fading blocks
void InitFadingBlocks( int board[BOARDWIDTH][BOARDHEIGHT], int line_number, int fading_line );

// draw the fading blocks
void DrawFadingBlocks( int ms, int num_lines );

#endif
