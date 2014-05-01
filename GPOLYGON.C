/* gpolygon.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */

#include <vesa.h>

#include "jmgraph.h"
#include "graph2.h"

struct dda
{
  int n, m;
  int e, x, y;
  int xi, yi;
  int c;
};

struct line
{
  int x0, y0, x1, y1;
  int y;
};

struct seg
{
  int x0, x1, i;
};


static int cmpx (const void *p1, const void *p2)
{
  const struct seg *s1, *s2;
  int n1, n2;

  s1 = p1; s2 = p2;
  n1 = s1->x0;
  n2 = s2->x0;
  if (n1 == n2)
    {
      n1 = s1->x1;
      n2 = s2->x1;
    }
  if (n1 < n2)
    return (-1);
  else if (n1 > n2)
    return (1);
  else
    return (0);
}


static void gpolyfill (const int *x, const int *y, int n, int color)
{
  int y1;
  struct line lt;
  struct dda dt;
  int i, j1, j2, k, t, m;
  int x1, x2;
  struct dda *dda;
  struct seg *seg;
  struct line *line;

  dda = alloca (n * sizeof (*dda));
  seg = alloca (n * sizeof (*seg));
  line = alloca (n * sizeof (*line));
  /* initialize line descriptors */
  y1 = y[0];
  for (i = 0; i < n; ++i)
    {
      lt.x0 = x[i];
      lt.y0 = y[i];
      if (i < n-1)
        {
          lt.x1 = x[i+1];
          lt.y1 = y[i+1];
        }
      else
        {
          lt.x1 = x[0];
          lt.y1 = y[0];
        }
      if (lt.y0 > lt.y1)
        {
          t = lt.x0; lt.x0 = lt.x1; lt.x1 = t;
          t = lt.y0; lt.y0 = lt.y1; lt.y1 = t;
        }
      if (lt.y0 < y1)
        y1 = lt.y0;
      line[i] = lt;
    }
  /* initialize DDAs */
  for (i = 0; i < n; ++i)
    {
      lt = line[i];
      dt.y = lt.y0;
      dt.x = lt.x0;
      dt.n = 1 + abs (lt.y1 - lt.y0);
      dt.m = 1 + abs (lt.x1 - lt.x0);
      if (lt.y1 > lt.y0)
        dt.yi = 1;
      else
        dt.yi = -1;
      if (lt.x1 > lt.x0)
        dt.xi = 1;
      else
        dt.xi = -1;
      if (dt.n > dt.m)
        {
          dt.c = dt.n;
          dt.e = dt.m/2;
        }
      else
        {
          dt.c = dt.m;
          dt.e = dt.n/2;
        }
      dda[i] = dt;
      lt.y = lt.y0-1;
      line[i] = lt;
    }
  /* one iteration per scan line */
  for (;;)
    {
      m = 0;
      /* find intersection of lines with scan line */
      for (i = 0; i < n; ++i)
        {
          if (line[i].y == y1-1)
            {
              dt = dda[i];
              if (dt.c < 1)
                line[i].y = y1 - 2;
              else
                {
                  line[i].y = dt.y;
                  x1 = dt.x;
                  if (dt.n > dt.m)
                    {
                      x2 = dt.x;
                      --dt.c;
                      dt.y += dt.yi;
                      dt.e += dt.m;
                      if (dt.e >= dt.n)
                        {
                          dt.e -= dt.n;
                          dt.x += dt.xi;
                        }
                    }
                  else
                    {
                      for (;;)
                        {
                          --dt.c;
                          dt.x += dt.xi;
                          dt.e += dt.n;
                          if (dt.e >= dt.m)
                            {
                              dt.e -= dt.m;
                              dt.y += dt.yi;
                              break;
                            }
                        }
                      x2 = dt.x - dt.xi;
                    }
                  dda[i] = dt;
                  if (x1 < x2)
                    {
                      seg[m].x0 = x1;
                      seg[m].x1 = x2;
                    }
                  else
                    {
                      seg[m].x0 = x2;
                      seg[m].x1 = x1;
                    }
                  if (y1 == line[i].y0 || y1 == line[i].y1)
                    seg[m].i = i;
                  else
                    seg[m].i = -1;
                  ++m;
                }
            }
        }
      /* find vertices to be removed */
      if (m == 0)
        break;
      for (i = 0; i < m; ++i)
        if (seg[i].i >= 0)
          {
            if (i < m-1)
              k = i + 1;
            else
              k = 0;
            if (seg[k].i >= 0)
              {
                j1 = seg[i].i;
                j2 = seg[k].i;
                t = abs(j1-j2);
                if (t == 1 || t == n-1)
                  /* segments of two adjacent line */
                  if ((y1 == line[j1].y0) != (y1 == line[j2].y0))
                    /* it's not a `v' or `^' corner */
                    if (line[j1].y0 != line[j1].y1 &&
                        line[j2].y0 != line[j2].y1)
                      /* no horizontal line */
                      {
                        if (seg[k].x0 < seg[i].x0)
                          seg[i].x0 = seg[k].x0;
                        if (seg[k].x1 > seg[i].x1)
                          seg[i].x1 = seg[k].x1;
                        seg[i].i = -1; /* ignore */
                        seg[k].i = -2; /* remove */
                      }
              }
          }
      /* remove line segments */
      for (i = 0; i < m;)
        if (seg[i].i == -2)
          seg[i] = seg[--m];
        else
          ++i;
      /* draw line segments */
      if (m >= 2)
        qsort (seg, m, sizeof (struct seg), cmpx);
      for (i = 0; i+1 < m; i += 2)
        g_hline (y1, seg[i].x0, seg[i+1].x1, color);
      ++y1;
    }
}


void g_polygon (const int *x, const int *y, int n, int color, int fill_flag)
{
  int i;

  if (n >= 3)
    {
      GLOCK;
      if (fill_flag)
        gpolyfill (x, y, n, color);
      else
        {
          for (i = 0; i < n-1; ++i)
            g_line (x[i], y[i], x[i+1], y[i+1], color);
          g_line (x[n-1], y[n-1], x[0], y[0], color);
        }
      GUNLOCK;
    }
  else if (n == 2)
    g_line (x[0], y[0], x[1], y[1], color);
  else if (n == 1)
    g_set (x[0], y[0], color);
}
