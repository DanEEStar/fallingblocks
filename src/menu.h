/******************************************************************************
 * name: menu.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 01.08.03
 * description: definitions for the menu
 ******************************************************************************/

#ifndef MENU_H
#define MENU_H

// the menu id's
enum
{
	MENU_START = 100,
	MENU_CONFIG,
	MENU_ABOUT,
	MENU_QUIT
};

typedef struct MenuEntry_type
{
	// the position of the entry in the menu
	int x, y, w, h;
	
	// is the entry active
	int active;
	
	// the unique id of the menu
	int entry_id;
	
	SDL_Surface *surf_low, *surf_high;
} MenuEntry;

typedef struct Menu_type
{
	MenuEntry *entry;
	int x, y;
	int num_of_entries;
	int init_entries;
} Menu;

// load all the menu data and initalize it
void LoadMenuData( Menu *current_menu );

void UnloadMenuData( Menu *current_menu );

// this function initializes the menu
void MenuInit(Menu *current_menu, int entries);

// looks if a menu entry gets active
void MenuHandleMotion( Menu *current_menu, int x, int y );

// process a mouse click and returns the actual menu item-id
int MenuHandleClick( Menu *current_menu );

// add an entry to the menu
void MenuAddEntry( Menu *current_menu, int entry_id, char *img_low, char *img_high );

// draw the menu
void MenuDraw( Menu *current_menu );

#endif
