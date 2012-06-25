/******************************************************************************
 * name: helper.c
 * author: Daniel Egger, degger@bluemail.ch
 * date: 07.08.04
 * description: some helper functions for the main game
 ******************************************************************************/

#include "defs.h"
#include "helper.h"

extern SDL_Surface *screen;

void GetGameOptions( Config *conf, GameInput *input, GameOptions *options )
{
	ConfigInit( conf, 8, 50, 50, 500);
	//ConfigAddNumberRange( conf, "Sound volume", 5, 0, 10 );
	//ConfigAddNumberRange( conf, "Music Volume", 5, 0, 10 );
	ConfigAddToggle( conf, "Play sounds", &options->playSound );
	ConfigAddToggle( conf, "Play music", &options->playMusic );
	ConfigAddNumberRange( conf, "Starting level", &(options->startLevel), 1, 9 );
	ConfigAddKey( conf, "Rotation Key", &input->key_rotate_right );
	ConfigAddKey( conf, "Left Key", &input->key_left );
	ConfigAddKey( conf, "Right Key", &input->key_right );
	ConfigAddKey( conf, "Down Key", &input->key_down );
	ConfigAddSimpleText( conf, "Press Escpape to return to main menu" );
	
	return;
}

void DrawPause()
{
	extern SFont_FontInfo almont_font;
	extern SDL_Surface *pause;
	SDL_Rect dest;
	dest.x = 400 - pause->w / 2;
	dest.y = 50;
	
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 0));
	SDL_BlitSurface(pause, NULL, screen, &dest);
	XCenteredString2(screen, &almont_font, 300, "press 'p' to continue");
	
	return;
}

void DrawGameOver()
{
	extern SDL_Surface *over;
	SDL_Rect dest;
	dest.x = 400 - over->w / 2;
	dest.y = 300 - over->h / 2;
	
	SDL_BlitSurface(over, NULL, screen, &dest);
	
	return;
}

void DrawAbout()
{
	extern SFont_FontInfo blue_font;
	extern SFont_FontInfo neonblue_font;
	
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 128, 0));
	XCenteredString2(screen, &blue_font, 100, "This game was createt by:");
	XCenteredString2(screen, &neonblue_font, 130, "Daniel \"DanEE\" Egger");
}

void MakeScreenShot()
{
	static i = 1;
	char buf[32];
	
	sprintf(buf, "screen%d.bmp", i);
	SDL_SaveBMP(screen, buf);
	
	i++;
	
	return;
}
