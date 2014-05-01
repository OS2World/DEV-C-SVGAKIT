/* gset.c (emx/gcc) --- Copyright (c) 1993 by Johannes Martin */

#include <sys/hw.h>
#include <string.h>
#include <vesa.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

static int clip(int *x0, int *y0, int *x1, int *y1, int *skip, int *visible)
{
  int i;

  if (*x0 > *x1)
    {
      i   = *x0;
      *x0 = *x1;
      *x1 = i;
    }
  if (*y0 > *y1)
    {
      i   = *y0;
      *y0 = *y1;
      *y1 = i;
    }
  if ((*x1 < _g_clipx0) || (*x0 > _g_clipx1) ||
      (*y1 < _g_clipy0) || (*y0 > _g_clipy1))
    return(0);
  if (*x1 > _g_clipx1)
    *visible = _g_clipx1 - *x0 + 1;
  else
    *visible = *x1 - *x0 + 1;
  if (*y1 > _g_clipy1)
    *y1 = _g_clipy1;
  *x1 -= *x0 - 1;
  *y1 -= *y0 - 1;
  if (*x0 < _g_clipx0)
    {
      *skip = _g_clipx0 - *x0;
      *x0   = _g_clipx0;
    }
  else
    *skip = 0;
  if (*y0 < _g_clipy0)
    {
      *skip += (_g_clipy0 - *y0) * *x1;
      *y0    = _g_clipy0;
    }
  return(1);
}

void FLAT16_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int i, old_linesize, planesize;
  void *old_rmem;

  planesize    = g_memsize / 4;
  old_rmem     = _g_rmem;
  old_linesize = _g_linesize;
  _g_linesize /= 4;
  x           /= 8;
  width       /= 8;
  skip        /= 8;
  visible     /= 8;
  for (i = 0; i < 4; i++)
    {
      FLAT256_getimage(x, y, width, height, skip, visible, fullheight, graph_buf);
      graph_buf += width * fullheight;
      _g_rmem   += planesize;
    }
  _g_rmem      = old_rmem;
  _g_linesize  = old_linesize;
}

void FLAT256_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int offset;

  graph_buf += skip;
  offset = y * _g_linesize + x;
  while (height-- > 0)
    {
      memcpy(graph_buf, _g_rmem + offset, visible);
      offset    += _g_linesize;
      graph_buf += width;
    }
}

void FLAT32k_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  FLAT256_getimage(x * 2, y, width * 2, height, skip * 2, visible * 2, fullheight, graph_buf);
}

void FLAT16m_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  FLAT256_getimage(x * 3, y, width * 3, height, skip * 3, visible * 3, fullheight, graph_buf);
}

void FLAT16m_32Bit_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  FLAT256_getimage(x * 4, y, width * 4, height, skip * 4, visible * 4, fullheight, graph_buf);
}

void SEG16_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int i;

  x       /= 8;
  width   /= 8;
  skip    /= 8;
  visible /= 8;
  for (i = 0; i < 4; i++)
    {
      _outp16(GRA_I, (i << 8) | 4);
      SEG256_getimage(x, y, width, height, skip, visible, fullheight, graph_buf);
      graph_buf += width * fullheight;
    }
}

void SEG256_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int offset, page, rest;

  graph_buf += skip;
  offset = y * _g_linesize + x;
  page   = offset / _g_wingran;
  offset %= _g_wingran;
  GSETRWIN(page);
  while (height-- > 0)
    {
      if ((rest = _g_winsize - offset) < visible)
        {
          memcpy(graph_buf, _g_rmem + offset, rest);
          page += _g_winfact;
          GSETRWIN(page);
          memcpy(graph_buf + rest, _g_rmem, visible - rest);
          offset += _g_linesize - _g_winsize;
        }
      else
	{
          memcpy(graph_buf, _g_wmem + offset, visible);
          if ((offset += _g_linesize) > _g_winsize)
            {
              page += _g_winfact;
              GSETRWIN(page);
              offset -= _g_winsize;
            }
        }
      graph_buf += width;
    }
}

void SEG32k_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  SEG256_getimage(x * 2, y, width * 2, height, skip * 2, visible * 2, fullheight, graph_buf);
}

void SEG16m_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  SEG256_getimage(x * 3, y, width * 3, height, skip * 3, visible * 3, fullheight, graph_buf);
}

void SEG16m_32Bit_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  SEG256_getimage(x * 4, y, width * 4, height, skip * 4, visible * 4, fullheight, graph_buf);
}

void ALT256_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int first, last, plane, i, j;
  char *buf1, *buf2;
  char *mem0, *mem1, *mem2;

  graph_buf += skip;
  mem0 = _g_rmem + y * _g_linesize;
  y = x + width - 1;
  _outp8(GRA_I, 0x04);

  for(plane = 0; plane < 4; plane++)
    {
      _outp8(GRA_D, plane);

      first = (x >> 2) + ((x & 3) > plane);
      last  = (y >> 2) - ((y & 3) < plane);

      buf1 = graph_buf + (plane + 4 - (x & 3)) % 4;
      mem1 = mem0 + first;

      j = height;

      while (j-- > 0)
        {
          buf2 = buf1;
          mem2 = mem1;
          i    = first;
          while (i++ <= last)
            {
              *buf2 = *mem2++;
              buf2 += 4;
            }
          buf1 += width;
          mem1 += _g_linesize;
        }
    }
}

void g_getimage(int x0, int y0, int x1, int y1, void *graph_buf)
{
  int visible, skip, fullheight;

  fullheight = y1 - y0 + 1;
  if (clip(&x0, &y0, &x1, &y1, &skip, &visible))
    {
      GLOCK;
      _g_getimage(x0, y0, x1, y1, skip, visible, fullheight, graph_buf);
      GUNLOCKNM;
    }
}

void FLAT16_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int i, old_linesize, planesize;
  void *old_wmem;

  planesize    = g_memsize / 4;
  old_wmem     = _g_wmem;
  old_linesize = _g_linesize;
  _g_linesize /= 4;
  x           /= 8;
  width       /= 8;
  skip        /= 8;
  visible     /= 8;
  for (i = 0; i < 4; i++)
    {
      FLAT256_putimage(x, y, width, height, skip, visible, fullheight, graph_buf);
      graph_buf += width * fullheight;
      _g_wmem   += planesize;
    }
  _g_wmem      = old_wmem;
  _g_linesize  = old_linesize;
}

void FLAT256_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int offset;

  graph_buf += skip;
  offset = y * _g_linesize + x;
  while (height-- > 0)
    {
      memcpy(_g_wmem + offset, graph_buf, visible);
      offset    += _g_linesize;
      graph_buf += width;
    }
}

void FLAT32k_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  FLAT256_putimage(x * 2, y, width * 2, height, skip * 2, visible * 2, fullheight, graph_buf);
}

void FLAT16m_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  FLAT256_putimage(x * 3, y, width * 3, height, skip * 3, visible * 3, fullheight, graph_buf);
}

void FLAT16m_32Bit_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  FLAT256_putimage(x * 4, y, width * 4, height, skip * 4, visible * 4, fullheight, graph_buf);
}

void SEG16_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int i;

  x       /= 8;
  width   /= 8;
  skip    /= 8;
  visible /= 8;
  _outp16(GRA_I, 0x0005);
  _outp16(GRA_I, 0x0001);
  _outp16(GRA_I, 0xFF08);
  for (i = 0; i < 4; i++)
    {
      _outp16(SEQ_I, (0x100 << i) | 2);
      SEG256_putimage(x, y, width, height, skip, visible, fullheight, graph_buf);
      graph_buf += width * fullheight;
    }
  _outp16(GRA_I, 0x0F01);
  _outp16(SEQ_I, 0x0F02);
  _outp16(GRA_I, 0x0205);
}

void SEG256_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int offset, page, rest;

  graph_buf += skip;
  offset = y * _g_linesize + x;
  page   = offset / _g_wingran;
  offset %= _g_wingran;
  GSETWWIN(page);
  while (height-- > 0)
    {
      if ((rest = _g_winsize - offset) < visible)
        {
          memcpy(_g_wmem + offset, graph_buf, rest);
          page += _g_winfact;
	  GSETWWIN(page);
          memcpy(_g_wmem, graph_buf + rest, visible - rest);
          offset += _g_linesize - _g_winsize;
        }
      else
        {
          memcpy(_g_wmem + offset, graph_buf, visible);
          if ((offset += _g_linesize) > _g_winsize)
            {
              page += _g_winfact;
              GSETWWIN(page);
              offset -= _g_winsize;
            }
        }
      graph_buf += width;
    }
}

void SEG32k_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  SEG256_putimage(x * 2, y, width * 2, height, skip * 2, visible * 2, fullheight, graph_buf);
}

void SEG16m_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  SEG256_putimage(x * 3, y, width * 3, height, skip * 3, visible * 3, fullheight, graph_buf);
}

void SEG16m_32Bit_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  SEG256_putimage(x * 4, y, width * 4, height, skip * 4, visible * 4, fullheight, graph_buf);
}

void ALT256_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *graph_buf)
{
  int first, last, plane, i, j;
  char *buf1, *buf2;
  char *mem0, *mem1, *mem2;

  graph_buf += skip;
  mem0 = _g_wmem + y * _g_linesize;
  y = x + width - 1;

  _outp8(SEQ_I, 0x02);

  for(plane = 0; plane < 4; plane++)
    {
      _outp8(SEQ_D, 1 << plane);

      first = (x >> 2) + ((x & 3) > plane);
      last  = (y >> 2) - ((y & 3) < plane);

      buf1 = graph_buf + (plane + 4 - (x & 3)) % 4;
      mem1 = mem0 + first;

      j = height;

      while (j-- > 0)
        {
          buf2 = buf1;
          mem2 = mem1;
          i    = first;
          while (i++ <= last)
            {
              *mem2++ = *buf2;
              buf2 += 4;
            }
          buf1 += width;
          mem1 += _g_linesize;
        }
    }
}

void g_putimage(int x0, int y0, int x1, int y1, void *graph_buf)
{
  int visible, skip, fullheight;

  fullheight = y1 - y0 + 1;
  if (clip(&x0, &y0, &x1, &y1, &skip, &visible))
    {
      GLOCK;
      _g_putimage(x0, y0, x1, y1, skip, visible, fullheight, graph_buf);
      GUNLOCK;
    }
}

int g_imagesize(int width, int height)
{
  if (g_colors == 16)
    width &= ~7;
  return((width * height * _g_rlinesize) / g_xsize);
}
