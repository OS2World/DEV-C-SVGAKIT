/* gclear.c (emx/gcc) --- Copyright (c) 1993 by Johannes Martin */
#include <string.h>
#include <stdio.h>

#include <sys/hw.h>
#include <vesa.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"


void FLAT16_clear(int color)
{
  int h, i;

  h = g_memsize / 4;

  for (i = 0; i < 4; i++)
    memset(_g_wmem + i * h, color & (1 << i) ? 0xFF : 0, h);
}

void FLAT256_clear(int color)
{
  memset(_g_linesize > 0 ? _g_wmem : _g_wmem + (g_ysize - 1) * _g_linesize,
	 color, g_memsize);
}

void FLAT32k_clear(int color)
{
  memsetw(_g_wmem, color, g_memsize >> 1);
}

void FLAT16m_clear(int color)
{
  int j;
  char *base;

  for (base = _g_wmem, j = g_ysize; j > 0; j--, base += _g_linesize)
    memset3(base, color, g_xsize);
}

void FLAT16m_32Bit_clear(int color)
{
  memsetl(_g_wmem, color, g_memsize >> 2);
}

void SEG16_clear(int color)
{
  int i = _g_wincount * _g_winfact;

  _outp16(GRA_I, 0x0005);
  _outp16(GRA_I, color << 8);
  _outp16(GRA_I, 0xFF08);
  do
    {
      _g_setwin(_g_wwin, i -= _g_winfact);
      _g_setwin(_g_rwin, i);
      memcpy(_g_wmem, _g_rmem, _g_winsize);
    }
  while (i > 0);
  _g_wwinstat  = 0;
  *_g_rwinstat = 0;
  _outp16(GRA_I, 0x0205);
}

void SEG256_clear(int color)
{
  int i = _g_wincount * _g_winfact;

  do
    {
      _g_setwin(_g_wwin, i -= _g_winfact);
      memset(_g_wmem, color, _g_winsize);
    }
  while (i > 0);
  _g_wwinstat = 0;
}

void SEG32k_clear(int color)
{
  int i = _g_wincount * _g_winfact;
  int k;

  if (color)
    {
      k = _g_winsize >> 1;
      do
	{
	  _g_setwin(_g_wwin, i -= _g_winfact);
	  memsetw(_g_wmem, color, k);
	}
      while (i > 0);
    }
  else
    {
      do
	{
	  _g_setwin(_g_wwin, i -= _g_winfact);
	  memset(_g_wmem, 0, _g_winsize);
	}
      while (i > 0);
    }
  _g_wwinstat = 0;
}

void SEG16m_clear(int color)
{
  int i, j, k, ws, rest;
  int *base;
  int ColBuf[3], c1, c2, c3;

  i = _g_wincount * _g_winfact;

  if (color)
    {
      c1 = color & 0xFF;
      c2 = (color >> 8) & 0xFF;
      c3 = (color >> 16) & 0xFF;

      ColBuf[0] = c1 | (c2 << 8) | (c3 << 16) | (c1 << 24);
      ColBuf[1] = c2 | (c3 << 8) | (c1 << 16) | (c2 << 24);
      ColBuf[2] = c3 | (c1 << 8) | (c2 << 16) | (c3 << 24);

      ws   = _g_winsize >> 2;
      rest = 0;
      for (j = 0; j < i; j += _g_winfact)
	{
	  base = (int*)_g_wmem;
	  _g_setwin(_g_wwin, j);
	  for (k = rest; k < ws+rest; k++) *base++ = ColBuf[k % 3];
	  rest = k % 3;
	}
      _g_wwinstat = j;
    }
  else
    {
      int i = _g_wincount * _g_winfact;
      do
	{
	  _g_setwin(_g_wwin, i -= _g_winfact);
	  memset(_g_wmem, 0, _g_winsize);
	}
      while (i > 0);
     _g_wwinstat = 0;
   }
}

void SEG16m_32Bit_clear(int color)
{
  int i = _g_wincount * _g_winfact;
  int k;

  if (color)
    {
      k = _g_winsize >> 2;
      do
	{
	  _g_setwin(_g_wwin, i -= _g_winfact);
	  memsetl(_g_wmem, color, k);
	}
      while (i > 0);
    }
  else
    {
      do
	{
	  _g_setwin(_g_wwin, i -= _g_winfact);
	  memset(_g_wmem, 0, _g_winsize);
	}
      while (i > 0);
    }
  _g_wwinstat = 0;
}

void ALT256_clear(int color)
{
  _outp8(SEQ_I, 0x02);
  _outp8(SEQ_D, 0x0F);
  memset(_g_wmem, color, 65536);
}

void g_clear(int color)
{
  GLOCK;
  _g_clear(color);
  GUNLOCK;
}
