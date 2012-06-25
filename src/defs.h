/******************************************************************************
 * name: defs.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 26.07.03
 * description: This file holds the global definitions of the project
 ******************************************************************************/
#ifndef DEFS_H
#define DEFS_H

// all the to included files
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "SFont.h"
#include "game.h"
#include "fadingblocks.h"
#include "menu.h"
#include "data.h"
#include "vidinfo.h"
#include "config.h"
#include "utils.h"
#include "helper.h"

#define BLOCKSIZE	28

#define REAL_BOARD_DRAW_STARTX	100
#define BOARD_DRAW_STARTX	REAL_BOARD_DRAW_STARTX - 3 * BLOCKSIZE
#define BOARD_DRAW_STARTY	20

#define NEXT_BLOCK_DRAWX	442
#define NEXT_BLOCK_DRAWY	88

#define SCORE_DRAW_X	500
#define SCORE_DRAW_Y	322
#define LEVEL_DRAW_X	500
#define LEVEL_DRAW_Y	464
#define LINES_DRAW_X	500
#define LINES_DRAW_Y	392

#define BOARDHEIGHTREAL	20
#define BOARDWIDTHSTART	3
#define BOARDWIDTHEND	13
#define BLOCKXSTART	6

#define FADING_TIME	1500

#endif
