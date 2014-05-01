/* gget.c (emx+gcc) -- Copyright (c) 1987-1993 by Johannes Martin */

#include <sys/hw.h>
#include <vesa.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

int FLAT16_get(int x, int y)
{
  int offset, mask, size;
  int i, color;

  offset = y * _g_linesize / 4 + x / 8;
  mask   = 0x80 >> (x % 8);
  size   = g_memsize / 4;
  color  = 0;

  i = 4;
  while (i-- > 0)
    {
      color <<= 1;
      if (_g_rmem[i * size + offset] & mask)
	color |= 1;
    }
  return(color);
}

int FLAT256_get(int x, int y)
{
  return(_g_rmem[y * _g_linesize + x]);
}

int FLAT32k_get(int x, int y)
{
  return(*(short *) (_g_rmem + y * _g_linesize + x * 2));
}

int FLAT16m_get(int x, int y)
{
  int offset = y * _g_linesize + x * 3;

  return ((int)(_g_rmem[offset]) | ((int)_g_rmem[offset + 1] << 8) | ((int)_g_rmem[offset + 2] << 16));
}

int FLAT16m_32Bit_get(int x, int y)
{
  return *(int*)(_g_rmem + y * _g_linesize + (x << 2));
}


int SEG16_get(int x, int y)
{
  int offset, win;

  offset  = y * _g_linesize + (x >> 3);
  win     = offset >> _g_winshift;
  offset &= _g_winmask;

  GSETRWIN(win);

  y = 0;
  x = 7 - (x & 7);
  for (win = 0; win < 4; win++)
    {
      _outp16(GRA_I, (win << 8) | 4);
      y |= ((_g_rmem[offset] >> x) & 1) << win;
    }
  return(y);
}

int SEG256_get(int x, int y)
{
  int offset, win;

  offset = y * _g_linesize + x;
  win    = offset >> _g_winshift;

  GSETRWIN(win);

  return(_g_rmem[offset & _g_winmask]);
}

int SEG32k_get(int x, int y)
{
  int offset, win;

  offset = y * _g_linesize + (x << 1);
  win    = offset / _g_wingran;

  GSETRWIN(win);

  return(*(short *) (_g_rmem + (offset & _g_winmask)));
}

int SEG16m_get(int x, int y)
{
  int offset, win;

  offset = y * _g_linesize + x * 3;
  win    = offset >> _g_winshift;

  GSETRWIN(win);

  offset &= _g_winmask;

  return ((int)(_g_rmem[offset]) | ((int)_g_rmem[offset + 1] << 8) | ((int)_g_rmem[offset + 2] << 16));
}

int SEG16m_split_get(int x, int y)
{
  int offset, win, c;

  offset = y * _g_linesize + x * 3;
  win    = offset >> _g_winshift;

  GSETRWIN(win);

  offset &= _g_winmask;

  if ((SplitPositions[win].is_critical) &&
      (SplitPositions[win].x == x) &&
      (SplitPositions[win].y == y))
    {
      if (SplitPositions[win].is_critical == 1)
	{
	  c  = _g_rmem[offset];
	  c |= (int)_g_rmem[offset+1] << 8;
	  GSETRWIN(win+1);
	  c |= (int)_g_rmem[0] << 16;
	}
      else
	{
	  c  = _g_rmem[offset];
	  GSETRWIN(win+1);
	  c |= (int)_g_rmem[0] << 8;
	  c |= (int)_g_rmem[1] << 16;
	}
      return c;
    }
  else return ((int)(_g_rmem[offset]) | ((int)_g_rmem[offset + 1] << 8) | ((int)_g_rmem[offset + 2] << 16));
}

int SEG16m_32Bit_get(int x, int y)
{
  int offset, win;

  offset = y * _g_linesize + (x << 2);
  win    = offset >> _g_winshift;

  GSETRWIN(win);

  return *(int *) (_g_rmem + (offset & _g_winmask));
}

int ALT256_get(int x, int y)
{
  _outp8(GRA_I, 0x04);
  _outp8(GRA_D, x & 3);

  return(_g_rmem[ y * _g_linesize + (x >> 2)]);
}

int g_get(int x, int y)
{
  int color;

  if ((x < _g_clipx0) || (x > _g_clipx1) || (y < _g_clipy0) || (y > _g_clipy1))
    return(-1);
  GLOCK;
  color = _g_get(x, y);
  GUNLOCKNM;
  return(color);
}
