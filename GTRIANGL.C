/* gtriangl.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */

#include <vesa.h>

#include "jmgraph.h"
#include "graph2.h"

static void set3a (int x1, int y1, int x2, int y2, int x3, int y3, int color)
{
  int xl, xincl, errl, xdl, ydl;
  int xr, xincr, errr, xdr, ydr;
  int xxl, xxr, oxl, oxr, minx, maxx;
  int y;

  ydr = 1 + y3 - y1;
  if (x1 < x3)
    {
      xincr = 1;
      xdr = 1 + x3 - x1;
    }
  else
    {
      xincr = -1;
      xdr = 1 + x1 - x3;
    }
  if (xdr <= ydr)
    errr = xdr/2;
  else
    errr = ydr/2;
  xl = x1;
  xr = x1;

  ydl = 1 + y2 - y1;
  if (x1 < x2)
    {
      xincl = 1;
      xdl = 1 + x2 - x1;
    }
  else
    {
      xincl = -1;
      xdl = 1 + x1 - x2;
    }
  if (xdl <= ydl)
    errl = xdl/2;
  else
    errl = ydl/2;

  for (y = y1; y <= y2; ++y)
    {
      oxl = xl; oxr = xr;
      if (xdl <= ydl)
        {
          xxl = xl;
          errl += xdl;
          if (errl >= ydl)
            {
              errl -= ydl;
              xl += xincl;
            }
        }
      else
        for (;;)
          {
            xxl = xl;
            xl += xincl;
            errl += ydl;
            if (errl >= xdl)
              {
                errl -= xdl;
                break;
              }
          }
      if (xdr <= ydr)
        {
          xxr = xr;
          errr += xdr;
          if (errr >= ydr)
            {
              errr -= ydr;
              xr += xincr;
            }
        }
      else
        for (;;)
          {
            xxr = xr;
            xr += xincr;
            errr += ydr;
            if (errr >= xdr)
              {
                errr -= xdr;
                break;
              }
          }
      if (oxl < oxr)
        minx = oxl;
      else
        minx = oxr;
      if (xxl < minx)
        minx = xxl;
      if (xxr < minx)
        minx = xxr;
      if (oxl > oxr)
        maxx = oxl;
      else
        maxx = oxr;
      if (xxl > maxx)
        maxx = xxl;
      if (xxr > maxx)
        maxx = xxr;
      g_hline (y, minx, maxx, color);
    }

  xl = x2;
  ydl = 1 + y3 - y2;
  if (x2 < x3)
    {
      xincl = 1;
      xdl = 1 + x3 - x2;
    }
  else
    {
      xincl = -1;
      xdl = 1 + x2 - x3;
        }
  if (xdl <= ydl)
    errl = xdl/2;
  else
    errl = ydl/2;

  if (xdl <= ydl)
    {
      errl += xdl;
      if (errl >= ydl)
        {
          errl -= ydl;
          xl += xincl;
        }
    }
  else
    for (;;)
      {
        xl += xincl;
        errl += ydl;
        if (errl >= xdl)
          {
            errl -= xdl;
            break;
          }
      }

  for (y = y2+1; y <= y3; ++y)
    {
      oxl = xl; oxr = xr;
      if (xdl <= ydl)
        {
          xxl = xl;
          errl += xdl;
          if (errl >= ydl)
            {
              errl -= ydl;
              xl += xincl;
            }
        }
      else
        for (;;)
          {
            xxl = xl;
            xl += xincl;
            errl += ydl;
            if (errl >= xdl)
              {
                errl -= xdl;
                break;
              }
          }
      if (xdr <= ydr)
        {
          xxr = xr;
          errr += xdr;
          if (errr >= ydr)
            {
              errr -= ydr;
              xr += xincr;
            }
        }
      else
        for (;;)
          {
            xxr = xr;
            xr += xincr;
            errr += ydr;
            if (errr >= xdr)
              {
                errr -= xdr;
                break;
              }
          }
      if (oxl < oxr)
        minx = oxl;
      else
        minx = oxr;
      if (xxl < minx)
        minx = xxl;
      if (xxr < minx)
        minx = xxr;
      if (oxl > oxr)
        maxx = oxl;
      else
        maxx = oxr;
      if (xxl > maxx)
        maxx = xxl;
      if (xxr > maxx)
        maxx = xxr;
      g_hline (y, minx, maxx, color);
    }
}


void g_triangle (int x1, int y1, int x2, int y2, int x3, int y3, int color,
                 int fill_flag)
{
  int temp, xmin, xmax;

  GLOCK;
  if (fill_flag)
    {
      if (y1 > y2)
        {
          temp = y2; y2 = y1; y1 = temp;
          temp = x2; x2 = x1; x1 = temp;
        }
      if (y1 > y3)
        {
          temp = y3; y3 = y1; y1 = temp;
          temp = x3; x3 = x1; x1 = temp;
        }
      if (y2 > y3)
        {
          temp = y3; y3 = y2; y2 = temp;
          temp = x3; x3 = x2; x2 = temp;
        }
      if (y1 == y3) /* y1 == y2 && y2 == y3 */
        {
          xmin = x1;
          if (x2 < xmin) xmin = x2;
          if (x3 < xmin) xmin = x3;
          xmax = x1;
          if (x2 > xmax) xmax = x2;
          if (x3 > xmax) xmax = x3;
          g_hline (y1, xmin, xmax, color);
        }
      else
        set3a (x1, y1, x2, y2, x3, y3, color);
    }
  else
    {
      g_line (x3, y3, x1, y1, color);
      g_line (x1, y1, x2, y2, color);
      g_line (x2, y2, x3, y3, color);
    }
  GUNLOCK;
}
