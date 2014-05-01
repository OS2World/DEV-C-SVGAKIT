/* jmgraph.h (emx+gcc) */

#if !defined (_JMGRAPH_H)
#define _JMGRAPH_H

#if defined (__cplusplus)
extern "C" {
#endif

/* g_mode() and g_modeset() arguments (MODE) */

#define GTEXT          0x003

#define G640x480x16    0x012
#define G800x600x16    0x102
#define G1024x768x16   0x104
#define G1280x1024x16  0x106

#define G320x200x256   0x013
#define G640x400x256   0x100
#define G640x480x256   0x101
#define G800x600x256   0x103
#define G1024x768x256  0x105
#define G1280x1024x256 0x107

#define G320x200x32k   0x10d
#define G320x200x64k   0x10e
#define G320x200x16m   0x10f

#define G640x480x32k   0x110
#define G640x480x64k   0x111
#define G640x480x16m   0x112

#define G800x600x32k   0x113
#define G800x600x64k   0x114
#define G800x600x16m   0x115

#define G1024x768x32k  0x116
#define G1024x768x64k  0x117
#define G1024x768x16m  0x118

#define G1280x1024x32k 0x119
#define G1280x1024x64k 0x11a
#define G1280x1024x16m 0x11b

#define G512x768x32k   0x200
#define G512x768x64k   0x201
#define G341x768x16m   0x202

#define G_MODE_OFF     GTEXT
#define G_MODE_VGA_L   G320x200x256
#define G_MODE_VGA_S   G640x480x256
#define G_MODE_VGA_M   G800x600x256
#define G_MODE_VGA_H   G1024x768x256
#define G_MODE_VGA_32k G800x600x32k
#define G_MODE_VGA_64k G800x600x64k
#define G_MODE_VGA_16m G640x480x16m

#define G_SET_KEEP    0         /* No change         */

/* g_wmode() arguments */

#define G_NORM 0                /* Normal write mode */
#define G_AND  1                /* `AND' mode        */
#define G_OR   2                /* `OR' mode         */
#define G_XOR  3                /* `XOR' mode        */

/* possible directions of text writing */

#define RIGHT  0
#define UP     1
#define DOWN   2
#define NORMAL RIGHT

/* default values for x- and y-scaling */

#define DEFAULT_WIDTH   1.0
#define DEFAULT_HEIGHT  1.0

/* predefined font names */

#define DEFFONT "default.chr"
#define TRIPLEX "trip.chr"
#define SANSSER "sans.chr"
#define EUROFON "euro.chr"
#define GOTHICF "goth.chr"
#define SCRIPTF "scri.chr"
#define LCOMFON "lcom.chr"
#define LITTLEF "litt.chr"
#define TSCRFON "tscr.chr"
#define SIMPLEX "simp.chr"

/* Fill or draw outline */

#define G_OUTLINE      0             /* Draw outline */
#define G_FILL         1             /* Fill         */

/* EGA colors */

#define G_INTENSITY    8

#define G_BLACK        0
#define G_BLUE         1
#define G_GREEN        2
#define G_CYAN         3
#define G_RED          4
#define G_MAGENTA      5
#define G_BROWN        6
#define G_LIGHTGRAY    7

#define G_DARKGRAY     (G_BLACK     | G_INTENSITY)
#define G_LIGHTBLUE    (G_BLUE      | G_INTENSITY)
#define G_LIGHTGREEN   (G_GREEN     | G_INTENSITY)
#define G_LIGHTCYAN    (G_CYAN      | G_INTENSITY)
#define G_LIGHTRED     (G_RED       | G_INTENSITY)
#define G_LIGHTMAGENTA (G_MAGENTA   | G_INTENSITY)
#define G_YELLOW       (G_BROWN     | G_INTENSITY)
#define G_WHITE        (G_LIGHTGRAY | G_INTENSITY)

/* HI-/TRUE-Colors   */

/* Don't modify the following variables at runtime ! */
extern int _g_blue_mask;
extern int _g_green_mask;
extern int _g_red_mask;
extern int _g_blue_shift;
extern int _g_green_shift;
extern int _g_red_shift;
extern int _g_col_shr;

#define RGB(r,g,b) (((((int)(b) & _g_blue_mask) << _g_blue_shift) | \
		     (((int)(g) & _g_green_mask) << _g_green_shift) | \
		     (((int)(r) & _g_red_mask) << _g_red_shift)) >> _g_col_shr)

#define RGB15(r,g,b) RGB(r,g,b)
#define RGB16(r,g,b) RGB(r,g,b)
#define RGB24(r,g,b) RGB(r,g,b)

/* Screen dimensions */

extern int g_xsize;             /* Screen width             */
extern int g_ysize;             /* Screen height            */
extern int g_colors;            /* Number of colors         */
extern int g_memsize;           /* Size of Display Buffer   */
extern int g_32Bit;             /* 32 Bit TrueColor flag    */

void g_clear(int color);
void g_box(int x0, int y0, int x1, int y1, int color, int fill_flag);
void g_clip(int x0, int y0, int x1, int y1);
void g_ellipse(int cx, int cy, int rx, int ry, int color, int fill_flag);
int  g_get(int x, int y);
void g_getimage(int x0, int y0, int x1, int y1, void *graph_buf);
void g_getpal(char *pal, int first, int n);
void g_hline(int y, int x0, int x1, int color);
int  g_imagesize(int width, int height);
int  g_init(void);
void g_line(int x0, int y0, int x1, int y1, int color);
void g_lock(void);
int  g_mode(int mode);
int  g_modeset(int mode, int flag);
void g_polygon(__const__ int *x, __const__ int *y, int n, int color,
	       int fill_flag);
void g_putimage(int x0, int y0, int x1, int y1, void *graph_buf);
void g_restore(void *buf);
void g_save(void *buf);
void g_set(int x, int y, int color);
void g_setpal(const char *pal, int first, int n, int wait_flag);
void g_triangle(int x0, int y0, int x1, int y1, int x2, int y2, int color,
		int fill_flag);
void g_unlock(int modified);
void g_unlockall(int modified);
void g_vline(int x, int y0, int y1, int color);
void g_waitv(void);
void g_wmode(int wmode);

int  g_virtual(int copy_buf, int copy_pal);
void g_refresh(int x0, int y0, int x1, int y1, int x2, int y2, int pal);
void g_real(int copy_buf, int copy_pal);

int  g_virtual2(int copy_buf, int copy_pal, void *buf, void *pal);
void g_real2(int copy_buf, int copy_pal, void **buf, void **pal);

#define g_vgapal g_setpal

int  gf_loadfont(const char *name);
int  gf_getfontinfo(char **name, int *orgcap, int *orgbase, int *orgdec);

void gf_setscale(double width, double height);
void gf_getscale(double *width, double *height);

void gf_setdirection(int direction);
int  gf_getdirection(void);

int  gf_getcharwidth(char c);
int  gf_getcharheight(char c);

int  gf_getstringwidth(const char *str);
int  gf_getstringheight(const char *str);

void gf_drawchar(int x, int y, char c, int color);
void gf_drawstring(int x, int y, const char *str, int color);

#if defined (__cplusplus)
}
#endif

#endif /* !defined (_JMGRAPH_H) */
