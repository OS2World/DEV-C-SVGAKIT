/* gbox.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */

#include <vesa.h>

#include "jmgraph.h"
#include "graph2.h"

void g_box (int x0, int y0, int x1, int y1, int color, int fill_flag)
{
  int temp, dx, dy;

  if (y0 > y1)
    {
      temp = y0; y0 = y1; y1 = temp;
    }
  if (x0 > x1)
    {
      temp = x0; x0 = x1; x1 = temp;
    }
  if (x0 > _g_clipx1 || x1 < _g_clipx0 || y0 > _g_clipy1 || y1 < _g_clipy0)
    return;
  if (fill_flag)
    {
      if (y0 < _g_clipy0)
        y0 = _g_clipy0;
      if (y1 > _g_clipy1)
        y1 = _g_clipy1;
      if (x0 < _g_clipx0)
        x0 = _g_clipx0;
      if (x1 > _g_clipx1)
        x1 = _g_clipx1;
      GLOCK;
      if (x1-x0 >= 3 || y1-y0 <= 4)
        {
          for (; y0 <= y1; ++y0)
            g_hline (y0, x0, x1, color);
        }
      else
        {
          for (; x0 <= x1; ++x0)
            g_vline (x0, y0, y1, color);
        }
      GUNLOCK;
    }
  else
    {
      dx = x1-x0; dy = y1-y0;
      if (dx == 0)
        g_vline (x0, y0, y1, color);
      else if (dy == 0)
        g_hline (y0, x0, x1, color);
      else
        {
          if (dx != 1 || dy == 1)
            {
              g_hline (y0, x0, x1, color);
              g_hline (y1, x0, x1, color);
              ++y0; --y1;
            }
          if (dy != 1)
            {
              g_vline (x0, y0, y1, color);
              g_vline (x1, y0, y1, color);
            }
        }
    }
}
