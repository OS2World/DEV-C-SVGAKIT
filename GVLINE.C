/* gvline.c (emx+gcc) -- Copyright (c) 1993 by Johannes Martin */

#include <sys/hw.h>
#include <vesa.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

void FLAT16_vline(int x, int y0, int y1, int color)
{
  int mask, size, lsize, i;
  char *base1, *base2;

  size  = g_memsize >> 2;
  lsize = _g_linesize >> 2;
  mask  = 0x80 >> (x % 8);
  base1 = _g_wmem + y0 * lsize + (x >> 3);
  y1   -= y0;

  switch (_g_wmode)
    {
      case G_NORM:
	for (i = 0; i < 4; i++)
	  {
	    base2 = base1 + i * size;
	    y0 = y1;
	    if (color & 1)
	      while (y0-- >= 0)
		{
		  *base2 |= mask;
		  base2  += lsize;
		}
	    else
	      while (y0-- >= 0)
		{
		  *base2 &= ~mask;
		  base2  += lsize;
		}
	    color >>= 1;
	  }
	break;
      case G_AND:
	for (i = 0; i < 4; i++)
	  {
	    base2 = base1 + i * size;
	    y0 = y1;
	    if ((color & 1) == 0)
	      while (y0-- >= 0)
		{
		  *base2 &= ~mask;
		  base2  += lsize;
		}
	    color >>= 1;
	  }
	break;
      case G_OR:
        for (i = 0; i < 4; i++)
          {
            base2 = base1 + i * size;
            y0 = y1;
            if (color & 1)
              while (y0-- >= 0)
                {
                  *base2 |= mask;
                  base2  += lsize;
                }
            color >>= 1;
          }
        break;
      case G_XOR:
        for (i = 0; i < 4; i++)
          {
            base2 = base1 + i * size;
	    y0 = y1;
            if (color & 1)
              while (y0-- >= 0)
                {
                  *base2 ^= mask;
                  base2  += lsize;
                }
            color >>= 1;
          }
        break;
    }
}

void FLAT256_vline(int x, int y0, int y1, int color)
{
  char *base;

  base = _g_wmem + y0 * _g_linesize + x;
  y1  -= y0;
  switch (_g_wmode)
    {
      case G_NORM:
	while (y1-- >= 0)
	  {
	    *base = color;
	    base += _g_linesize;
	  }
        break;
      case G_AND:
        while (y1-- >= 0)
          {
            *base &= color;
            base  += _g_linesize;
          }
        break;
      case G_OR:
        while (y1-- >= 0)
          {
            *base |= color;
            base  += _g_linesize;
          }
        break;
      case G_XOR:
        while (y1-- >= 0)
	  {
            *base ^= color;
            base  += _g_linesize;
          }
        break;
    }
}

void FLAT32k_vline(int x, int y0, int y1, int color)
{
  char *base;

  base = _g_wmem + y0 * _g_linesize + (x << 1);
  y1  -= y0;

  switch (_g_wmode)
    {
      case G_NORM:
	while (y1-- >= 0)
	  {
	    *(unsigned short *) base = color;
	    base += _g_linesize;
	  }
	break;
      case G_AND:
	while (y1-- >= 0)
	  {
	    *(unsigned short *) base &= color;
	    base += _g_linesize;
	  }
	break;
      case G_OR:
	while (y1-- >= 0)
	  {
	    *(unsigned short *) base |= color;
	    base += _g_linesize;
	  }
	break;
      case G_XOR:
	while (y1-- >= 0)
	  {
	    *(unsigned short *) base ^= color;
	    base += _g_linesize;
	  }
	break;
    }
}

void FLAT16m_vline(int x, int y0, int y1, int color)
{
  char *base;
  char c1, c2, c3;

  c1 = color;
  c2 = color >> 8;
  c3 = color >> 16;

  base = _g_wmem + y0 * _g_linesize + x * 3;
  y1  -= y0;

  switch (_g_wmode)
    {
      case G_NORM:
	while (y1-- >= 0)
	  {
	    *base++ = c1;
	    *base++ = c2;
	    *base   = c3;
	    base   += _g_linesize - 2;
	  }
	break;
      case G_AND:
	while (y1-- >= 0)
	  {
	    *base++ &= c1;
	    *base++ &= c2;
	    *base   &= c3;
	    base    += _g_linesize - 2;
	  }
	break;
      case G_OR:
	while (y1-- >= 0)
	  {
	    *base++ |= c1;
	    *base++ |= c2;
	    *base   |= c3;
	    base    += _g_linesize - 2;
	  }
	break;
      case G_XOR:
	while (y1-- >= 0)
	  {
	    *base++ ^= c1;
	    *base++ ^= c2;
	    *base   ^= c3;
	    base    += _g_linesize - 2;
	  }
	break;
    }
}

void FLAT16m_32Bit_vline(int x, int y0, int y1, int color)
{
  char *base;

  base = _g_wmem + y0 * _g_linesize + (x << 2);
  y1  -= y0;

  switch (_g_wmode)
    {
      case G_NORM:
	while (y1-- >= 0)
	  {
	    *(int*) base = color;
	    base += _g_linesize;
	  }
	break;
      case G_AND:
	while (y1-- >= 0)
	  {
	    *(int *) base &= color;
	    base += _g_linesize;
	  }
	break;
      case G_OR:
	while (y1-- >= 0)
	  {
	    *(int *) base |= color;
	    base += _g_linesize;
	  }
	break;
      case G_XOR:
	while (y1-- >= 0)
	  {
	    *(int *) base ^= color;
	    base += _g_linesize;
	  }
	break;
    }
}

void SEG16_vline(int x, int y0, int y1, int color)
{
  int  offset, win;
  volatile char *base1, *base2;

  offset  = y0 * _g_linesize + (x >> 3);
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  base1   = _g_wmem + offset;
  base2   = _g_rmem + offset;

  _outp16(GRA_I, color << 8);
  _outp16(GRA_I, (0x8000 >> (x & 7)) | 8);

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if (win != *_g_rwinstat)
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      *base1 = *base2;
      *base1 = color;
      base1 += _g_linesize;
      base2 += _g_linesize;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  base1   = _g_wmem + offset;
	  base2   = _g_rmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat  = win;
  *_g_rwinstat = win;
}

void SEG256_vline_1(int x, int y0, int y1, int color)
{
  int  offset, win;
  char *base;

  offset  = y0 * _g_linesize + x;
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  base    = _g_wmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      switch (_g_wmode)
	{
	  case G_NORM:
	    *base = color;
	    break;
	  case G_AND:
	    *base &= color;
	    break;
	  case G_OR:
	    *base |= color;
	    break;
	  case G_XOR:
	    *base ^= color;
	    break;
	}
      base += _g_linesize;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  base    = _g_wmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void SEG256_vline_2(int x, int y0, int y1, int color)
{
  int  offset, win;
  char *basew, *baser;

  offset  = y0 * _g_linesize + x;
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  basew   = _g_wmem + offset;
  baser   = _g_rmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      switch (_g_wmode)
	{
	  case G_NORM:
	    *basew = color;
	    break;
	  case G_AND:
	    *basew = *baser & color;
	    break;
	  case G_OR:
	    *basew = *baser | color;
	    break;
	  case G_XOR:
	    *basew = *baser ^ color;
	    break;
	}
      basew += _g_linesize;
      baser += _g_linesize;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  basew   = _g_wmem + offset;
	  baser   = _g_rmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void SEG32k_vline_1(int x, int y0, int y1, int color)
{
  int  offset, win;
  char *base;

  offset  = y0 * _g_linesize + (x << 1);
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  base    = _g_wmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      switch (_g_wmode)
	{
	  case G_NORM:
	    *(unsigned short *) base = color;
	    break;
	  case G_AND:
	    *(unsigned short *) base &= color;
	    break;
	  case G_OR:
	    *(unsigned short *) base |= color;
	    break;
	  case G_XOR:
	    *(unsigned short *) base ^= color;
	    break;
	}
      base += _g_linesize;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  base    = _g_wmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void SEG32k_vline_2(int x, int y0, int y1, int color)
{
  int  offset, win;
  char *baser, *basew;

  offset  = y0 * _g_linesize + (x << 1);
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  basew   = _g_wmem + offset;
  baser   = _g_rmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      switch (_g_wmode)
	{
	  case G_NORM:
	    *(unsigned short *) basew = color;
	    break;
	  case G_AND:
	    *(unsigned short *) basew = *(unsigned short *) baser & color;
	    break;
	  case G_OR:
	    *(unsigned short *) basew = *(unsigned short *) baser | color;
	    break;
	  case G_XOR:
	    *(unsigned short *) basew = *(unsigned short *) baser ^ color;
	    break;
	}
      basew += _g_linesize;
      baser += _g_linesize;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  basew   = _g_wmem + offset;
	  baser   = _g_rmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void SEG16m_vline_1(int x, int y0, int y1, int color)
{
  int  offset, win, ypos = y0;
  char *base, c1, c2, c3;

  c1 = color;
  c2 = color >> 8;
  c3 = color >> 16;

  offset  = y0 * _g_linesize + x * 3;
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  base    = _g_wmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      if (offset + 3 > _g_winsize) SEG16m_split_set_1(x, ypos, color);
      else

      switch (_g_wmode)
	{
	  case G_NORM:
	    *base++ = c1;
	    *base++ = c2;
	    *base   = c3;
	    break;
	  case G_AND:
	    *base++ &= c1;
	    *base++ &= c2;
	    *base   &= c3;
	    break;
	  case G_OR:
	    *base++ |= c1;
	    *base++ |= c2;
	    *base   |= c3;
	    break;
	  case G_XOR:
	    *base++ ^= c1;
	    *base++ ^= c2;
	    *base   ^= c3;
	    break;
	}
      base   += _g_linesize - 2;
      ypos++;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  base    = _g_wmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void SEG16m_vline_2(int x, int y0, int y1, int color)
{
  int  offset, win, ypos = y0;
  char *basew, *baser, c1, c2, c3;

  c1 = color;
  c2 = color >> 8;
  c3 = color >> 16;

  offset  = y0 * _g_linesize + x * 3;
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  basew   = _g_wmem + offset;
  baser   = _g_rmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      if (offset + 3 > _g_winsize) SEG16m_split_set_2(x, ypos, color);
      else

      switch (_g_wmode)
	{
	  case G_NORM:
	    *basew++ = c1;
	    *basew++ = c2;
	    *basew   = c3;
	    break;
	  case G_AND:
	    *basew++ = *baser++ & c1;
	    *basew++ = *baser++ & c2;
	    *basew   = *baser   & c3;
	    break;
	  case G_OR:
	    *basew++ = *baser++ | c1;
	    *basew++ = *baser++ | c2;
	    *basew   = *baser   | c3;
	    break;
	  case G_XOR:
	    *basew++ = *baser++ ^ c1;
	    *basew++ = *baser++ ^ c2;
	    *basew   = *baser   ^ c3;
	    break;
	}
      basew   += _g_linesize - 2;
      baser   += _g_linesize - 2;
      ypos++;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  basew   = _g_wmem + offset;
	  baser   = _g_rmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void SEG16m_32Bit_vline_1(int x, int y0, int y1, int color)
{
  int  offset, win;
  char *base;

  offset  = y0 * _g_linesize + (x << 2);
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  base    = _g_wmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      switch (_g_wmode)
	{
	  case G_NORM:
	    *(int *) base = color;
	    break;
	  case G_AND:
	    *(int *) base &= color;
	    break;
	  case G_OR:
	    *(int *) base |= color;
	    break;
	  case G_XOR:
	    *(int *) base ^= color;
	    break;
	}
      base += _g_linesize;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  base    = _g_wmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void SEG16m_32Bit_vline_2(int x, int y0, int y1, int color)
{
  int  offset, win;
  char *basew, *baser;

  offset  = y0 * _g_linesize + (x << 2);
  win     = offset >> _g_winshift;
  offset &= _g_winmask;
  y1     -= y0;
  basew   = _g_wmem + offset;
  baser   = _g_rmem + offset;

  if (win != _g_wwinstat)
    _g_setwin(_g_wwin, win);
  if ((_g_wmode != G_NORM) && (win != *_g_rwinstat))
    _g_setwin(_g_rwin, win);

  while (y1-- >= 0)
    {
      switch (_g_wmode)
	{
	  case G_NORM:
	    *(int *) basew = color;
	    break;
	  case G_AND:
	    *(int *) basew = *(int *) baser & color;
	    break;
	  case G_OR:
	    *(int *) basew = *(int *) baser | color;
	    break;
	  case G_XOR:
	    *(int *) basew = *(int *) baser ^ color;
	    break;
	}
      basew += _g_linesize;
      baser += _g_linesize;
      if ((offset += _g_linesize) >= _g_winsize)
	{
	  offset &= _g_winmask;
	  basew   = _g_wmem + offset;
	  baser   = _g_rmem + offset;
	  win    += _g_winfact;
	  _g_setwin(_g_wwin, win);
	  if (_g_wmode != G_NORM)
	    _g_setwin(_g_rwin, win);
	}
    }
  _g_wwinstat = win;
  if (_g_wmode != G_NORM)
    *_g_rwinstat = win;
}

void ALT256_vline(int x, int y0, int y1, int color)
{
  char *base;

  _outp8(SEQ_I, 0x02);
  _outp8(SEQ_D, 1 << (x & 3));
  _outp8(GRA_I, 0x04);
  _outp8(GRA_D, x & 3);

  base = _g_wmem + y0 * _g_linesize + (x >> 2);

  y1 -= y0;

  switch (_g_wmode)
    {
      case G_NORM:
	while (y1-- >= 0)
	  {
	    *base = color;
	    base += _g_linesize;
	  }
	break;
      case G_AND:
	while (y1-- >= 0)
	  {
	    *base &= color;
	    base  += _g_linesize;
	  }
	break;
      case G_OR:
	while (y1-- >= 0)
	  {
	    *base |= color;
	    base  += _g_linesize;
	  }
	break;
      case G_XOR:
	while (y1-- >= 0)
	  {
	    *base ^= color;
	    base  += _g_linesize;
	  }
	break;
    }
}

void g_vline(int x, int y0, int y1, int color)
{
  int  y;

  if (y0 > y1)
    {
      y  = y0;
      y0 = y1;
      y1 = y;
    }
  if ((x < _g_clipx0) || (x > _g_clipx1) || (y0 > _g_clipy1) || (y1 < _g_clipy0))
    return;
  if (y0 < _g_clipy0)
    y0 = _g_clipy0;
  if (y1 > _g_clipy1)
    y1 = _g_clipy1;
  GLOCK;
  _g_vline(x, y0, y1, color);
  GUNLOCK;
}
