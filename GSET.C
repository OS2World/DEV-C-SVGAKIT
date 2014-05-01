/* gset.c (emx/gcc) --- Copyright (c) 1993 by Johannes Martin */

#include <sys/hw.h>
#include <vesa.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

void FLAT16_set(int x, int y, int color)
{
  int offset, mask, size, i;

  offset = (y * _g_linesize >> 2) + (x >> 3);
  mask   = 0x80 >> (x % 8);
  size   = g_memsize >> 2;

  switch (_g_wmode)
    {
      case G_NORM:
	for (i = 0; i < 4; i++)
	  {
	    if (color & 1)
	      _g_wmem[i * size + offset] |= mask;
	    else
	      _g_wmem[i * size + offset] &= ~mask;
	    color >>= 1;
	  }
	break;
      case G_AND:
	for (i = 0; i < 4; i++)
	  {
	    if ((color & 1) == 0)
	      _g_wmem[i * size + offset] &= ~mask;
            color >>= 1;
	  }
        break;
      case G_OR:
        for (i = 0; i < 4; i++)
	  {
            if (color & 1)
	      _g_wmem[i * size + offset] |= mask;
            color >>= 1;
          }
        break;
      case G_XOR:
	for (i = 0; i < 4; i++)
	  {
            if (color & 1)
	      _g_wmem[i * size + offset] ^= mask;
            color >>= 1;
          }
	break;
    }
}

void FLAT256_set(int x, int y, int color)
{
  switch (_g_wmode)
    {
      case G_NORM:
	_g_wmem[y * _g_linesize + x] = color;
	return;
      case G_AND:
	_g_wmem[y * _g_linesize + x] &= color;
	return;
      case G_OR:
	_g_wmem[y * _g_linesize + x] |= color;
	return;
      case G_XOR:
	_g_wmem[y * _g_linesize + x] ^= color;
	return;
    }
}

void FLAT32k_set(int x, int y, int color)
{
  switch (_g_wmode)
    {
      case G_NORM:
	*(short *) (_g_wmem + y * _g_linesize + (x >> 1)) = color;
	return;
      case G_AND:
	*(short *) (_g_wmem + y * _g_linesize + (x >> 1)) &= color;
	return;
      case G_OR:
	*(short *) (_g_wmem + y * _g_linesize + (x >> 1)) |= color;
	return;
      case G_XOR:
	*(short *) (_g_wmem + y * _g_linesize + (x >> 1)) ^= color;
	return;
    }
}

void FLAT16m_set(int x, int y, int color)
{
  int offset = y * _g_linesize + x * 3;

  switch (_g_wmode)
    {
      case G_NORM:
	_g_wmem[offset    ] = color;
	_g_wmem[offset + 1] = color >> 8;
	_g_wmem[offset + 2] = color >> 16;
	return;
      case G_AND:
	_g_wmem[offset    ] &= color;
	_g_wmem[offset + 1] &= color >> 8;
	_g_wmem[offset + 2] &= color >> 16;
	return;
      case G_OR:
	_g_wmem[offset    ] |= color;
	_g_wmem[offset + 1] |= color >> 8;
	_g_wmem[offset + 2] |= color >> 16;
	return;
      case G_XOR:
	_g_wmem[offset    ] ^= color;
	_g_wmem[offset + 1] ^= color >> 8;
	_g_wmem[offset + 2] ^= color >> 16;
	return;
    }
}

void FLAT16m_32Bit_set(int x, int y, int color)
{
  switch (_g_wmode)
    {
      case G_NORM:
	*(int*) (_g_wmem + y * _g_linesize + (x << 2)) = color;
	return;
      case G_AND:
	*(int*) (_g_wmem + y * _g_linesize + (x << 2)) &= color;
	return;
      case G_OR:
	*(int*) (_g_wmem + y * _g_linesize + (x << 2)) |= color;
	return;
      case G_XOR:
	*(int*) (_g_wmem + y * _g_linesize + (x << 2)) ^= color;
	return;
    }
}

void SEG16_set(int x, int y, int color)
{
  int offset, win;

  _outp16(GRA_I, (0x8000 >> (x & 7)) | 8);

  offset  = y * _g_linesize + (x >> 3);
  win     = offset >> _g_winshift;
  offset &= _g_winmask;

  GSETWWIN(win);
  GSETRWIN(win);
  _g_wmem[offset] = _g_rmem[offset];
  _g_wmem[offset] = color;
}

void SEG256_set_1(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + x;
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	_g_wmem[offset] = color;
	return;
      case G_AND:
	GSETRWIN(win);
	_g_wmem[offset] &= color;
	return;
      case G_OR:
	GSETRWIN(win);
	_g_wmem[offset] |= color;
	return;
      case G_XOR:
	GSETRWIN(win);
	_g_wmem[offset] ^= color;
	return;
    }
}

void SEG256_set_2(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + x;
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	_g_wmem[offset] = color;
	return;
      case G_AND:
	GSETRWIN(win);
	_g_wmem[offset] = color & _g_rmem[offset];
	return;
      case G_OR:
	GSETRWIN(win);
	_g_wmem[offset] = color | _g_rmem[offset];
	return;
      case G_XOR:
	GSETRWIN(win);
	_g_wmem[offset] = color ^ _g_rmem[offset];
	return;
    }
}

void SEG32k_set_1(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + (x << 1);
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	*(short *) (_g_wmem + offset) = color;
	return;
      case G_AND:
	GSETRWIN(win);
	*(short *) (_g_wmem + offset) &= color;
	return;
      case G_OR:
	GSETRWIN(win);
	*(short *) (_g_wmem + offset) |= color;
	return;
      case G_XOR:
	GSETRWIN(win);
	*(short *) (_g_wmem + offset) ^= color;
	return;
    }
}

void SEG32k_set_2(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + (x << 1);
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	*(short *) (_g_wmem + offset) = color;
	return;
      case G_AND:
	GSETRWIN(win);
	*(short *) (_g_wmem + offset) = color & *(short *) (_g_rmem + offset);
	return;
      case G_OR:
	GSETRWIN(win);
	*(short *) (_g_wmem + offset) = color | *(short *) (_g_rmem + offset);
	return;
      case G_XOR:
	GSETRWIN(win);
	*(short *) (_g_wmem + offset) = color ^ *(short *) (_g_rmem + offset);
	return;
    }
}

void SEG16m_set_1(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + x * 3;
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	_g_wmem[offset    ] = color;
	_g_wmem[offset + 1] = color >> 8;
	_g_wmem[offset + 2] = color >> 16;
	return;
      case G_AND:
	GSETRWIN(win);
	_g_wmem[offset    ] &= color;
	_g_wmem[offset + 1] &= color >> 8;
	_g_wmem[offset + 2] &= color >> 16;
	return;
      case G_OR:
	GSETRWIN(win);
	_g_wmem[offset    ] |= color;
	_g_wmem[offset + 1] |= color >> 8;
	_g_wmem[offset + 2] |= color >> 16;
	return;
      case G_XOR:
	GSETRWIN(win);
	_g_wmem[offset    ] ^= color;
	_g_wmem[offset + 1] ^= color >> 8;
	_g_wmem[offset + 2] ^= color >> 16;
	return;
    }
}

void SEG16m_set_2(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + x * 3;
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	_g_wmem[offset    ] = color;
	_g_wmem[offset + 1] = (color >> 8);
	_g_wmem[offset + 2] = (color >> 16);
	return;
      case G_AND:
	GSETRWIN(win);
	_g_wmem[offset    ] = _g_rmem[offset    ] & color;
	_g_wmem[offset + 1] = _g_rmem[offset + 1] & (color >> 8);
	_g_wmem[offset + 2] = _g_rmem[offset + 2] & (color >> 16);
	return;
      case G_OR:
	GSETRWIN(win);
	_g_wmem[offset    ] = _g_rmem[offset    ] | color;
	_g_wmem[offset + 1] = _g_rmem[offset + 1] | (color >> 8);
	_g_wmem[offset + 2] = _g_rmem[offset + 2] | (color >> 16);
	return;
      case G_XOR:
	GSETRWIN(win);
	_g_wmem[offset    ] = _g_rmem[offset    ] ^ color;
	_g_wmem[offset + 1] = _g_rmem[offset + 1] ^ (color >> 8);
	_g_wmem[offset + 2] = _g_rmem[offset + 2] ^ (color >> 16);
	return;
    }
}

void SEG16m_split_set_1(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + x * 3;
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  if ((SplitPositions[win].is_critical) &&
      (x == SplitPositions[win].x) && (y == SplitPositions[win].y)) {

    switch (_g_wmode)
      {
	case G_NORM:
	  _g_wmem[offset] = color;
	  if (SplitPositions[win++].is_critical == 1)
	    {
	      _g_wmem[offset + 1] = color >> 8;
	      GSETWWIN(win);
	      _g_wmem[0] = color >> 16;
	    }
	  else
	    {
	      GSETWWIN(win);
	      _g_wmem[0] = color >> 8;
	      _g_wmem[1] = color >> 16;
	    }
	  return;
      case G_AND:
	GSETRWIN(win);
	_g_wmem[offset] &= color;
	if (SplitPositions[win++].is_critical == 1)
	  {
	    _g_wmem[offset + 1] &= color >> 8 ;
	    GSETRWWIN(win);
	    _g_wmem[0] &= color >> 16;
	  }
	else
	  {
	    GSETRWWIN(win);
	    _g_wmem[0] &= color >> 8 ;
	    _g_wmem[1] &= color >> 16;
	  }
	return;
      case G_OR:
	GSETRWIN(win);
	_g_wmem[offset] |= color;
	if (SplitPositions[win++].is_critical == 1)
	  {
	    _g_wmem[offset + 1] |= color >> 8 ;
	    GSETRWWIN(win);
	    _g_wmem[0] |= color >> 16;
	  }
	else
	  {
	    GSETRWWIN(win);
	    _g_wmem[0] |= color >> 8 ;
	    _g_wmem[1] |= color >> 16;
	  }
	return;
      case G_XOR:
	GSETRWIN(win);
	_g_wmem[offset] ^= color;
	if (SplitPositions[win++].is_critical == 1)
	  {
	    _g_wmem[offset + 1] ^= color >> 8 ;
	    GSETRWWIN(win);
	    _g_wmem[0] ^= color >> 16;
	  }
	else
	  {
	    GSETRWWIN(win);
	    _g_wmem[0] ^= color >> 8 ;
	    _g_wmem[1] ^= color >> 16;
	  }
	return;
    }
   }
  else
    {
    switch (_g_wmode)
      {
	case G_NORM:
	  _g_wmem[offset    ]  = color;
	  _g_wmem[offset + 1] = color >> 8;
	  _g_wmem[offset + 2]   = color >> 16;
	  return;
	case G_AND:
	  GSETRWIN(win);
	  _g_wmem[offset    ] &= color;
	  _g_wmem[offset + 1] &= color >> 8;
	  _g_wmem[offset + 2] &= color >> 16;
	  return;
	case G_OR:
	  GSETRWIN(win);
	  _g_wmem[offset    ] |= color;
	  _g_wmem[offset + 1] |= color >> 8;
	  _g_wmem[offset + 2] |= color >> 16;
	  return;
	case G_XOR:
	  GSETRWIN(win);
	  _g_wmem[offset    ] ^= color;
	  _g_wmem[offset + 1] ^= color >> 8;
	  _g_wmem[offset + 2] ^= color >> 16;
	  return;
      }
   }
}

void SEG16m_split_set_2(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + x * 3;
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  if ((SplitPositions[win].is_critical) &&
      (x == SplitPositions[win].x) && (y == SplitPositions[win].y)) {

    switch (_g_wmode)
      {
	case G_NORM:
	  _g_wmem[offset] = color;
	  if (SplitPositions[win++].is_critical == 1)
	    {
	      _g_wmem[offset + 1] = color >> 8 ;
	      GSETWWIN(win);
	      _g_wmem[0] = color >> 16;
	    }
	  else
	    {
	      GSETWWIN(win);
	      _g_wmem[0] = color >> 8 ;
	      _g_wmem[1] = color >> 16;
	    }
	  return;
      case G_AND:
	GSETRWIN(win);
	_g_wmem[offset] = _g_rmem[offset] & color;
	if (SplitPositions[win++].is_critical == 1)
	  {
	    _g_wmem[offset + 1]  = _g_rmem[offset + 1] & (color >> 8 );
	    GSETRWWIN(win);
	    _g_wmem[0] = _g_rmem[0] & (color >> 16);
	  }
	else
	  {
	    GSETRWWIN(win);
	    _g_wmem[0] = _g_rmem[0] & (color >> 8 );
	    _g_wmem[1] = _g_rmem[1] & (color >> 16);
	  }
	return;
      case G_OR:
	GSETRWIN(win);
	_g_wmem[offset] = _g_rmem[offset] | color;
	if (SplitPositions[win++].is_critical == 1)
	  {
	    _g_wmem[offset + 1] = _g_rmem[offset + 1] | (color >> 8 );
	    GSETRWWIN(win);
	    _g_wmem[0] = _g_rmem[0] | (color >> 16);
	  }
	else
	  {
	    GSETRWWIN(win);
	    _g_wmem[0] = _g_rmem[0] | (color >> 8 );
	    _g_wmem[1] = _g_rmem[1] | (color >> 16);
	  }
	return;
      case G_XOR:
	GSETRWIN(win);
	_g_wmem[offset] = _g_rmem[offset] ^ color;
	if (SplitPositions[win++].is_critical == 1)
	  {
	    _g_wmem[offset + 1] = _g_rmem[offset + 1] ^ (color >> 8 );
	    GSETRWWIN(win);
	    _g_wmem[0] = _g_rmem[0] ^ (color >> 16);
	  }
	else
	  {
	    GSETRWWIN(win);
	    _g_wmem[0] = _g_rmem[0] ^ (color >> 8 );
	    _g_wmem[1] = _g_rmem[1] ^ (color >> 16);
	  }
	return;
    }
   }
  else
    {
    switch (_g_wmode)
      {
	case G_NORM:
	  _g_wmem[offset    ] = color;
	  _g_wmem[offset + 1] = (color >> 8);
	  _g_wmem[offset + 2] = (color >> 16);
	  return;
	case G_AND:
	  GSETRWIN(win);
	  _g_wmem[offset    ] = _g_rmem[offset    ] & color;
	  _g_wmem[offset + 1] = _g_rmem[offset + 1] & (color >> 8);
	  _g_wmem[offset + 2] = _g_rmem[offset + 2] & (color >> 16);
	  return;
	case G_OR:
	  GSETRWIN(win);
	  _g_wmem[offset    ] = _g_rmem[offset    ] | color;
	  _g_wmem[offset + 1] = _g_rmem[offset + 1] | (color >> 8);
	  _g_wmem[offset + 2] = _g_rmem[offset + 2] | (color >> 16);
	  return;
	case G_XOR:
	  GSETRWIN(win);
	  _g_wmem[offset    ] = _g_rmem[offset    ] ^ color;
	  _g_wmem[offset + 1] = _g_rmem[offset + 1] ^ (color >> 8);
	  _g_wmem[offset + 2] = _g_rmem[offset + 2] ^ (color >> 16);
	  return;
      }
   }
}

void SEG16m_32Bit_set_1(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + (x << 2);
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	*(int*) (_g_wmem + offset) = color;
	return;
      case G_AND:
	GSETRWIN(win);
	*(int*) (_g_wmem + offset) &= color;
	return;
      case G_OR:
	GSETRWIN(win);
	*(int*) (_g_wmem + offset) |= color;
	return;
      case G_XOR:
	GSETRWIN(win);
	*(int*) (_g_wmem + offset) ^= color;
	return;
    }
}

void SEG16m_32Bit_set_2(int x, int y, int color)
{
  int offset, win;

  offset = y * _g_linesize + (x << 2);
  win    = offset >> _g_winshift;

  GSETWWIN(win);

  offset &= _g_winmask;

  switch (_g_wmode)
    {
      case G_NORM:
	*(int*) (_g_wmem + offset) = color;
	return;
      case G_AND:
	GSETRWIN(win);
	*(int*) (_g_wmem + offset) = color &
	*(int*) (_g_rmem + offset);
	return;
      case G_OR:
	GSETRWIN(win);
	*(int*) (_g_wmem + offset) = color |
	*(int*) (_g_rmem + offset);
	return;
      case G_XOR:
	GSETRWIN(win);
	*(int*) (_g_wmem + offset) = color ^
	*(int*) (_g_rmem + offset);
	return;
    }
}

void ALT256_set(int x, int y, int color)
{
  _outp8(SEQ_I, 0x02);
  _outp8(SEQ_D, 1 << (x & 3));
  _outp8(GRA_I, 0x04);
  _outp8(GRA_D, x & 3);

  switch (_g_wmode)
    {
      case G_NORM:
	_g_wmem[y * _g_linesize + (x >> 2)] = color;
	break;
      case G_AND:
	_g_wmem[y * _g_linesize + (x >> 2)] &= color;
	break;
      case G_OR:
	_g_wmem[y * _g_linesize + (x >> 2)] |= color;
	break;
      case G_XOR:
	_g_wmem[y * _g_linesize + (x >> 2)] ^= color;
	break;
    }
}

void g_set(int x, int y, int color)
{
  if (x >= _g_clipx0 && x <= _g_clipx1 && y >= _g_clipy0 && y <= _g_clipy1)
    {
      GLOCK;
      _g_set(x, y, color);
      GUNLOCK;
    }
}
