/* gfont.h (emx+gcc) */

/* possible directions of text writing */

#define         RIGHT           0
#define         UP              1
#define         DOWN            2
#define         NORMAL          RIGHT


/* default values for x- and y-scaling */

#define         DEFAULT_WIDTH   1.0
#define         DEFAULT_HEIGHT  1.0

/* predefined font names */

#define         DEFFONT         "default.chr"
#define         TRIPLEX         "trip.chr"
#define         SANSSER         "sans.chr"
#define         EUROFON         "euro.chr"
#define         GOTHICF         "goth.chr"
#define         SCRIPTF         "scri.chr"
#define         LCOMFON         "lcom.chr"
#define         LITTLEF         "litt.chr"
#define         TSCRFON         "tscr.chr"
#define         SIMPLEX         "simp.chr"

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
