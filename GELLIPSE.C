/* gellipse.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */

#include <vesa.h>

#include "jmgraph.h"
#include "graph2.h"

static void gcirfx (int cx, int cy, int rx, int ry, int color)
{
  int x, y, r, x1, y1, oy, ox, y0, qr, qx, qy, z;

  r = rx + 1;
  x = r - 1; y = 0; qy = 0; qx = x * x; qr = r * r;
  ox = -1; oy = -1; y0 = -1;
  for (;;)
    {
      z = y * ry; y1 = z / rx;
      z = x * ry; x1 = z / rx;
      if (y1 != oy)
        {
          g_hline (cy-y1, cx-x, cx+x, color);
          if (y1 != 0)    /* draw diameter only once */
            g_hline (cy+y1, cx-x, cx+x, color);
        }
      if (x1 != ox)
        {
          if (y0 != -1 && y1 < ox)
	    {
              g_hline (cy-ox, cx-y0, cx+y0, color);
              g_hline (cy+ox, cx-y0, cx+y0, color);
            }
        }
      oy = y1; ox = x1; y0 = y;
      if (x <= y)
        break;
      qy = qy + 2 * y + 1;
      ++y;
      if (qx+qy > qr)
        {
          qx = qx - 2 * x + 1;
          --x;
        }
    }
}


static void gcirpx (int cx, int cy, int rx, int ry, int color)
{
  int x, y, r, x1, y1, oy, ox, y0, x2, y2, qr, qx, qy, z;

  r = rx + 1;
  x = r - 1; y = 0; qy = 0; qx = x * x; qr = r * r;
  ox = -1; oy = -1; y0 = -1; x2 = rx; y2 = 0;
  for (;;)
    {
      z = y * ry; y1 = z / rx;
      z = x * ry; x1 = z / rx;
      if (y1 != oy)
        {
          if (x > x2)
            {
              g_set (cx-x, cy-y1, color);
              g_set (cx+x, cy-y1, color);
              if (y1 != 0)  /* set these points only once */
                {
                  g_set (cx-x, cy+y1, color);
		  g_set (cx+x, cy+y1, color);
		}
            }
          else
            {
              g_hline (cy-y1, cx-x, cx-x2, color);
              g_hline (cy-y1, cx+x, cx+x2, color);
              if (y1 != 0)   /* set these 'points' only once */
                {
                  g_hline (cy+y1, cx-x, cx-x2, color);
                  g_hline (cy+y1, cx+x, cx+x2, color);
                }
            }
          x2 = x-1;
        }
      if (x1 != ox)
        {
          if (y0 != -1 && y1 < ox)
            {
              g_hline (cy-ox, cx-y0, cx-y2, color);
	      g_hline (cy+ox, cx-y0, cx-y2, color);
              if (y2 == 0)  /* draw the dots only once */
                y2 = 1;
              g_hline (cy-ox, cx+y0, cx+y2, color);
              g_hline (cy+ox, cx+y0, cx+y2, color);
              y2 = y0+1;
            }
        }
      oy = y1; ox = x1; y0 = y;
      if (x <= y)
        break;
      qy = qy + 2 * y + 1;
      ++y;
      if (qx+qy > qr)
        {
          qx = qx - 2 * x + 1;
          --x;
        }
    }
  if (x2 >= y2)
    {
      g_hline (cy-y1, cx-y2, cx-x2, color);
      g_hline (cy-y1, cx+y2, cx+x2, color);
      if (y1 != 0)
	{
	  g_hline (cy+y1, cx-y2, cx-x2, color);
	  g_hline (cy+y1, cx+y2, cx+x2, color);
	}
    }
}


static void gcirfy (int cx, int cy, int rx, int ry, int color)
{
  int x, y, r, x1, y1, oy, ox, x0, qr, qx, qy, z;

  r = ry + 1;
  x = r - 1; y = 0; qy = 0; qx = x * x; qr = r * r;
  ox = -1; oy = -1; x0 = -1;
  for (;;)
    {
      z = y * rx; y1 = z / ry;
      z = x * rx; x1 = z / ry;
      if (x != ox)
        {
          g_hline (cy-x, cx-y1, cx+y1, color);
          g_hline (cy+x, cx-y1, cx+y1, color);
        }
      if (y != oy)
        {
          if (x0 != -1 && x > oy)
            {
	      g_hline (cy-oy, cx-x0, cx+x0, color);
              if (oy != 0)   /* draw diameter only once */
                g_hline (cy+oy, cx-x0, cx+x0, color);
            }
        }
      ox = x; oy = y; x0 = x1;
      if (x <= y)
        break;
      qy = qy + 2 * y + 1;
      ++y;
      if (qx+qy > qr)
        {
          qx = qx - 2 * x + 1;
          --x;
        }
    }
}


static void gcirpy (int cx, int cy, int rx, int ry, int color)
{
  int x, y, r, x1, y1, oy, ox, y0, x2, y2, qr, qx, qy, z;

  r = ry + 1;
  x = r - 1; y = 0; qy = 0; qx = x * x; qr = r * r;
  ox = -1; oy = -1; y0 = -1; x2 = rx; y2 = 0;
  for (;;)
    {
      z = y * rx; y1 = z / ry;
      z = x * rx; x1 = z / ry;
      if (y1 != oy)
        {
          if (x > x2)
            {
              g_set (cx-y1, cy-x, color);
              g_set (cx-y1, cy+x, color);
              if (y1 != 0)  /* set these points only once */
                {
                  g_set (cx+y1, cy-x, color);
		  g_set (cx+y1, cy+x, color);
                }
            }
          else
            {
              g_vline (cx-y1, cy-x, cy-x2, color);
              g_vline (cx-y1, cy+x, cy+x2, color);
              if (y1 != 0)   /* set these 'points' only once */
                {
                  g_vline (cx+y1, cy-x, cy-x2, color);
                  g_vline (cx+y1, cy+x, cy+x2, color);
                }
            }
          x2 = x-1;
        }
      if (x1 != ox)
        {
          if (y0 != -1 && y1 < ox)
            {
              g_vline (cx-ox, cy-y0, cy-y2, color);
	      g_vline (cx+ox, cy-y0, cy-y2, color);
              if (y2 == 0)  /* draw the dots only once */
                y2 = 1;
              g_vline (cx-ox, cy+y0, cy+y2, color);
              g_vline (cx+ox, cy+y0, cy+y2, color);
              y2 = y0+1;
            }
        }
      oy = y1; ox = x1; y0 = y;
      if (x <= y)
        break;
      qy = qy + 2 * y + 1;
      ++y;
      if (qx+qy > qr)
        {
          qx = qx - 2 * x + 1;
          --x;
        }
    }
  if (x2 >= y2)
    {
      g_vline (cx-y1, cy-y2, cy-x2, color);
      g_vline (cx-y1, cy+y2, cy+x2, color);
      if (y1 != 0)
        {
          g_vline (cx+y1, cy-y2, cy-x2, color);
          g_vline (cx+y1, cy+y2, cy+x2, color);
        }
    }
}


void g_ellipse (int cx, int cy, int rx, int ry, int color, int fill_flag)
{
  if (rx == 0 && ry == 0)
    g_set (cx, cy, color);
  else if (rx == 0)
    g_vline (cx, cy-ry, cy+ry, color);
  else if (ry == 0)
    g_hline (cy, cx-rx, cx+rx, color);
  else
    {
      GLOCK;
      if (rx >= ry)
        if (fill_flag)
          gcirfx (cx, cy, rx, ry, color);
        else
          gcirpx (cx, cy, rx, ry, color);
      else
        if (fill_flag)
          gcirfy (cx, cy, rx, ry, color);
        else
	  gcirpy (cx, cy, rx, ry, color);
      GUNLOCK;
    }
}
