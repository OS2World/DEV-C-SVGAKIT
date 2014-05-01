/* ghline.c (emx/gcc) --- Copyright (c) 1993 Johannes Martin */
#include <string.h>

#include <sys/hw.h>
#include <vesa.h>

#include "vgaports.h"
#include "jmgraph.h"
#include "graph2.h"
#include "graph3.h"

void FLAT16_hline(int y, int x0, int x1, int color)
{
  int offset0, offset1;
  int size, i;

  offset0  = y * _g_linesize >> 2;
  offset1  = offset0 + (x1 << 3);
  offset0 += x0 >> 3;
  size     = g_memsize >> 2;

  if (offset0 == offset1)
    {
      int mask = (0xFF >> (x0 & 7)) & (0xFF << (7 - (x1 & 7)));

      switch (_g_wmode)
	{
	  case G_NORM:
	    for (i = 0; i < 4; i++)
	      {
		if (color & 1)
		  _g_wmem[offset0] |= mask;
		else
		  _g_wmem[offset0] &= ~mask;
		color >>= 1;
		offset0 += size;
	      }
	    break;
	  case G_AND:
	    for (i = 0; i < 4; i++)
	      {
		if ((color & 1) == 0)
		  _g_wmem[offset0] &= ~mask;
		color >>= 1;
		offset0 += size;
	      }
	    break;
	  case G_OR:
	    for (i = 0; i < 4; i++)
	      {
		if (color & 1)
		  _g_wmem[offset0] |= mask;
		color >>= 1;
		offset0 += size;
	      }
	    break;
	  case G_XOR:
	    for (i = 0; i < 4; i++)
	      {
		if (color & 1)
		  _g_wmem[offset0] ^= mask;
		color >>= 1;
		offset0 += size;
	      }
	    break;
	}
    }
  else
    {
      int mask1 = 0xFF >> (x0 & 7);
      int mask2 = 0xFF << (7 - (x1 & 7));

      switch (_g_wmode)
	{
	  case G_NORM:
	    for (i = 0; i < 4; i++)
	      {
		if (color & 1)
		  {
		    _g_wmem[offset0] |= mask1;
		    offset0++;
		    memset(_g_wmem + offset0, 0xFF, offset1 - offset0);
		    _g_wmem[offset1] |= mask2;
		  }
		else
		  {
		    _g_wmem[offset0] &= ~mask1;
		    offset0++;
		    memset(_g_wmem + offset0, 0, offset1 - offset0);
		    _g_wmem[offset1] &= ~mask2;
		  }
		color >>= 1;
		offset0 += size - 1;
		offset1 += size;
	      }
	    break;
	  case G_AND:
	    for (i = 0; i < 4; i++)
	      {
		if (color & 1)
		  offset0++;
		else
		  {
		    _g_wmem[offset0] &= ~mask1;
		    offset0++;
		    memset(_g_wmem + offset0, 0, offset1 - offset0);
		    _g_wmem[offset1] &= ~mask2;
		  }
		color >>= 1;
		offset0 += size - 1;
		offset1 += size;
	      }
	    break;
	  case G_OR:
	    for (i = 0; i < 4; i++)
	      {
		if (color & 1)
		  {
		    _g_wmem[offset0] |= mask1;
		    offset0++;
		    memset(_g_wmem + offset0, 0xFF, offset1 - offset0);
		    _g_wmem[offset1] |= mask2;
		  }
		else
		  offset0++;
		color >>= 1;
		offset0 += size - 1;
		offset1 += size;
	      }
	    break;
	  case G_XOR:
	    for (i = 0; i < 4; i++)
	      {
		if (color & 1)
		  {
		    _g_wmem[offset0] ^= mask1;
		    offset0++;
		    memxorcpy1(_g_wmem + offset0, 0xFF, offset1 - offset0);
		    _g_wmem[offset1] ^= mask2;
		  }
		else
		  offset0++;
		color >>= 1;
		offset0 += size - 1;
		offset1 += size;
	      }
	    break;
	}
    }
}

void FLAT256_hline(int y, int x0, int x1, int color)
{
  switch (_g_wmode)
    {
      case G_NORM:
	memset(_g_wmem + y * _g_linesize + x0, color, x1 - x0 + 1);
	break;
      case G_AND:
	memandcpy1(_g_wmem + y * _g_linesize + x0, color, x1 - x0 + 1);
	break;
      case G_OR:
	memorcpy1(_g_wmem + y * _g_linesize + x0, color, x1 - x0 + 1);
	break;
      case G_XOR:
	memxorcpy1(_g_wmem + y * _g_linesize + x0, color, x1 - x0 + 1);
	break;
    }
}

void FLAT32k_hline(int y, int x0, int x1, int color)
{
  switch (_g_wmode)
    {
      case G_NORM:
	memsetw(_g_wmem + y * _g_linesize + (x0 << 1), color, x1 - x0 + 1);
	break;
      case G_AND:
	memandcpyw1(_g_wmem + y * _g_linesize + (x0 << 1), color, x1 - x0 + 1);
	break;
      case G_OR:
	memorcpyw1(_g_wmem + y * _g_linesize + (x0 << 1), color, x1 - x0 + 1);
	break;
      case G_XOR:
	memxorcpyw1(_g_wmem + y * _g_linesize + (x0 << 1), color, x1 - x0 + 1);
	break;
    }
}

void FLAT16m_hline(int y, int x0, int x1, int color)
{
  switch (_g_wmode)
    {
      case G_NORM:
	memset3(_g_wmem + y * _g_linesize + x0 * 3, color, x1 - x0 + 1);
	break;
      case G_AND:
	memandcpy3_1(_g_wmem + y * _g_linesize + x0 * 3, color, x1 - x0 + 1);
	break;
      case G_OR:
	memorcpy3_1(_g_wmem + y * _g_linesize + x0 * 3, color, x1 - x0 + 1);
	break;
      case G_XOR:
	memxorcpy3_1(_g_wmem + y * _g_linesize + x0 * 3, color, x1 - x0 + 1);
	break;
    }
}

void FLAT16m_32Bit_hline(int y, int x0, int x1, int color)
{
  switch (_g_wmode)
    {
      case G_NORM:
	memsetl(_g_wmem + y * _g_linesize + (x0 << 2), color, x1 - x0 + 1);
	break;
      case G_AND:
	memandcpyl1(_g_wmem + y * _g_linesize + (x0 << 2), color, x1 - x0 + 1);
	break;
      case G_OR:
	memorcpyw1(_g_wmem + y * _g_linesize + (x0 << 2), color, x1 - x0 + 1);
	break;
      case G_XOR:
	memxorcpyw1(_g_wmem + y * _g_linesize + (x0 << 2), color, x1 - x0 + 1);
	break;
    }
}

static void mymemset(int offset, int color, int len)
{
  while (len--)
    {
      _g_wmem[offset] = _g_rmem[offset];
      _g_wmem[offset] = color;
      offset++;
    }
}

void SEG16_hline(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + (x1 >> 3);
  offset0 += x0 >> 3;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETRWWIN(win0);

  _outp16(GRA_I, color << 8);

  if (offset0 == offset1)
    {
      _outp16(GRA_I, ((0xFF00 >> (x0 & 7)) & (0xFF00 << (7 - (x1 & 7)))) | 8);
      _g_wmem[offset0 & (_g_wingran - 1)] = _g_rmem[offset0 & (_g_wingran - 1)];
      _g_wmem[offset0 & (_g_wingran - 1)] = color;
    }
  else
    {
      _outp16(GRA_I, ((0xFF00 >> (x0 & 7)) & 0xFF00) | 8);
      _g_wmem[offset0 & (_g_wingran - 1)] = _g_rmem[offset0 & (_g_wingran - 1)];
      _g_wmem[offset0 & (_g_wingran - 1)] = color;
      offset0++;

      _outp16(GRA_I, 0xFF08);
      if (win0 == win1)
	mymemset((offset0 & (_g_wingran - 1)), color, offset1 - offset0);
      else
	{
	  int offset2;

	  offset2 = win1 * _g_wingran;
	  mymemset((offset0 & (_g_wingran - 1)), color, offset2 - offset0);

	  GSETWWIN(win1);

	  mymemset(0, color, offset1 - offset2);
	}

      _outp16(GRA_I, (0xFF00 << (7 - (x1 & 7))) | 8);
      _g_wmem[offset1 & (_g_wingran - 1)] = _g_rmem[offset1 & (_g_wingran - 1)];
      _g_wmem[offset1 & (_g_wingran - 1)] = color;
    }
}

void SEG256_hline_1(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + x1;
  offset0 += x0;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memset(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  GSETRWIN(win0);
	  memandcpy1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_OR:
	  GSETRWIN(win0);
	  memorcpy1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  GSETRWIN(win0);
	  memxorcpy1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memset(_g_wmem + (offset0 & _g_winmask), color, offset2 - offset0);
	    GSETWWIN(win1);
	    memset(_g_wmem, color, offset1 - offset2 + 1);
	    return;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpy1(_g_wmem + (offset0 & _g_winmask), color, offset2 - offset0);
	    GSETRWWIN(win1);
	    memandcpy1(_g_wmem, color, offset1 - offset2 + 1);
	    return;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpy1(_g_wmem + (offset0 & _g_winmask), color, offset2 - offset0);
	    GSETRWWIN(win1);
	    memorcpy1(_g_wmem, color, offset1 - offset2 + 1);
	    return;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpy1(_g_wmem + (offset0 & _g_winmask), color, offset2 - offset0);
	    GSETRWWIN(win1);
	    memxorcpy1(_g_wmem, color, offset1 - offset2 + 1);
	    return;
	}
    }
}

void SEG256_hline_2(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + x1;
  offset0 += x0;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memset(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  GSETRWIN(win0);
	  offset0 &= _g_winmask;
	  memandcpy2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_OR:
	  GSETRWIN(win0);
	  offset0 &= _g_winmask;
	  memorcpy2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  GSETRWIN(win0);
	  offset0 &= _g_winmask;
	  memxorcpy2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memset(_g_wmem + (offset0 & _g_winmask), color, offset2 - offset0);
	    GSETWWIN(win1);
	    memset(_g_wmem, color, offset1 - offset2 + 1);
	    return;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpy2(_g_wmem + (offset0 & _g_winmask),
		   _g_rmem + (offset0 & _g_winmask), color, offset2 - offset0);
	    GSETRWWIN(win1);
	    memandcpy2(_g_wmem, _g_rmem, color, offset1 - offset2 + 1);
	    return;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpy2(_g_wmem + (offset0 & _g_winmask),
		     _g_rmem + (offset0 & _g_winmask),
		     color, offset2 - offset0);
	    GSETRWWIN(win1);
	    memorcpy2(_g_wmem, _g_rmem, color, offset1 - offset2 + 1);
	    return;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpy2(_g_wmem + (offset0 & _g_winmask),
		       _g_rmem + (offset0 & _g_winmask),
		      color, offset2 - offset0);
	    GSETRWWIN(win1);
	    memxorcpy2(_g_wmem, _g_rmem, color, offset1 - offset2 + 1);
	    return;
	}
    }
}

void SEG32k_hline_1(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + (x1 << 1);
  offset0 += x0 << 1;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memsetw(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  GSETRWIN(win0);
	  memandcpyw1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_OR:
	  GSETRWIN(win0);
	  memorcpyw1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  GSETRWIN(win0);
	  memxorcpyw1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memsetw(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETWWIN(win1);
	    memsetw(_g_wmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpyw1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETRWWIN(win1);
	    memandcpyw1(_g_wmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpyw1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETRWWIN(win1);
	    memorcpyw1(_g_wmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpyw1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETRWWIN(win1);
	    memxorcpyw1(_g_wmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	}
    }
}

void SEG32k_hline_2(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + (x1 << 1);
  offset0 += x0 << 1;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memsetw(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memandcpyw2(_g_wmem + offset0,
		      _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_OR:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memorcpyw2(_g_wmem + offset0,
		     _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memxorcpyw2(_g_wmem + offset0,
		      _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memsetw(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETWWIN(win1);
	    memsetw(_g_wmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpyw2(_g_wmem + (offset0 & _g_winmask),
		       _g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETRWWIN(win1);
	    memandcpyw2(_g_wmem, _g_rmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpyw2(_g_wmem + (offset0 & _g_winmask),
		       _g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETRWWIN(win1);
	    memorcpyw2(_g_wmem, _g_rmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpyw2(_g_wmem + (offset0 & _g_winmask),
			_g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 1);
	    GSETRWWIN(win1);
	    memxorcpyw2(_g_wmem, _g_rmem, color, ((offset1 - offset2) >> 1) + 1);
	    break;
	}
    }
}

void SEG16m_hline_1(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + x1 * 3;
  offset0 += x0 * 3;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memset3(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  GSETRWIN(win0);
	  memandcpy3_1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_OR:
	  GSETRWIN(win0);
	  memorcpy3_1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  GSETRWIN(win0);
	  memxorcpy3_1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memset3(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETWWIN(win1);
	    memset3(_g_wmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpy3_1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETRWWIN(win1);
	    memandcpy3_1(_g_wmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpy3_1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETRWWIN(win1);
	    memorcpy3_1(_g_wmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpy3_1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETRWWIN(win1);
	    memxorcpy3_1(_g_wmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	}
    }
}

void SEG16m_hline_2(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + x1 * 3;
  offset0 += x0 * 3;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memset3(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memandcpy3_2(_g_wmem + offset0,
		       _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_OR:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memorcpy3_2(_g_wmem + offset0,
		      _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memxorcpy3_2(_g_wmem + offset0,
		       _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memset3(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETWWIN(win1);
	    memset3(_g_wmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpy3_2(_g_wmem + (offset0 & _g_winmask),
			 _g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETRWWIN(win1);
	    memandcpy3_2(_g_wmem, _g_rmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpy3_2(_g_wmem + (offset0 & _g_winmask),
			_g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETRWWIN(win1);
	    memorcpy3_2(_g_wmem, _g_rmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpy3_2(_g_wmem + (offset0 & _g_winmask),
			 _g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) / 3);
	    GSETRWWIN(win1);
	    memxorcpy3_2(_g_wmem, _g_rmem, color, (offset1 - offset2) / 3 + 1);
	    break;
	}
    }
}

void SEG16m_split_hline_1(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + x1 * 3;
  offset0 += x0 * 3;
  win0     = offset0 >> _g_winshift;
  win1     = (offset1 + 2) >> _g_winshift;
  offset0 &= _g_winmask;

  GSETWWIN(win0);

  if (win0 == win1) {
    switch (_g_wmode)
      {
	case G_NORM:
	  memset3(_g_wmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_AND:
	  GSETRWIN(win0);
	  memandcpy3_1(_g_wmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_OR:
	  GSETRWIN(win0);
	  memorcpy3_1(_g_wmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  GSETRWIN(win0);
	  memxorcpy3_1(_g_wmem + offset0, color, x1 - x0 + 1);
	  break;
      }
    }
  else
    {
      int           r = 0, width = 3*(x1 - x0 + 1);
      unsigned char RGB[3];
      unsigned char *src = _g_wmem + offset0;

      RGB[0] = color;
      RGB[1] = color >> 8;
      RGB[2] = color >> 16;

      switch (_g_wmode)
	{
	  case G_NORM:
	    while (offset0++ < _g_wingran) {
	      *src++ = RGB[r++ % 3];
	      width--;
	    }
	    GSETWWIN(win1);
	    src = _g_wmem;
	    while (width-- > 0) *src++ = RGB[r++ % 3];
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    while (offset0++ < _g_wingran) {
	      *src++ &= RGB[r++ % 3];
	      width--;
	    }
	    GSETRWWIN(win1);
	    src = _g_wmem;
	    while (width-- > 0) *src++ &= RGB[r++ % 3];
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    while (offset0++ < _g_wingran) {
	      *src++ |= RGB[r++ % 3];
	      width--;
	    }
	    GSETRWWIN(win1);
	    src = _g_wmem;
	    while (width-- > 0) *src++ |= RGB[r++ % 3];
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    while (offset0++ < _g_wingran) {
	      *src++ ^= RGB[r++ % 3];
	      width--;
	    }
	    GSETRWWIN(win1);
	    src = _g_wmem;
	    while (width-- > 0) *src++ ^= RGB[r++ % 3];
	    break;
	}
    }
}

void SEG16m_split_hline_2(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + x1 * 3;
  offset0 += x0 * 3;
  win0     = offset0 >> _g_winshift;
  win1     = (offset1 + 2) >> _g_winshift;
  offset0 &= _g_winmask;

  GSETWWIN(win0);

  if (win0 == win1) {
    switch (_g_wmode)
      {
	case G_NORM:
	  memset3(_g_wmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_AND:
	  GSETRWIN(win0);
	  memandcpy3_2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_OR:
	  GSETRWIN(win0);
	  memorcpy3_2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  GSETRWIN(win0);
	  memxorcpy3_2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
      }
    }
  else
    {
      int           r = 0, width = 3*(x1 - x0 + 1);
      unsigned char RGB[3];
      unsigned char *dst = _g_wmem + offset0, *src;

      RGB[0] = color;
      RGB[1] = color >> 8;
      RGB[2] = color >> 16;

      switch (_g_wmode)
	{
	  case G_NORM:
	    while (offset0++ < _g_wingran) {
	      *dst++ = RGB[r++ % 3];
	      width--;
	    }
	    GSETWWIN(win1);
	    dst = _g_wmem;
	    while (width-- > 0) *dst++ = RGB[r++ % 3];
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    src = _g_rmem + offset0;
	    while (offset0++ < _g_wingran) {
	      *dst++ = *src++ & RGB[r++ % 3];
	      width--;
	    }
	    GSETRWWIN(win1);
	    dst = _g_wmem;
	    src = _g_rmem;
	    while (width-- > 0) *dst++ = *src++ & RGB[r++ % 3];
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    src = _g_rmem + offset0;
	    while (offset0++ < _g_wingran) {
	      *dst++ = *src++ | RGB[r++ % 3];
	      width--;
	    }
	    GSETRWWIN(win1);
	    dst = _g_wmem;
	    src = _g_rmem;
	    while (width-- > 0) *dst++ = *src++ | RGB[r++ % 3];
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    src = _g_rmem + offset0;
	    while (offset0++ < _g_wingran) {
	      *dst++ = *src++ ^ RGB[r++ % 3];
	      width--;
	    }
	    GSETRWWIN(win1);
	    dst = _g_wmem;
	    src = _g_rmem;
	    while (width-- > 0) *dst++ = *src++ ^ RGB[r++ % 3];
	    break;
	}
    }
}

void SEG16m_32Bit_hline_1(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + (x1 << 2);
  offset0 += x0 << 2;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memsetl(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  GSETRWIN(win0);
	  memandcpyl1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_OR:
	  GSETRWIN(win0);
	  memorcpyl1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  GSETRWIN(win0);
	  memxorcpyl1(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memsetl(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETWWIN(win1);
	    memsetl(_g_wmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpyl1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETRWWIN(win1);
	    memandcpyl1(_g_wmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpyl1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETRWWIN(win1);
	    memorcpyl1(_g_wmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpyl1(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETRWWIN(win1);
	    memxorcpyl1(_g_wmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	}
    }
}

void SEG16m_32Bit_hline_2(int y, int x0, int x1, int color)
{
  int offset0, offset1, win0, win1;

  offset0  = y * _g_linesize;
  offset1  = offset0 + (x1 << 2);
  offset0 += x0 << 2;
  win0     = offset0 >> _g_winshift;
  win1     = offset1 >> _g_winshift;

  GSETWWIN(win0);

  if (win0 == win1)
    switch (_g_wmode)
      {
	case G_NORM:
	  memsetl(_g_wmem + (offset0 & _g_winmask), color, x1 - x0 + 1);
	  break;
	case G_AND:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memandcpyl2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_OR:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memorcpyl2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
	case G_XOR:
	  offset0 &= _g_winmask;
	  GSETRWIN(win0);
	  memxorcpyl2(_g_wmem + offset0, _g_rmem + offset0, color, x1 - x0 + 1);
	  break;
      }
  else
    {
      int offset2;

      offset2 = win1 * _g_wingran;
      switch (_g_wmode)
	{
	  case G_NORM:
	    memsetl(_g_wmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETWWIN(win1);
	    memsetl(_g_wmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	  case G_AND:
	    GSETRWIN(win0);
	    memandcpyl2(_g_wmem + (offset0 & _g_winmask),
			_g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETRWWIN(win1);
	    memandcpyl2(_g_wmem, _g_rmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	  case G_OR:
	    GSETRWIN(win0);
	    memorcpyl2(_g_wmem + (offset0 & _g_winmask),
		       _g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETRWWIN(win1);
	    memorcpyl2(_g_wmem, _g_rmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	  case G_XOR:
	    GSETRWIN(win0);
	    memxorcpyl2(_g_wmem + (offset0 & _g_winmask),
			_g_rmem + (offset0 & _g_winmask), color, (offset2 - offset0) >> 2);
	    GSETRWWIN(win1);
	    memxorcpyl2(_g_wmem, _g_rmem, color, ((offset1 - offset2) >> 2) + 1);
	    break;
	}
    }
}

void ALT256_hline(int y, int x0, int x1, int color)
{
  int first, last, plane;
  char *base;

  _outp8(SEQ_I, 0x02);
  _outp8(GRA_I, 0x04);

  base = _g_wmem + y * _g_linesize;

  for(plane = 0; plane < 4; plane++)
    {
      _outp8(SEQ_D, 1 << plane);

      first = (x0 >> 2) + ((x0 & 3) > plane);
      last  = (x1 >> 2) - ((x1 & 3) < plane);
      if (last >= first)
	switch (_g_wmode)
	  {
	    case G_NORM:
	      memset(base + first, color, last - first + 1);
	      break;
	    case G_AND:
	      _outp8(GRA_D, plane);
	      memandcpy1(base + first, color, last - first + 1);
	      break;
	    case G_OR:
	      _outp8(GRA_D, plane);
	      memorcpy1(base + first, color, last - first + 1);
	      break;
	    case G_XOR:
	      _outp8(GRA_D, plane);
	      memxorcpy1(base + first, color, last - first + 1);
	      break;
	  }
    }
}

void g_hline(int y, int x0, int x1, int color)
{
  int x;

  if (x0 > x1)
    {
      x  = x0;
      x0 = x1;
      x1 = x;
    }
  if ((x1 < _g_clipx0) || (x0 > _g_clipx1) || (y > _g_clipy1) || (y < _g_clipy0))
    return;
  if (x0 < _g_clipx0)
    x0 = _g_clipx0;
  if (x1 > _g_clipx1)
    x1 = _g_clipx1;
  GLOCK;
  _g_hline(y, x0, x1, color);
  GUNLOCK;
}
