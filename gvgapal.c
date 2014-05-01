/* gvgapal.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */
/*                                   changes 1993 by Johannes Martin */

#include <string.h>
#include <sys/hw.h>
#include <vesa.h>
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"
#include "vgaports.h"

void PORT_setpal (const char *pal, int first, int n, int wait_flag)
{
  VesaSetPalette(first, n, pal, wait_flag);
}

void PORT_getpal(char *pal, int first, int n)
{
  VesaGetPalette(first, n, pal);
}

void MEM_setpal (const char *pal, int first, int n, int wait_flag)
{
  memcpy(_g_vpal + 3 * first, pal, 3 * n);
}

void MEM_getpal(char *pal, int first, int n)
{
  memcpy(pal, _g_vpal + 3 * first, 3 * n);
}

void DUMMY_setpal (const char *pal, int first, int n, int wait_flag)
{
}

void DUMMY_getpal(char *pal, int first, int n)
{
}

void g_setpal (const char *pal, int first, int n, int wait_flag)
{
  if (first + n > g_colors)
    n = g_colors - first;
  GLOCK;
  _g_setpal(pal, first, n, wait_flag);
  GUNLOCKNM;
}

void g_getpal(char *pal, int first, int n)
{
  if (first + n > g_colors)
    n = g_colors - first;
  GLOCK;
  _g_getpal(pal, first, n);
  GUNLOCKNM;
}
