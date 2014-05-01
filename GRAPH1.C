/* graph1.c (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */
/*                                  changes 1993 by Johannes Martin */

#include <sys/hw.h>
#include <vesa.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

#define COL16        16
#define COL256       256
#define COL32k       32768
#define COL64k       65536
#define COL16m       16777216

int g_xsize;
int g_ysize;
int g_colors;
int g_memsize;
int g_32Bit;

int _g_mode;
int _g_clipx0;
int _g_clipx1;
int _g_clipy0;
int _g_clipy1;
int _g_locklevel = 0;
int _g_wwin;
int _g_rwin;
int _g_wwinstat;
int _g_rwinstatx;
int *_g_rwinstat;
int _g_winsize;
int _g_wingran;
int _g_winfact;
int _g_winmask;
int _g_winshift;
int _g_linesize;
int _g_rlinesize;
int _g_wincount;
int _g_memmodel;
int _g_wmode;
int _g_different_winsegs;
int _g_blue_shift;
int _g_green_shift;
int _g_red_shift;
int _g_blue_mask;
int _g_red_mask;
int _g_green_mask;
int _g_col_shr;

CriticalPosition *SplitPositions = NULL;

VESASET _g_setwin;

unsigned char *_g_wmem;
unsigned char *_g_rmem;
unsigned char *_g_vpal;

HMTX _g_hmtxLock = 0;

struct _vinfo _g_vinfo = { 0, 0, NULL, NULL, NULL, NULL };

VGAFUNC_clear    _g_clear;
VGAFUNC_get      _g_get;
VGAFUNC_getimage _g_getimage;
VGAFUNC_hline    _g_hline;
VGAFUNC_putimage _g_putimage;
VGAFUNC_set      _g_set;
VGAFUNC_vline    _g_vline;

VGAFUNC_restore  _g_restore;
VGAFUNC_save     _g_save;

VGAFUNC_getpal   _g_getpal;
VGAFUNC_setpal   _g_setpal;

static int SetFlatFuncs(void)
{
  _g_restore = FLAT_restore;
  _g_save    = FLAT_save;
  switch (g_colors)
    {
      case COL16:
	_g_clear    = FLAT16_clear;
	_g_get      = FLAT16_get;
	_g_getimage = FLAT16_getimage;
	_g_hline    = FLAT16_hline;
	_g_putimage = FLAT16_putimage;
	_g_set      = FLAT16_set;
	_g_vline    = FLAT16_vline;
	_g_getpal   = MEM_getpal;
	_g_setpal   = MEM_setpal;
	return(TRUE);
      case COL256:
	_g_clear    = FLAT256_clear;
	_g_get      = FLAT256_get;
	_g_getimage = FLAT256_getimage;
	_g_hline    = FLAT256_hline;
	_g_putimage = FLAT256_putimage;
	_g_set      = FLAT256_set;
	_g_vline    = FLAT256_vline;
	_g_getpal   = MEM_getpal;
	_g_setpal   = MEM_setpal;
	return(TRUE);
      case COL32k:
      case COL64k:
	_g_clear    = FLAT32k_clear;
	_g_get      = FLAT32k_get;
	_g_getimage = FLAT32k_getimage;
	_g_hline    = FLAT32k_hline;
	_g_putimage = FLAT32k_putimage;
	_g_set      = FLAT32k_set;
	_g_vline    = FLAT32k_vline;
	_g_getpal   = DUMMY_getpal;
	_g_setpal   = DUMMY_setpal;
	return(TRUE);
      case COL16m:
	if (g_32Bit) {
	  _g_clear    = FLAT16m_32Bit_clear;
	  _g_get      = FLAT16m_32Bit_get;
	  _g_getimage = FLAT16m_32Bit_getimage;
	  _g_hline    = FLAT16m_32Bit_hline;
	  _g_putimage = FLAT16m_32Bit_putimage;
	  _g_set      = FLAT16m_32Bit_set;
	  _g_vline    = FLAT16m_32Bit_vline;
	  _g_getpal   = DUMMY_getpal;
	  _g_setpal   = DUMMY_setpal;
	}
	else {
	  _g_clear    = FLAT16m_clear;
	  _g_get      = FLAT16m_get;
	  _g_getimage = FLAT16m_getimage;
	  _g_hline    = FLAT16m_hline;
	  _g_putimage = FLAT16m_putimage;
	  _g_set      = FLAT16m_set;
	  _g_vline    = FLAT16m_vline;
	  _g_getpal   = DUMMY_getpal;
	  _g_setpal   = DUMMY_setpal;
	}
	return(TRUE);
    }
  return(FALSE);
}

/* This function tests if TrueColor-Pixels crosses bank boundaries */
static int TestCritical24BitMode(void)
{
  int b, i, j, offset;

  if (_g_winfact > 1) return 0;
  if (SplitPositions != NULL) free(SplitPositions);
  SplitPositions = (CriticalPosition*)malloc(_g_wincount*sizeof(CriticalPosition));
  if (SplitPositions == NULL) return 0;

  for (i=0; i < _g_wincount; i++) SplitPositions[i].is_critical = 0;

  for (j=0; j < g_ysize; j++)
    {
      offset = j * _g_linesize;
      for (i=0; i < g_xsize; i++)
	{
	  if ((offset & _g_winmask) + 3 > _g_wingran) {
	    b = offset >> _g_winshift;
	    SplitPositions[b].is_critical = (offset & _g_winmask) + 3 - _g_wingran;
	    SplitPositions[b].x           = i;
	    SplitPositions[b].y           = j;
	  }
	  offset += 3;
	}
    }

  for (i=0; i < _g_wincount; i++)
    if (SplitPositions[i].is_critical) return 1;

  return 0;
}

static int SetSegFuncs(void)
{
  if ((_g_wincount == 1) &&
      (((_g_memmodel == 4) && (g_colors == COL256)) ||
       ((_g_memmodel == 6) && (g_colors >= COL32k))))
    {
      int rc = SetFlatFuncs();
      if (g_colors <= COL256)
	{
	  _g_getpal = PORT_getpal;
	  _g_setpal = PORT_setpal;
	}
      _g_setwin(0, 0);
      _g_setwin(1, 0);
      return(rc);
    }
  switch (g_colors)
    {
      case COL16:
	{
	  int i, h;
	  #define wait for (i = 0; i < 10; i++);

	  _portaccess(ATT_IW, ATT_R);
	  _portaccess(SEQ_I, SEQ_D);
	  _portaccess(GRA_I, GRA_D);
	  _outp16(GRA_I, 0x0F01);
	  _outp16(GRA_I, 0x0205);
	  _outp16(GRA_I, (_g_wmode << 11) | 3);
	  _inp8(IS1_R);
	  wait;
	  _outp8(ATT_IW, 0x10);
	  wait;
	  _outp8(ATT_IW, _inp8(ATT_R) | 0x80);
	  wait;
	  _outp8(ATT_IW, 0x14);
	  wait;
	  _outp8(ATT_IW, 0);
	  for (h = 0; h < 16; h++)
	    {
	      wait;
	      _outp8(ATT_IW, h);
	      wait;
	      _outp8(ATT_IW, h);
	    }
	  wait;
	  _outp8(ATT_IW, 0x20);
	  _g_clear    = SEG16_clear;
	  _g_get      = SEG16_get;
	  _g_getimage = SEG16_getimage;
	  _g_hline    = SEG16_hline;
	  _g_putimage = SEG16_putimage;
	  _g_set      = SEG16_set;
	  _g_vline    = SEG16_vline;
	  _g_restore  = SEG16_restore;
	  _g_save     = SEG16_save;
	  _g_getpal   = PORT_getpal;
	  _g_setpal   = PORT_setpal;
	  return(TRUE);
	}
      case COL256:
	if (_g_memmodel == 5)
	  {
	    _portaccess(SEQ_I, SEQ_D);
	    _portaccess(GRA_I, GRA_D);
	    _g_clear    = ALT256_clear;
	    _g_get      = ALT256_get;
	    _g_getimage = ALT256_getimage;
	    _g_hline    = ALT256_hline;
	    _g_putimage = ALT256_putimage;
	    _g_set      = ALT256_set;
	    _g_vline    = ALT256_vline;
	    _g_restore  = ALT_restore;
	    _g_save     = ALT_save;
	  }
	else
	  {
	    _g_clear    = SEG256_clear;
	    _g_get      = SEG256_get;
	    _g_getimage = SEG256_getimage;
	    _g_putimage = SEG256_putimage;
	    _g_restore  = SEG_restore;
	    _g_save     = SEG_save;
	    _g_getpal = PORT_getpal;
	    _g_setpal = PORT_setpal;
	    if (_g_different_winsegs)
	      {
		_g_hline    = SEG256_hline_2;
		_g_set      = SEG256_set_2;
		_g_vline    = SEG256_vline_2;
	      }
	    else
	      {
		_g_hline    = SEG256_hline_1;
		_g_set      = SEG256_set_1;
		_g_vline    = SEG256_vline_1;
	      }
	  }
	return(TRUE);
      case COL32k:
      case COL64k:
	_g_clear    = SEG32k_clear;
	_g_get      = SEG32k_get;
	_g_getimage = SEG32k_getimage;
	_g_putimage = SEG32k_putimage;
	_g_restore  = SEG_restore;
	_g_save     = SEG_save;
	_g_getpal   = DUMMY_getpal;
	_g_setpal   = DUMMY_setpal;
	if (_g_different_winsegs)
	  {
	    _g_hline = SEG32k_hline_2;
	    _g_set   = SEG32k_set_2;
	    _g_vline = SEG32k_vline_2;
	  }
	else
	  {
	    _g_hline = SEG32k_hline_1;
	    _g_set   = SEG32k_set_1;
	    _g_vline = SEG32k_vline_1;
	  }
	return(TRUE);
      case COL16m:
	if (g_32Bit) {
	  _g_clear    = SEG16m_32Bit_clear;
	  _g_get      = SEG16m_32Bit_get;
	  _g_getimage = SEG16m_32Bit_getimage;
	  _g_putimage = SEG16m_32Bit_putimage;
	  _g_restore  = SEG_restore;
	  _g_save     = SEG_save;
	  _g_getpal   = DUMMY_getpal;
	  _g_setpal   = DUMMY_setpal;
	  if (_g_different_winsegs)
	    {
	      _g_hline = SEG16m_32Bit_hline_2;
	      _g_set   = SEG16m_32Bit_set_2;
	      _g_vline = SEG16m_32Bit_vline_2;
	    }
	  else
	    {
	      _g_hline = SEG16m_32Bit_hline_1;
	      _g_set   = SEG16m_32Bit_set_1;
	      _g_vline = SEG16m_32Bit_vline_1;
	    }
	  }
	else {
	  _g_clear    = SEG16m_clear;
	  _g_getimage = SEG16m_getimage;
	  _g_putimage = SEG16m_putimage;
	  _g_restore  = SEG_restore;
	  _g_save     = SEG_save;
	  _g_getpal   = DUMMY_getpal;
	  _g_setpal   = DUMMY_setpal;
	  if (TestCritical24BitMode()) {
	    _g_get      = SEG16m_split_get;
	    if (_g_different_winsegs) {
	      _g_hline    = SEG16m_split_hline_2;
	      _g_set      = SEG16m_split_set_2;
	      _g_vline    = SEG16m_vline_2;
	    }
	    else {
	      _g_hline    = SEG16m_split_hline_1;
	      _g_set      = SEG16m_split_set_1;
	      _g_vline    = SEG16m_vline_1;
	    }
	  }
	  else {
	    _g_get      = SEG16m_get;
	    if (_g_different_winsegs) {
	      _g_hline    = SEG16m_hline_2;
	      _g_set      = SEG16m_set_2;
	      _g_vline    = SEG16m_vline_2;
	    }
	    else {
	      _g_hline    = SEG16m_hline_1;
	      _g_set      = SEG16m_set_1;
	      _g_vline    = SEG16m_vline_1;
	    }
	  }
	}
	return(TRUE);
    }
  return(FALSE);
}

static void g_switch(int Restore)
{
  DosRequestMutexSem(_g_hmtxLock, -1);
  DosEnterCritSec();
  if (Restore)
    {
      if (_g_vinfo.wmem == NULL)
	{
	  VESAMODEINFO ModeInfo;

	  VesaGetModeInfo(_g_mode, &ModeInfo);
          _g_vinfo.wmem = _g_wwin ? ModeInfo.WindowBStart : ModeInfo.WindowAStart;
          _g_vinfo.rmem = _g_rwin ? ModeInfo.WindowBStart : ModeInfo.WindowAStart;
        }
      SetSegFuncs();
      _g_wmem     = _g_vinfo.wmem;
      _g_rmem     = _g_vinfo.rmem;
      _g_linesize = _g_vinfo.lsize;
      _g_restore(_g_vinfo.bmem);
      if ((g_colors == COL16) || (g_colors == COL256))
        _g_setpal(_g_vinfo.bpal, 0, g_colors, 0);
      if (_g_vinfo.level & 2)
        {
          SetFlatFuncs();
          _g_wmem     = _g_vinfo.vmem;
          _g_rmem     = _g_vinfo.vmem;
          _g_vpal     = _g_vinfo.vpal;
          _g_linesize = _g_rlinesize;
	}
      _g_vinfo.level &= 0xFE;
    }
  else
    {
      if (_g_vinfo.level & 2)
        {
          SetSegFuncs();
          _g_wmem     = _g_vinfo.wmem;
          _g_rmem     = _g_vinfo.rmem;
	  _g_linesize = _g_vinfo.lsize;
	}
      _g_save(_g_vinfo.bmem);
      if ((g_colors == COL16) || (g_colors == COL256))
	_g_getpal(_g_vinfo.bpal, 0, g_colors);
      SetFlatFuncs();
      if (_g_vinfo.level & 2)
	{
	  _g_wmem = _g_vinfo.vmem;
	  _g_rmem = _g_vinfo.vmem;
	  _g_vpal = _g_vinfo.vpal;
	}
      else
	{
	  _g_wmem = _g_vinfo.bmem;
	  _g_rmem = _g_vinfo.bmem;
	  _g_vpal = _g_vinfo.bpal;
	}
      _g_linesize = _g_rlinesize;
      _g_vinfo.level |= 1;
    }
  DosExitCritSec();
  DosReleaseMutexSem(_g_hmtxLock);
}

static void InitPalette(void)
{
  const char palette[256][3]
   = { {  0,  0,  0 }, {  0,  0, 42 }, {  0, 42,  0 }, {  0, 42, 42 },
       { 42,  0,  0 }, { 42,  0, 42 }, { 42, 21,  0 }, { 42, 42, 42 },
       { 21, 21, 21 }, { 21, 21, 63 }, { 21, 63, 21 }, { 21, 63, 63 },
       { 63, 21, 21 }, { 63, 21, 63 }, { 63, 63, 21 }, { 63, 63, 63 },
       {  0,  0,  0 }, {  5,  5,  5 }, {  8,  8,  8 }, { 11, 11, 11 },
       { 14, 14, 14 }, { 17, 17, 17 }, { 20, 20, 20 }, { 24, 24, 24 },
       { 28, 28, 28 }, { 32, 32, 32 }, { 36, 36, 36 }, { 40, 40, 40 },
       { 45, 45, 45 }, { 50, 50, 50 }, { 56, 56, 56 }, { 63, 63, 63 },
       {  0,  0, 63 }, { 16,  0, 63 }, { 31,  0, 63 }, { 47,  0, 63 },
       { 63,  0, 63 }, { 63,  0, 47 }, { 63,  0, 31 }, { 63,  0, 16 },
       { 63,  0,  0 }, { 63, 16,  0 }, { 63, 31,  0 }, { 63, 47,  0 },
       { 63, 63,  0 }, { 47, 63,  0 }, { 31, 63,  0 }, { 16, 63,  0 },
       {  0, 63,  0 }, {  0, 63, 16 }, {  0, 63, 31 }, {  0, 63, 47 },
       {  0, 63, 63 }, {  0, 47, 63 }, {  0, 31, 63 }, {  0, 16, 63 },
       { 31, 31, 63 }, { 39, 31, 63 }, { 47, 31, 63 }, { 55, 31, 63 },
       { 63, 31, 63 }, { 63, 31, 55 }, { 63, 31, 47 }, { 63, 31, 39 },
       { 63, 31, 31 }, { 63, 39, 31 }, { 63, 47, 31 }, { 63, 55, 31 },
       { 63, 63, 31 }, { 55, 63, 31 }, { 47, 63, 31 }, { 39, 63, 31 },
       { 31, 63, 31 }, { 31, 63, 39 }, { 31, 63, 47 }, { 31, 63, 55 },
       { 31, 63, 63 }, { 31, 55, 63 }, { 31, 47, 63 }, { 31, 39, 63 },
       { 45, 45, 63 }, { 49, 45, 63 }, { 54, 45, 63 }, { 58, 45, 63 },
       { 63, 45, 63 }, { 63, 45, 58 }, { 63, 45, 54 }, { 63, 45, 49 },
       { 63, 45, 45 }, { 63, 49, 45 }, { 63, 54, 45 }, { 63, 58, 45 },
       { 63, 63, 45 }, { 58, 63, 45 }, { 54, 63, 45 }, { 49, 63, 45 },
       { 45, 63, 45 }, { 45, 63, 49 }, { 45, 63, 54 }, { 45, 63, 58 },
       { 45, 63, 63 }, { 45, 58, 63 }, { 45, 54, 63 }, { 45, 49, 63 },
       {  0,  0, 28 }, {  7,  0, 28 }, { 14,  0, 28 }, { 21,  0, 28 },
       { 28,  0, 28 }, { 28,  0, 21 }, { 28,  0, 14 }, { 28,  0,  7 },
       { 28,  0,  0 }, { 28,  7,  0 }, { 28, 14,  0 }, { 28, 21,  0 },
       { 28, 29,  0 }, { 21, 28,  0 }, { 14, 28,  0 }, {  7, 28,  0 },
       {  0, 28,  0 }, {  0, 28,  7 }, {  0, 28, 14 }, {  0, 28, 21 },
       {  0, 28, 28 }, {  0, 21, 28 }, {  0, 14, 28 }, {  0,  7, 28 },
       { 14, 14, 28 }, { 17, 14, 28 }, { 21, 14, 28 }, { 24, 14, 28 },
       { 28, 14, 28 }, { 28, 14, 24 }, { 28, 14, 21 }, { 28, 14, 17 },
       { 28, 14, 14 }, { 28, 17, 14 }, { 28, 21, 14 }, { 28, 24, 14 },
       { 28, 28, 14 }, { 24, 28, 14 }, { 21, 28, 14 }, { 17, 28, 14 },
       { 14, 28, 14 }, { 14, 28, 17 }, { 14, 28, 21 }, { 14, 28, 24 },
       { 14, 28, 28 }, { 14, 24, 28 }, { 14, 21, 28 }, { 14, 17, 28 },
       { 20, 20, 28 }, { 22, 20, 28 }, { 24, 20, 28 }, { 26, 20, 28 },
       { 28, 20, 28 }, { 28, 20, 26 }, { 28, 20, 24 }, { 28, 20, 22 },
       { 28, 20, 20 }, { 28, 22, 20 }, { 28, 24, 20 }, { 28, 26, 20 },
       { 28, 28, 20 }, { 26, 28, 20 }, { 24, 28, 20 }, { 22, 28, 20 },
       { 20, 28, 20 }, { 20, 28, 22 }, { 20, 28, 24 }, { 20, 28, 26 },
       { 20, 28, 28 }, { 20, 26, 28 }, { 20, 24, 28 }, { 20, 22, 28 },
       {  0,  0, 16 }, {  4,  0, 16 }, {  8,  0, 16 }, { 12,  0, 16 },
       { 16,  0, 16 }, { 16,  0, 12 }, { 16,  0,  8 }, { 16,  0,  4 },
       { 16,  0,  0 }, { 16,  4,  0 }, { 16,  8,  0 }, { 16, 12,  0 },
       { 16, 16,  0 }, { 12, 16,  0 }, {  8, 16,  0 }, {  4, 16,  0 },
       {  0, 16,  0 }, {  0, 16,  4 }, {  0, 16,  8 }, {  0, 16, 12 },
       {  0, 16, 16 }, {  0, 12, 16 }, {  0,  8, 16 }, {  0,  4, 16 },
       {  8,  8, 16 }, { 10,  8, 16 }, { 12,  8, 16 }, { 14,  8, 16 },
       { 16,  8, 16 }, { 16,  8, 14 }, { 16,  8, 12 }, { 16,  8, 10 },
       { 16,  8,  8 }, { 16, 10,  8 }, { 16, 12,  8 }, { 16, 14,  8 },
       { 16, 16,  8 }, { 14, 16,  8 }, { 12, 16,  8 }, { 10, 16,  8 },
       {  8, 16,  8 }, {  8, 16, 10 }, {  8, 16, 12 }, {  8, 16, 14 },
       {  8, 16, 16 }, {  8, 14, 16 }, {  8, 12, 16 }, {  8, 10, 16 },
       { 11, 11, 16 }, { 12, 11, 16 }, { 13, 11, 16 }, { 15, 11, 16 },
       { 16, 11, 16 }, { 16, 11, 15 }, { 16, 11, 13 }, { 16, 11, 12 },
       { 16, 11, 11 }, { 16, 12, 11 }, { 16, 13, 11 }, { 16, 15, 11 },
       { 16, 16, 11 }, { 15, 16, 11 }, { 13, 16, 11 }, { 12, 16, 11 },
       { 11, 16, 11 }, { 11, 16, 12 }, { 11, 16, 13 }, { 11, 16, 15 },
       { 11, 16, 16 }, { 11, 15, 16 }, { 11, 13, 16 }, { 11, 12, 16 },
       {  0,  0,  0 }, {  0,  0,  0 }, {  0,  0,  0 }, {  0,  0,  0 },
       {  0,  0,  0 }, {  0,  0,  0 }, {  0,  0,  0 }, {  0,  0,  0 } };

  g_setpal(&palette[0][0], 0, g_colors, TRUE);
}

static int prep_switch(void)
{
  if (((g_colors == COL16) || (g_colors == COL256)) &&
      ((_g_vinfo.bpal = malloc(g_colors * 3)) == NULL))
    return(FALSE);
  if ((_g_vinfo.bmem = malloc(g_memsize)) == NULL)
    {
      free(_g_vinfo.bpal);
      return(FALSE);
    }
  if (DosCreateMutexSem(NULL, &_g_hmtxLock, 0, 0) != 0)
    return(FALSE);
  if (!VesaLockScreen(0))
    {
      _g_vinfo.level = 1;
      _g_wmem        = _g_vinfo.bmem;
      _g_rmem        = _g_vinfo.bmem;
      _g_linesize    = _g_rlinesize;
      SetFlatFuncs();
      g_clear(0);
    }
  else
    VesaUnlockScreen();
  return(TRUE);
}

static void end_switch(void)
{
  VesaSetSwitchFunc(NULL);
  if (_g_hmtxLock != 0)
    {
      DosCloseMutexSem(_g_hmtxLock);
      _g_hmtxLock = 0;
    }
  if (_g_vinfo.bpal != NULL)
    {
      free(_g_vinfo.bpal);
      _g_vinfo.bpal = NULL;
    }
  if (_g_vinfo.bmem != NULL)
    {
      free(_g_vinfo.bmem);
      _g_vinfo.bmem = NULL;
    }
}

static int TestMemModel(void)
{
  switch (_g_memmodel)
    {
      case 3: /* normal 16 color modes */
	return(g_colors == COL16);
      case 4: /* normal 256 color modes */
      case 5: /* alternative 256 color modes */
	return(g_colors == COL256);
      case 6: /* hi- and truecolor modes */
	return((g_colors == COL32k) ||
	       (g_colors == COL64k) ||
	       (g_colors == COL16m));
      case 0xf4: /* special 256 color mode for vesa_pm */
	if ((g_colors == COL256) && (_g_wincount == 1))
	  {
	    _g_memmodel = 4;
	    _g_wmem    += (g_ysize - 1) * _g_linesize;
	    _g_rmem    += (g_ysize - 1) * _g_linesize;
	    _g_linesize = - _g_linesize;
	    return(TRUE);
	  }
	break;
      case 0xf6: /* special 16m color mode for vesa_pm */
	if ((g_colors == COL16m) && (_g_wincount == 1))
	  {
	    _g_memmodel = 6;
	    _g_wmem    += (g_ysize - 1) * _g_linesize;
	    _g_rmem    += (g_ysize - 1) * _g_linesize;
	    _g_linesize = - _g_linesize;
	    return(TRUE);
	  }
	break;
    }
  return(FALSE);
}

int g_init(void)
{
  return(VesaInitialize());
}

static int get_exp2(int x)
{
  int i = 0;
  while (!(x & 1)) {
    i++;
    x >>= 1;
  }
  return i;
}

int g_mode(int mode)
{
  VESAMODEINFO ModeInfo;
  int colmask[9] = {0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

  if (SplitPositions) free(SplitPositions);
  if (_g_vinfo.level & 2)
    g_real(FALSE, FALSE);
  if (_osmode == OS2_MODE)
    end_switch();
  if ((mode == GTEXT)    ||
      !VesaSetMode(mode) ||
      !VesaGetModeInfo(mode, &ModeInfo))
    {
      VesaSetMode(_g_mode = GTEXT);
      return(FALSE);
    }
  _portaccess(IS1_R,  IS1_R);
  g_xsize      = ModeInfo.Width;
  g_ysize      = ModeInfo.Height;
  if (ModeInfo.RedMaskSize + ModeInfo.GreenMaskSize + ModeInfo.BlueMaskSize > 0)
    g_colors   = 1 << (ModeInfo.RedMaskSize + ModeInfo.GreenMaskSize + ModeInfo.BlueMaskSize);
  else
    g_colors   = 1 << ModeInfo.NumberOfBitsPerPixel;

  g_32Bit      = (ModeInfo.NumberOfBitsPerPixel == 32);
  g_memsize    = ModeInfo.Width * ModeInfo.Height * ModeInfo.NumberOfBitsPerPixel / 8;

  _g_mode      = mode;
  _g_clipx0    = 0;
  _g_clipy0    = 0;
  _g_clipx1    = g_xsize - 1;
  _g_clipy1    = g_ysize - 1;

  _g_wwin      = (ModeInfo.WindowAAttributes & 5) == 5 ? 0 : 1;
  _g_rwin      = (ModeInfo.WindowBAttributes & 3) == 3 ? 1 : 0;
  if (_g_rwin == _g_wwin) _g_rwinstat = &_g_wwinstat;
  else _g_rwinstat = &_g_rwinstatx;
  _g_wwinstat  = -1;
  *_g_rwinstat = -1;
  _g_winsize   = ModeInfo.WindowSize * 1024;
  _g_wingran   = ModeInfo.WindowGranularity * 1024;
  _g_winmask   = _g_wingran - 1;
  _g_winshift  = get_exp2(_g_wingran);
  _g_winfact   = _g_winsize / _g_wingran;
  _g_linesize  = ModeInfo.BytesPerScanline;
  _g_rlinesize = g_xsize * ModeInfo.NumberOfBitsPerPixel / 8;
  _g_wincount  = g_ysize / (_g_winsize / _g_linesize)
	      + (g_ysize % (_g_winsize / _g_linesize) != 0);
  _g_memmodel  = ModeInfo.MemoryModelType;
  _g_wmem      = _g_wwin ? ModeInfo.WindowBStart : ModeInfo.WindowAStart;
  _g_rmem      = _g_rwin ? ModeInfo.WindowBStart : ModeInfo.WindowAStart;
  _g_different_winsegs = _g_wmem != _g_rmem;
  _g_setwin    = ModeInfo.SetWindowFunc;
  _g_blue_shift = ModeInfo.BlueMaskPosition;
  _g_green_shift = ModeInfo.GreenMaskPosition;
  _g_red_shift = ModeInfo.RedMaskPosition;

  if (ModeInfo.BlueMaskSize > 8 || ModeInfo.GreenMaskSize > 8 ||
      ModeInfo.RedMaskSize > 8) return FALSE;

  _g_blue_mask  = colmask[ModeInfo.BlueMaskSize];
  _g_green_mask = colmask[ModeInfo.GreenMaskSize];
  _g_red_mask   = colmask[ModeInfo.RedMaskSize];

  if (g_colors > COL256 && g_colors < COL16m)
    {
      _g_blue_shift  -= 8 - ModeInfo.BlueMaskSize;
      _g_green_shift -= 8 - ModeInfo.GreenMaskSize;
      _g_red_shift   -= 8 - ModeInfo.RedMaskSize;

      if (_g_blue_shift < 0) {
	_g_col_shr      = -_g_blue_shift;
	_g_blue_shift   = 0;
	_g_green_shift += _g_col_shr;
	_g_red_shift   += _g_col_shr;
      }

      if (_g_green_shift < 0) {
	_g_col_shr      = -_g_green_shift;
	_g_green_shift  = 0;
	_g_blue_shift  += _g_col_shr;
	_g_red_shift   += _g_col_shr;
      }

      if (_g_red_shift < 0) {
	_g_col_shr      = -_g_red_shift;
	_g_red_shift    = 0;
	_g_blue_shift  += _g_col_shr;
	_g_green_shift += _g_col_shr;
      }
    }
  else _g_col_shr = 0;

  if (!TestMemModel() || !SetSegFuncs())
    {
      VesaSetMode(_g_mode = GTEXT);
      return(FALSE);
    }

  _g_vinfo.level = 0;
  _g_vinfo.lsize = _g_linesize;
  _g_vinfo.wmem  = _g_wmem;
  _g_vinfo.rmem  = _g_rmem;

  if ((_osmode == OS2_MODE) && !prep_switch())
    {
      VesaSetMode(_g_mode = GTEXT);
      return(FALSE);
    }
  if (!VesaSetSwitchFunc(g_switch))
    {
      VesaSetMode(_g_mode = GTEXT);
      return(FALSE);
    }
  if ((g_colors == COL16) || (g_colors == COL256))
    InitPalette();
  g_wmode(G_NORM);
  return(TRUE);
}

int g_modeset(int mode, int flag)
{
  return(mode);
}

void g_wmode(int mode)
{
  _g_wmode = mode;
  if ((g_colors == COL16) && ((_g_vinfo.level & 1) == 0))
    _outp16(GRA_I, (mode << 11) | 3);
}

int g_virtual2(int copy_buf, int copy_pal, void *buf, void *pal)
{
  if (_g_vinfo.level & 2)
    return(FALSE);
  if (pal != NULL)
    _g_vinfo.vpal = pal;
  else
    if (((g_colors == COL16) || (g_colors == COL256)) &&
	((_g_vinfo.vpal = malloc(g_colors * 3)) == NULL))
      return(FALSE);
  if (buf != NULL)
    _g_vinfo.vmem = buf;
  else
    if ((_g_vinfo.vmem = malloc(g_memsize)) == NULL)
      {
	if (pal == NULL)
	  free(_g_vinfo.vpal);
	return(FALSE);
      }
  if ((_osmode == OS2_MODE) &&
      (_g_locklevel == 0))
    DosRequestMutexSem(_g_hmtxLock, -1);
  if (copy_buf)
    _g_save(_g_vinfo.vmem);
  if (copy_pal && ((g_colors == COL16) || (g_colors == COL256)))
    _g_getpal(_g_vinfo.vpal, 0, g_colors);
  SetFlatFuncs();
  _g_wmem     = _g_vinfo.vmem;
  _g_rmem     = _g_vinfo.vmem;
  _g_vpal     = _g_vinfo.vpal;
  _g_linesize = _g_rlinesize;
  _g_vinfo.level |= 2;
  if (_osmode == OS2_MODE)
    DosReleaseMutexSem(_g_hmtxLock);
  return(TRUE);
}

int g_virtual(int copy_buf, int copy_pal)
{
  return(g_virtual2(copy_buf, copy_pal, NULL, NULL));
}

void g_refresh(int x0, int y0, int x1, int y1, int x2, int y2, int pal)
{
  if (((_g_vinfo.level & 2) == 0)        ||
      (x0 >= g_xsize) || (y0 >= g_ysize) ||
      (x2 >= g_xsize) || (y2 >= g_ysize))
    return;
  if (x1 >= g_xsize)
    x1 = g_xsize - x0;
  else
    x1 -= x0 - 1;
  if (y1 >= g_ysize)
    y1 = g_ysize - y0;
  else
    y1 -= y0 - 1;
  if (x2 + x1 > g_xsize)
    x1 = g_xsize - x2;
  if (y2 + y1 > g_ysize)
    y1 = g_ysize - y2;
  if (_osmode == OS2_MODE)
    DosRequestMutexSem(_g_hmtxLock, -1);
  if (_g_vinfo.level & 1)
    {
      _g_wmem = _g_vinfo.bmem;
      _g_rmem = _g_vinfo.bmem;
      _g_vpal = _g_vinfo.bpal;
    }
  else
    {
      SetSegFuncs();
      _g_wmem     = _g_vinfo.wmem;
      _g_rmem     = _g_vinfo.rmem;
      _g_linesize = _g_vinfo.lsize;
    }
  _g_putimage(x2, y2, g_xsize, y1, y0 * g_xsize + x0, x1, g_ysize, _g_vinfo.vmem);
  if (pal && ((g_colors == COL16) || (g_colors == COL256)))
    _g_setpal(_g_vinfo.vpal, 0, g_colors, 0);
  SetFlatFuncs();
  _g_wmem     = _g_vinfo.vmem;
  _g_rmem     = _g_vinfo.vmem;
  _g_vpal     = _g_vinfo.vpal;
  _g_linesize = _g_rlinesize;
  if (_osmode == OS2_MODE)
    {
      DosReleaseMutexSem(_g_hmtxLock);
      VesaSetModified();
    }
}

void g_real2(int copy_buf, int copy_pal, void **buf, void **pal)
{
  if (!(_g_vinfo.level & 2))
    return;
  if (_osmode == OS2_MODE)
    DosRequestMutexSem(_g_hmtxLock, -1);
  if (_g_vinfo.level & 1)
    {
      _g_wmem = _g_vinfo.bmem;
      _g_rmem = _g_vinfo.bmem;
      _g_vpal = _g_vinfo.bpal;
    }
  else
    {
      SetSegFuncs();
      _g_wmem     = _g_vinfo.wmem;
      _g_rmem     = _g_vinfo.rmem;
      _g_linesize = _g_vinfo.lsize;
    }
  _g_vinfo.level &= 0xFD;
  if (copy_buf)
    _g_restore(_g_vinfo.vmem);
  if (copy_pal && ((g_colors == COL16) || (g_colors == COL256)))
    _g_setpal(_g_vinfo.vpal, 0, g_colors, 0);
  if (pal != NULL)
    *pal = _g_vinfo.vpal;
  else
    free(_g_vinfo.vpal);
  if (buf != NULL)
    *buf = _g_vinfo.vmem;
  else
    free(_g_vinfo.vmem);
  if ((_osmode == OS2_MODE) &&
      (_g_locklevel == 0))
    {
      DosReleaseMutexSem(_g_hmtxLock);
      VesaSetModified();
    }
}


void g_real(int copy_buf, int copy_pal)
{
  g_real2(copy_buf, copy_pal, NULL, NULL);
}
