/* graph2.h (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */
/*                                  changes 1993 by Johannes Martin */

extern int _g_mode;
extern int _g_clipx0;
extern int _g_clipy0;
extern int _g_clipx1;
extern int _g_clipy1;
extern int _g_locklevel;
extern int _g_wmode;

extern struct _vinfo
  {
    int level;    /* Bit 0 = 1: program running in background (OS/2 only) */
                  /* Bit 1 = 1: virtual screen active                     */
    int  lsize;   /* size of a single scanline in physical video buffer   */
    void *wmem;   /* pointer to physical video buffer (write access)      */
    void *rmem;   /* pointer to physical video buffer (read  access)      */
    void *bmem;   /* virtual video buffer if program in bg (OS/2 only)    */
    void *bpal;   /* virtual palette if program in background (OS/2 only) */
    void *vmem;   /* virtual video buffer for virtual screen              */
    void *vpal;   /* virtual palette for virtual screen                   */
  } _g_vinfo;

#include <stdlib.h>

#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS

#include <os2emx.h>

extern HMTX _g_hmtxLock;

/* (un)lock screen if - running in OS/2-mode    */
/*                    - screen not yet locked   */
/*                    - virtual screen inactive */
/* GLOCK     - lock screen                      */
/* GUNLOCK   - unlock screen when modified      */
/* GUNLOCKNM - unlock screen when not modified  */

#define GLOCK                                        \
    if ((_osmode == OS2_MODE) &&                     \
        (_g_locklevel == 0)   &&                     \
        ((_g_vinfo.level & 2) == 0))                 \
      DosRequestMutexSem(_g_hmtxLock, -1);

#define GUNLOCK                                      \
    if ((_osmode == OS2_MODE) &&                     \
        (_g_locklevel == 0)   &&                     \
        ((_g_vinfo.level & 2) == 0))                 \
      {                                              \
        DosReleaseMutexSem(_g_hmtxLock);             \
        VesaSetModified();                           \
      }

#define GUNLOCKNM                                    \
    if ((_osmode == OS2_MODE) &&                     \
        (_g_locklevel == 0)   &&                     \
        ((_g_vinfo.level & 2) == 0))                 \
      DosReleaseMutexSem(_g_hmtxLock);
