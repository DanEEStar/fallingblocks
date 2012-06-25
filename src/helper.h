/******************************************************************************
 * name: helper.h
 * author: Daniel Egger, degger@bluemail.ch
 * date: 07.08.04
 * description: some helper functions for the main game
 ******************************************************************************/

#ifndef HELPER_H
#define HELPER_H

void GetGameOptions( Config *conf, GameInput *input, GameOptions *options );

/******************************************************************************/
void DrawPause();

/******************************************************************************/
void DrawGameOver();

/******************************************************************************/
void DrawAbout();

/******************************************************************************/
void MakeScreenShot();

#endif
