#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "defs.h"

#ifdef WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL_image.lib")
#pragma comment(lib, "SDL_mixer.lib")
#endif

// the possible game states
enum {
	GAME_RUN = 1000,
	GAME_PAUSE,
	GAME_OVER,
	GAME_MENU,
	GAME_CONFIG,
	GAME_ABOUT,
	GAME_HIGHSCORE,
	GAME_QUIT
};

SDL_Surface *screen;

extern Mix_Music *music;

void GetOptions( Config *conf, GameInput *input )
{
	ConfigInit( conf, 9, 50, 50, 500);
	ConfigAddNumberRange( conf, "Sound volume", 5, 0, 10 );
	ConfigAddNumberRange( conf, "Music Volume", 5, 0, 10 );
	ConfigAddToggle( conf, "Play sounds", 1 );
	ConfigAddToggle( conf, "Play music", 1 );
	ConfigAddKey( conf, "Rotation Key", &input->key_rotate_right );
	ConfigAddKey( conf, "Left Key", &input->key_left );
	ConfigAddKey( conf, "Right Key", &input->key_right );
	ConfigAddKey( conf, "Down Key", &input->key_down );
	ConfigAddSimpleText( conf, "This is a test" );
	
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
	XCenteredString2(screen, &neonblue_font, 130, "Daniel Egger");
	XCenteredString2(screen, &blue_font, 160, "www.danielegger.tk");
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

static void WaitForInput( void )
{
	SDL_Event event;
	
	while ( 1 ) {
		SDL_WaitEvent(&event);
		if (event.type == SDL_QUIT) {
			return;
		}
		if (event.type == SDL_KEYDOWN)
			return;
	}
}

int main(int argc, char *argv[])
{
	int cur_ticks, prev_ticks;
	int ms;
	char frame[255];
	int game_state;
	double frames = 0;
	double time = 0;
	int bDrawFrames = 0;
	int bPause = 0;
		
	int menu_id = 0;
	
	Uint8 *keystate = 0;
	int buttonstate = 0;
	int x, y;
	
	int vidflags = 0;
	
	int done = 0;
	
	SDL_Event event;
	
	GameInput input;
	Game new_game;
	Menu new_menu;
	Config conf;
	
	if(argc == 2)
	{
		if(strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0)
		{
			printf("-h or --help: print this help message\n");
			printf("-f or --fullscreen: execute in fullscreen\n");
			printf("without anything %s starts in window-mode\n", argv[0]);
			exit(0);
		}
		if(strcmp("--fullscreen", argv[1]) == 0)
		{
			vidflags = 1;
		}
	}
	
	/* Initialize SDL's Video system */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	
	atexit(SDL_Quit);
	
	// try to open the audio defice
	if(Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024 ))
	{
		printf("Unable to init audio: %s\n", Mix_GetError());
		return 1;
	}
	
	printf("bpp: %d\n", SDL_VideoModeOK(800, 600, 16, SDL_DOUBLEBUF | SDL_HWSURFACE ));
	
	/* Try to set video mode */
	if(vidflags)
		screen = SDL_SetVideoMode(800, 600, 32, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_ANYFORMAT | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(800, 600, 32, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_ANYFORMAT);
	
	if(screen == NULL)
	{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	
	srand( SDL_GetTicks() );
	ResetGameInput(&input);
	LoadMenuData( &new_menu );
	LoadFontSystem();
	GetOptions( &conf, &input );
	game_state = GAME_MENU;
	
	cur_ticks = prev_ticks = SDL_GetTicks();
	
	while( !done )
	{
		prev_ticks = cur_ticks;
		cur_ticks = SDL_GetTicks();
		ms = cur_ticks - prev_ticks;
		
		// update events
		SDL_PumpEvents();
		buttonstate = SDL_GetMouseState( &x, &y );
		keystate = SDL_GetKeyState( 0 );
		
		// get Input
		while(SDL_PollEvent(&event) != 0)
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
				{
					switch( event.key.keysym.sym )
					{
						case SDLK_q:
						{
							game_state = GAME_QUIT;
							break;
						}
						case SDLK_f:
						{
							bDrawFrames = !bDrawFrames;
							break;
						}
						case SDLK_s:
						{
							MakeScreenShot();
							break;
						}
						case SDLK_p:
						{
							if(game_state == GAME_RUN || game_state == GAME_PAUSE)
							{
								bPause = !bPause;
								if(bPause)
								{
									game_state = GAME_PAUSE;
								}
								else
								{
									game_state = GAME_RUN;
								}
								
								break;
							}
						}
						case SDLK_d:
						{
							if(game_state == GAME_RUN)
							{
								extern SDL_Surface *game_back;
								PrintVideoInfo();
								PrintSurfaceInfo(screen, "screen");
								PrintSurfaceInfo(game_back, "game_back");
							}
							break;
						}
					}
					break;
				}
				case SDL_QUIT:
				{
					game_state = GAME_QUIT;
					break;
				}
			}
		}
		
		frames += 1;
		time += (float)ms / 1000;
		
		// game_loop
		switch(game_state)
		{
			case GAME_RUN:
			{
				// get input
				GetGameInput( &input, keystate );
				
				// update game
				if(UpdateGame( &new_game, &input, ms ) == 0)
				{
					// the game is over
					game_state = GAME_OVER;
				}
				break;
			}
			case GAME_OVER:
			{
				Mix_HaltMusic();
				DrawGame( &new_game );
				DrawGameOver();
				SDL_Flip(screen);
				SDL_Delay(1000);
				WaitForInput();
				game_state = GAME_MENU;
				LoadMenuData( &new_menu );
				UnloadGameData();
				break;
			}
			case GAME_PAUSE:
			{
				DrawPause();
				break;
			}
			case GAME_MENU:
			{
				MenuDraw( &new_menu );
				// look if the left mousbutton is down
				if(buttonstate & SDL_BUTTON(1))
				{
					menu_id = MenuHandleClick( &new_menu );
					switch (menu_id)
					{
						case MENU_START:
						{
							LoadGameData();
							InitGame(&new_game, 6);
							game_state = GAME_RUN;
							Mix_PlayMusic(music, -1);
							UnloadMenuData( &new_menu );
							break;
						}
						case MENU_CONFIG:
						{
							game_state = GAME_CONFIG;
							UnloadMenuData( &new_menu );
							break;
						}
						case MENU_ABOUT:
						{
							game_state = GAME_ABOUT;
							UnloadMenuData( &new_menu );
							break;
						}
						case MENU_QUIT:
						{
							game_state = GAME_QUIT;
							UnloadMenuData( &new_menu );
							break;
						}
					}
				}
				else
				{
					MenuHandleMotion( &new_menu, x, y );
				}
				
				break;
			}
			case GAME_QUIT:
			{
				printf("Thx for playing falling-blocks\n");
				done = 1;
				break;
			}
			case GAME_ABOUT:
			{
				if( keystate[SDLK_ESCAPE] )
				{
					LoadMenuData( &new_menu );
					game_state = GAME_MENU;
				}
				else
				{
					DrawAbout();
				}
				break;
			}
			case GAME_CONFIG:
			{
				if( keystate[SDLK_ESCAPE] )
				{
					LoadMenuData( &new_menu );
					game_state = GAME_MENU;
				}
				else if( keystate[SDLK_r] )
				{
					ResetGameInput(&input);
				}
				else
				{
					static int click = 0;
					SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 0));
					ConfigDraw( &conf );
					
					if( buttonstate & SDL_BUTTON( 1 ) )
					{
						if( click == 0 )
						{
							
							ConfigHandleClick( &conf );
							click = 1;
						}
					}
					else
					{
						click = 0;
						ConfigHandleMotion( &conf, x, y );
					}
				}
				break;
			}
		}
		
		// Draw Framrate
		if(bDrawFrames)
		{
			extern SFont_FontInfo blue_font;
			sprintf(frame, "%d", ms);
			PutString2(screen, &blue_font, 20, 300, frame);
		}
		
		// Flip Surfaces
		SDL_Flip(screen);
	}

	return 0;
}
