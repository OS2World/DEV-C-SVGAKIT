/* graph.c (emx+gcc) */

/* Test graphics library */

/* Usage: graph */

/* Keyboard control: */

/*   RETURN     Move to next test                               */
/*   ESC        Quit                                            */
/*   SPACE      Pause (type SPACE to continue or ESC to quit)   */

#define INCL_VIO
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <getopt.h>
#include <limits.h>
#include <vesa.h>
#include "jmgraph.h"

#define FALSE 0
#define TRUE  1

static int pal_mode;
static int wait_flag;

static char pal_1[3*255*2];
static int pal_1_idx;

static char pal_2[3*255];
static int color_2[3];
static int dir_2[3];

static char pal[3*256];

/* Return a random number between 0 and n-1.  If n is not a power of two, */
/* the return value is slightly less random than expected at first sight. */
/* Better results can be achieved by discarding values returned by rand() */
/* which are greater than or equal to n * ((RAND_MAX+1) / n).  Of course, */
/* this doesn't matter with this program.                                 */

#define RND(n) ((n) < RAND_MAX ? rand() % (n) : (rand() + 1) * (n) / (RAND_MAX + 1) - 1)

#define SLEEP2_MIN 500

#define RND_COLOR (g_colors > 256 ? RGB(RND(256),RND(256),RND(256)) : RND(g_colors))

static unsigned loops_per_second;
static unsigned loops;
static unsigned volatile counter;
static jmp_buf alarm_jmp;

static void alarm_handler (int sig)
{
  longjmp (alarm_jmp, 1);
}

static void init_waste_time (void)
{
  loops_per_second = ULONG_MAX;
  if (setjmp (alarm_jmp) != 0)
    loops_per_second = counter;
  else
    {
      counter = 0;
      signal (SIGALRM, alarm_handler);
      alarm (1);
      for (counter = 0; counter < loops_per_second; ++counter)
        ;
    }
}


static void waste_time (unsigned millisec)
{
  if (millisec >= SLEEP2_MIN)
    _sleep2 (millisec);
  else
    {
      loops = (loops_per_second * millisec) / 1000;
      for (counter = 0; counter < loops; ++counter)
        ;
    }
}


static void kchar (unsigned char c)
{
  if (c == 0x1b)
    {
      g_mode (G_MODE_OFF);
      exit (0);
    }
  else if (c == ' ')
    {
      VesaGetCharacter(&c);
      if (c == 0x1b)
        {
          g_mode (G_MODE_OFF);
          exit (0);
        }
    }
}


static void kwait (void)
{
  unsigned char c;

  do
    {
      VesaGetCharacter(&c);
      kchar (c);
    } while (c == ' ');
}


static int khit_counter = 0;

static int khit (void)
{
  unsigned char c;

  ++khit_counter;
  if (khit_counter < 8)
    return (0);
  khit_counter = 0;
  if (!VesaKeyboardHit())
    return(0);
  VesaGetCharacter(&c);
  kchar (c);
  return (c != ' ');
}

static void test_1 (void)
{
  int x, y, c;

  for (x = 0; x < g_xsize; ++x)
    {
      g_lock ();
      if (g_32Bit)
        {
          for (y = 0; y < g_ysize; ++y) {
            c = (x+y) * (long long) g_colors / (g_xsize + g_ysize);
            g_set (x, y, RGB(c >> 16, c >> 8, c));
          }
        }
      else
        {
          for (y = 0; y < g_ysize; ++y)
            g_set (x, y, (x+y) * (long long) g_colors / (g_xsize + g_ysize));
        }
      g_unlock (TRUE);
    }
  kwait ();
}


static void test_2 (void)
{
  char mouse[16][16]
  = { { 0, 0, 0, 0, 0, 0, 1, 7, 1, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 1, 7, 7, 7, 1, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 1, 7, 7, 1, 7, 7, 1, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 1, 7, 7, 7, 7, 7, 1, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 1, 7, 1, 1, 7, 1, 1, 7, 1, 0, 0, 0, 0 },
      { 0, 0, 0, 1, 7, 7, 7, 7, 7, 7, 7, 1, 0, 0, 0, 0 },
      { 0, 0, 0, 1, 7, 7, 7, 7, 7, 7, 7, 1, 0, 0, 0, 0 },
      { 0, 0, 0, 1, 7, 7, 7, 7, 7, 7, 7, 1, 0, 0, 0, 0 },
      { 0, 0, 0, 1, 7, 7, 7, 7, 7, 7, 7, 1, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 1, 7, 7, 7, 7, 7, 1, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 1, 7, 7, 7, 1, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 1, 8, 1, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 1, 8, 1, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 1, 8, 1, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 8, 1, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 8, 1, 0, 0, 0 } };
  int x, y;
  int i, j, k;
  char bufa[4][16][2], bufb[4][16][2];
  char buf1[16][16], buf2[16][16];
  short buf3[16][16], buf4[16][16];
  char buf5[16][16][3], buf6[16][16][3];
  int buf7[16][16], buf8[16][16];

  x = g_xsize / 2;
  y = g_ysize / 2;
  while (!khit())
    {
      x += 8 - RND(16);
      y += 8 - RND(16);
      x -= 8 - RND(16);
      y -= 8 - RND(16);
      if (x < 0)
        x = 0;
      if (x > g_xsize - 17)
        x = g_xsize - 17;
      if (y < 0)
        y = 0;
      if (y > g_ysize - 17)
        y = g_ysize - 17;
      switch (g_colors)
        {
          case 0x10:
            g_getimage(x, y, x + 15, y + 15, bufa);
            for (k = 0; k < 4; k++)
              for (i = 0; i < 16; i++)
                {
                  bufb[k][i][0] = 0;
                  for (j = 0; j < 8; j++)
                    bufb[k][i][0] |= ((mouse[i][j] & (1 << k)) ? 0xFF : bufa[k][i][0]) & (0x80 >> j);
                  bufb[k][i][1] = 0;
                  for (j = 8; j < 16; j++)
                    bufb[k][i][1] |= ((mouse[i][j] & (1 << k)) ? 0xFF : bufa[k][i][1]) & (0x80 >> (j - 8));
                }
            g_putimage(x, y, x + 15, y + 15, bufb);
            _sleep2(20);
            g_putimage(x, y, x + 15, y + 15, bufa);
            break;
          case 0x100:
            g_getimage(x, y, x + 15, y + 15, buf1);
            for (i = 0; i < 16; i++)
              for (j = 0; j < 16; j++)
                buf2[i][j] = mouse[i][j] ? mouse[i][j] : buf1[i][j];
            g_putimage(x, y, x + 15, y + 15, buf2);
            _sleep2(20);
            g_putimage(x, y, x + 15, y + 15, buf1);
            break;
          case 0x8000:
          case 0x10000:
            g_getimage(x, y, x + 15, y + 15, buf3);
            for (i = 0; i < 16; i++)
              for (j = 0; j < 16; j++)
                buf4[i][j] = mouse[i][j] ? mouse[i][j] : buf3[i][j];
            g_putimage(x, y, x + 15, y + 15, buf4);
            _sleep2(20);
            g_putimage(x, y, x + 15, y + 15, buf3);
            break;
          case 0x1000000:
            if (g_32Bit)
              {
                g_getimage(x, y, x + 15, y + 15, buf7);
                for (i = 0; i < 16; i++)
                  for (j = 0; j < 16; j++)
                    buf8[i][j] = mouse[i][j] ? mouse[i][j] : buf7[i][j];
                g_putimage(x, y, x + 15, y + 15, buf8);
                _sleep2(20);
                g_putimage(x, y, x + 15, y + 15, buf7);
                break;
              }
            else
              {
                g_getimage(x, y, x + 15, y + 15, buf5);
                for (i = 0; i < 16; i++)
                  for (j = 0; j < 16; j++)
                    {
                      buf6[i][j][0] = mouse[i][j] ? mouse[i][j] : buf5[i][j][0];
                      buf6[i][j][1] = mouse[i][j] ? mouse[i][j] : buf5[i][j][1];
                      buf6[i][j][2] = mouse[i][j] ? mouse[i][j] : buf5[i][j][2];
                    }
                g_putimage(x, y, x + 15, y + 15, buf6);
                _sleep2(20);
                g_putimage(x, y, x + 15, y + 15, buf5);
              }
            break;
        }
    }
}


static void test_3 (void)
{
  int x[256], y[256], color[256], i;

  g_wmode(G_XOR);
  do
    {
      color[0] = RND_COLOR;

      g_box (x[0] = RND (g_xsize), y[0] = RND (g_ysize),
             x[1] = RND (g_xsize), y[1] = RND (g_ysize),
             color[0], G_FILL);
      _sleep2(100);
      if (khit())
        break;
      g_box (x[0], y[0], x[1], y[1], color[0], G_FILL);
    } while (1);
  do
    {
      for (i = 0; i < 256; i++)
        {
          x[i] = RND (g_xsize);
          y[i] = RND (g_ysize);
          color[i] = RND_COLOR;
          g_set(x[i], y[i], color[i]);
        }
      _sleep2(100);
      if (khit())
        break;
      while (i-- > 0)
        g_set(x[i], y[i], color[i]);
    } while (1);
  g_wmode(G_NORM);
}


static void test_4 (void)
{
  int c1, c2, c3, c4;
  int x, y;

  if (g_colors > 256)
    {
      c1 = RGB(0, 0, 255);   c2 = RGB(0, 255, 0);
      c3 = RGB(0, 255, 255); c4 = RGB(255, 0, 0);
    }
  else
    {
      c1 = G_BLUE; c2 = G_GREEN; c3 = G_CYAN; c4 = G_RED;
    }

  g_clear (G_BLACK);
  for (x = 0; x < g_xsize; x += 5)
    g_line (x, 0, g_xsize/2, g_ysize/2, c1);
  for (y = 0; y < g_ysize; y += 5)
    g_line (g_xsize-1, y, g_xsize/2, g_ysize/2, c2);
  for (x = g_xsize-1; x >= 0; x -= 5)
    g_line (x, g_ysize-1, g_xsize/2, g_ysize/2, c3);
  for (y = g_ysize-1; y >= 0; y -= 5)
    g_line (0, y, g_xsize/2, g_ysize/2, c4);
  kwait ();
}


static void test_5 (void)
{
  int c, x, y;

  g_clear (G_BLACK);
  if (g_32Bit)
    {
      for (y = 0; y < g_ysize; ++y)
        {
          c = y * (long long) g_colors / g_ysize;
          g_line (0, 0, g_xsize-1, y, RGB(c >> 16, c >> 8, c));
        }
      for (x = g_xsize-1; x >= 0; --x)
        {
          c = x * (long long) g_colors / g_xsize;
          g_line (0, 0, x, g_ysize-1, RGB(c >> 16, c >> 8, c));
        }
    }
  else
    {
      for (y = 0; y < g_ysize; ++y)
        g_line (0, 0, g_xsize-1, y, y * (long long) g_colors / g_ysize);
      for (x = g_xsize-1; x >= 0; --x)
        g_line (0, 0, x, g_ysize-1, x * (long long) g_colors / g_xsize);
    }
  kwait ();
}


static void test_6 (void)
{
  g_clear (G_BLACK);
  do
    {
      g_line (RND (g_xsize), RND (g_ysize), RND (g_xsize), RND (g_ysize),
                RND_COLOR);
    } while (!khit ());
}


static void test_7 (void)
{
  int y, c;

  if (g_colors > 256) c = RGB(127, 127, 127);
  else c = G_WHITE;

  g_clear (G_BLACK);
  for (y = 0; y*3 < g_xsize && y < g_ysize; ++y)
    g_hline (y, y, y*3, c);
  kwait ();

  if (g_colors > 256) c = RGB(127, 0, 0);
  else c = G_RED;

  g_wmode(G_XOR);
  for (y = 0; y*3+50 < g_xsize && y < g_ysize; ++y)
    g_hline (y, y+50, y*3+50, c);
  kwait ();
  for (y = 0; y*3+50 < g_xsize && y < g_ysize; ++y)
    g_hline (y, y+50, y*3+50, c);
  kwait ();
  g_wmode(G_NORM);
}


static void test_8 (void)
{
  int x, c;

  if (g_colors > 256) c = RGB(127, 127, 127);
  else c = G_WHITE;

  g_clear (G_BLACK);
  for (x = 0; x*3 < g_ysize && x < g_xsize; ++x)
    g_vline (x, x, x*3, c);
  kwait ();

  if (g_colors > 256) c = RGB(127, 0, 0);
  else c = G_RED;

  g_wmode(G_XOR);
  for (x = 0; x*3+50 < g_ysize && x < g_xsize; ++x)
    g_vline (x, x+50, x*3+50, c);
  kwait ();
  for (x = 0; x*3+50 < g_ysize && x < g_xsize; ++x)
    g_vline (x, x+50, x*3+50, c);
  kwait ();
  g_wmode(G_NORM);
}


static void test_10 (void)
{
  g_clear (G_BLACK);
  do
    {
      g_box (RND (g_xsize), RND (g_ysize),
             RND (g_xsize), RND (g_ysize),
             RND_COLOR, G_OUTLINE);
    } while (!khit ());
}


static void test_11 (void)
{
  g_clear (G_BLACK);
  do
    {
      g_box (RND (g_xsize), RND (g_ysize),
             RND (g_xsize), RND (g_ysize),
             RND_COLOR, G_FILL);
    } while (!khit ());
}


static void test_19 (void)
{
  int mx, my, rx, ry, fill;
  int cn;


  for (fill = 0; fill <= 1; ++fill)
    {
      g_clear (G_BLACK);
      if (fill == 0)
        cn = g_colors-1;
      else
        cn = g_colors;
      do
        {
          mx = RND (g_xsize);
          my = RND (g_ysize);
          rx = 20 + RND (g_xsize/2);
          ry = 10 + RND (g_ysize/2);
          if (mx+rx < g_xsize && mx-rx > 0 && my+ry < g_ysize && my-ry > 0)
            g_ellipse (mx, my, rx, ry, RND_COLOR, fill);
        } while (!khit ());
    }
}


#define MAX_VERT 20

static void test_23 (void)
{
  int i, n, x[MAX_VERT], y[MAX_VERT];

  g_clear (G_BLACK);
  do
    {
      n = 0;
      for (i = 0; i < 2*(MAX_VERT-3); ++i)
        n += RND (2);
      n = 3 + abs (n - (MAX_VERT-3));
      for (i = 0; i < n; ++i)
        {
          x[i] = RND (g_xsize);
          y[i] = RND (g_ysize);
        }
      g_polygon (x, y, n,  RND_COLOR, G_FILL);
    } while (!khit ());
}


static void test_30 (void)
{
  char s[30] = "This is the default font.";
  int c[4];
  int width, height;
  int i, h;

  if (g_colors > 256)
    {
       c[0] = RGB(0, 0, 255);   c[1] = RGB(0, 255, 0);
       c[2] = RGB(0, 255, 255); c[3] = RGB(255, 0, 0);
    }
  else
    {
       c[0] = G_BLUE; c[1] = G_GREEN; c[2] = G_CYAN; c[3] = G_RED;
    }
  g_clear(G_BLACK);

  gf_loadfont(DEFFONT);
  gf_setdirection(RIGHT);
  gf_setscale(DEFAULT_HEIGHT, DEFAULT_WIDTH);
  width  = gf_getstringwidth(s);
  height = gf_getstringheight(s);
  h = 0;
  for (i = 2; i < 11; i++)
    {
      gf_setscale(i * g_xsize / (width * 12.0),
                  i * g_ysize / (height * 55.0));
      h += i;
      gf_drawstring((g_xsize - gf_getstringwidth(s)) / 2, h * g_ysize / 55, s, c[i % 4]);
    }
  gf_setdirection(DOWN);
  gf_setscale(0.06 * g_xsize / height, 0.95 * g_ysize / width);
  gf_drawstring(10, 10, s, c[0]);
  gf_setdirection(UP);
  gf_drawstring(g_xsize - 10, g_ysize - 10, s, c[1]);
  kwait();
}

static void init_pal_1 (void)
{
  int i, j;
  unsigned char func[255];

  for (j = 0; j <= 127; ++j)
    {
      func[j] = (char)(j/2);
      func[254-j] = (char)(j/2);
    }
  for (i = 0; i < 255; ++i)
    {
      pal_1[3*i+0] = func[(i+0*85)%255];
      pal_1[3*i+1] = func[(i+1*85)%255];
      pal_1[3*i+2] = func[(i+2*85)%255];
    }
  memcpy (pal_1+3*255, pal_1, 3*255);
  pal_1_idx = 1;
}


static void make_pal_1 (void)
{
  memcpy (pal+3, pal_1 + pal_1_idx * 3, 3*255);
  ++pal_1_idx;
  if (pal_1_idx > 254)
    pal_1_idx -= 255;
}


static void init_pal_2 (void)
{
  int i;

  memset (pal_2, 0, 3*255);
  for (i = 0; i < 3; ++i)
    {
      dir_2[i] = 1;
      color_2[i] = 0;
    }
}


static void make_pal_2 (void)
{
  int d, i;

  memcpy (pal+3, pal_2, 3*255);
  memmove (pal_2, pal_2+3, 3*254);
  d = 3 * 254;
  pal_2[d+0] = (char)color_2[0];
  pal_2[d+1] = (char)color_2[1];
  pal_2[d+2] = (char)color_2[2];
  for (i = 0; i < 3; ++i)
    {
      color_2[i] += dir_2[i];
      if (color_2[i] < 0 || color_2[i] > 63)
        {
          dir_2[i] = -dir_2[i];
          color_2[i] += dir_2[i];
        }
      else
        break;
    }
}


static void set_pal (void)
{
  g_vgapal (pal, 0, 256, wait_flag);
}


static void make_pal (void)
{
  switch (pal_mode)
    {
    case 1:
      make_pal_1 ();
      set_pal ();
      break;
    case 2:
      make_pal_2 ();
      set_pal ();
      break;
    }
}


static void pal_demo (unsigned millisec)
{
  while (!khit ())
    {
      make_pal ();
      waste_time (millisec);
    }
}


static void demo_1 (unsigned millisec)
{
  int a, color, h, k, m, n, r, s, w, x1, x2, xv1, xv2, y1, y2, yv1, yv2, z;

  w = g_xsize / 2;
  h = g_ysize / 2;
  if (g_ysize < w)
    z = g_ysize;
  else
    z = w;
  k = (z-10)/2;
  r = z / 30;
  s = r / 2;
  m = 20;
  for (;;)
    {
      g_clear (G_BLACK);
      n = 3 * (g_ysize / 2 + RND (g_ysize));
      x1 = 1 + RND (k-1);
      y1 = 1 + RND (k-1);
      x2 = 1 + RND (k-1);
      y2 = 1 + RND (k-1);
      a = 0;
      for (;;)
        {
          if (khit ())
            return;
          if (a <= 0)
            {
              xv1 = RND (r) - s;
              yv1 = RND (r) - s;
              xv2 = RND (r) - s;
              yv2 = RND (r) - s;
              a = 1 + RND (m -1);
              color = RND_COLOR;
            }
          g_lock ();
          g_line (w+2*x1, h-y1, w+2*x2, h-y2, color);
          g_line (w-2*y1, h+x1, w-2*y2, h+x2, color);
          g_line (w-2*x1, h-y1, w-2*x2, h-y2, color);
          g_line (w-2*y1, h-x1, w-2*y2, h-x2, color);
          g_line (w-2*x1, h+y1, w-2*x2, h+y2, color);
          g_line (w+2*y1, h-x1, w+2*y2, h-x2, color);
          g_line (w+2*x1, h+y1, w+2*x2, h+y2, color);
          g_line (w+2*y1, h+x1, w+2*y2, h+x2, color);
          g_unlock (TRUE);
          if (n == 1)
            break;
          --a;
          if (n > 0)
            --n;
          waste_time (millisec); /* This demo looks better when slowed down */
          x1 = (x1+xv1) % k;
          y1 = (y1+yv1) % k;
          x2 = (x2+xv2) % k;
          y2 = (y2+yv2) % k;
        }
    }
}


static void demo_2 (unsigned millisec)
{
  int i, x, y, c, d, k, n, m;
  static int inc_x[4] = {1, 0, -1,  0};
  static int inc_y[4] = {0, 1,  0, -1};

  c = 1; d = 0; n = 16; m = 1;
  x = 0 - inc_x[d];
  y = 0 - inc_y[d];
  k = n;
  for (i = 0; i < 255; ++i)
    {
      x += inc_x[d]; y += inc_y[d];
      g_box (x*20, y*12, (x+1)*20-1, (y+1)*12-1, c, G_FILL);
      if (k <= 1)
        {
          d = (d+1) % 4;
          --m;
          if (m == 0)
            {
              --n; m = 2;
            }
          k = n;
        }
      else
        --k;
      ++c;
    }
  pal_demo (millisec);
}


static void demo_3 (unsigned millisec)
{
  int i;

  for (i = 0; i < 256; ++i)
    g_vline (i, 0, g_ysize-1, i);
  pal_demo (millisec);
}


static void usage (void)
{
  VESAINFO     vinfo;
  VESAMODEINFO minfo;
  PVESAWORD    pmode;

  fprintf(stderr, "Usage: graph [-d#] [-p#] [-s#] [-w]\n\n");
  fprintf(stderr, "-d#    Select demo mode [0]:\n");
  fprintf(stderr, "         -d0  test mode\n");
  fprintf(stderr, "         -d1  kaleidoscope demo\n");
  fprintf(stderr, "         -d2  palette demo (spiral)\n");
  fprintf(stderr, "         -d3  palette demo (band)\n");
  fprintf(stderr, "-p#    Select palette [0]:\n");
  fprintf(stderr, "         -p0  default palette\n");
  fprintf(stderr, "         -p1  smooth sequence of hues\n");
  fprintf(stderr, "         -p2  smooth sequence of all colors\n");
  fprintf(stderr, "-s#    Set delay (# milliseconds) for demo modes [6]\n");
  fprintf(stderr, "-w     Wait for vertical retrace when modifying palette\n");
  fprintf(stderr, "-m#    Select video mode # [%x]\n", G_MODE_VGA_L);
  fprintf(stderr, "-b#    in test mode: Skip first # tests [0]\n");
  fprintf(stderr, "-h     display this help\n\n");
  if (VesaGetInfo(&vinfo))
    {
      fprintf(stdout, "       Available modes:\n");
      fprintf(stdout, "       ModeNr    xres yres colors\n");
      pmode = vinfo.Modes;
      while (*pmode != 0xFFFF)
        {
          if (VesaGetModeInfo(*pmode, &minfo))
            {
              if ((minfo.ModeAttributes & 0x10) == 0x10)
                {
                  int colors;
                  int vioram;

                  if (vinfo.MemoryBanks) vioram = minfo.BytesPerScanline * minfo.Height;
                  else vioram = 0;
                  if (vioram <= vinfo.MemoryBanks << 16)
                    {
                       if (minfo.RedMaskSize + minfo.GreenMaskSize + minfo.BlueMaskSize > 0)
                         colors = 1 << (minfo.RedMaskSize + minfo.GreenMaskSize +
                                        minfo.BlueMaskSize);
                       else
                         colors = 1 << minfo.NumberOfBitsPerPixel;

                       if (minfo.NumberOfBitsPerPixel == 32)
                         fprintf(stdout, "%13x%8i%5i %i  32 Bit\n", *pmode, minfo.Width,
                                      minfo.Height, colors);
                       else
                         fprintf(stdout, "%13x%8i%5i %i \n", *pmode, minfo.Width,
                                            minfo.Height, colors);
                    }
                }
            }
          else
            fprintf(stderr, "%13x --- VesaGetModeInfo failed ---\n", *pmode);
          pmode++;
        }
      fprintf(stdout, "\n");
    }
  exit (1);
}


int main (int argc, char *argv[])
{
  int c, demo_mode, video_mode, delay, skip;
  char *p;

  if (!g_init())
    {
      fprintf(stderr, "could not initialize graphics library, aborting...\n");
      exit(1);
    }
  demo_mode = 0; pal_mode = 0; delay = 6; wait_flag = FALSE;
  video_mode = G_MODE_VGA_L; skip = 0;
  while ((c = getopt (argc, argv, "d:p:s:m:b:wh")) != EOF)
    switch (c)
      {
      case 'd':
        errno = 0;
        demo_mode = strtol (optarg, &p, 0);
        if (errno != 0 || *p != 0 || demo_mode < 0 || demo_mode > 3)
          usage ();
        break;
      case 'p':
        errno = 0;
        pal_mode = strtol (optarg, &p, 0);
        if (errno != 0 || pal_mode < 1 || pal_mode > 2)
          usage ();
        break;
      case 's':
        errno = 0;
        delay = strtol (optarg, &p, 0);
        if (errno != 0 || *p != 0)
          usage ();
        break;
      case 'm':
        if ((sscanf(optarg, "%x", &video_mode) != 1) || (video_mode < 0))
          usage ();
        break;
      case 'b':
        errno = 0;
        skip = strtol (optarg, &p, 0);
        if (errno != 0 || *p != 0 || skip < 0)
          usage ();
        break;
      case 'w':
        wait_flag = TRUE;
        break;
      case 'h':
      default:
        usage ();
      }
  if (optind < argc)
    usage ();
  memset (pal, 0, 3*256);
  switch (pal_mode)
    {
    case 1:
      init_pal_1 ();
      break;
    case 2:
      switch (demo_mode)
        {
        case 0:
          fprintf (stderr, "-p2 should not be used with -d0\n");
          exit (1);
        }
      init_pal_2 ();
      break;
    default:
      switch (demo_mode)
        {
        case 2:
        case 3:
          fprintf(stderr, "-d%d should not be used with -p0\n", demo_mode);
          exit (1);
        }
    }
  switch (demo_mode)
    {
    case 1:
    case 2:
    case 3:
      if (delay < SLEEP2_MIN)
        {
          printf("Please wait...");
          fflush(stderr);
          init_waste_time ();
        }
      break;
    }
  if (!g_mode (video_mode))
    {
      fprintf (stderr, "Cannot switch to graphics mode 0x%x\n", video_mode);
      exit(1);
    }
  make_pal ();
  switch (demo_mode)
    {
    case 0:
      if (skip <= 0)
        test_1 ();
      if (skip <= 1)
        test_2 ();
      if (skip <= 2)
        test_3 ();
      if (skip <= 3)
        test_4 ();
      if (skip <= 4)
        test_5 ();
      if (skip <= 5)
        test_6 ();
      if (skip <= 6)
        test_7 ();
      if (skip <= 7)
        test_8 ();
      if (skip <= 8)
        test_10 ();
      if (skip <= 9)
        test_11 ();
      if (skip <= 10)
        test_19 ();
      if (skip <= 11)
        test_23 ();
      if (skip <= 12)
        test_30 ();
      if (skip <= 13)
      demo_1 (0);
      break;
    case 1:
      demo_1 (delay);
      break;
    case 2:
      demo_2 (delay);
      break;
    case 3:
      demo_3 (delay);
      break;
    }
  g_mode (G_MODE_OFF);
  exit(0);
}
