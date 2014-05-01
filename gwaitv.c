/* gwaitv.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */

#include <sys/hw.h>
#include <vesa.h>

#include "jmgraph.h"
#include "graph2.h"
#include "vgaports.h"

void g_waitv (void)
{
  GLOCK;
  _wait01 (IS1_R, 0x08);
  GUNLOCKNM;
}
