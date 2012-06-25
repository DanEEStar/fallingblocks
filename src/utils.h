/******************************************************************************
 * name: utils.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 09.08.03
 * description: some handy utility-functions
 ******************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <SDL_mixer.h>

// returns 1 if the point is in the rectangle 0 if not
int PointInRect(int x, int y, SDL_Rect *rect);

// returns the keysym constant of a key
int WaitForKeyPress();

// plays a sound if play is true
void PlaySound( Mix_Chunk *sound, int play );

#endif
