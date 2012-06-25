/******************************************************************************
 * name: main.c
 * author: Daniel Egger, degger@bluemail.ch
 * date: 07.08.04
 * description: This is the file containing the main function and the main
 * game loop
 ******************************************************************************/

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

GameOptions options;

extern Mix_Music *music;

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
	
	
	/**
	 * Initialize the whole game
	 */
	
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
	
	options.startLevel = 1;
	options.playSound = 1;
	options.playMusic = 1;
	options.musicStarted = 0;
	
	srand( SDL_GetTicks() );
	ResetGameInput( &input );
	LoadMenuData( &new_menu );
	LoadFontSystem();
	GetGameOptions( &conf, &input, &options );
	game_state = GAME_MENU;
	
	cur_ticks = prev_ticks = SDL_GetTicks();
	
	
	/*
	 * The main game loop
	 */
	
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
							if( game_state != GAME_CONFIG || game_state != GAME_ABOUT )
							{
								game_state = GAME_QUIT;
							}
							break;
						}
						case SDLK_ESCAPE:
						{
							if( game_state == GAME_RUN )
							{
								game_state = GAME_OVER;
							}
							break;
						}
						case SDLK_f:
						{
							bDrawFrames = !bDrawFrames;
							break;
						}
						case SDLK_F1:
						{
							MakeScreenShot();
							break;
						}
						case SDLK_s:
						{
							if( options.playSound != 0 )
							{
								options.playSound = 0;
							}
							else
							{
								options.playSound = 1;
							}
							break;
						}
						case SDLK_m:
						{
							if( options.playMusic != 0 )
							{
								options.playMusic = 0;
								if( options.musicStarted )
								{
									Mix_PauseMusic();
								}
							}
							else
							{
								options.playMusic = 1;
								if( options.musicStarted )
								{
									Mix_ResumeMusic();
								}
							}
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
							// print some infos about the video mode
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
		switch( game_state )
		{
			case GAME_RUN:
			{
				// get input
				GetGameInput( &input, keystate );
				
				// update game
				if( UpdateGame( &new_game, &input, ms ) == 0 )
				{
					// the game is over
					game_state = GAME_OVER;
				}
				break;
			}
			case GAME_OVER:
			{
				Mix_HaltMusic();
				options.musicStarted = 0;
				DrawGame( &new_game );
				DrawGameOver();
				SDL_Flip( screen );
				SDL_Delay( 1000 );
				WaitForKeyPress();
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
				if( buttonstate & SDL_BUTTON(1) )
				{
					menu_id = MenuHandleClick( &new_menu );
					switch (menu_id)
					{
						case MENU_START:
						{
							LoadGameData();
							InitGame( &new_game, options.startLevel );
							game_state = GAME_RUN;
							if( options.playMusic )
							{
								Mix_PlayMusic(music, -1);
								options.musicStarted = 1;
							}
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
				printf( "Thx for playing falling-blocks\n" );
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
		if( bDrawFrames )
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
