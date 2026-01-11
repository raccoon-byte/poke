#include "poke.h"

static Display *dpy;
static XftFont *font;
static Window rw;
static Window wd;
static int scr;
static int nlines;
static int dpyw;
static int dpyh;

int
main(int argc, char **argv) {
	char **slines;
	unsigned fontph;
	unsigned targetwh;

	if (--argc && strcmp(argv[1], "-v") == 0) {
		printf("poke 0.0.1\n");
		return 0;
	}
	if (!argc) {
		printf("usage: poke \"msg1\" \"msg2\"...\n");
		return 0;
	}

	if (!xinit())
		fprintf(stderr, "could not initialize X resources\n");

	slines = malloc(maxlines * sizeof(char *));
	sanitize(slines, argc, ++argv);

	fontph = pixelheight(*slines, strlen(*slines));
	if ((targetwh = ((fontph * nlines) + 2*wpad + linesep * (nlines - 1))) > wh)
          XResizeWindow(dpy, wd, ww, targetwh);

	shownotif(slines);

	return 0;
}

void
quit(const char *err, ...)
{
	va_list ap;
	va_start(ap, err);
	vfprintf(stderr, err, ap);
	va_end(ap);
	exit(1);
}

/* Initializes X-related resources */
int 
xinit(void) {
	XSetWindowAttributes attrs;

	if (!(dpy = XOpenDisplay(NULL)))
		quit("could not open X display\n");
	rw = XDefaultRootWindow(dpy);
	scr = DefaultScreen(dpy);
	dpyw = DisplayWidth(dpy, scr);
	dpyh = DisplayHeight(dpy, scr);

	if(!(font = XftFontOpenName(dpy, scr, fontname)))
		quit("could not load provided font\n");

	attrs.background_pixel = bgcolor;
	attrs.save_under = 1;
	/* attrs.override_redirect = 1; TODO: Set this + related stuff */

	wd = XCreateWindow(dpy, 
					rw, 
					getwx(mode),
					getwy(mode), 
					ww,
					wh, 
					200, 
					DefaultDepth(dpy, scr), 
					InputOutput, 
					CopyFromParent, 
					CWBackPixel | CWSaveUnder, &attrs);
	XSetTransientForHint(dpy, wd, wd);

	return 1;
}

/* more like wrap */
void
sanitize(char** slines, int count, char **lines) {
	char *tmpbuf;

	int i, lastspace;
	nlines = 0;
	while (count-- > 0) {
		tmpbuf = malloc(maxlinew);
		i = 0;
		lastspace = 0;

		while (i < maxlinew && (tmpbuf[i] = *(*lines)++) != '\0') {
			if (tmpbuf[i] == ' ' || tmpbuf[i] == '\t')
				lastspace = i;

			if (pixelwidth(tmpbuf, i + 1) + 2*wpad >= ww) {
				(*lines)--;

				if (lastspace) {
					tmpbuf[lastspace] = '\0'; 
					*lines -= i - lastspace - 1;
				}
				else
					tmpbuf[i] = '\0'; 

				++count;
				break;
			}

			++i;
		}
		*slines++ = tmpbuf;	
		++lines; /* Unneeded since strings are adjacent I guess */

		if (++nlines == maxlines)
			break;
	}

	*slines = NULL;
}

unsigned
getwx(int mode) {
	return mode % 2 == 1 ? wx : dpyw - ww - wx;
}

unsigned
getwy(int mode) {
	return mode <= 2 ? wy : dpyh - wh - wy;
}

unsigned
pixelheight(char *l, int len) {
	XGlyphInfo info;
	XftTextExtentsUtf8(dpy, font, (FcChar8 *)l, len, &info);
	return info.height;
}

unsigned
pixelwidth(char *l, int len) {
	XGlyphInfo info;
	XftTextExtentsUtf8(dpy, font, (FcChar8 *)l, len, &info);
	return info.xOff;
}

/* TODO: error check all Xsomething functions that return ints with
	statuses or something like that */
int
shownotif(char **lines) {
    XftColor color;
	XftDraw *draw;
	XEvent e;
	unsigned ph;
	fd_set fdset;
	struct timeval tv;
	unsigned xfd;

	ph = pixelheight(*lines, strlen(*lines));

	if (!XftColorAllocName(dpy, CopyFromParent, DefaultColormap(dpy, scr),
						"#7e6249", &color))
          quit("could not allocate xft color\n");
	draw = XftDrawCreate(dpy, wd, DefaultVisual(dpy, scr),
						DefaultColormap(dpy, scr));

	XMapWindow(dpy, wd);

	XSelectInput(dpy, wd, ExposureMask | KeyPressMask);

	xfd = ConnectionNumber(dpy);
	FD_SET(xfd, &fdset);
	tv.tv_usec = 0;
	tv.tv_sec = timeout;

    for(;;) {
        while(XPending(dpy)) {
            XNextEvent(dpy, &e);
			if (e.type == Expose) {
				int offset;
				for (offset = wpad; *lines; offset += linesep + ph, ++lines)
					XftDrawStringUtf8(
						draw, &color, font, wpad, wpad + offset,
						(const FcChar8 *)*lines, strlen(*lines));
            }
			if (e.type == KeyPress)
				return 0;
			}

        if (!select(xfd + 1, &fdset, NULL, NULL, &tv))
			return 0;
    }

	return 0;
}

