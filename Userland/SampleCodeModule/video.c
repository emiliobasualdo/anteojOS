#include "video.h"

unsigned int currentX = 0;
unsigned int currentY = 0;

unsigned int xRes = 0;
unsigned int yRes = 0;

Colour white = {255, 255, 255};
Colour green = {0, 255, 0};
Colour red = {255, 0, 0};
Colour yellow = {255, 234, 0};

#define THINKING 2                  /** el filósofo está pensando */
#define HUNGRY 1                    /** el filósofo está buscando los tenedores */
#define EATING 0                    /** el filósofo está comiendo */

static void drawCircle(int x0, int y0, int radius, Colour colour);
static void drawPlate(int x0, int y0, Colour colour);
static void drawNameAndState(int x0, int y0, char * name, int state);

void check()
{
    if (xRes == 0 || yRes == 0)
    {
        getResolutions(&xRes,&yRes);
    }
}

void drawAPixelWithColour(int x, int y, Colour col)
{
    userDrawPixel(x,y,col.red,col.green,col.blue);
}

void drawImageFromHexaMap(unsigned  int ox, unsigned int oy, const unsigned short* hexaMap, unsigned int width, unsigned int height)
{
    Colour b;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            unsigned short hexValue = hexaMap[i*width + j];
            b.red = (uint8_t) ((hexValue >> 16) & 0xFF);
            b.green = (uint8_t) ((hexValue >> 8) & 0xFF);
            b.blue = (uint8_t) ((hexValue) & 0xFF);
            drawAPixelWithColour(ox+j,oy+i,b);
        }
    }
}

void newWindow ()
{
    check();
    for (int j=0; j<yRes; j++)
    {
        for (int i=0; i<xRes; i++)
        {
            drawAPixelWithColour(i, j, getCurrentBackgroundColour());
        }
    }
    currentX=0;
    currentY=0;
    setCoordinates(currentX, currentY);
}

void drawDiningTableInit()
{
    check();
    newWindow();
    drawCircle(xRes/2, yRes/2, yRes/3, white);
}

void drawDiningTable(int * state, int size)
{

    int x0, y0;

    printF("To increase the number of philosophers, press 1. Up to 5 philosophers are available.\n"
           "To lower the number of philosophers, press 0.\n"
           "To quit, press 'q'.\n");

    for (int i=0; i<size; i++)
    {
        y0 = yRes/2;
        x0 = xRes/2;
        switch (i)
        {
            case 0:
                y0 -= yRes/4;
                break;
            case 1:
                x0 -= xRes/7;
                y0 -= yRes/7;
                break;
            case 2:
                x0 -= xRes/7;
                y0 += yRes/7;
                break;
            case 3:
                x0 += xRes/7;
                y0 += yRes/7;
                break;
            case 4:
                x0 += xRes/7;
                y0 -= yRes/7;
                break;
            default: break;
        }
        switch (state[i])
        {
            case EATING:
                drawPlate(x0, y0, green);
                break;
            case HUNGRY:
                drawPlate(x0, y0, yellow);
                break;
            default:
                drawPlate(x0, y0, red);
                break;
        }
        drawNameAndState(x0-yRes/20, y0 + yRes/20, getPhilName(i), state[i]);
    }
}

static void drawPlate(int x0, int y0, Colour colour)
{
    for (int i=0; (yRes/20)-i > 0; i++)
    {
        drawCircle(x0, y0, (yRes/20)-i, colour);
    }
}

void clearPlate (int index)
{
    int y0 = yRes/2;
    int x0 = xRes/2;
    switch (index)
    {
        case 0:
            y0 -= yRes/4;
            break;
        case 1:
            x0 -= xRes/7;
            y0 -= yRes/7;
            break;
        case 2:
            x0 -= xRes/7;
            y0 += yRes/7;
            break;
        case 3:
            x0 += xRes/7;
            y0 += yRes/7;
            break;
        case 4:
            x0 += xRes/7;
            y0 -= yRes/7;
            break;
        default: break;
    }
    drawPlate(x0, y0, getCurrentBackgroundColour());
    char * name = getPhilName(index);
    int length = strlen(name) + strlen(" is thinking");
    setCoordinates(x0 - yRes/20 + length*8, y0 + yRes/20);
    for (int i = 0; i < length; i++)
    {
        removeChar();
    }
    setCoordinates(0, 0);
    drawCircle(xRes/2, yRes/2, yRes/3, white);
}

/** source = https://en.m.wikipedia.org/wiki/Midpoint_circle_algorithm */
static void drawCircle(int x0, int y0, int radius, Colour colour)
{
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        drawAPixelWithColour(x0 + x, y0 + y, colour);
        drawAPixelWithColour(x0 + y, y0 + x, colour);
        drawAPixelWithColour(x0 - y, y0 + x, colour);
        drawAPixelWithColour(x0 - x, y0 + y, colour);
        drawAPixelWithColour(x0 - x, y0 - y, colour);
        drawAPixelWithColour(x0 - y, y0 - x, colour);
        drawAPixelWithColour(x0 + y, y0 - x, colour);
        drawAPixelWithColour(x0 + x, y0 - y, colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

static void drawNameAndState(int x0, int y0, char * name, int state)
{
    setCoordinates((unsigned )x0, (unsigned )y0);
    int size = strlen(name);
    for (int i=0; i<size; i++)
    {
        drawCharU(name[i]);
    }
    drawCharU(' ');
    drawCharU('i');
    drawCharU('s');
    drawCharU(' ');
    switch (state)
    {
        case EATING:
            drawCharU('e');
            drawCharU('a');
            drawCharU('t');
            drawCharU('i');
            drawCharU('n');
            drawCharU('g');
            drawCharU(' ');
            drawCharU(' ');
            break;
        case HUNGRY:
            drawCharU('h');
            drawCharU('u');
            drawCharU('n');
            drawCharU('g');
            drawCharU('r');
            drawCharU('y');
            drawCharU(' ');
            drawCharU(' ');
            break;
        case THINKING:
            drawCharU('t');
            drawCharU('h');
            drawCharU('i');
            drawCharU('n');
            drawCharU('k');
            drawCharU('i');
            drawCharU('n');
            drawCharU('g');
            break;
        default:
            break;
    }
    setCoordinates(0, 0);
}