/* gclip.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */

#include "jmgraph.h"
#include "graph2.h"

void g_clip (int x0, int y0, int x1, int y1)
{
  if (x0 < 0)
    _g_clipx0 = 0;
  else
    _g_clipx0 = x0;
  if (y0 < 0)
    _g_clipy0 = 0;
  else
    _g_clipy0 = y0;
  if (x1 >= g_xsize)
    _g_clipx1 = g_xsize - 1;
  else
    _g_clipx1 = x1;
  if (y1 >= g_ysize)
    _g_clipy1 = g_ysize - 1;
  else
    _g_clipy1 = y1;
}
