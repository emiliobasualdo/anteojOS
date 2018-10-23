#ifndef _VIDEO_H
#define _VIDEO_H

#include "colour.h"
#include "clock.h"
#include <stdint.h>
#include "piloNumbers.h"
#include "scLib.h"
#include "stdio.h"
#include "philosophers.h"

/* sets the current clock coordinates to the given ones */
void setClockCoordinates(unsigned int *x, unsigned int *y);

/* refreshes the width and height resolutions */
void check();

/* draws a given image (on a pixel map), from given coordinates and a specified width and height */
void drawPixelImage(unsigned int , unsigned int , Colour *, unsigned int , unsigned int );

/*  */
void renderBitmap(Colour ** , const unsigned short* , unsigned int , unsigned int );

/* draws a pixel on given coordinates with a given colour */
void drawAPixelWithColour(int, int , Colour );

/* draws a given font from a given coordinate with a given font colour and background colour */
void drawFont(int, int, const char* (*getFont)(int,int), int, Colour, Colour);

/* draws a given image (on a hexa map), from given coordinates and a specified width and height */
void drawImageFromHexaMap(unsigned  int , unsigned int , const unsigned short* , unsigned int , unsigned int );

/* Renders a string font */
void renderFont(Colour * start,const char* (*getFont)(int,int), int font,Colour fColour,Colour bColour);

/* clears the current window and sets a new one */
void newWindow();

/* draws the dining table for the dining philosophers problem with no philosophers*/
void drawDiningTableInit();

/* draws the dining table for the dining philosophers problem with the #size philosophers and their states*/
void drawDiningTable(int * state, int size);

/* clears the plate of a philosopher */
void clearPlate (int index);

#endif
