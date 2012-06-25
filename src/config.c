/******************************************************************************
 * name: config.c
 * author: Daniel Egger, degger@bluemail.ch
 * date: 07.08.03
 * description: definitions for the config-window
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "defs.h"

#define ARIAL_HEIGHT	36
#define VALUES_X	600

extern SDL_Surface *screen;
extern SFont_FontInfo blue_font;
extern SFont_FontInfo neonblue_font;

/******************************************************************************/
void ConfigInit( Config *conf, int number, int x, int y, int h )
{
	conf->entry = NULL;
	
	conf->x = x;
	conf->y = y;
	conf->w = 0;
	conf->h = h;
	
	conf->num_of_entries = number;
	conf->init_entries = 0;
	
	conf->entry = (ConfigEntry*) malloc( number * sizeof(ConfigEntry) );
	if(conf->entry == NULL)
	{
		// not enough memory
		printf("out of memory\n");
		exit(0);
	}
	
	return;
}

/******************************************************************************/
void ConfigInitEntry( ConfigEntry *entry )
{
	
	entry->num_range = NULL;
	entry->toggle = NULL;
	entry->key = NULL;
	
	entry->type = 0;
	entry->x = entry->y = entry->h = entry->w = 0;
	entry->active = 0;
	
	return;
}

/******************************************************************************/
void ConfigAddNumberRange( Config *conf, char *text, int *active, int min, int max )
{
	if(conf->init_entries + 1 > conf->num_of_entries)
	{
		// not enough memory allocate
		printf("too many config-entries added\n");
		exit(0);
	}
	
	ConfigInitEntry( &conf->entry[conf->init_entries] );
	
	conf->entry[conf->init_entries].num_range = (ConfigNumberRange*) malloc( sizeof(ConfigNumberRange) );
	if(conf->entry[conf->init_entries].num_range == NULL)
	{
		// not enough memory
		printf("not enough memory\n");
		exit(0);
	}
	
	// fill the strings with content
	strncpy( conf->entry[conf->init_entries].num_range->name, text, NAME_LENGHT - 1 );
	conf->entry[conf->init_entries].num_range->name[NAME_LENGHT - 1] = '\0';
	sprintf(conf->entry[conf->init_entries].num_range->numstring, "%d", *active);
	sprintf(conf->entry[conf->init_entries].text,
	        "%s: %s",
	        conf->entry[conf->init_entries].num_range->name,
	        conf->entry[conf->init_entries].num_range->numstring);
	
	// initialize the numbers
	conf->entry[conf->init_entries].num_range->active_num = active;
	conf->entry[conf->init_entries].num_range->range_min = min;
	conf->entry[conf->init_entries].num_range->range_max = max;
	
	conf->entry[conf->init_entries].type = CONF_NUM_RANGE;
	
	conf->entry[conf->init_entries].x = conf->x;
	conf->entry[conf->init_entries].y = conf->init_entries * (conf->h / conf->num_of_entries) + conf->y;
	conf->entry[conf->init_entries].w = TextWidth2(&blue_font, conf->entry[conf->init_entries].text);
	conf->entry[conf->init_entries].h = ARIAL_HEIGHT;
	
	conf->init_entries++;
	
	return;
}

/******************************************************************************/
void ConfigAddToggle( Config *conf, char *text, int *status )
{
	if(conf->init_entries + 1 > conf->num_of_entries)
	{
		// not enough memory allocate
		printf("too many config-entries added\n");
	}
	
	ConfigInitEntry( &conf->entry[conf->init_entries] );
	
	conf->entry[conf->init_entries].toggle = (ConfigToggle*) malloc( sizeof(ConfigToggle) );
	if(conf->entry[conf->init_entries].toggle == NULL)
	{
		// not enough memory
		printf("not enough memory\n");
		exit(0);
	}
	
	// initialize the strings
	strncpy( conf->entry[conf->init_entries].toggle->name, text, NAME_LENGHT - 1);
	conf->entry[conf->init_entries].toggle->name[NAME_LENGHT - 1] = '\0';
	strcpy(conf->entry[conf->init_entries].toggle->strstatus, status ? "on" : "off");
	sprintf(conf->entry[conf->init_entries].text,
	        "%s: %s",
		conf->entry[conf->init_entries].toggle->name,
		conf->entry[conf->init_entries].toggle->strstatus);
	
	// initialize the numbers
	conf->entry[conf->init_entries].toggle->status = status;
	
	conf->entry[conf->init_entries].type = CONF_TOGGLE;
	
	conf->entry[conf->init_entries].x = conf->x;
	conf->entry[conf->init_entries].y = conf->init_entries * (conf->h / conf->num_of_entries) + conf->y;
	conf->entry[conf->init_entries].w = TextWidth2(&blue_font, conf->entry[conf->init_entries].text);
	conf->entry[conf->init_entries].h = ARIAL_HEIGHT;
	
	conf->init_entries++;
	
	return;
}

/******************************************************************************/
void ConfigAddKey( Config *conf, char *text, int *key_id )
{
	if(conf->init_entries + 1 > conf->num_of_entries)
	{
		// not enough memory allocate
		printf("too many config-entries added\n");
	}
	
	ConfigInitEntry( &conf->entry[conf->init_entries] );
	
	conf->entry[conf->init_entries].key = (ConfigKey*) malloc( sizeof(ConfigKey) );
	if(conf->entry[conf->init_entries].key == NULL)
	{
		// not enough memory
		printf("not enough memory\n");
		exit(0);
	}
	
	// initialize the strings
	strncpy( conf->entry[conf->init_entries].key->name, text, NAME_LENGHT - 1 );
	conf->entry[conf->init_entries].key->name[NAME_LENGHT - 1] = '\0';
	strcpy( conf->entry[conf->init_entries].key->keyname, SDL_GetKeyName(*key_id));
	sprintf( conf->entry[conf->init_entries].text,
	         "%s: %s",
		 conf->entry[conf->init_entries].key->name,
		 conf->entry[conf->init_entries].key->keyname);
	
	// initialize the numbers
	conf->entry[conf->init_entries].key->key_id = key_id;
	
	conf->entry[conf->init_entries].type = CONF_KEY;
	
	conf->entry[conf->init_entries].x = conf->x;
	conf->entry[conf->init_entries].y = conf->init_entries * (conf->h / conf->num_of_entries) + conf->y;
	conf->entry[conf->init_entries].w = TextWidth2(&blue_font, conf->entry[conf->init_entries].text);
	conf->entry[conf->init_entries].h = ARIAL_HEIGHT;
	
	conf->init_entries++;
	
	return;
}

/******************************************************************************/
void ConfigAddSimpleText( Config *conf, char *text )
{
	if(conf->init_entries + 1 > conf->num_of_entries)
	{
		// not enough memory allocate
		printf("too many config-entries added\n");
	}
	
	ConfigInitEntry( &conf->entry[conf->init_entries] );
	
	// initialize the strings
	strncpy( conf->entry[conf->init_entries].text, text, 31 );
	conf->entry[conf->init_entries].text[31] = '\0';
	
	// initialize the numbers
	conf->entry[conf->init_entries].type = CONF_SIMPLE_TEXT;
	
	conf->entry[conf->init_entries].x = conf->x;
	conf->entry[conf->init_entries].y = conf->init_entries * (conf->h / conf->num_of_entries) + conf->y;
	conf->entry[conf->init_entries].w = TextWidth2(&blue_font, conf->entry[conf->init_entries].text);
	conf->entry[conf->init_entries].h = ARIAL_HEIGHT;
	
	conf->init_entries++;
	
	return;
}

/******************************************************************************/
// this function draws the config-screen
void ConfigDraw( Config *conf )
{
	int i;
	char buf[32];
	
	for(i = 0; i < conf->init_entries; i++)
	{
		if(conf->entry[i].active)
		{
			PutString2( screen, &neonblue_font, conf->entry[i].x, conf->entry[i].y, conf->entry[i].text);
		}
		else
		{
			PutString2( screen, &blue_font, conf->entry[i].x, conf->entry[i].y, conf->entry[i].text);
		}
	}
	return;
}

/******************************************************************************/
void ConfigHandleMotion( Config *conf, int x, int y )
{
	int i;
	SDL_Rect rect;
	
	for(i = 0; i < conf->init_entries; i++)
	{
		rect.x = conf->entry[i].x;
		rect.y = conf->entry[i].y;
		rect.h = conf->entry[i].h;
		rect.w = conf->entry[i].w;
		
		// is the menu active
		if( PointInRect(x, y, &rect) )
		{
			// was the entry already active
			if( !conf->entry[i].active )
			{
				// entry becomes active
				if( conf->entry[i].type != CONF_SIMPLE_TEXT )
				{
					conf->entry[i].active = 1;
				}
			}
			
			if( conf->entry[i].type == CONF_KEY )
			{
				strcpy( conf->entry[i].key->keyname, SDL_GetKeyName(*conf->entry[i].key->key_id));
				sprintf( conf->entry[i].text,
				         "%s: %s",
					 conf->entry[i].key->name,
					 conf->entry[i].key->keyname);
			}
		}
		else
		{
			// entry is not active
			conf->entry[i].active = 0;
		}
	}
	
	return;
}

/******************************************************************************/
void ConfigHandleClick( Config *conf )
{
	int i;
	char buf[32];
	
	for( i = 0; i < conf->init_entries; i++ )
	{
		if( conf->entry[i].active )
		{
			switch( conf->entry[i].type )
			{
				case CONF_NUM_RANGE:
				{
					if( ++*conf->entry[i].num_range->active_num > conf->entry[i].num_range->range_max )
					{
						*conf->entry[i].num_range->active_num = conf->entry[i].num_range->range_min;
					}
					sprintf(conf->entry[i].num_range->numstring, "%d", *conf->entry[i].num_range->active_num);
					sprintf(conf->entry[i].text,
					        "%s: %s",
					         conf->entry[i].num_range->name,
					         conf->entry[i].num_range->numstring);
					
					break;
				}
				case CONF_KEY:
				{
					// draw message box
					SDL_Rect dest;
					SDL_Surface *newkey;
					
					dest.x = 250;
					dest.y = 300;
					
					newkey = LoadImage( "gfx/newkey.png" );
					SDL_BlitSurface( newkey, NULL, screen, &dest );
					SDL_Flip( screen );
					
					// wait for key press
					*conf->entry[i].key->key_id = WaitForKeyPress();
					
					strcpy( conf->entry[i].key->keyname, SDL_GetKeyName(*conf->entry[i].key->key_id));
					sprintf( conf->entry[i].text,
					         "%s: %s",
						 conf->entry[i].key->name,
						 conf->entry[i].key->keyname);
					break;
				}
				case CONF_TOGGLE:
				{
					if( *conf->entry[i].toggle->status != 0 )
					{
						*conf->entry[i].toggle->status = 0;
					}
					else
					{
						*conf->entry[i].toggle->status = 1;
					}
					
					strcpy(conf->entry[i].toggle->strstatus,
						*conf->entry[i].toggle->status ? "on" : "off");
					sprintf( conf->entry[i].text,
						"%s: %s",
						conf->entry[i].toggle->name,
						conf->entry[i].toggle->strstatus );
					
					break;
				}
			}
		}
	}
	
	return;
}
