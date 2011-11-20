/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2011 Jimmy Christensen <dusted@dusted.dk>             *
 * Wizznic is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * Wizznic is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#include "levelselector.h"
#include <math.h>
#include "text.h"
#include "pixel.h"
#include "ticks.h"
#include "pack.h"
#include "defs.h"
#include "settings.h"

static int lastSelected = -1;
static SDL_Surface* lvlPreviewImg=0;
static char buf[128];
static char buf2[128];
static char buf3[128];


void resetLevelSelector()
{
  lastSelected=-1;
  if(lvlPreviewImg)
  {
    SDL_FreeSurface(lvlPreviewImg);
    lvlPreviewImg=0;
  }
}

void levelSelector(SDL_Surface* screen, int l, int stats)
{

  if(lastSelected != l)
  {
    //Unload the previously loaded level-preview
    if(lastSelected != -1)
    {
      SDL_FreeSurface(lvlPreviewImg);
    }
    //Load the level-preview for the currently selected image
    //printf("Loading file: %s\n",levelInfo(l)->imgFile);
    lvlPreviewImg=loadImg(levelInfo(l)->imgFile);

    //If it wasen't found, load the "No image" image
    if(!lvlPreviewImg) lvlPreviewImg=loadImg("data/nolvlimg.png");

    lastSelected=l;
    sprintf(buf, "Level %i", l);
    sprintf(buf2, "Time %i:%1.2i", levelInfo(l)->time/60, levelInfo(l)->time%60 );
    sprintf(buf3,"By: %s", levelInfo(l)->author);
  }

  if(l+1 > getNumLevels()) stats=2;

  drawPreviewImg(screen, lvlPreviewImg, stats);

  if(stats!=2)
  {
    txtWriteCenter(screen, FONTMEDIUM, buf, HSCREENW,HSCREENH-22 );
    txtWriteCenter(screen, FONTSMALL, levelInfo(l)->levelName, HSCREENW,HSCREENH);
    txtWriteCenter(screen, FONTSMALL, buf3, HSCREENW,HSCREENH+12 );

    if(stats)
    {
      txtWriteCenter(screen, FONTSMALL, buf2, HSCREENW,HSCREENH+36 );
    } else if(stats) {
      txtWriteCenter(screen, FONTSMALL, "Locked!", HSCREENW,HSCREENH+36 );
    }
  }
}



static float z=200.0;
static float rot=0.0;
void drawPreviewImg(SDL_Surface* screen, SDL_Surface* img, int stats)
{
  int x, y; //In the source image
  int offSetX=HSCREENW-(55*z/100);
  int offSetY=HSCREENH-(55*z/100);
  int nx, ny; //new x/y value for px
  uint32_t col; //Color of pixel
  char r,g,b;
  uint32_t grey;

  float pxInc = 6.28318531/110.0;

  float xInc;

  rot-=(float)getTicks()/200;


  for(y=0; y < 110; y++)
  {
    xInc = cos(rot+y*pxInc)*1000;

    for(x=0; x < 110; x++)
    {
      col = *(uint32_t*)( (char*)(img->pixels)+img->pitch*y+img->format->BytesPerPixel*x );

      //Do expensive colorkeying
      r = ((col & img->format->Rmask) >> img->format->Rshift);
      g = ((col & img->format->Gmask) >> img->format->Gshift);
      b = ((col & img->format->Bmask) >> img->format->Bshift);
      if( (setting()->bpp==2 && !(r==0 && g==63 && b==31)) || (setting()->bpp==3 && !(r==0x0 && g==0xff && b==0xff) ) )
      {

        //Do b/w
        if(!stats)
        {
          grey = (r+g+b)/3;
          col = (grey << img->format->Rshift) | (grey << img->format->Gshift)<<((setting()->bpp==2)?1:0) | (grey << img->format->Bshift);
        }


        nx = x*(int)z;
        ny = y*(int)z;
        nx += xInc;
        nx /= 100;
        ny /= 100;

        plotPixel(screen, nx+offSetX,ny+offSetY, col);
      }
    }
  }
}





