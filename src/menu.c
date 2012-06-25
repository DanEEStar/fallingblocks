/******************************************************************************
 * name: menu.c
 * author: Daniel Egger, degger@bluemail.ch
 * date: 01.08.03
 * description: definitions for the menu
 ******************************************************************************/

#include <stdlib.h>
#include "defs.h"
#include "menu.h"

#define MENU_X	30
#define MENU_Y	130

#define ENTRY_HEIGHT	100

// Menu Surfaces
static SDL_Surface *menu_back;

extern SDL_Surface *screen;

extern GameOptions options;

// Sounds
static Mix_Chunk *motion;

/******************************************************************************/
void LoadMenuData( Menu *current_menu )
{
	menu_back = LoadImage("gfx/menu.jpg");
	motion = LoadSound("sound/motion.wav");
	
	MenuInit( current_menu, 4 );
	
	MenuAddEntry( current_menu, MENU_START, "gfx/startlow.png", "gfx/starthigh.png" );
	MenuAddEntry( current_menu, MENU_CONFIG, "gfx/optionslow.png", "gfx/optionshigh.png" );
	MenuAddEntry( current_menu, MENU_ABOUT, "gfx/aboutlow.png", "gfx/abouthigh.png") ;
	MenuAddEntry( current_menu, MENU_QUIT, "gfx/quitlow.png", "gfx/quithigh.png" );
	
	return;
}

void UnloadMenuData( Menu *current_menu )
{
	int i;
	
	SDL_FreeSurface(menu_back);
	
	// free all entry surfaces
	for(i = 0; i < current_menu->init_entries; i++)
	{
		SDL_FreeSurface(current_menu->entry[i].surf_low);
		SDL_FreeSurface(current_menu->entry[i].surf_high);
	}
	
	// free the data structure
	free(current_menu->entry);
	current_menu->entry = NULL;
}

/******************************************************************************/
void MenuInit(Menu *current_menu, int entries)
{	
	current_menu->entry = NULL;
	current_menu->x = MENU_X;
	current_menu->y = MENU_Y;
	current_menu->num_of_entries = entries;
	current_menu->init_entries = 0;
	
	
	// try to get some memory
	current_menu->entry = (MenuEntry *) malloc( entries * sizeof(MenuEntry));
	if(!current_menu->entry)
	{
		// not enough memory!
		printf("Out of memory in menu entry\n");
		exit(0);
	}
	
	return;
}

/******************************************************************************/
void MenuAddEntry( Menu *current_menu, int entry_id, char *img_low, char *img_high )
{
	if(current_menu->init_entries + 1 > current_menu->num_of_entries)
	{
		// to many entries added
		printf("to many entries added\n");
		exit(0);
	}
	
	current_menu->entry[current_menu->init_entries].x = current_menu->x;
	current_menu->entry[current_menu->init_entries].y = current_menu->y + ENTRY_HEIGHT * current_menu->init_entries;
	current_menu->entry[current_menu->init_entries].entry_id = entry_id;
	
	current_menu->entry[current_menu->init_entries].surf_low = LoadImage( img_low );
	current_menu->entry[current_menu->init_entries].surf_high = LoadImage( img_high );
	
	current_menu->entry[current_menu->init_entries].w = current_menu->entry[current_menu->init_entries].surf_high->w;
	current_menu->entry[current_menu->init_entries].h = current_menu->entry[current_menu->init_entries].surf_high->h;
	
	current_menu->entry[current_menu->init_entries].active = 0;
	
	current_menu->init_entries++;
	
	return;
}

/******************************************************************************/
void MenuHandleMotion( Menu *current_menu, int x, int y)
{
	int i;
	SDL_Rect rect;
	
	// look if one of the entries is active
	for(i = 0; i < current_menu->init_entries; i++)
	{
		rect.x = current_menu->entry[i].x;
		rect.y = current_menu->entry[i].y;
		rect.w = current_menu->entry[i].w;
		rect.h = current_menu->entry[i].h;
		
		if( PointInRect( x, y, &rect) )
		{
			// is entry already active?
			if(!current_menu->entry[i].active)
			{
				// this entry becomes active
				current_menu->entry[i].active = 1;
				
				// play a sound
				PlaySound( motion, options.playSound );
			}
		}
		else
		{
			// entry not acitve
			current_menu->entry[i].active = 0;
		}
	}
	
	return;
}

/******************************************************************************/
int MenuHandleClick( Menu *current_menu )
{
	int i;
	
	// look for an active menu item
	for( i = 0; i < current_menu->init_entries; i++ )
	{
		if(current_menu->entry[i].active)
		{
			// an item is active return its id
			return current_menu->entry[i].entry_id;
		}
	}
	
	// no item active
	return 0;
}

/******************************************************************************/
void MenuDraw( Menu *current_menu )
{
	int i;
	SDL_Rect dest;
	
	// draw background
	SDL_BlitSurface(menu_back, NULL, screen, NULL);
	
	// draw elements
	for(i = 0; i < current_menu->init_entries; i++)
	{
		dest.x = current_menu->entry[i].x;
		dest.y = current_menu->entry[i].y;
		
		if(current_menu->entry[i].active)
			SDL_BlitSurface(current_menu->entry[i].surf_high, NULL, screen, &dest);
		else
			SDL_BlitSurface(current_menu->entry[i].surf_low, NULL, screen, &dest);
	}
	
	return;
}
