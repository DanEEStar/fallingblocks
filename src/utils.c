/******************************************************************************
 * name: utils.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 09.08.03
 * description: some handy utility-functions
 ******************************************************************************/

#include "utils.h"
#include "defs.h"

// returns 1 if the point is in the rectangle 0 if not
int PointInRect(int x, int y, SDL_Rect *rect)
{
	if( x > rect->x && x < rect->x + rect->w
	   && y > rect->y && y < rect->y + rect->h)
   	{
		// the point is in the Rectangle
		return 1;
	}
	else
	{
		// point is not in the rectangle
		return 0;
	}
	
	return 0;
}

int WaitForKeyPress()
{
	SDL_Event event;
	
	while( SDL_WaitEvent( &event ) != 0 )
	{
		if( event.type == SDL_KEYDOWN )
		{
			return event.key.keysym.sym;
		}
	}
	
	return 0;
}

void PlaySound( Mix_Chunk *sound, int play )
{
	if( play )
	{
		Mix_PlayChannel( -1, sound, 0 );
	}
}

