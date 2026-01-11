#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/Xft/Xft.h>

void		quit(const char *err, ...);
int 		xinit(void);
int 		shownotif(char *lines[]);
void		sanitize(char** slines, int count, char **lines);
unsigned	pixelwidth(char *l, int len);
unsigned	pixelheight(char *l, int len);
unsigned	getwx(int mode);
unsigned	getwy(int mode);

/* Config */

/* 1: Top left, 2: Top right, 3: Bottom left, 4: Bottom right */
static int mode = 2;

static unsigned			maxlines = 8;
static unsigned			maxlinew = 1024;
static const char		*fontname = "TerminessNerdFontMono-Regular:pixelsize=18:antialias=true:weight=demibold";
static const time_t		timeout = 10;	/* in seconds */
static const unsigned	wx = 30;	/* screen distance on the x axis reference, mode dependent */
static const unsigned	wy = 30;	/* screen distance on the y axis reference, mode dependent */
static const unsigned	ww = 500;	/* window width */
static const unsigned	wh = 90;	/* minimum window height (can dinamically change with input) */
static const int		bgcolor = 0xf5efe5;
static const int		wpad = 10;	/* window inner padding */
static const int		linesep = 2;	/* pixels between lines */

