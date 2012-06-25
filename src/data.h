/******************************************************************************
 * name: data.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 28.07.03
 * description: This file holds the functions to initialize the global data
 ******************************************************************************/

#ifndef DATA_H
#define DATA_H

void LoadGameData();
void UnloadGameData();

void LoadFontSystem();
void UnloadFontSystem();

SDL_Surface *LoadImage(char *filename);
Mix_Chunk *LoadSound(char *filename);

#endif
