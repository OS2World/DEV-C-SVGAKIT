/* gfont.c (emx+gcc) -- Copyright (c) 1993-1994 by Matthias Burian */
/*              changes Copyright (c) 1994      by Johannes Martin */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <vesa.h>
#include "jmgraph.h"

#define HEADERSIZE 0x80

static char *fontname    = NULL;
static char *fontptr     = NULL;
static int xscale        = 256;
static int yscale        = 256;
static int cur_direction = RIGHT;

static char *offsets[256];
static char char_width[256];
static int orgcap, orgbase, orgdec, char_height;

int gf_loadfont(const char *name)
{
  FILE *file;
  int  filelen, first_char, n_chars, i;
  unsigned char *tmpptr;

  if (fontname != NULL)
    if (strcmp(fontname, name) == 0)
      return(TRUE);
    else
      {
        free(fontname);
        free(fontptr);
      }
  if ((file = fopen(name, "rb")) == NULL)
    {
      char *c;

      if ((c = getenv("EMXFONT")) == NULL)
        return(FALSE);
      if ((fontname = malloc(strlen(c) + strlen(name) + 2)) == NULL)
        return(FALSE);
      strcpy(fontname, c);
      strcat(fontname, "/");
      strcat(fontname, name);
      if ((file = fopen(fontname, "rb")) == NULL)
        {
          free(fontname);
          return(FALSE);
        }
    }
  else
    {
      if ((fontname = malloc(strlen(name) + 1)) == NULL)
        {
          fclose(file);
          return(FALSE);
        }
      strcpy(fontname, name);
    }
  fseek(file, 0L, SEEK_END);
  filelen = ftell(file);
  fseek(file, 0L, SEEK_SET);

  if ((fontptr = malloc(filelen)) == NULL)
    {
      fclose(file);
      free(fontname);
      return(FALSE);
    }

  if (fread(fontptr, filelen, 1, file) != 1)
    {
      fclose(file);
      free(fontptr);
      free(fontname);
      return(FALSE);
    }

  fclose(file);

  memset(offsets, 0, sizeof(offsets));
  memset(char_width, 0, sizeof(char_width));

  tmpptr = fontptr + HEADERSIZE;

  first_char  = *(tmpptr+4);
  n_chars     = *(tmpptr+1);
  orgcap      = *(tmpptr+8);
  orgbase     = *(tmpptr+9);
  orgdec      = *(tmpptr+10) - 256;
  char_height = orgcap - orgdec;

  tmpptr = fontptr + 0x8f;

  for (i = 0; i < n_chars; i++)
    offsets[first_char + i] = fontptr + 0x90 + 3 * n_chars
                            + *(tmpptr + 2 * (i + 1)) * 256
                            + *(tmpptr + 2 * (i + 1) - 1);

  tmpptr = fontptr + 0x90 + 2 * n_chars;

  for (i = 0; i < n_chars; i++)
    char_width[first_char + i] = *(tmpptr + i);

  return(TRUE);
}

int gf_getfontinfo(char **_name, int *_orgcap, int *_orgbase, int *_orgdec)
{
  if (fontname)
    {
      *_name    = fontname;
      *_orgcap  = orgcap;
      *_orgbase = orgbase;
      *_orgdec  = orgdec;
      return(TRUE);
    }
  return(FALSE);
}

void gf_setscale(double width, double height)
{
  yscale = rint(height * 256);
  xscale = rint(width  * 256);
}

void gf_getscale(double *width, double *height)
{
  *width  = (double) xscale / 256;
  *height = (double) yscale / 256;
}

void gf_setdirection(int direction)
{
  if ((direction >= 0) && (direction <= 2))
    cur_direction = direction;
}

int gf_getdirection(void)
{
  return(cur_direction);
}

int gf_getcharwidth(char c)
{
  if (cur_direction)
    return(char_height * yscale / 256);
  else
    return(char_width[(int) c] * xscale / 256);
}

int gf_getcharheight(char c)
{
  if (cur_direction)
    return(char_width[(int) c] * xscale / 256);
  else
    return(char_height * yscale / 256);
}

int gf_getstringwidth(const char *str)
{
  if (cur_direction)
    return(char_height * yscale / 256);
  else
    {
      int i = 0;

      while (*str != '\0')
        {
          i += char_width[(int) *str];
          str++;
        }
      return(i * xscale / 256);
    }
}

int gf_getstringheight(const char *str)
{
  if (cur_direction)
    {
      int i = 0;

      while (*str != '\0')
        {
          i += char_width[(int) *str];
          str++;
        }
      return(i * xscale / 256);
    }
  else
    return(char_height * yscale / 256);
}

void gf_drawchar(int x, int y, char c, int color)
{
  int x1, y1, x2, y2;
  int flag;
  unsigned char *tmpptr;

  x1 = 0;
  y1 = 0;
  if ((tmpptr = offsets[(int) c]) == 0)
    return;

  while (TRUE)
    {
      flag = 0;

      x2 = *tmpptr++;
      y2 = *tmpptr++;

      if (x2 >= 128)
        {
          flag++;
          x2 -= 128;
        }
      if (x2 >= 64)
        x2 -= 128;

      if (y2 >= 128)
        {
          flag++;
          y2 -= 128;
        }
      if (y2 >= 64)
        y2 -= 128;

      if (flag == 0)
        break;


      if (xscale != DEFAULT_WIDTH)
        x2 = (cur_direction ? yscale : xscale) * x2 / 256;
      if (yscale != DEFAULT_HEIGHT)
        y2 = (cur_direction ? xscale : yscale) * y2 / 256;

      if (flag == 2)
        switch (cur_direction)
          {
            case RIGHT:
              g_line(x+x1, y-y1, x+x2, y-y2, color);
              break;
            case UP:
              g_line(x-y1, y-x1, x-y2, y-x2, color);
              break;
            case DOWN:
              g_line(x+y1, y+x1, x+y2, y+x2, color);
              break;
          }

      x1 = x2;
      y1 = y2;
    }
}

void gf_drawstring(int x, int y, const char *str, int color)
{
  if (!fontname)
    return;

  while (*str != '\0')
    {
      gf_drawchar(x, y, *str, color);

      switch (cur_direction)
        {
          case UP:
            y -= char_width[(int) *str] * yscale / 256;
            break;
          case DOWN:
            y += char_width[(int) *str] * yscale / 256;
            break;
          default:
            x += char_width[(int) *str] * xscale / 256;
            break;
        }

      str++;
    }
}
