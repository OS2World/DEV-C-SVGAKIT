/* graph3.h (emx+gcc) -- Copyright (c) 1987-1993 by Eberhard Mattes */
/*                                  changes 1993 by Johannes Martin */

typedef struct {
  int is_critical;
  int x, y;
} CriticalPosition;

typedef void (*VGAFUNC_clear) (int color);
typedef int  (*VGAFUNC_get) (int x, int y);
typedef void (*VGAFUNC_getimage) (int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
typedef void (*VGAFUNC_getpal) (char *pal, int first, int n);
typedef void (*VGAFUNC_hline) (int y, int x0, int x1, int color);
typedef void (*VGAFUNC_putimage) (int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
typedef void (*VGAFUNC_restore) (void *buf);
typedef void (*VGAFUNC_save) (void *buf);
typedef void (*VGAFUNC_set) (int x, int y, int color);
typedef void (*VGAFUNC_setpal) (const char *pal, int first, int n, int wait_flag);
typedef void (*VGAFUNC_vline) (int x, int y0, int y1,int color);

extern int _g_wwin;
extern int _g_rwin;
extern int _g_wwinstat;
extern int *_g_rwinstat;
extern int _g_winsize;
extern int _g_wingran;
extern int _g_winfact;
extern int _g_linesize;
extern int _g_rlinesize;
extern int _g_wincount;
extern int _g_winmask;
extern int _g_winshift;
extern int _g_memmodel;
extern int _g_blue_shift;
extern int _g_green_shift;
extern int _g_red_shift;
extern int _g_blue_mask;
extern int _g_green_mask;
extern int _g_red_mask;
extern int _g_col_shr;

extern CriticalPosition *SplitPositions;

extern VESASET _g_setwin;

extern unsigned char *_g_wmem;
extern unsigned char *_g_rmem;
extern unsigned char *_g_vpal;

static __inline__ void memandcpy1(void *s, char c, size_t n)
{
  while (n-- > 0)
    *((unsigned char *) s)++ &= c;
}

static __inline__ void memandcpy2(void *d, void *s, char c, size_t n)
{
  while (n-- > 0)
    *((unsigned char *) d)++ = *((unsigned char *) s)++ & c;
}

static __inline__ void memorcpy1(void *s, char c, size_t n)
{
  while (n-- > 0)
    *((unsigned char *) s)++ |= c;
}

static __inline__ void memorcpy2(void *d, void *s, char c, size_t n)
{
  while (n-- > 0)
    *((unsigned char *) d)++ = *((unsigned char *) s)++ | c;
}

static __inline__ void memxorcpy1(void *s, char c, size_t n)
{
  while (n-- > 0)
    *((unsigned char *) s)++ ^= c;
}

static __inline__ void memxorcpy2(void *d, void *s, char c, size_t n)
{
  while (n-- > 0)
    *((unsigned char *) d)++ = *((unsigned char *) s)++ ^ c;
}

static __inline__ void memsetw(void *s, int c, size_t n)
{
  while (n-- > 0)
    *((unsigned short *) s)++ = c;
}

static __inline__ void memandcpyw1(void *s, short c, size_t n)
{
  while (n-- > 0)
    *((unsigned short *) s)++ &= c;
}

static __inline__ void memandcpyw2(void *d, void *s, short c, size_t n)
{
  while (n-- > 0)
    *((unsigned short *) d)++ = *((unsigned short *) s)++ & c;
}

static __inline__ void memorcpyw1(void *s, short c, size_t n)
{
  while (n-- > 0)
    *((unsigned short *) s)++ |= c;
}

static __inline__ void memorcpyw2(void *d, void *s, short c, size_t n)
{
  while (n-- > 0)
    *((unsigned short *) d)++ = *((unsigned short *) s)++ | c;
}

static __inline__ void memxorcpyw1(void *s, short c, size_t n)
{
  while (n-- > 0)
    *((unsigned short *) s)++ ^= c;
}

static __inline__ void memxorcpyw2(void *d, void *s, short c, size_t n)
{
  while (n-- > 0)
    *((unsigned short *) d)++ = *((unsigned short *) s)++ ^ c;
}

static __inline__ void memset3(void *s, int c, size_t n)
{
  unsigned char c1 = c;
  unsigned char c2 = c >> 8;
  unsigned char c3 = c >> 16;

  while (n-- > 0)
    {
      *((unsigned char *) s)++ = c1;
      *((unsigned char *) s)++ = c2;
      *((unsigned char *) s)++ = c3;
    }
}

static __inline__ void memandcpy3_1(void *s, int c, size_t n)
{
  unsigned char c1 = c;
  unsigned char c2 = c >> 8;
  unsigned char c3 = c >> 16;

  while (n-- > 0)
    {
      *((unsigned char *) s)++ &= c1;
      *((unsigned char *) s)++ &= c2;
      *((unsigned char *) s)++ &= c3;
    }
}

static __inline__ void memandcpy3_2(void *d, void *s, int c, size_t n)
{
  unsigned char c1 = c;
  unsigned char c2 = c >> 8;
  unsigned char c3 = c >> 16;

  while (n-- > 0)
    {
      *((unsigned char *) d)++ = *((unsigned char *) s)++ & c1;
      *((unsigned char *) d)++ = *((unsigned char *) s)++ & c2;
      *((unsigned char *) d)++ = *((unsigned char *) s)++ & c3;
    }
}

static __inline__ void memorcpy3_1(void *s, int c, size_t n)
{
  unsigned char c1 = c;
  unsigned char c2 = c >> 8;
  unsigned char c3 = c >> 16;

  while (n-- > 0)
    {
      *((unsigned char *) s)++ |= c1;
      *((unsigned char *) s)++ |= c2;
      *((unsigned char *) s)++ |= c3;
    }
}

static __inline__ void memorcpy3_2(void *d, void *s, int c, size_t n)
{
  unsigned char c1 = c;
  unsigned char c2 = c >> 8;
  unsigned char c3 = c >> 16;

  while (n-- > 0)
    {
      *((unsigned char *) d)++ = *((unsigned char *) s)++ | c1;
      *((unsigned char *) d)++ = *((unsigned char *) s)++ | c2;
      *((unsigned char *) d)++ = *((unsigned char *) s)++ | c3;
    }
}

static __inline__ void memxorcpy3_1(void *s, int c, size_t n)
{
  unsigned char c1 = c;
  unsigned char c2 = c >> 8;
  unsigned char c3 = c >> 16;

  while (n-- > 0)
    {
      *((unsigned char *) s)++ ^= c1;
      *((unsigned char *) s)++ ^= c2;
      *((unsigned char *) s)++ ^= c3;
    }
}

static __inline__ void memxorcpy3_2(void *d, void *s, int c, size_t n)
{
  unsigned char c1 = c;
  unsigned char c2 = c >> 8;
  unsigned char c3 = c >> 16;

  while (n-- > 0)
    {
      *((unsigned char *) d)++ = *((unsigned char *) s)++ ^ c1;
      *((unsigned char *) d)++ = *((unsigned char *) s)++ ^ c2;
      *((unsigned char *) d)++ = *((unsigned char *) s)++ ^ c3;
    }
}

static __inline__ void memsetl(void *s, int c, size_t n)
{
  while (n-- > 0)
    *((int *) s)++ = c;
}

static __inline__ void memandcpyl1(void *s, int c, size_t n)
{
  while (n-- > 0)
    *((int *) s)++ &= c;
}

static __inline__ void memandcpyl2(void *d, void *s, int c, size_t n)
{
  while (n-- > 0)
    *((int *) d)++ = *((int *) s)++ & c;
}

static __inline__ void memorcpyl1(void *s, int c, size_t n)
{
  while (n-- > 0)
    *((int *) s)++ |= c;
}

static __inline__ void memorcpyl2(void *d, void *s, int c, size_t n)
{
  while (n-- > 0)
    *((int *) d)++ = *((int *) s)++ | c;
}

static __inline__ void memxorcpyl1(void *s, int c, size_t n)
{
  while (n-- > 0)
    *((int *) s)++ ^= c;
}

static __inline__ void memxorcpyl2(void *d, void *s, int c, size_t n)
{
  while (n-- > 0)
    *((int *) d)++ = *((int *) s)++ ^ c;
}

extern VGAFUNC_clear    _g_clear;
extern VGAFUNC_get      _g_get;
extern VGAFUNC_getimage _g_getimage;
extern VGAFUNC_hline    _g_hline;
extern VGAFUNC_putimage _g_putimage;
extern VGAFUNC_set      _g_set;
extern VGAFUNC_vline    _g_vline;

extern VGAFUNC_restore  _g_restore;
extern VGAFUNC_save     _g_save;

extern VGAFUNC_getpal   _g_getpal;
extern VGAFUNC_setpal   _g_setpal;

void FLAT16_clear(int color);
int  FLAT16_get(int x, int y);
void FLAT16_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT16_hline(int y, int x0, int x1, int color);
void FLAT16_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT16_set(int x, int y, int color);
void FLAT16_vline(int x, int y0, int y1, int color);

void FLAT256_clear(int color);
int  FLAT256_get(int x, int y);
void FLAT256_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT256_hline(int y, int x0, int x1, int color);
void FLAT256_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT256_set(int x, int y, int color);
void FLAT256_vline(int x, int y0, int y1, int color);

void FLAT32k_clear(int color);
int  FLAT32k_get(int x, int y);
void FLAT32k_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT32k_hline(int y, int x0, int x1, int color);
void FLAT32k_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT32k_set(int x, int y, int color);
void FLAT32k_vline(int x, int y0, int y1, int color);

void FLAT16m_clear(int color);
int  FLAT16m_get(int x, int y);
void FLAT16m_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT16m_hline(int y, int x0, int x1, int color);
void FLAT16m_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT16m_set(int x, int y, int color);
void FLAT16m_vline(int x, int y0, int y1, int color);

void FLAT16m_32Bit_clear(int color);
int  FLAT16m_32Bit_get(int x, int y);
void FLAT16m_32Bit_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT16m_32Bit_hline(int y, int x0, int x1, int color);
void FLAT16m_32Bit_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void FLAT16m_32Bit_set(int x, int y, int color);
void FLAT16m_32Bit_vline(int x, int y0, int y1, int color);

void SEG16_clear(int color);
int  SEG16_get(int x, int y);
void SEG16_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void SEG16_hline(int y, int x0, int x1, int color);
void SEG16_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void SEG16_set(int x, int y, int color);
void SEG16_vline(int x, int y0, int y1, int color);

void SEG256_clear(int color);
int  SEG256_get(int x, int y);
void SEG256_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void SEG256_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);

void SEG256_hline_1(int y, int x0, int x1, int color);
void SEG256_set_1(int x, int y, int color);
void SEG256_vline_1(int x, int y0, int y1, int color);

void SEG256_hline_2(int y, int x0, int x1, int color);
void SEG256_set_2(int x, int y, int color);
void SEG256_vline_2(int x, int y0, int y1, int color);

void SEG32k_clear(int color);
int  SEG32k_get(int x, int y);
void SEG32k_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void SEG32k_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);

void SEG32k_hline_1(int y, int x0, int x1, int color);
void SEG32k_set_1(int x, int y, int color);
void SEG32k_vline_1(int x, int y0, int y1, int color);

void SEG32k_hline_2(int y, int x0, int x1, int color);
void SEG32k_set_2(int x, int y, int color);
void SEG32k_vline_2(int x, int y0, int y1, int color);

void SEG16m_clear(int color);
int  SEG16m_get(int x, int y);
void SEG16m_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void SEG16m_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);

void SEG16m_hline_1(int y, int x0, int x1, int color);
void SEG16m_set_1(int x, int y, int color);
void SEG16m_vline_1(int x, int y0, int y1, int color);

void SEG16m_hline_2(int y, int x0, int x1, int color);
void SEG16m_set_2(int x, int y, int color);
void SEG16m_vline_2(int x, int y0, int y1, int color);

int  SEG16m_split_get(int x, int y);

void SEG16m_split_hline_1(int y, int x0, int x1, int color);
void SEG16m_split_set_1(int x, int y, int color);

void SEG16m_split_hline_2(int y, int x0, int x1, int color);
void SEG16m_split_set_2(int x, int y, int color);

void SEG16m_32Bit_clear(int color);
int  SEG16m_32Bit_get(int x, int y);
void SEG16m_32Bit_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void SEG16m_32Bit_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);

void SEG16m_32Bit_hline_1(int y, int x0, int x1, int color);
void SEG16m_32Bit_set_1(int x, int y, int color);
void SEG16m_32Bit_vline_1(int x, int y0, int y1, int color);

void SEG16m_32Bit_hline_2(int y, int x0, int x1, int color);
void SEG16m_32Bit_set_2(int x, int y, int color);
void SEG16m_32Bit_vline_2(int x, int y0, int y1, int color);

void ALT256_clear(int color);
int  ALT256_get(int x, int y);
void ALT256_getimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void ALT256_hline(int y, int x0, int x1, int color);
void ALT256_putimage(int x, int y, int width, int height, int skip, int visible, int fullheight, void *buf);
void ALT256_set(int x, int y, int color);
void ALT256_vline(int x, int y0, int y1, int color);

void FLAT_restore(void *buf);
void FLAT_save(void *buf);

void SEG_restore(void *buf);
void SEG_save(void *buf);

void SEG16_restore(void *buf);
void SEG16_save(void *buf);

void ALT_restore(void *buf);
void ALT_save(void *buf);

void PORT_getpal(char *pal, int first, int n);
void PORT_setpal(const char *pal, int first, int n, int wait_flag);
void MEM_getpal(char *pal, int first, int n);
void MEM_setpal(const char *pal, int first, int n, int wait_flag);
void DUMMY_getpal(char *pal, int first, int n);
void DUMMY_setpal(const char *pal, int first, int n, int wait_flag);

#define GSETWWIN(nr)                                 \
    if (nr != _g_wwinstat)                           \
      _g_setwin(_g_wwin, _g_wwinstat = nr);          \

#define GSETRWIN(nr)                                 \
    if (nr != *_g_rwinstat)                          \
      _g_setwin(_g_rwin, *_g_rwinstat = nr);         \

#define GSETRWWIN(nr)                                \
    if (nr != _g_wwinstat)                           \
      _g_setwin(_g_wwin, _g_wwinstat = nr);          \
    if (nr != *_g_rwinstat)                          \
      _g_setwin(_g_rwin, *_g_rwinstat = nr);         \

