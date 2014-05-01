/* glock.c (emx/gcc) --- Copyright (c) 1993 Johannes Martin */

#include <vesa.h>

#include "jmgraph.h"
#include "graph2.h"

void g_lock (void)
{
  if ((_osmode == OS2_MODE) &&
      (_g_locklevel++ == 0) &&
      ((_g_vinfo.level & 2) == 0))
    DosRequestMutexSem(_g_hmtxLock, -1);
}

void g_unlockall (int modified)
{
  if (_g_locklevel != 0)
    {
      _g_locklevel = 1;
      g_unlock (modified);
    }
}

void g_unlock (int modified)
{
  if ((_osmode == OS2_MODE) &&
      (_g_locklevel > 0)    &&
      (--_g_locklevel == 0) &&
      ((_g_vinfo.level & 2) == 0))
    {
      DosReleaseMutexSem(_g_hmtxLock);
      if (modified)
        VesaSetModified();
    }
}
