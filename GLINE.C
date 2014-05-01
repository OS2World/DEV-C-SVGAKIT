/* gline.c (emx+gcc) --- Copyright (c) 1987-1993 by Eberhard Mattes */
/*                                  changes 1993 by Johannes Martin */

#include <vesa.h>

#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

static int round(int n)
{
  if (n & 1)
    return (n/2+1);
  else
    return (n/2);
}

static int clip(int x1, int y1, int *x2p, int *y2p)
{
  if (*y2p < _g_clipy0 && y1 >= _g_clipy0)
    {
      *x2p = x1 + round (2 * (_g_clipy0 - y1) * (*x2p - x1) / (*y2p - y1));
      *y2p = _g_clipy0;
    }
  else if (*y2p > _g_clipy1 && y1 <= _g_clipy1)
    {
      *x2p = x1 + round (2 * (_g_clipy1 - y1) * (*x2p - x1) / (*y2p - y1));
      *y2p = _g_clipy1;
    }
  if (*x2p < _g_clipx0 && x1 >= _g_clipx0)
    {
      *y2p = y1 + round (2 * (_g_clipx0 - x1) * (*y2p - y1) / (*x2p - x1));
      *x2p = _g_clipx0;
    }
  else if (*x2p > _g_clipx1 && x1 <= _g_clipx1)
    {
      *y2p = y1 + round (2* (_g_clipx1 - x1) * (*y2p - y1) / (*x2p - x1));
      *x2p = _g_clipx1;
    }
  return (*x2p >= _g_clipx0 && *x2p <= _g_clipx1
          && *y2p >= _g_clipy0 && *y2p <= _g_clipy1);
}

void g_line(int x1, int y1, int x2, int y2, int color)
{
  if (clip (x1, y1, &x2, &y2) && clip (x2, y2, &x1, &y1))
    {
      int dx = x2 - x1;
      int dy = y2 - y1;
      int ax = abs(dx) << 1;
      int ay = abs(dy) << 1;
      int sx = (dx >= 0) ? 1 : -1;
      int sy = (dy >= 0) ? 1 : -1;

      int x  = x1;
      int y  = y1;

      GLOCK;
      if (ax > ay)
        {
          int d = ay - (ax >> 1);
          while (x != x2)
            {
              _g_set(x, y, color);

              if (d > 0 || (d == 0 && sx == 1))
                {
                  y += sy;
                  d -= ax;
                }
              x += sx;
              d += ay;
            }
        }
      else
        {
          int d = ax - (ay >> 1);
          while (y != y2)
            {
              _g_set(x, y, color);

              if (d > 0 || (d == 0 && sy == 1))
                {
                  x += sx;
                  d -= ay;
                }
              y += sy;
              d += ax;
            }
        }
      _g_set(x, y, color);
      GUNLOCK;
    }
}

