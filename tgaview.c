#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "jmgraph.h"
#include "vesa.h"

#pragma pack(1)

FILE *Open_Targa(char *name, int *width, int *height)
{
  FILE *f;
  int  i;

  if ((f = fopen(name, "rb")) == NULL)
    return(NULL);
  for (i = 0; i < 12; i++)
    fgetc(f);
  *width  = fgetc(f);
  *width  |= fgetc(f) << 8;
  *height = fgetc(f);
  *height |= fgetc(f) << 8;
  return(f);
}

void Close_Targa(FILE *f)
{
  fclose(f);
}

int Read_Targa(FILE *f, unsigned char *pixel, int n)
{
  n *= 3;
  while (n-- > 0)
    *pixel++ = fgetc(f);
  return(ferror(f) || feof(f));
}

#define BUFSIZE  1024

unsigned char  *colors = NULL;
unsigned short *counts = NULL;

int Find_Palette(char *filename)
{
  FILE *f;
  int width, height, rest, x, y, i, h;
  unsigned char pixel[3];
  unsigned char  *_colors;
  unsigned short *_counts;

  if (((colors = malloc(3 * BUFSIZE)) == NULL) ||
      ((counts = malloc(2 * BUFSIZE)) == NULL))
    return(1);
  memset(counts, 0, 2 * BUFSIZE);
  if ((f = Open_Targa(filename, &width, &height)) == NULL)
    return(1);
  if (height > g_ysize)
    height = g_ysize;
  if (width > g_xsize)
    {
      rest  = width - g_xsize;
      width = g_xsize;
    }
  else
    rest = 0;
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          if (Read_Targa(f, pixel, 1))
            return(1);
          pixel[0] >>= 2;
          pixel[1] >>= 2;
          pixel[2] >>= 2;
          _counts = counts;
          _colors = colors;
          i = 0;
          while ((i < BUFSIZE) && (*_counts > 0) &&
                 ((pixel[1] != _colors[0]) || (pixel[0] != _colors[1]) || (pixel[2] != _colors[2])))
            {
              _counts++;
              _colors += 3;
              i++;
            }
          if (i <= BUFSIZE)
            if (*_counts > 0)
              {
                if (*_counts < 0xFFFF)
                  {
                    ++*_counts;
                    i = 0;
                    while ((_counts > counts) && (*_counts > *(_counts - 1)))
                      {
                        _counts--;
                        i++;
                      }
                    if (i > 0)
                      {
                        _colors -= 3 * i;
                        h = *_counts;
                        memmove(_counts + 1, _counts, i * 2);
                        memmove(_colors + 3, _colors, i * 3);
                        _colors[0] = pixel[1];
                        _colors[1] = pixel[0];
                        _colors[2] = pixel[2];
                        *_counts   = h;
                      }
                  }
              }
            else
              {
                _colors[0] = pixel[1];
                _colors[1] = pixel[0];
                _colors[2] = pixel[2];
                *_counts   = 1;
              }
        }
      for (x = 0; x < rest; x++)
        if (Read_Targa(f, pixel, 1))
          return(1);
    }
  free(counts);
  g_setpal(colors, 0, g_colors, TRUE);
  return(0);
}

int Pal_Index(int r, int g, int b)
{
  unsigned char *_colors;
  int i, mask;

  mask    = 0;
  r     >>= 2;
  g     >>= 2;
  b     >>= 2;
  while ((mask & 0x40) == 0)
    {
      _colors = colors;
      for (i = 0; i < g_colors; i++)
        if ((r == (_colors[0] | mask)) && (g == (_colors[1] | mask)) &&
            (b == (_colors[2] | mask)))
          return(i);
        else
          _colors += 3;
      mask = (mask << 1) | 1;
      r   |= mask;
      g   |= mask;
      b   |= mask;
    }
  return(0);
}

int Show_Targa(char *filename)
{
  FILE *f;
  int width, height, rest, x, y;
  unsigned char pixel[3];

  if ((f = Open_Targa(filename, &width, &height)) == NULL)
    return(1);
  if (height > g_ysize)
    height = g_ysize;
  if (width > g_xsize)
    {
      rest  = width - g_xsize;
      width = g_xsize;
    }
  else
    rest = 0;
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          if (Read_Targa(f, pixel, 1))
            return(1);
          switch (g_colors)
            {
              case 16:
              case 256:
                g_set(x, y, 1);
                g_set(x, y, Pal_Index(pixel[1], pixel[0], pixel[2]));
                break;
              case 32768:
                g_set(x, y, RGB15(pixel[1], pixel[0], pixel[2]));
                break;
              case 65536:
                g_set(x, y, RGB16(pixel[1], pixel[0], pixel[2]));
                break;
              default:
                g_set(x, y, RGB24(pixel[1], pixel[0], pixel[2]));
            }
        }
      for (x = 0; x < rest; x++)
        if (Read_Targa(f, pixel, 1))
          return(1);
    }
  return(0);
}

static void usage (void)
{
  VESAINFO     vinfo;
  VESAMODEINFO minfo;
  PVESAWORD    pmode;

  fprintf(stderr, "Usage: tgaview [-f<name>] [-m#]\n\n");
  fprintf(stderr, "-f<name>  View file name [test.tga]\n");
  fprintf(stderr, "-m#       Select video mode # [%x]\n", G_MODE_VGA_L);
  if (VesaGetInfo(&vinfo))
    {
      fprintf(stderr, "       Available modes:\n");
      fprintf(stderr, "       ModeNr    xres yres colors\n");
      pmode = vinfo.Modes;
      while (*pmode != 0xFFFF)
        {
          if (VesaGetModeInfo(*pmode, &minfo))
            {
              if ((minfo.ModeAttributes & 0x10) == 0x10)
                {
                  int colors;

                  if (minfo.RedMaskSize != 0)
                    colors = 1 << (minfo.RedMaskSize + minfo.GreenMaskSize +
                                   minfo.BlueMaskSize);
                  else
                    colors = 1 << minfo.NumberOfBitsPerPixel;
                  fprintf(stderr, "%13x%8i%5i %i\n", *pmode, minfo.Width,
                                  minfo.Height, colors);
                }
            }
          else
            fprintf(stderr, "%13x --- VesaGetModeInfo failed ---\n", *pmode);
          pmode++;
        }
      fprintf(stderr, "\n");
    }
  exit (1);
}

int main (int argc, char *argv[])
{
  int rc, video_mode;
  char *filename, c;

  video_mode = G_MODE_VGA_L;
  filename   = "test.tga";
  while ((c = getopt (argc, argv, "f:m:")) != EOF)
    switch (c)
      {
      case 'm':
        if ((sscanf(optarg, "%x", &video_mode) != 1) || (video_mode < 0))
          usage ();
        break;
      case 'f':
        filename = optarg;
        break;
      default:
        usage ();
      }
  if (optind < argc)
    usage ();
  if (!g_mode (video_mode))
    {
      fprintf (stderr, "Cannot switch to graphics mode %i\n", video_mode);
      return (1);
    }
  if (g_colors <= 256)
    rc = Find_Palette(filename);
  else
    rc = 0;
  if (rc == 0)
    rc = Show_Targa(filename);
  getchar();
  g_mode (G_MODE_OFF);
  if (rc)
    {
      fprintf (stderr, "Error reading file: %s\n", filename);
      return (1);
    }
  return (0);
}
