/******************************************************************************
 * name: config.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 01.08.03
 * description: definitions for the config-window
 ******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#define TEXT_LENGHT	64
#define NAME_LENGHT	32

// the possible config-entries
enum
{
	CONF_NUM_RANGE = 10,
	CONF_TOGGLE,
	CONF_KEY,
	CONF_SIMPLE_TEXT
};

// the entry-elements
typedef struct ConfigNumberRange_type
{
	char name[NAME_LENGHT];
	char numstring[8];
	int *active_num;
	int range_min, range_max;
} ConfigNumberRange;

typedef struct ConfigToggle_type
{
	char name[NAME_LENGHT];
	char strstatus[8];
	int *status;	// 1 for on, 0 for off
} ConfigToggle;

typedef struct ConfigKey_type
{
	char name[NAME_LENGHT];
	char keyname[16];
	int *key_id;	// the SDL-Key constant
} ConfigKey;

// this structure holds a config entry
typedef struct ConfigEntry_type
{
	ConfigNumberRange *num_range;
	ConfigToggle *toggle;
	ConfigKey *key;
	
	char text[TEXT_LENGHT];
	int type;
	int x, y, w, h;
	int active;
} ConfigEntry;

// this is a structure for a config element
typedef struct Config_type
{
	ConfigEntry *entry;
	int num_of_entries;
	int init_entries;
	
	int x, y, w, h;
} Config;

// this function initialize a config-structure
void ConfigInit( Config *conf, int number, int x, int y, int h );

// these functions initialze a certain entry
void ConfigAddNumberRange( Config *conf, char *text, int *active, int min, int max );
void ConfigAddToggle( Config *conf, char *text, int *status );
void ConfigAddKey( Config *conf, char *text, int *key_id );
void ConfigAddSimpleText( Config *conf, char *text );

// this function draws the config-screen
void ConfigDraw( Config *conf );

// this fucntion handle the motion of the mouse
void ConfigHandleMotion( Config *conf, int x, int y );

void ConfigHandleClick( Config *conf );

#endif
