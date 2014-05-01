/* gsave.c (emx/gcc) --- Copyright (c) 1993 by Johannes Martin */

#include <sys/hw.h>
#include <vesa.h>

#include <string.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

void FLAT_save(void *graph_buf)
{
  if (_g_linesize > 0)
    memcpy(graph_buf, _g_rmem, g_memsize);
  else
    {
      char *mem, *end;

      mem = _g_rmem;
      end = _g_rmem + g_ysize * _g_linesize;
      while (mem > end)
        {
          memcpy(graph_buf, mem, _g_rlinesize);
          mem += _g_linesize;
          graph_buf += _g_rlinesize;
        }
    }
}

void SEG16_save(void *graph_buf)
{
  int old_memsize   = g_memsize;
  int old_rlinesize = _g_rlinesize;
  int i;

  g_memsize    /= 4;
  _g_rlinesize /= 4;
  for (i = 0; i < 4; i++)
    {
      _outp16(GRA_I, (i << 8) | 4);
      SEG_save(graph_buf + i * g_memsize);
    }
  g_memsize    = old_memsize;
  _g_rlinesize = old_rlinesize;
}

void SEG_save(void *graph_buf)
{
  int m = (_g_wincount - 1) * _g_winfact;

  if (_g_linesize == _g_rlinesize)
    {
      int i;

      for (i = 0; i < m; i += _g_winfact)
        {
          _g_setwin(_g_rwin, i);
          memcpy(graph_buf, _g_rmem, _g_winsize);
          graph_buf += _g_winsize;
        }
      _g_setwin(_g_rwin, *_g_rwinstat = i);
      memcpy(graph_buf, _g_rmem, (i = g_memsize % _g_winsize) ? i : _g_winsize);
    }
  else
    {
      int  i, j, k;
      void *base;

      j = _g_winsize / _g_linesize;

      for (i = 0; i < m; i += _g_winfact)
        {
          _g_setwin(_g_rwin, i);
          base = _g_rmem;
          for (k = 0; k < j; k++)
            {
              memcpy(graph_buf, base, _g_rlinesize);
              graph_buf += _g_rlinesize;
              base      += _g_linesize;
            }
        }
      _g_setwin(_g_rwin, *_g_rwinstat = i);
      base = _g_rmem;
      if ((k = g_ysize % j) != 0)
        j = k;
      for (k = 0; k < j; k++)
        {
          memcpy(graph_buf, base, _g_rlinesize);
          graph_buf += _g_rlinesize;
          base      += _g_linesize;
        }
    }
}

void ALT_save(void *graph_buf)
{
  int i, j;
  char *src, *dst;

  _outp8(GRA_I, 4);
  for (i = 0; i < 4; i++)
    {
      _outp8(GRA_D, i);
      src = _g_rmem;
      dst = graph_buf + i;
      j   = g_memsize / 4;
      while (j-- > 0)
        {
          *dst = *src;
          src++;
          dst += 4;
        }
    }
}

void g_save(void *graph_buf)
{
  GLOCK;
  _g_save(graph_buf);
  GUNLOCKNM;
}

void FLAT_restore(void *graph_buf)
{
  if (_g_linesize > 0)
    memcpy(_g_wmem, graph_buf, g_memsize);
  else
    {
      char *mem, *end;

      mem = _g_wmem;
      end = _g_wmem + g_ysize * _g_linesize;
      while (mem > end)
        {
          memcpy(mem, graph_buf, _g_rlinesize);
          mem += _g_linesize;
          graph_buf += _g_rlinesize;
        }
    }
}

void SEG16_restore(void *graph_buf)
{
  int old_memsize   = g_memsize;
  int old_rlinesize = _g_rlinesize;
  int i;

  g_memsize    /= 4;
  _g_rlinesize /= 4;
  _outp16(GRA_I, 0x0005);
  _outp16(GRA_I, 0x0001);
  _outp16(GRA_I, 0xFF08);
  for (i = 0; i < 4; i++)
    {
      _outp16(SEQ_I, (0x100 << i) | 2);
      SEG_restore(graph_buf + i * g_memsize);
    }
  _outp16(GRA_I, 0x0F01);
  _outp16(SEQ_I, 0x0F02);
  _outp16(GRA_I, 0x0205);
  g_memsize    = old_memsize;
  _g_rlinesize = old_rlinesize;
}

void SEG_restore(void *graph_buf)
{
  int m = (_g_wincount - 1) * _g_winfact;

  if (_g_linesize == _g_rlinesize)
    {
      int i;

      for (i = 0; i < m; i += _g_winfact)
        {
          _g_setwin(_g_wwin, i);
          memcpy(_g_wmem, graph_buf, _g_winsize);
          graph_buf += _g_winsize;
        }
      _g_setwin(_g_wwin, _g_wwinstat = i);
      memcpy(_g_wmem, graph_buf, (i = g_memsize % _g_winsize) ? i : _g_winsize);
    }
  else
    {
      int  i, j, k;
      void *base;

      j = _g_winsize / _g_linesize;

      for (i = 0; i < m; i += _g_winfact)
        {
          _g_setwin(_g_wwin, i);
          base = _g_wmem;
          for (k = 0; k < j; k++)
            {
              memcpy(base, graph_buf, _g_rlinesize);
              graph_buf += _g_rlinesize;
              base      += _g_linesize;
            }
        }
      _g_setwin(_g_wwin, _g_wwinstat = i);
      base = _g_wmem;
      if ((k = g_ysize % j) != 0)
        j = k;
      for (k = 0; k < j; k++)
        {
          memcpy(base, graph_buf, _g_rlinesize);
          graph_buf += _g_rlinesize;
          base      += _g_linesize;
        }
    }
}

void ALT_restore(void *graph_buf)
{
  int i, j;
  char *src, *dst;

  _outp8(SEQ_I, 2);
  for (i = 0; i < 4; i++)
    {
      _outp8(SEQ_D, 1 << i);
      src = graph_buf + i;
      dst = _g_wmem;
      j   = g_memsize / 4;
      while (j-- > 0)
        {
          *dst = *src;
          src += 4;
          dst++;
        }
    }
}

void g_restore(void *graph_buf)
{
  GLOCK;
  _g_restore(graph_buf);
  GUNLOCK;
}
