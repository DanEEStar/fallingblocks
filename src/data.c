/******************************************************************************
 * name: data.c
 * author: Daniel Egger, degger@bluemail.ch
 * date: 28.07.03
 * description: This file holds the global data
 ******************************************************************************/

#include <stdlib.h>
#include "defs.h"
#include "data.h"

extern SDL_Surface *screen;

// The Surfaces
SDL_Surface *game_back;
SDL_Surface *blocks;
SDL_Surface *pause;
SDL_Surface *fadingblocks;

SDL_Surface *over;

// Sound
Mix_Chunk *rotwav;
Mix_Chunk *fanfare;
Mix_Chunk *down;
Mix_Chunk *line;
Mix_Chunk *nextlevel;

Mix_Music *music;

// Font-System
SDL_Surface *blue_font_surf;
SDL_Surface *neonblue_font_surf;
SDL_Surface *almont_font_surf;
SFont_FontInfo blue_font;
SFont_FontInfo neonblue_font;
SFont_FontInfo almont_font;

SDL_Surface *LoadImage(char *filename)
{
	SDL_Surface *temp, *surf;
	
	// load the file temporarely
	temp = IMG_Load(filename);
	
	// file loaded correctly?
	if(temp == NULL || (surf = SDL_DisplayFormat(temp)) == NULL)
	{
		printf("Unable to load image-file: %s\n", filename);
		printf("Error: %s\n", IMG_GetError());
		exit(0);
	}
	
	SDL_FreeSurface(temp);
	
	return surf;
}

Mix_Chunk *LoadSound(char *filename)
{
	Mix_Chunk *sound;
	if((sound = Mix_LoadWAV(filename)) == NULL)
	{
		printf("Unable to load audio-file: %s\n", filename);
		printf("Error: %s\n", Mix_GetError());
		exit(0);
	}
	
	return sound;
}

void LoadGameData()
{
	// load needed surfaces
	game_back = LoadImage("gfx/back.png");
	blocks = LoadImage("gfx/blocks.png");
	fadingblocks = LoadImage("gfx/blocks.png");
	pause = LoadImage("gfx/pause.png");
	
	// load gameover
	over = IMG_Load("gfx/gameover.png");
	SDL_SetAlpha(over, SDL_SRCALPHA, 0);
	
	// Try to load the sounds
	rotwav = LoadSound("sound/rot.wav");
	fanfare = LoadSound("sound/fanfare.wav");
	down = LoadSound("sound/stop.wav");
	line = LoadSound("sound/explosion.wav");
	
	music = Mix_LoadMUS("sound/4thsym.it");
	
	return;
}

void UnloadGameData()
{
	SDL_FreeSurface(game_back);
	SDL_FreeSurface(blocks);
	SDL_FreeSurface(pause);
	SDL_FreeSurface(fadingblocks);
	
	Mix_FreeMusic(music);
	
	return;
}

void LoadFontSystem()
{
	blue_font_surf = LoadImage("gfx/NeonBlue.png");
	neonblue_font_surf = LoadImage("gfx/NeonYellow.png");
	almont_font_surf = IMG_Load( "gfx/failed.png" );
	
	// Init the font system
	blue_font.Surface = blue_font_surf;
	InitFont2(&blue_font);
	
	neonblue_font.Surface = neonblue_font_surf;
	InitFont2(&neonblue_font);
	
	almont_font.Surface = almont_font_surf;
	InitFont2(&almont_font);
	
	return;
}

void UnloadFontSystem()
{
	SDL_FreeSurface(blue_font_surf);
	SDL_FreeSurface(neonblue_font_surf);
	SDL_FreeSurface(almont_font_surf);
	
	return;
}
