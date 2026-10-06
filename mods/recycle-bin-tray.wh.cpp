// ==WindhawkMod==
// @id              recycle-bin-tray
// @name            Recycle Bin in System Tray
// @description     Adds a working Recycle Bin icon to the notification area (system tray), next to the clock: live empty/full state, item count and size in the tooltip, double-click to open, right-click for Open / Empty Recycle Bin.
// @version         2.0
// @author          Yevhen Veprytskyi
// @github          https://github.com/YevhenVe
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lshell32 -ladvapi32 -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Recycle Bin in System Tray

Puts a working Recycle Bin icon into the notification area (system tray), next
to the clock.

* Live icon state: empty or full, using the system Recycle Bin icons.
* The tooltip shows the number of items and the total size.
* Double-click (or single click, optional) opens the Recycle Bin.
* Right-click menu: Open, Empty Recycle Bin (with optional confirmation),
  plus a live item-count header.
* The icon updates instantly when the Recycle Bin changes, and follows
  Windows theme changes for the taskbar automatically.
* Custom icons: point the mod at your own .ico or .svg files for the empty
  and full states (see the settings). SVG files are rasterized at runtime,
  so no conversion is needed. Separate icons can be set for a light taskbar;
  the mod follows the Windows theme automatically.

Notes:

* On Windows 11, new tray icons start hidden in the overflow area. Open
  Settings > Personalization > Taskbar and enable the icon so it sits next to
  the clock, then drag it right next to the clock.
* The mod runs as a tool in its own dedicated process and doesn't hook
  anything, so it can't affect Explorer or the taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- confirmBeforeEmpty: true
  $name: Confirm before emptying
  $description: Show the standard Windows confirmation dialog before emptying the Recycle Bin.
- singleClickToOpen: false
  $name: Open on single click
  $description: Open the Recycle Bin with a single left click instead of a double-click.
- customEmptyIconPath: ""
  $name: Custom empty icon
  $description: Full path to a custom .ico or .svg file shown when the Recycle Bin is empty. Leave empty to use the system icon.
  #! $format: filePath
- customFullIconPath: ""
  $name: Custom full icon
  $description: Full path to a custom .ico or .svg file shown when the Recycle Bin is not empty. Leave empty to use the system icon.
  #! $format: filePath
- customEmptyIconPathLight: ""
  $name: Custom empty icon (light taskbar)
  $description: Full path to a dark .ico or .svg file shown when the Recycle Bin is empty and Windows uses a light taskbar. Leave empty to fall back to the icon above.
  #! $format: filePath
- customFullIconPathLight: ""
  $name: Custom full icon (light taskbar)
  $description: Full path to a dark .ico or .svg file shown when the Recycle Bin is not empty and Windows uses a light taskbar. Leave empty to fall back to the icon above.
  #! $format: filePath
*/
// ==/WindhawkModSettings==

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
// Must come before the shell headers: gives storage to the FOLDERID_* GUIDs
// (DEFINE_KNOWNFOLDER), otherwise the linker can't find them.
#include <initguid.h>
#include <shellapi.h>
#include <shlobj.h>
#include <knownfolders.h>

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>

// Embedded NanoSVG (https://github.com/memononen/nanosvg, zlib-licensed).
// Lets the mod rasterize plain .svg files at runtime, so custom icons don't
// have to be converted to .ico beforehand.
#define NANOSVG_IMPLEMENTATION
/*
 * Copyright (c) 2013-14 Mikko Mononen memon@inside.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * The SVG parser is based on Anti-Grain Geometry 2.4 SVG example
 * Copyright (C) 2002-2004 Maxim Shemanarev (McSeem) (http://www.antigrain.com/)
 *
 * Arc calculation code based on canvg (https://code.google.com/p/canvg/)
 *
 * Bounding box calculation based on http://blog.hackers-cafe.net/2009/06/how-to-calculate-bezier-curves-bounding.html
 *
 */

#ifndef NANOSVG_H
#define NANOSVG_H

#ifndef NANOSVG_CPLUSPLUS
#ifdef __cplusplus
extern "C" {
#endif
#endif

// NanoSVG is a simple stupid single-header-file SVG parse. The output of the parser is a list of cubic bezier shapes.
//
// The library suits well for anything from rendering scalable icons in your editor application to prototyping a game.
//
// NanoSVG supports a wide range of SVG features, but something may be missing, feel free to create a pull request!
//
// The shapes in the SVG images are transformed by the viewBox and converted to specified units.
// That is, you should get the same looking data as your designed in your favorite app.
//
// NanoSVG can return the paths in few different units. For example if you want to render an image, you may choose
// to get the paths in pixels, or if you are feeding the data into a CNC-cutter, you may want to use millimeters.
//
// The units passed to NanoSVG should be one of: 'px', 'pt', 'pc' 'mm', 'cm', or 'in'.
// DPI (dots-per-inch) controls how the unit conversion is done.
//
// If you don't know or care about the units stuff, "px" and 96 should get you going.


/* Example Usage:
	// Load SVG
	NSVGimage* image;
	image = nsvgParseFromFile("test.svg", "px", 96);
	printf("size: %f x %f\n", image->width, image->height);
	// Use...
	for (NSVGshape *shape = image->shapes; shape != NULL; shape = shape->next) {
		for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
			for (int i = 0; i < path->npts-1; i += 3) {
				float* p = &path->pts[i*2];
				drawCubicBez(p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
			}
		}
	}
	// Delete
	nsvgDelete(image);
*/

enum NSVGpaintType {
	NSVG_PAINT_UNDEF = -1,
	NSVG_PAINT_NONE = 0,
	NSVG_PAINT_COLOR = 1,
	NSVG_PAINT_LINEAR_GRADIENT = 2,
	NSVG_PAINT_RADIAL_GRADIENT = 3
};

enum NSVGspreadType {
	NSVG_SPREAD_PAD = 0,
	NSVG_SPREAD_REFLECT = 1,
	NSVG_SPREAD_REPEAT = 2
};

enum NSVGlineJoin {
	NSVG_JOIN_MITER = 0,
	NSVG_JOIN_ROUND = 1,
	NSVG_JOIN_BEVEL = 2
};

enum NSVGlineCap {
	NSVG_CAP_BUTT = 0,
	NSVG_CAP_ROUND = 1,
	NSVG_CAP_SQUARE = 2
};

enum NSVGfillRule {
	NSVG_FILLRULE_NONZERO = 0,
	NSVG_FILLRULE_EVENODD = 1
};

enum NSVGflags {
	NSVG_FLAGS_VISIBLE = 0x01
};

enum NSVGpaintOrder {
	NSVG_PAINT_FILL = 0x00,
	NSVG_PAINT_MARKERS = 0x01,
	NSVG_PAINT_STROKE = 0x02,
};

typedef struct NSVGgradientStop {
	unsigned int color;
	float offset;
} NSVGgradientStop;

typedef struct NSVGgradient {
	float xform[6];
	char spread;
	float fx, fy;
	int nstops;
	NSVGgradientStop stops[1];
} NSVGgradient;

typedef struct NSVGpaint {
	signed char type;
	union {
		unsigned int color;
		NSVGgradient* gradient;
	};
} NSVGpaint;

typedef struct NSVGpath
{
	float* pts;					// Cubic bezier points: x0,y0, [cpx1,cpx1,cpx2,cpy2,x1,y1], ...
	int npts;					// Total number of bezier points.
	char closed;				// Flag indicating if shapes should be treated as closed.
	float bounds[4];			// Tight bounding box of the shape [minx,miny,maxx,maxy].
	struct NSVGpath* next;		// Pointer to next path, or NULL if last element.
} NSVGpath;

typedef struct NSVGshape
{
	char id[64];				// Optional 'id' attr of the shape or its group
	NSVGpaint fill;				// Fill paint
	NSVGpaint stroke;			// Stroke paint
	float opacity;				// Opacity of the shape.
	float strokeWidth;			// Stroke width (scaled).
	float strokeDashOffset;		// Stroke dash offset (scaled).
	float strokeDashArray[8];	// Stroke dash array (scaled).
	char strokeDashCount;		// Number of dash values in dash array.
	char strokeLineJoin;		// Stroke join type.
	char strokeLineCap;			// Stroke cap type.
	float miterLimit;			// Miter limit
	char fillRule;				// Fill rule, see NSVGfillRule.
    unsigned char paintOrder;	// Encoded paint order (3×2-bit fields) see NSVGpaintOrder
	unsigned char flags;		// Logical or of NSVG_FLAGS_* flags
	float bounds[4];			// Tight bounding box of the shape [minx,miny,maxx,maxy].
	char fillGradient[64];		// Optional 'id' of fill gradient
	char strokeGradient[64];	// Optional 'id' of stroke gradient
	float xform[6];				// Root transformation for fill/stroke gradient
	NSVGpath* paths;			// Linked list of paths in the image.
	struct NSVGshape* next;		// Pointer to next shape, or NULL if last element.
} NSVGshape;

typedef struct NSVGimage
{
	float width;				// Width of the image.
	float height;				// Height of the image.
	NSVGshape* shapes;			// Linked list of shapes in the image.
} NSVGimage;

// Parses SVG file from a file, returns SVG image as paths.
NSVGimage* nsvgParseFromFile(const char* filename, const char* units, float dpi);

// Parses SVG file from a null terminated string, returns SVG image as paths.
// Important note: changes the string.
NSVGimage* nsvgParse(char* input, const char* units, float dpi);

// Duplicates a path.
NSVGpath* nsvgDuplicatePath(NSVGpath* p);

// Deletes an image.
void nsvgDelete(NSVGimage* image);

#ifndef NANOSVG_CPLUSPLUS
#ifdef __cplusplus
}
#endif
#endif

#ifdef NANOSVG_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NSVG_PI (3.14159265358979323846264338327f)
#define NSVG_KAPPA90 (0.5522847493f)	// Length proportional to radius of a cubic bezier handle for 90deg arcs.

#define NSVG_ALIGN_MIN 0
#define NSVG_ALIGN_MID 1
#define NSVG_ALIGN_MAX 2
#define NSVG_ALIGN_NONE 0
#define NSVG_ALIGN_MEET 1
#define NSVG_ALIGN_SLICE 2

#define NSVG_NOTUSED(v) do { (void)(1 ? (void)0 : ( (void)(v) ) ); } while(0)
#define NSVG_RGB(r, g, b) (((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16))

#ifdef _MSC_VER
	#pragma warning (disable: 4996) // Switch off security warnings
	#pragma warning (disable: 4100) // Switch off unreferenced formal parameter warnings
	#ifdef __cplusplus
	#define NSVG_INLINE inline
	#else
	#define NSVG_INLINE
	#endif
#else
	#define NSVG_INLINE inline
#endif


static int nsvg__isspace(char c)
{
	return strchr(" \t\n\v\f\r", c) != 0;
}

static int nsvg__isdigit(char c)
{
	return c >= '0' && c <= '9';
}

static NSVG_INLINE float nsvg__minf(float a, float b) { return a < b ? a : b; }
static NSVG_INLINE float nsvg__maxf(float a, float b) { return a > b ? a : b; }


// Simple XML parser

#define NSVG_XML_TAG 1
#define NSVG_XML_CONTENT 2
#define NSVG_XML_MAX_ATTRIBS 256

static void nsvg__parseContent(char* s,
							   void (*contentCb)(void* ud, const char* s),
							   void* ud)
{
	// Trim start white spaces
	while (*s && nsvg__isspace(*s)) s++;
	if (!*s) return;

	if (contentCb)
		(*contentCb)(ud, s);
}

static void nsvg__parseElement(char* s,
							   void (*startelCb)(void* ud, const char* el, const char** attr),
							   void (*endelCb)(void* ud, const char* el),
							   void* ud)
{
	const char* attr[NSVG_XML_MAX_ATTRIBS];
	int nattr = 0;
	char* name;
	int start = 0;
	int end = 0;
	char quote;

	// Skip white space after the '<'
	while (*s && nsvg__isspace(*s)) s++;

	// Check if the tag is end tag
	if (*s == '/') {
		s++;
		end = 1;
	} else {
		start = 1;
	}

	// Skip comments, data and preprocessor stuff.
	if (!*s || *s == '?' || *s == '!')
		return;

	// Get tag name
	name = s;
	while (*s && !nsvg__isspace(*s)) s++;
	if (*s) { *s++ = '\0'; }

	// Get attribs
	while (!end && *s && nattr < NSVG_XML_MAX_ATTRIBS-3) {
		char* name = NULL;
		char* value = NULL;

		// Skip white space before the attrib name
		while (*s && nsvg__isspace(*s)) s++;
		if (!*s) break;
		if (*s == '/') {
			end = 1;
			break;
		}
		name = s;
		// Find end of the attrib name.
		while (*s && !nsvg__isspace(*s) && *s != '=') s++;
		if (*s) { *s++ = '\0'; }
		// Skip until the beginning of the value.
		while (*s && *s != '\"' && *s != '\'') s++;
		if (!*s) break;
		quote = *s;
		s++;
		// Store value and find the end of it.
		value = s;
		while (*s && *s != quote) s++;
		if (*s) { *s++ = '\0'; }

		// Store only well formed attributes
		if (name && value) {
			attr[nattr++] = name;
			attr[nattr++] = value;
		}
	}

	// List terminator
	attr[nattr++] = 0;
	attr[nattr++] = 0;

	// Call callbacks.
	if (start && startelCb)
		(*startelCb)(ud, name, attr);
	if (end && endelCb)
		(*endelCb)(ud, name);
}

int nsvg__parseXML(char* input,
				   void (*startelCb)(void* ud, const char* el, const char** attr),
				   void (*endelCb)(void* ud, const char* el),
				   void (*contentCb)(void* ud, const char* s),
				   void* ud)
{
	char* s = input;
	char* mark = s;
	int state = NSVG_XML_CONTENT;
	while (*s) {
		if (*s == '<' && state == NSVG_XML_CONTENT) {
			// Start of a tag
			*s++ = '\0';
			nsvg__parseContent(mark, contentCb, ud);
			mark = s;
			state = NSVG_XML_TAG;
		} else if (*s == '>' && state == NSVG_XML_TAG) {
			// Start of a content or new tag.
			*s++ = '\0';
			nsvg__parseElement(mark, startelCb, endelCb, ud);
			mark = s;
			state = NSVG_XML_CONTENT;
		} else {
			s++;
		}
	}

	return 1;
}


/* Simple SVG parser. */

#define NSVG_MAX_ATTR 128

enum NSVGgradientUnits {
	NSVG_USER_SPACE = 0,
	NSVG_OBJECT_SPACE = 1
};

#define NSVG_MAX_DASHES 8
#define NSVG_MAX_CLASSES 32

enum NSVGunits {
	NSVG_UNITS_USER,
	NSVG_UNITS_PX,
	NSVG_UNITS_PT,
	NSVG_UNITS_PC,
	NSVG_UNITS_MM,
	NSVG_UNITS_CM,
	NSVG_UNITS_IN,
	NSVG_UNITS_PERCENT,
	NSVG_UNITS_EM,
	NSVG_UNITS_EX
};

typedef struct NSVGcoordinate {
	float value;
	int units;
} NSVGcoordinate;

typedef struct NSVGlinearData {
	NSVGcoordinate x1, y1, x2, y2;
} NSVGlinearData;

typedef struct NSVGradialData {
	NSVGcoordinate cx, cy, r, fx, fy;
} NSVGradialData;

typedef struct NSVGgradientData
{
	char id[64];
	char ref[64];
	signed char type;
	union {
		NSVGlinearData linear;
		NSVGradialData radial;
	};
	char spread;
	char units;
	float xform[6];
	int nstops;
	NSVGgradientStop* stops;
	struct NSVGgradientData* next;
} NSVGgradientData;

typedef struct NSVGattrib
{
	char id[64];
	float xform[6];
	unsigned int fillColor;
	unsigned int strokeColor;
	float opacity;
	float fillOpacity;
	float strokeOpacity;
	char fillGradient[64];
	char strokeGradient[64];
	float strokeWidth;
	float strokeDashOffset;
	float strokeDashArray[NSVG_MAX_DASHES];
	int strokeDashCount;
	char strokeLineJoin;
	char strokeLineCap;
	float miterLimit;
	char fillRule;
	float fontSize;
	unsigned int stopColor;
	float stopOpacity;
	float stopOffset;
	char hasFill;
	char hasStroke;
	char visible;
    unsigned char paintOrder;
} NSVGattrib;

typedef struct NSVGstyleDeclaration
{
	char* className;
	char* propertiesText;
	struct NSVGstyleDeclaration* next;
} NSVGstyleDeclaration;

typedef struct NSVGparser
{
	NSVGattrib attr[NSVG_MAX_ATTR];
	int attrHead;
	float* pts;
	int npts;
	int cpts;
	NSVGpath* plist;
	NSVGimage* image;
	NSVGstyleDeclaration* styles;
	NSVGgradientData* gradients;
	NSVGshape* shapesTail;
	float viewMinx, viewMiny, viewWidth, viewHeight;
	int alignX, alignY, alignType;
	float dpi;
	char pathFlag;
	char defsFlag;
	char styleFlag;
} NSVGparser;

static void nsvg__xformIdentity(float* t)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetTranslation(float* t, float tx, float ty)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = tx; t[5] = ty;
}

static void nsvg__xformSetScale(float* t, float sx, float sy)
{
	t[0] = sx; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = sy;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetSkewX(float* t, float a)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = tanf(a); t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetSkewY(float* t, float a)
{
	t[0] = 1.0f; t[1] = tanf(a);
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetRotation(float* t, float a)
{
	float cs = cosf(a), sn = sinf(a);
	t[0] = cs; t[1] = sn;
	t[2] = -sn; t[3] = cs;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformMultiply(float* t, float* s)
{
	float t0 = t[0] * s[0] + t[1] * s[2];
	float t2 = t[2] * s[0] + t[3] * s[2];
	float t4 = t[4] * s[0] + t[5] * s[2] + s[4];
	t[1] = t[0] * s[1] + t[1] * s[3];
	t[3] = t[2] * s[1] + t[3] * s[3];
	t[5] = t[4] * s[1] + t[5] * s[3] + s[5];
	t[0] = t0;
	t[2] = t2;
	t[4] = t4;
}

static void nsvg__xformInverse(float* inv, float* t)
{
	double invdet, det = (double)t[0] * t[3] - (double)t[2] * t[1];
	if (det > -1e-6 && det < 1e-6) {
		nsvg__xformIdentity(t);
		return;
	}
	invdet = 1.0 / det;
	inv[0] = (float)(t[3] * invdet);
	inv[2] = (float)(-t[2] * invdet);
	inv[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
	inv[1] = (float)(-t[1] * invdet);
	inv[3] = (float)(t[0] * invdet);
	inv[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
}

static void nsvg__xformPremultiply(float* t, float* s)
{
	float s2[6];
	memcpy(s2, s, sizeof(float)*6);
	nsvg__xformMultiply(s2, t);
	memcpy(t, s2, sizeof(float)*6);
}

static void nsvg__xformPoint(float* dx, float* dy, float x, float y, float* t)
{
	*dx = x*t[0] + y*t[2] + t[4];
	*dy = x*t[1] + y*t[3] + t[5];
}

static void nsvg__xformVec(float* dx, float* dy, float x, float y, float* t)
{
	*dx = x*t[0] + y*t[2];
	*dy = x*t[1] + y*t[3];
}

#define NSVG_EPSILON (1e-12)

static int nsvg__ptInBounds(float* pt, float* bounds)
{
	return pt[0] >= bounds[0] && pt[0] <= bounds[2] && pt[1] >= bounds[1] && pt[1] <= bounds[3];
}


static double nsvg__evalBezier(double t, double p0, double p1, double p2, double p3)
{
	double it = 1.0-t;
	return it*it*it*p0 + 3.0*it*it*t*p1 + 3.0*it*t*t*p2 + t*t*t*p3;
}

static void nsvg__curveBounds(float* bounds, float* curve)
{
	int i, j, count;
	double roots[2], a, b, c, b2ac, t, v;
	float* v0 = &curve[0];
	float* v1 = &curve[2];
	float* v2 = &curve[4];
	float* v3 = &curve[6];

	// Start the bounding box by end points
	bounds[0] = nsvg__minf(v0[0], v3[0]);
	bounds[1] = nsvg__minf(v0[1], v3[1]);
	bounds[2] = nsvg__maxf(v0[0], v3[0]);
	bounds[3] = nsvg__maxf(v0[1], v3[1]);

	// Bezier curve fits inside the convex hull of it's control points.
	// If control points are inside the bounds, we're done.
	if (nsvg__ptInBounds(v1, bounds) && nsvg__ptInBounds(v2, bounds))
		return;

	// Add bezier curve inflection points in X and Y.
	for (i = 0; i < 2; i++) {
		a = -3.0 * v0[i] + 9.0 * v1[i] - 9.0 * v2[i] + 3.0 * v3[i];
		b = 6.0 * v0[i] - 12.0 * v1[i] + 6.0 * v2[i];
		c = 3.0 * v1[i] - 3.0 * v0[i];
		count = 0;
		if (fabs(a) < NSVG_EPSILON) {
			if (fabs(b) > NSVG_EPSILON) {
				t = -c / b;
				if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
					roots[count++] = t;
			}
		} else {
			b2ac = b*b - 4.0*c*a;
			if (b2ac > NSVG_EPSILON) {
				t = (-b + sqrt(b2ac)) / (2.0 * a);
				if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
					roots[count++] = t;
				t = (-b - sqrt(b2ac)) / (2.0 * a);
				if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
					roots[count++] = t;
			}
		}
		for (j = 0; j < count; j++) {
			v = nsvg__evalBezier(roots[j], v0[i], v1[i], v2[i], v3[i]);
			bounds[0+i] = nsvg__minf(bounds[0+i], (float)v);
			bounds[2+i] = nsvg__maxf(bounds[2+i], (float)v);
		}
	}
}

static unsigned char nsvg__encodePaintOrder(enum NSVGpaintOrder a, enum NSVGpaintOrder b, enum NSVGpaintOrder c) {
    return (a & 0x03) | ((b & 0x03) << 2) | ((c & 0x03) << 4);
}

static NSVGparser* nsvg__createParser(void)
{
	NSVGparser* p;
	p = (NSVGparser*)malloc(sizeof(NSVGparser));
	if (p == NULL) goto error;
	memset(p, 0, sizeof(NSVGparser));

	p->image = (NSVGimage*)malloc(sizeof(NSVGimage));
	if (p->image == NULL) goto error;
	memset(p->image, 0, sizeof(NSVGimage));

	// Init style
	nsvg__xformIdentity(p->attr[0].xform);
	memset(p->attr[0].id, 0, sizeof p->attr[0].id);
	p->attr[0].fillColor = NSVG_RGB(0,0,0);
	p->attr[0].strokeColor = NSVG_RGB(0,0,0);
	p->attr[0].opacity = 1;
	p->attr[0].fillOpacity = 1;
	p->attr[0].strokeOpacity = 1;
	p->attr[0].stopOpacity = 1;
	p->attr[0].strokeWidth = 1;
	p->attr[0].strokeLineJoin = NSVG_JOIN_MITER;
	p->attr[0].strokeLineCap = NSVG_CAP_BUTT;
	p->attr[0].miterLimit = 4;
	p->attr[0].fillRule = NSVG_FILLRULE_NONZERO;
	p->attr[0].hasFill = 1;
	p->attr[0].visible = 1;
    p->attr[0].paintOrder = nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS);

	return p;

error:
	if (p) {
		if (p->image) free(p->image);
		free(p);
	}
	return NULL;
}
static void nsvg__deleteStyles(NSVGstyleDeclaration* style) {
	while (style) {
		NSVGstyleDeclaration* next = style->next;
		free(style->className);
		free(style->propertiesText);
		free(style);
		style = next;
	}
}

static void nsvg__deletePaths(NSVGpath* path)
{
	while (path) {
		NSVGpath *next = path->next;
		if (path->pts != NULL)
			free(path->pts);
		free(path);
		path = next;
	}
}

static void nsvg__deletePaint(NSVGpaint* paint)
{
	if (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT)
		free(paint->gradient);
}

static void nsvg__deleteGradientData(NSVGgradientData* grad)
{
	NSVGgradientData* next;
	while (grad != NULL) {
		next = grad->next;
		free(grad->stops);
		free(grad);
		grad = next;
	}
}

static void nsvg__deleteParser(NSVGparser* p)
{
	if (p != NULL) {
		nsvg__deleteStyles(p->styles);
		nsvg__deletePaths(p->plist);
		nsvg__deleteGradientData(p->gradients);
		nsvgDelete(p->image);
		free(p->pts);
		free(p);
	}
}

static void nsvg__resetPath(NSVGparser* p)
{
	p->npts = 0;
}

static void nsvg__addPoint(NSVGparser* p, float x, float y)
{
	if (p->npts+1 > p->cpts) {
		p->cpts = p->cpts ? p->cpts*2 : 8;
		p->pts = (float*)realloc(p->pts, p->cpts*2*sizeof(float));
		if (!p->pts) return;
	}
	p->pts[p->npts*2+0] = x;
	p->pts[p->npts*2+1] = y;
	p->npts++;
}

static void nsvg__moveTo(NSVGparser* p, float x, float y)
{
	if (p->npts > 0) {
		p->pts[(p->npts-1)*2+0] = x;
		p->pts[(p->npts-1)*2+1] = y;
	} else {
		nsvg__addPoint(p, x, y);
	}
}

static void nsvg__lineTo(NSVGparser* p, float x, float y)
{
	float px,py, dx,dy;
	if (p->npts > 0) {
		px = p->pts[(p->npts-1)*2+0];
		py = p->pts[(p->npts-1)*2+1];
		dx = x - px;
		dy = y - py;
		nsvg__addPoint(p, px + dx/3.0f, py + dy/3.0f);
		nsvg__addPoint(p, x - dx/3.0f, y - dy/3.0f);
		nsvg__addPoint(p, x, y);
	}
}

static void nsvg__cubicBezTo(NSVGparser* p, float cpx1, float cpy1, float cpx2, float cpy2, float x, float y)
{
	if (p->npts > 0) {
		nsvg__addPoint(p, cpx1, cpy1);
		nsvg__addPoint(p, cpx2, cpy2);
		nsvg__addPoint(p, x, y);
	}
}

static NSVGattrib* nsvg__getAttr(NSVGparser* p)
{
	return &p->attr[p->attrHead];
}

static void nsvg__pushAttr(NSVGparser* p)
{
	if (p->attrHead < NSVG_MAX_ATTR-1) {
		p->attrHead++;
		memcpy(&p->attr[p->attrHead], &p->attr[p->attrHead-1], sizeof(NSVGattrib));
	}
}

static void nsvg__popAttr(NSVGparser* p)
{
	if (p->attrHead > 0)
		p->attrHead--;
}

static float nsvg__actualOrigX(NSVGparser* p)
{
	return p->viewMinx;
}

static float nsvg__actualOrigY(NSVGparser* p)
{
	return p->viewMiny;
}

static float nsvg__actualWidth(NSVGparser* p)
{
	return p->viewWidth;
}

static float nsvg__actualHeight(NSVGparser* p)
{
	return p->viewHeight;
}

static float nsvg__actualLength(NSVGparser* p)
{
	float w = nsvg__actualWidth(p), h = nsvg__actualHeight(p);
	return sqrtf(w*w + h*h) / sqrtf(2.0f);
}

static float nsvg__convertToPixels(NSVGparser* p, NSVGcoordinate c, float orig, float length)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	switch (c.units) {
		case NSVG_UNITS_USER:		return c.value;
		case NSVG_UNITS_PX:			return c.value;
		case NSVG_UNITS_PT:			return c.value / 72.0f * p->dpi;
		case NSVG_UNITS_PC:			return c.value / 6.0f * p->dpi;
		case NSVG_UNITS_MM:			return c.value / 25.4f * p->dpi;
		case NSVG_UNITS_CM:			return c.value / 2.54f * p->dpi;
		case NSVG_UNITS_IN:			return c.value * p->dpi;
		case NSVG_UNITS_EM:			return c.value * attr->fontSize;
		case NSVG_UNITS_EX:			return c.value * attr->fontSize * 0.52f; // x-height of Helvetica.
		case NSVG_UNITS_PERCENT:	return orig + c.value / 100.0f * length;
		default:					return c.value;
	}
	return c.value;
}

static NSVGgradientData* nsvg__findGradientData(NSVGparser* p, const char* id)
{
	NSVGgradientData* grad = p->gradients;
	if (id == NULL || *id == '\0')
		return NULL;
	while (grad != NULL) {
		if (strcmp(grad->id, id) == 0)
			return grad;
		grad = grad->next;
	}
	return NULL;
}

static NSVGgradient* nsvg__createGradient(NSVGparser* p, const char* id, const float* localBounds, float *xform, signed char* paintType)
{
	NSVGgradientData* data = NULL;
	NSVGgradientData* ref = NULL;
	NSVGgradientStop* stops = NULL;
	NSVGgradient* grad;
	float ox, oy, sw, sh, sl;
	int nstops = 0;
	int refIter;

	data = nsvg__findGradientData(p, id);
	if (data == NULL) return NULL;

	// TODO: use ref to fill in all unset values too.
	ref = data;
	refIter = 0;
	while (ref != NULL) {
		NSVGgradientData* nextRef = NULL;
		if (stops == NULL && ref->stops != NULL) {
			stops = ref->stops;
			nstops = ref->nstops;
			break;
		}
		nextRef = nsvg__findGradientData(p, ref->ref);
		if (nextRef == ref) break; // prevent infite loops on malformed data
		ref = nextRef;
		refIter++;
		if (refIter > 32) break; // prevent infite loops on malformed data
	}
	if (stops == NULL) return NULL;

	grad = (NSVGgradient*)malloc(sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*(nstops-1));
	if (grad == NULL) return NULL;

	// The shape width and height.
	if (data->units == NSVG_OBJECT_SPACE) {
		ox = localBounds[0];
		oy = localBounds[1];
		sw = localBounds[2] - localBounds[0];
		sh = localBounds[3] - localBounds[1];
	} else {
		ox = nsvg__actualOrigX(p);
		oy = nsvg__actualOrigY(p);
		sw = nsvg__actualWidth(p);
		sh = nsvg__actualHeight(p);
	}
	sl = sqrtf(sw*sw + sh*sh) / sqrtf(2.0f);

	if (data->type == NSVG_PAINT_LINEAR_GRADIENT) {
		float x1, y1, x2, y2, dx, dy;
		x1 = nsvg__convertToPixels(p, data->linear.x1, ox, sw);
		y1 = nsvg__convertToPixels(p, data->linear.y1, oy, sh);
		x2 = nsvg__convertToPixels(p, data->linear.x2, ox, sw);
		y2 = nsvg__convertToPixels(p, data->linear.y2, oy, sh);
		// Calculate transform aligned to the line
		dx = x2 - x1;
		dy = y2 - y1;
		grad->xform[0] = dy; grad->xform[1] = -dx;
		grad->xform[2] = dx; grad->xform[3] = dy;
		grad->xform[4] = x1; grad->xform[5] = y1;
	} else {
		float cx, cy, fx, fy, r;
		cx = nsvg__convertToPixels(p, data->radial.cx, ox, sw);
		cy = nsvg__convertToPixels(p, data->radial.cy, oy, sh);
		fx = nsvg__convertToPixels(p, data->radial.fx, ox, sw);
		fy = nsvg__convertToPixels(p, data->radial.fy, oy, sh);
		r = nsvg__convertToPixels(p, data->radial.r, 0, sl);
		// Calculate transform aligned to the circle
		grad->xform[0] = r; grad->xform[1] = 0;
		grad->xform[2] = 0; grad->xform[3] = r;
		grad->xform[4] = cx; grad->xform[5] = cy;
		grad->fx = fx / r;
		grad->fy = fy / r;
	}

	nsvg__xformMultiply(grad->xform, data->xform);
	nsvg__xformMultiply(grad->xform, xform);

	grad->spread = data->spread;
	memcpy(grad->stops, stops, nstops*sizeof(NSVGgradientStop));
	grad->nstops = nstops;

	*paintType = data->type;

	return grad;
}

static float nsvg__getAverageScale(float* t)
{
	float sx = sqrtf(t[0]*t[0] + t[2]*t[2]);
	float sy = sqrtf(t[1]*t[1] + t[3]*t[3]);
	return (sx + sy) * 0.5f;
}

static void nsvg__getLocalBounds(float* bounds, NSVGshape *shape, float* xform)
{
	NSVGpath* path;
	float curve[4*2], curveBounds[4];
	int i, first = 1;
	for (path = shape->paths; path != NULL; path = path->next) {
		nsvg__xformPoint(&curve[0], &curve[1], path->pts[0], path->pts[1], xform);
		for (i = 0; i < path->npts-1; i += 3) {
			nsvg__xformPoint(&curve[2], &curve[3], path->pts[(i+1)*2], path->pts[(i+1)*2+1], xform);
			nsvg__xformPoint(&curve[4], &curve[5], path->pts[(i+2)*2], path->pts[(i+2)*2+1], xform);
			nsvg__xformPoint(&curve[6], &curve[7], path->pts[(i+3)*2], path->pts[(i+3)*2+1], xform);
			nsvg__curveBounds(curveBounds, curve);
			if (first) {
				bounds[0] = curveBounds[0];
				bounds[1] = curveBounds[1];
				bounds[2] = curveBounds[2];
				bounds[3] = curveBounds[3];
				first = 0;
			} else {
				bounds[0] = nsvg__minf(bounds[0], curveBounds[0]);
				bounds[1] = nsvg__minf(bounds[1], curveBounds[1]);
				bounds[2] = nsvg__maxf(bounds[2], curveBounds[2]);
				bounds[3] = nsvg__maxf(bounds[3], curveBounds[3]);
			}
			curve[0] = curve[6];
			curve[1] = curve[7];
		}
	}
}

static void nsvg__addShape(NSVGparser* p)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	float scale = 1.0f;
	NSVGshape* shape;
	NSVGpath* path;
	int i;

	if (p->plist == NULL)
		return;

	shape = (NSVGshape*)malloc(sizeof(NSVGshape));
	if (shape == NULL) goto error;
	memset(shape, 0, sizeof(NSVGshape));

	memcpy(shape->id, attr->id, sizeof shape->id);
	memcpy(shape->fillGradient, attr->fillGradient, sizeof shape->fillGradient);
	memcpy(shape->strokeGradient, attr->strokeGradient, sizeof shape->strokeGradient);
	memcpy(shape->xform, attr->xform, sizeof shape->xform);
	scale = nsvg__getAverageScale(attr->xform);
	shape->strokeWidth = attr->strokeWidth * scale;
	shape->strokeDashOffset = attr->strokeDashOffset * scale;
	shape->strokeDashCount = (char)attr->strokeDashCount;
	for (i = 0; i < attr->strokeDashCount; i++)
		shape->strokeDashArray[i] = attr->strokeDashArray[i] * scale;
	shape->strokeLineJoin = attr->strokeLineJoin;
	shape->strokeLineCap = attr->strokeLineCap;
	shape->miterLimit = attr->miterLimit;
	shape->fillRule = attr->fillRule;
	shape->opacity = attr->opacity;
    shape->paintOrder = attr->paintOrder;

	shape->paths = p->plist;
	p->plist = NULL;

	// Calculate shape bounds
	shape->bounds[0] = shape->paths->bounds[0];
	shape->bounds[1] = shape->paths->bounds[1];
	shape->bounds[2] = shape->paths->bounds[2];
	shape->bounds[3] = shape->paths->bounds[3];
	for (path = shape->paths->next; path != NULL; path = path->next) {
		shape->bounds[0] = nsvg__minf(shape->bounds[0], path->bounds[0]);
		shape->bounds[1] = nsvg__minf(shape->bounds[1], path->bounds[1]);
		shape->bounds[2] = nsvg__maxf(shape->bounds[2], path->bounds[2]);
		shape->bounds[3] = nsvg__maxf(shape->bounds[3], path->bounds[3]);
	}

	// Set fill
	if (attr->hasFill == 0) {
		shape->fill.type = NSVG_PAINT_NONE;
	} else if (attr->hasFill == 1) {
		shape->fill.type = NSVG_PAINT_COLOR;
		shape->fill.color = attr->fillColor;
		shape->fill.color |= (unsigned int)(attr->fillOpacity*255) << 24;
	} else if (attr->hasFill == 2) {
		shape->fill.type = NSVG_PAINT_UNDEF;
	}

	// Set stroke
	if (attr->hasStroke == 0) {
		shape->stroke.type = NSVG_PAINT_NONE;
	} else if (attr->hasStroke == 1) {
		shape->stroke.type = NSVG_PAINT_COLOR;
		shape->stroke.color = attr->strokeColor;
		shape->stroke.color |= (unsigned int)(attr->strokeOpacity*255) << 24;
	} else if (attr->hasStroke == 2) {
		shape->stroke.type = NSVG_PAINT_UNDEF;
	}

	// Set flags
	shape->flags = (attr->visible ? NSVG_FLAGS_VISIBLE : 0x00);

	// Add to tail
	if (p->image->shapes == NULL)
		p->image->shapes = shape;
	else
		p->shapesTail->next = shape;
	p->shapesTail = shape;

	return;

error:
	if (shape) free(shape);
}

static void nsvg__addPath(NSVGparser* p, char closed)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	NSVGpath* path = NULL;
	float bounds[4];
	float* curve;
	int i;

	if (p->npts < 4)
		return;

	if (closed)
		nsvg__lineTo(p, p->pts[0], p->pts[1]);

	// Expect 1 + N*3 points (N = number of cubic bezier segments).
	if ((p->npts % 3) != 1)
		return;

	path = (NSVGpath*)malloc(sizeof(NSVGpath));
	if (path == NULL) goto error;
	memset(path, 0, sizeof(NSVGpath));

	path->pts = (float*)malloc(p->npts*2*sizeof(float));
	if (path->pts == NULL) goto error;
	path->closed = closed;
	path->npts = p->npts;

	// Transform path.
	for (i = 0; i < p->npts; ++i)
		nsvg__xformPoint(&path->pts[i*2], &path->pts[i*2+1], p->pts[i*2], p->pts[i*2+1], attr->xform);

	// Find bounds
	for (i = 0; i < path->npts-1; i += 3) {
		curve = &path->pts[i*2];
		nsvg__curveBounds(bounds, curve);
		if (i == 0) {
			path->bounds[0] = bounds[0];
			path->bounds[1] = bounds[1];
			path->bounds[2] = bounds[2];
			path->bounds[3] = bounds[3];
		} else {
			path->bounds[0] = nsvg__minf(path->bounds[0], bounds[0]);
			path->bounds[1] = nsvg__minf(path->bounds[1], bounds[1]);
			path->bounds[2] = nsvg__maxf(path->bounds[2], bounds[2]);
			path->bounds[3] = nsvg__maxf(path->bounds[3], bounds[3]);
		}
	}

	path->next = p->plist;
	p->plist = path;

	return;

error:
	if (path != NULL) {
		if (path->pts != NULL) free(path->pts);
		free(path);
	}
}

// We roll our own string to float because the std library one uses locale and messes things up.
static double nsvg__atof(const char* s)
{
	char* cur = (char*)s;
	char* end = NULL;
	double res = 0.0, sign = 1.0;
	long long intPart = 0, fracPart = 0;
	char hasIntPart = 0, hasFracPart = 0;

	// Parse optional sign
	if (*cur == '+') {
		cur++;
	} else if (*cur == '-') {
		sign = -1;
		cur++;
	}

	// Parse integer part
	if (nsvg__isdigit(*cur)) {
		// Parse digit sequence
		intPart = strtoll(cur, &end, 10);
		if (cur != end) {
			res = (double)intPart;
			hasIntPart = 1;
			cur = end;
		}
	}

	// Parse fractional part.
	if (*cur == '.') {
		cur++; // Skip '.'
		if (nsvg__isdigit(*cur)) {
			// Parse digit sequence
			fracPart = strtoll(cur, &end, 10);
			if (cur != end) {
				res += (double)fracPart / pow(10.0, (double)(end - cur));
				hasFracPart = 1;
				cur = end;
			}
		}
	}

	// A valid number should have integer or fractional part.
	if (!hasIntPart && !hasFracPart)
		return 0.0;

	// Parse optional exponent
	if (*cur == 'e' || *cur == 'E') {
		long expPart = 0;
		cur++; // skip 'E'
		expPart = strtol(cur, &end, 10); // Parse digit sequence with sign
		if (cur != end) {
			res *= pow(10.0, (double)expPart);
		}
	}

	return res * sign;
}


static const char* nsvg__parseNumber(const char* s, char* it, const int size)
{
	const int last = size-1;
	int i = 0;

	// sign
	if (*s == '-' || *s == '+') {
		if (i < last) it[i++] = *s;
		s++;
	}
	// integer part
	while (*s && nsvg__isdigit(*s)) {
		if (i < last) it[i++] = *s;
		s++;
	}
	if (*s == '.') {
		// decimal point
		if (i < last) it[i++] = *s;
		s++;
		// fraction part
		while (*s && nsvg__isdigit(*s)) {
			if (i < last) it[i++] = *s;
			s++;
		}
	}
	// exponent
	if ((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x')) {
		if (i < last) it[i++] = *s;
		s++;
		if (*s == '-' || *s == '+') {
			if (i < last) it[i++] = *s;
			s++;
		}
		while (*s && nsvg__isdigit(*s)) {
			if (i < last) it[i++] = *s;
			s++;
		}
	}
	it[i] = '\0';

	return s;
}

static const char* nsvg__getNextPathItemWhenArcFlag(const char* s, char* it)
{
	it[0] = '\0';
	while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
	if (!*s) return s;
	if (*s == '0' || *s == '1') {
		it[0] = *s++;
		it[1] = '\0';
		return s;
	}
	return s;
}

static const char* nsvg__getNextPathItem(const char* s, char* it)
{
	it[0] = '\0';
	// Skip white spaces and commas
	while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
	if (!*s) return s;
	if (*s == '-' || *s == '+' || *s == '.' || nsvg__isdigit(*s)) {
		s = nsvg__parseNumber(s, it, 64);
	} else {
		// Parse command
		it[0] = *s++;
		it[1] = '\0';
		return s;
	}

	return s;
}

static unsigned int nsvg__parseColorHex(const char* str)
{
	unsigned int r=0, g=0, b=0;
	if (sscanf(str, "#%2x%2x%2x", &r, &g, &b) == 3 )		// 2 digit hex
		return NSVG_RGB(r, g, b);
	if (sscanf(str, "#%1x%1x%1x", &r, &g, &b) == 3 )		// 1 digit hex, e.g. #abc -> 0xccbbaa
		return NSVG_RGB(r*17, g*17, b*17);			// same effect as (r<<4|r), (g<<4|g), ..
	return NSVG_RGB(128, 128, 128);
}

// Parse rgb color. The pointer 'str' must point at "rgb(" (4+ characters).
// This function returns gray (rgb(128, 128, 128) == '#808080') on parse errors
// for backwards compatibility. Note: other image viewers return black instead.

static unsigned int nsvg__parseColorRGB(const char* str)
{
	int i;
	unsigned int rgbi[3];
	float rgbf[3];
	// try decimal integers first
	if (sscanf(str, "rgb(%u, %u, %u)", &rgbi[0], &rgbi[1], &rgbi[2]) != 3) {
		// integers failed, try percent values (float, locale independent)
		const char delimiter[3] = {',', ',', ')'};
		str += 4; // skip "rgb("
		for (i = 0; i < 3; i++) {
			while (*str && (nsvg__isspace(*str))) str++; 	// skip leading spaces
			if (*str == '+') str++;				// skip '+' (don't allow '-')
			if (!*str) break;
			rgbf[i] = nsvg__atof(str);

			// Note 1: it would be great if nsvg__atof() returned how many
			// bytes it consumed but it doesn't. We need to skip the number,
			// the '%' character, spaces, and the delimiter ',' or ')'.

			// Note 2: The following code does not allow values like "33.%",
			// i.e. a decimal point w/o fractional part, but this is consistent
			// with other image viewers, e.g. firefox, chrome, eog, gimp.

			while (*str && nsvg__isdigit(*str)) str++;		// skip integer part
			if (*str == '.') {
				str++;
				if (!nsvg__isdigit(*str)) break;		// error: no digit after '.'
				while (*str && nsvg__isdigit(*str)) str++;	// skip fractional part
			}
			if (*str == '%') str++; else break;
			while (*str && nsvg__isspace(*str)) str++;
			if (*str == delimiter[i]) str++;
			else break;
		}
		if (i == 3) {
			rgbi[0] = roundf(rgbf[0] * 2.55f);
			rgbi[1] = roundf(rgbf[1] * 2.55f);
			rgbi[2] = roundf(rgbf[2] * 2.55f);
		} else {
			rgbi[0] = rgbi[1] = rgbi[2] = 128;
		}
	}
	// clip values as the CSS spec requires
	for (i = 0; i < 3; i++) {
		if (rgbi[i] > 255) rgbi[i] = 255;
	}
	return NSVG_RGB(rgbi[0], rgbi[1], rgbi[2]);
}

typedef struct NSVGNamedColor {
	const char* name;
	unsigned int color;
} NSVGNamedColor;

NSVGNamedColor nsvg__colors[] = {

	{ "red", NSVG_RGB(255, 0, 0) },
	{ "green", NSVG_RGB( 0, 128, 0) },
	{ "blue", NSVG_RGB( 0, 0, 255) },
	{ "yellow", NSVG_RGB(255, 255, 0) },
	{ "cyan", NSVG_RGB( 0, 255, 255) },
	{ "magenta", NSVG_RGB(255, 0, 255) },
	{ "black", NSVG_RGB( 0, 0, 0) },
	{ "grey", NSVG_RGB(128, 128, 128) },
	{ "gray", NSVG_RGB(128, 128, 128) },
	{ "white", NSVG_RGB(255, 255, 255) },

#ifdef NANOSVG_ALL_COLOR_KEYWORDS
	{ "aliceblue", NSVG_RGB(240, 248, 255) },
	{ "antiquewhite", NSVG_RGB(250, 235, 215) },
	{ "aqua", NSVG_RGB( 0, 255, 255) },
	{ "aquamarine", NSVG_RGB(127, 255, 212) },
	{ "azure", NSVG_RGB(240, 255, 255) },
	{ "beige", NSVG_RGB(245, 245, 220) },
	{ "bisque", NSVG_RGB(255, 228, 196) },
	{ "blanchedalmond", NSVG_RGB(255, 235, 205) },
	{ "blueviolet", NSVG_RGB(138, 43, 226) },
	{ "brown", NSVG_RGB(165, 42, 42) },
	{ "burlywood", NSVG_RGB(222, 184, 135) },
	{ "cadetblue", NSVG_RGB( 95, 158, 160) },
	{ "chartreuse", NSVG_RGB(127, 255, 0) },
	{ "chocolate", NSVG_RGB(210, 105, 30) },
	{ "coral", NSVG_RGB(255, 127, 80) },
	{ "cornflowerblue", NSVG_RGB(100, 149, 237) },
	{ "cornsilk", NSVG_RGB(255, 248, 220) },
	{ "crimson", NSVG_RGB(220, 20, 60) },
	{ "darkblue", NSVG_RGB( 0, 0, 139) },
	{ "darkcyan", NSVG_RGB( 0, 139, 139) },
	{ "darkgoldenrod", NSVG_RGB(184, 134, 11) },
	{ "darkgray", NSVG_RGB(169, 169, 169) },
	{ "darkgreen", NSVG_RGB( 0, 100, 0) },
	{ "darkgrey", NSVG_RGB(169, 169, 169) },
	{ "darkkhaki", NSVG_RGB(189, 183, 107) },
	{ "darkmagenta", NSVG_RGB(139, 0, 139) },
	{ "darkolivegreen", NSVG_RGB( 85, 107, 47) },
	{ "darkorange", NSVG_RGB(255, 140, 0) },
	{ "darkorchid", NSVG_RGB(153, 50, 204) },
	{ "darkred", NSVG_RGB(139, 0, 0) },
	{ "darksalmon", NSVG_RGB(233, 150, 122) },
	{ "darkseagreen", NSVG_RGB(143, 188, 143) },
	{ "darkslateblue", NSVG_RGB( 72, 61, 139) },
	{ "darkslategray", NSVG_RGB( 47, 79, 79) },
	{ "darkslategrey", NSVG_RGB( 47, 79, 79) },
	{ "darkturquoise", NSVG_RGB( 0, 206, 209) },
	{ "darkviolet", NSVG_RGB(148, 0, 211) },
	{ "deeppink", NSVG_RGB(255, 20, 147) },
	{ "deepskyblue", NSVG_RGB( 0, 191, 255) },
	{ "dimgray", NSVG_RGB(105, 105, 105) },
	{ "dimgrey", NSVG_RGB(105, 105, 105) },
	{ "dodgerblue", NSVG_RGB( 30, 144, 255) },
	{ "firebrick", NSVG_RGB(178, 34, 34) },
	{ "floralwhite", NSVG_RGB(255, 250, 240) },
	{ "forestgreen", NSVG_RGB( 34, 139, 34) },
	{ "fuchsia", NSVG_RGB(255, 0, 255) },
	{ "gainsboro", NSVG_RGB(220, 220, 220) },
	{ "ghostwhite", NSVG_RGB(248, 248, 255) },
	{ "gold", NSVG_RGB(255, 215, 0) },
	{ "goldenrod", NSVG_RGB(218, 165, 32) },
	{ "greenyellow", NSVG_RGB(173, 255, 47) },
	{ "honeydew", NSVG_RGB(240, 255, 240) },
	{ "hotpink", NSVG_RGB(255, 105, 180) },
	{ "indianred", NSVG_RGB(205, 92, 92) },
	{ "indigo", NSVG_RGB( 75, 0, 130) },
	{ "ivory", NSVG_RGB(255, 255, 240) },
	{ "khaki", NSVG_RGB(240, 230, 140) },
	{ "lavender", NSVG_RGB(230, 230, 250) },
	{ "lavenderblush", NSVG_RGB(255, 240, 245) },
	{ "lawngreen", NSVG_RGB(124, 252, 0) },
	{ "lemonchiffon", NSVG_RGB(255, 250, 205) },
	{ "lightblue", NSVG_RGB(173, 216, 230) },
	{ "lightcoral", NSVG_RGB(240, 128, 128) },
	{ "lightcyan", NSVG_RGB(224, 255, 255) },
	{ "lightgoldenrodyellow", NSVG_RGB(250, 250, 210) },
	{ "lightgray", NSVG_RGB(211, 211, 211) },
	{ "lightgreen", NSVG_RGB(144, 238, 144) },
	{ "lightgrey", NSVG_RGB(211, 211, 211) },
	{ "lightpink", NSVG_RGB(255, 182, 193) },
	{ "lightsalmon", NSVG_RGB(255, 160, 122) },
	{ "lightseagreen", NSVG_RGB( 32, 178, 170) },
	{ "lightskyblue", NSVG_RGB(135, 206, 250) },
	{ "lightslategray", NSVG_RGB(119, 136, 153) },
	{ "lightslategrey", NSVG_RGB(119, 136, 153) },
	{ "lightsteelblue", NSVG_RGB(176, 196, 222) },
	{ "lightyellow", NSVG_RGB(255, 255, 224) },
	{ "lime", NSVG_RGB( 0, 255, 0) },
	{ "limegreen", NSVG_RGB( 50, 205, 50) },
	{ "linen", NSVG_RGB(250, 240, 230) },
	{ "maroon", NSVG_RGB(128, 0, 0) },
	{ "mediumaquamarine", NSVG_RGB(102, 205, 170) },
	{ "mediumblue", NSVG_RGB( 0, 0, 205) },
	{ "mediumorchid", NSVG_RGB(186, 85, 211) },
	{ "mediumpurple", NSVG_RGB(147, 112, 219) },
	{ "mediumseagreen", NSVG_RGB( 60, 179, 113) },
	{ "mediumslateblue", NSVG_RGB(123, 104, 238) },
	{ "mediumspringgreen", NSVG_RGB( 0, 250, 154) },
	{ "mediumturquoise", NSVG_RGB( 72, 209, 204) },
	{ "mediumvioletred", NSVG_RGB(199, 21, 133) },
	{ "midnightblue", NSVG_RGB( 25, 25, 112) },
	{ "mintcream", NSVG_RGB(245, 255, 250) },
	{ "mistyrose", NSVG_RGB(255, 228, 225) },
	{ "moccasin", NSVG_RGB(255, 228, 181) },
	{ "navajowhite", NSVG_RGB(255, 222, 173) },
	{ "navy", NSVG_RGB( 0, 0, 128) },
	{ "oldlace", NSVG_RGB(253, 245, 230) },
	{ "olive", NSVG_RGB(128, 128, 0) },
	{ "olivedrab", NSVG_RGB(107, 142, 35) },
	{ "orange", NSVG_RGB(255, 165, 0) },
	{ "orangered", NSVG_RGB(255, 69, 0) },
	{ "orchid", NSVG_RGB(218, 112, 214) },
	{ "palegoldenrod", NSVG_RGB(238, 232, 170) },
	{ "palegreen", NSVG_RGB(152, 251, 152) },
	{ "paleturquoise", NSVG_RGB(175, 238, 238) },
	{ "palevioletred", NSVG_RGB(219, 112, 147) },
	{ "papayawhip", NSVG_RGB(255, 239, 213) },
	{ "peachpuff", NSVG_RGB(255, 218, 185) },
	{ "peru", NSVG_RGB(205, 133, 63) },
	{ "pink", NSVG_RGB(255, 192, 203) },
	{ "plum", NSVG_RGB(221, 160, 221) },
	{ "powderblue", NSVG_RGB(176, 224, 230) },
	{ "purple", NSVG_RGB(128, 0, 128) },
	{ "rosybrown", NSVG_RGB(188, 143, 143) },
	{ "royalblue", NSVG_RGB( 65, 105, 225) },
	{ "saddlebrown", NSVG_RGB(139, 69, 19) },
	{ "salmon", NSVG_RGB(250, 128, 114) },
	{ "sandybrown", NSVG_RGB(244, 164, 96) },
	{ "seagreen", NSVG_RGB( 46, 139, 87) },
	{ "seashell", NSVG_RGB(255, 245, 238) },
	{ "sienna", NSVG_RGB(160, 82, 45) },
	{ "silver", NSVG_RGB(192, 192, 192) },
	{ "skyblue", NSVG_RGB(135, 206, 235) },
	{ "slateblue", NSVG_RGB(106, 90, 205) },
	{ "slategray", NSVG_RGB(112, 128, 144) },
	{ "slategrey", NSVG_RGB(112, 128, 144) },
	{ "snow", NSVG_RGB(255, 250, 250) },
	{ "springgreen", NSVG_RGB( 0, 255, 127) },
	{ "steelblue", NSVG_RGB( 70, 130, 180) },
	{ "tan", NSVG_RGB(210, 180, 140) },
	{ "teal", NSVG_RGB( 0, 128, 128) },
	{ "thistle", NSVG_RGB(216, 191, 216) },
	{ "tomato", NSVG_RGB(255, 99, 71) },
	{ "turquoise", NSVG_RGB( 64, 224, 208) },
	{ "violet", NSVG_RGB(238, 130, 238) },
	{ "wheat", NSVG_RGB(245, 222, 179) },
	{ "whitesmoke", NSVG_RGB(245, 245, 245) },
	{ "yellowgreen", NSVG_RGB(154, 205, 50) },
#endif
};

static unsigned int nsvg__parseColorName(const char* str)
{
	int i, ncolors = sizeof(nsvg__colors) / sizeof(NSVGNamedColor);

	for (i = 0; i < ncolors; i++) {
		if (strcmp(nsvg__colors[i].name, str) == 0) {
			return nsvg__colors[i].color;
		}
	}

	return NSVG_RGB(128, 128, 128);
}

static unsigned int nsvg__parseColor(const char* str)
{
	size_t len = 0;
	while(*str == ' ') ++str;
	len = strlen(str);
	if (len >= 1 && *str == '#')
		return nsvg__parseColorHex(str);
	else if (len >= 4 && str[0] == 'r' && str[1] == 'g' && str[2] == 'b' && str[3] == '(')
		return nsvg__parseColorRGB(str);
	return nsvg__parseColorName(str);
}

static float nsvg__parseOpacity(const char* str)
{
	float val = nsvg__atof(str);
	if (val < 0.0f) val = 0.0f;
	if (val > 1.0f) val = 1.0f;
	return val;
}

static float nsvg__parseMiterLimit(const char* str)
{
	float val = nsvg__atof(str);
	if (val < 0.0f) val = 0.0f;
	return val;
}

static int nsvg__parseUnits(const char* units)
{
	if (units[0] == 'p' && units[1] == 'x')
		return NSVG_UNITS_PX;
	else if (units[0] == 'p' && units[1] == 't')
		return NSVG_UNITS_PT;
	else if (units[0] == 'p' && units[1] == 'c')
		return NSVG_UNITS_PC;
	else if (units[0] == 'm' && units[1] == 'm')
		return NSVG_UNITS_MM;
	else if (units[0] == 'c' && units[1] == 'm')
		return NSVG_UNITS_CM;
	else if (units[0] == 'i' && units[1] == 'n')
		return NSVG_UNITS_IN;
	else if (units[0] == '%')
		return NSVG_UNITS_PERCENT;
	else if (units[0] == 'e' && units[1] == 'm')
		return NSVG_UNITS_EM;
	else if (units[0] == 'e' && units[1] == 'x')
		return NSVG_UNITS_EX;
	return NSVG_UNITS_USER;
}

static int nsvg__isCoordinate(const char* s)
{
	// optional sign
	if (*s == '-' || *s == '+')
		s++;
	// must have at least one digit, or start by a dot
	return (nsvg__isdigit(*s) || *s == '.');
}

static NSVGcoordinate nsvg__parseCoordinateRaw(const char* str)
{
	NSVGcoordinate coord = {0, NSVG_UNITS_USER};
	char buf[64];
	coord.units = nsvg__parseUnits(nsvg__parseNumber(str, buf, 64));
	coord.value = nsvg__atof(buf);
	return coord;
}

static NSVGcoordinate nsvg__coord(float v, int units)
{
	NSVGcoordinate coord = {v, units};
	return coord;
}

static float nsvg__parseCoordinate(NSVGparser* p, const char* str, float orig, float length)
{
	NSVGcoordinate coord = nsvg__parseCoordinateRaw(str);
	return nsvg__convertToPixels(p, coord, orig, length);
}

static int nsvg__parseTransformArgs(const char* str, float* args, int maxNa, int* na)
{
	const char* end;
	const char* ptr;
	char it[64];

	*na = 0;
	ptr = str;
	while (*ptr && *ptr != '(') ++ptr;
	if (*ptr == 0)
		return 1;
	end = ptr;
	while (*end && *end != ')') ++end;
	if (*end == 0)
		return 1;

	while (ptr < end) {
		if (*ptr == '-' || *ptr == '+' || *ptr == '.' || nsvg__isdigit(*ptr)) {
			if (*na >= maxNa) return 0;
			ptr = nsvg__parseNumber(ptr, it, 64);
			args[(*na)++] = (float)nsvg__atof(it);
		} else {
			++ptr;
		}
	}
	return (int)(end - str);
}


static int nsvg__parseMatrix(float* xform, const char* str)
{
	float t[6];
	int na = 0;
	int len = nsvg__parseTransformArgs(str, t, 6, &na);
	if (na != 6) return len;
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseTranslate(float* xform, const char* str)
{
	float args[2];
	float t[6];
	int na = 0;
	int len = nsvg__parseTransformArgs(str, args, 2, &na);
	if (na == 1) args[1] = 0.0;

	nsvg__xformSetTranslation(t, args[0], args[1]);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseScale(float* xform, const char* str)
{
	float args[2];
	int na = 0;
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 2, &na);
	if (na == 1) args[1] = args[0];
	nsvg__xformSetScale(t, args[0], args[1]);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseSkewX(float* xform, const char* str)
{
	float args[1];
	int na = 0;
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 1, &na);
	nsvg__xformSetSkewX(t, args[0]/180.0f*NSVG_PI);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseSkewY(float* xform, const char* str)
{
	float args[1];
	int na = 0;
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 1, &na);
	nsvg__xformSetSkewY(t, args[0]/180.0f*NSVG_PI);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseRotate(float* xform, const char* str)
{
	float args[3];
	int na = 0;
	float m[6];
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 3, &na);
	if (na == 1)
		args[1] = args[2] = 0.0f;
	nsvg__xformIdentity(m);

	if (na > 1) {
		nsvg__xformSetTranslation(t, -args[1], -args[2]);
		nsvg__xformMultiply(m, t);
	}

	nsvg__xformSetRotation(t, args[0]/180.0f*NSVG_PI);
	nsvg__xformMultiply(m, t);

	if (na > 1) {
		nsvg__xformSetTranslation(t, args[1], args[2]);
		nsvg__xformMultiply(m, t);
	}

	memcpy(xform, m, sizeof(float)*6);

	return len;
}

static void nsvg__parseTransform(float* xform, const char* str)
{
	float t[6];
	int len;
	nsvg__xformIdentity(xform);
	while (*str)
	{
		if (strncmp(str, "matrix", 6) == 0)
			len = nsvg__parseMatrix(t, str);
		else if (strncmp(str, "translate", 9) == 0)
			len = nsvg__parseTranslate(t, str);
		else if (strncmp(str, "scale", 5) == 0)
			len = nsvg__parseScale(t, str);
		else if (strncmp(str, "rotate", 6) == 0)
			len = nsvg__parseRotate(t, str);
		else if (strncmp(str, "skewX", 5) == 0)
			len = nsvg__parseSkewX(t, str);
		else if (strncmp(str, "skewY", 5) == 0)
			len = nsvg__parseSkewY(t, str);
		else{
			++str;
			continue;
		}
		if (len != 0) {
			str += len;
		} else {
			++str;
			continue;
		}

		nsvg__xformPremultiply(xform, t);
	}
}

static void nsvg__parseUrl(char* id, const char* str)
{
	int i = 0;
	str += 4; // "url(";
	if (*str && *str == '#')
		str++;
	while (i < 63 && *str && *str != ')') {
		id[i] = *str++;
		i++;
	}
	id[i] = '\0';
}

static char nsvg__parseLineCap(const char* str)
{
	if (strcmp(str, "butt") == 0)
		return NSVG_CAP_BUTT;
	else if (strcmp(str, "round") == 0)
		return NSVG_CAP_ROUND;
	else if (strcmp(str, "square") == 0)
		return NSVG_CAP_SQUARE;
	// TODO: handle inherit.
	return NSVG_CAP_BUTT;
}

static char nsvg__parseLineJoin(const char* str)
{
	if (strcmp(str, "miter") == 0)
		return NSVG_JOIN_MITER;
	else if (strcmp(str, "round") == 0)
		return NSVG_JOIN_ROUND;
	else if (strcmp(str, "bevel") == 0)
		return NSVG_JOIN_BEVEL;
	// TODO: handle inherit.
	return NSVG_JOIN_MITER;
}

static char nsvg__parseFillRule(const char* str)
{
	if (strcmp(str, "nonzero") == 0)
		return NSVG_FILLRULE_NONZERO;
	else if (strcmp(str, "evenodd") == 0)
		return NSVG_FILLRULE_EVENODD;
	// TODO: handle inherit.
	return NSVG_FILLRULE_NONZERO;
}

static unsigned char nsvg__parsePaintOrder(const char* str)
{
	if (strcmp(str, "normal") == 0 || strcmp(str, "fill stroke markers") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS);
	else if (strcmp(str, "fill markers stroke") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_MARKERS, NSVG_PAINT_STROKE);
	else if (strcmp(str, "markers fill stroke") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_MARKERS, NSVG_PAINT_FILL, NSVG_PAINT_STROKE);
	else if (strcmp(str, "markers stroke fill") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_MARKERS, NSVG_PAINT_STROKE, NSVG_PAINT_FILL);
	else if (strcmp(str, "stroke fill markers") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_STROKE, NSVG_PAINT_FILL, NSVG_PAINT_MARKERS);
	else if (strcmp(str, "stroke markers fill") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS, NSVG_PAINT_FILL);
	// TODO: handle inherit.
	return nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS);
}

static const char* nsvg__getNextDashItem(const char* s, char* it)
{
	int n = 0;
	it[0] = '\0';
	// Skip white spaces and commas
	while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
	// Advance until whitespace, comma or end.
	while (*s && (!nsvg__isspace(*s) && *s != ',')) {
		if (n < 63)
			it[n++] = *s;
		s++;
	}
	it[n++] = '\0';
	return s;
}

static int nsvg__parseStrokeDashArray(NSVGparser* p, const char* str, float* strokeDashArray)
{
	char item[64];
	int count = 0, i;
	float sum = 0.0f;

	// Handle "none"
	if (str[0] == 'n')
		return 0;

	// Parse dashes
	while (*str) {
		str = nsvg__getNextDashItem(str, item);
		if (!*item) break;
		if (count < NSVG_MAX_DASHES)
			strokeDashArray[count++] = fabsf(nsvg__parseCoordinate(p, item, 0.0f, nsvg__actualLength(p)));
	}

	for (i = 0; i < count; i++)
		sum += strokeDashArray[i];
	if (sum <= 1e-6f)
		count = 0;

	return count;
}

static void nsvg__parseStyle(NSVGparser* p, const char* str);

// Apply any matching class styles for a "class" attribute value. We support only simple class
// selectors. The class attribute may contain multiple space-separated class names.
static void nsvg__applyClassStyles(NSVGparser* p, const char* value)
{
	const char* cur = value;
	while (*cur) {
		const char* classStart;
		size_t classLen;
		NSVGstyleDeclaration* style;

		while (*cur && nsvg__isspace(*cur))
			cur++;
		if (!*cur)
			break;

		classStart = cur;
		while (*cur && !nsvg__isspace(*cur))
			cur++;
		classLen = (size_t)(cur - classStart);

		for (style = p->styles; style != NULL; style = style->next) {
			if (strncmp(style->className, classStart, classLen) == 0 && style->className[classLen] == '\0') {
				nsvg__parseStyle(p, style->propertiesText);
			}
		}
	}
}

static int nsvg__parseAttr(NSVGparser* p, const char* name, const char* value)
{
	float xform[6];
	NSVGattrib* attr = nsvg__getAttr(p);
	if (!attr) return 0;

	if (strcmp(name, "style") == 0) {
		nsvg__parseStyle(p, value);
	} else if (strcmp(name, "display") == 0) {
		if (strcmp(value, "none") == 0)
			attr->visible = 0;
		// Don't reset ->visible on display:inline, one display:none hides the whole subtree

	} else if (strcmp(name, "fill") == 0) {
		if (strcmp(value, "none") == 0) {
			attr->hasFill = 0;
		} else if (strncmp(value, "url(", 4) == 0) {
			attr->hasFill = 2;
			nsvg__parseUrl(attr->fillGradient, value);
		} else {
			attr->hasFill = 1;
			attr->fillColor = nsvg__parseColor(value);
		}
	} else if (strcmp(name, "opacity") == 0) {
		attr->opacity = nsvg__parseOpacity(value);
	} else if (strcmp(name, "fill-opacity") == 0) {
		attr->fillOpacity = nsvg__parseOpacity(value);
	} else if (strcmp(name, "stroke") == 0) {
		if (strcmp(value, "none") == 0) {
			attr->hasStroke = 0;
		} else if (strncmp(value, "url(", 4) == 0) {
			attr->hasStroke = 2;
			nsvg__parseUrl(attr->strokeGradient, value);
		} else {
			attr->hasStroke = 1;
			attr->strokeColor = nsvg__parseColor(value);
		}
	} else if (strcmp(name, "stroke-width") == 0) {
		attr->strokeWidth = nsvg__parseCoordinate(p, value, 0.0f, nsvg__actualLength(p));
	} else if (strcmp(name, "stroke-dasharray") == 0) {
		attr->strokeDashCount = nsvg__parseStrokeDashArray(p, value, attr->strokeDashArray);
	} else if (strcmp(name, "stroke-dashoffset") == 0) {
		attr->strokeDashOffset = nsvg__parseCoordinate(p, value, 0.0f, nsvg__actualLength(p));
	} else if (strcmp(name, "stroke-opacity") == 0) {
		attr->strokeOpacity = nsvg__parseOpacity(value);
	} else if (strcmp(name, "stroke-linecap") == 0) {
		attr->strokeLineCap = nsvg__parseLineCap(value);
	} else if (strcmp(name, "stroke-linejoin") == 0) {
		attr->strokeLineJoin = nsvg__parseLineJoin(value);
	} else if (strcmp(name, "stroke-miterlimit") == 0) {
		attr->miterLimit = nsvg__parseMiterLimit(value);
	} else if (strcmp(name, "fill-rule") == 0) {
		attr->fillRule = nsvg__parseFillRule(value);
	} else if (strcmp(name, "font-size") == 0) {
		attr->fontSize = nsvg__parseCoordinate(p, value, 0.0f, nsvg__actualLength(p));
	} else if (strcmp(name, "transform") == 0) {
		nsvg__parseTransform(xform, value);
		nsvg__xformPremultiply(attr->xform, xform);
	} else if (strcmp(name, "stop-color") == 0) {
		attr->stopColor = nsvg__parseColor(value);
	} else if (strcmp(name, "stop-opacity") == 0) {
		attr->stopOpacity = nsvg__parseOpacity(value);
	} else if (strcmp(name, "offset") == 0) {
		attr->stopOffset = nsvg__parseCoordinate(p, value, 0.0f, 1.0f);
	} else if (strcmp(name, "paint-order") == 0) {
		attr->paintOrder = nsvg__parsePaintOrder(value);
	} else if (strcmp(name, "id") == 0) {
		strncpy(attr->id, value, 63);
		attr->id[63] = '\0';
	} else if (strcmp(name, "class") == 0) {
		nsvg__applyClassStyles(p, value);
	}
	else {
		return 0;
	}
	return 1;
}

static int nsvg__parseNameValue(NSVGparser* p, const char* start, const char* end)
{
	const char* str;
	const char* val;
	char name[512];
	char value[512];
	int n;

	str = start;
	while (str < end && *str != ':') ++str;

	val = str;

	// Right Trim
	while (str > start &&  (*str == ':' || nsvg__isspace(*str))) --str;
	++str;

	n = (int)(str - start);
	if (n > 511) n = 511;
	if (n) memcpy(name, start, n);
	name[n] = 0;

	while (val < end && (*val == ':' || nsvg__isspace(*val))) ++val;

	n = (int)(end - val);
	if (n > 511) n = 511;
	if (n) memcpy(value, val, n);
	value[n] = 0;

	return nsvg__parseAttr(p, name, value);
}

static void nsvg__parseStyle(NSVGparser* p, const char* str)
{
	const char* start;
	const char* end;

	while (*str) {
		// Left Trim
		while(*str && nsvg__isspace(*str)) ++str;
		start = str;
		while(*str && *str != ';') ++str;
		end = str;

		// Right Trim
		while (end > start &&  (*end == ';' || nsvg__isspace(*end))) --end;
		++end;

		nsvg__parseNameValue(p, start, end);
		if (*str) ++str;
	}
}

static void nsvg__parseAttribs(NSVGparser* p, const char** attr)
{
	int i;
	for (i = 0; attr[i]; i += 2)
	{
		if (strcmp(attr[i], "style") == 0)
			nsvg__parseStyle(p, attr[i + 1]);
		else
			nsvg__parseAttr(p, attr[i], attr[i + 1]);
	}
}

static int nsvg__getArgsPerElement(char cmd)
{
	switch (cmd) {
		case 'v':
		case 'V':
		case 'h':
		case 'H':
			return 1;
		case 'm':
		case 'M':
		case 'l':
		case 'L':
		case 't':
		case 'T':
			return 2;
		case 'q':
		case 'Q':
		case 's':
		case 'S':
			return 4;
		case 'c':
		case 'C':
			return 6;
		case 'a':
		case 'A':
			return 7;
		case 'z':
		case 'Z':
			return 0;
	}
	return -1;
}

static void nsvg__pathMoveTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel) {
		*cpx += args[0];
		*cpy += args[1];
	} else {
		*cpx = args[0];
		*cpy = args[1];
	}
	nsvg__moveTo(p, *cpx, *cpy);
}

static void nsvg__pathLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel) {
		*cpx += args[0];
		*cpy += args[1];
	} else {
		*cpx = args[0];
		*cpy = args[1];
	}
	nsvg__lineTo(p, *cpx, *cpy);
}

static void nsvg__pathHLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel)
		*cpx += args[0];
	else
		*cpx = args[0];
	nsvg__lineTo(p, *cpx, *cpy);
}

static void nsvg__pathVLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel)
		*cpy += args[0];
	else
		*cpy = args[0];
	nsvg__lineTo(p, *cpx, *cpy);
}

static void nsvg__pathCubicBezTo(NSVGparser* p, float* cpx, float* cpy,
								 float* cpx2, float* cpy2, float* args, int rel)
{
	float x2, y2, cx1, cy1, cx2, cy2;

	if (rel) {
		cx1 = *cpx + args[0];
		cy1 = *cpy + args[1];
		cx2 = *cpx + args[2];
		cy2 = *cpy + args[3];
		x2 = *cpx + args[4];
		y2 = *cpy + args[5];
	} else {
		cx1 = args[0];
		cy1 = args[1];
		cx2 = args[2];
		cy2 = args[3];
		x2 = args[4];
		y2 = args[5];
	}

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

static void nsvg__pathCubicBezShortTo(NSVGparser* p, float* cpx, float* cpy,
									  float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if (rel) {
		cx2 = *cpx + args[0];
		cy2 = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	} else {
		cx2 = args[0];
		cy2 = args[1];
		x2 = args[2];
		y2 = args[3];
	}

	cx1 = 2*x1 - *cpx2;
	cy1 = 2*y1 - *cpy2;

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

static void nsvg__pathQuadBezTo(NSVGparser* p, float* cpx, float* cpy,
								float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	float cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if (rel) {
		cx = *cpx + args[0];
		cy = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	} else {
		cx = args[0];
		cy = args[1];
		x2 = args[2];
		y2 = args[3];
	}

	// Convert to cubic bezier
	cx1 = x1 + 2.0f/3.0f*(cx - x1);
	cy1 = y1 + 2.0f/3.0f*(cy - y1);
	cx2 = x2 + 2.0f/3.0f*(cx - x2);
	cy2 = y2 + 2.0f/3.0f*(cy - y2);

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

static void nsvg__pathQuadBezShortTo(NSVGparser* p, float* cpx, float* cpy,
									 float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	float cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if (rel) {
		x2 = *cpx + args[0];
		y2 = *cpy + args[1];
	} else {
		x2 = args[0];
		y2 = args[1];
	}

	cx = 2*x1 - *cpx2;
	cy = 2*y1 - *cpy2;

	// Convert to cubix bezier
	cx1 = x1 + 2.0f/3.0f*(cx - x1);
	cy1 = y1 + 2.0f/3.0f*(cy - y1);
	cx2 = x2 + 2.0f/3.0f*(cx - x2);
	cy2 = y2 + 2.0f/3.0f*(cy - y2);

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

static float nsvg__sqr(float x) { return x*x; }
static float nsvg__vmag(float x, float y) { return sqrtf(x*x + y*y); }

static float nsvg__vecrat(float ux, float uy, float vx, float vy)
{
	return (ux*vx + uy*vy) / (nsvg__vmag(ux,uy) * nsvg__vmag(vx,vy));
}

static float nsvg__vecang(float ux, float uy, float vx, float vy)
{
	float r = nsvg__vecrat(ux,uy, vx,vy);
	if (r < -1.0f) r = -1.0f;
	if (r > 1.0f) r = 1.0f;
	return ((ux*vy < uy*vx) ? -1.0f : 1.0f) * acosf(r);
}

static void nsvg__pathArcTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	// Ported from canvg (https://code.google.com/p/canvg/)
	float rx, ry, rotx;
	float x1, y1, x2, y2, cx, cy, dx, dy, d;
	float x1p, y1p, cxp, cyp, s, sa, sb;
	float ux, uy, vx, vy, a1, da;
	float x, y, tanx, tany, a, px = 0, py = 0, ptanx = 0, ptany = 0, t[6];
	float sinrx, cosrx;
	int fa, fs;
	int i, ndivs;
	float hda, kappa;

	rx = fabsf(args[0]);				// y radius
	ry = fabsf(args[1]);				// x radius
	rotx = args[2] / 180.0f * NSVG_PI;		// x rotation angle
	fa = fabsf(args[3]) > 1e-6 ? 1 : 0;	// Large arc
	fs = fabsf(args[4]) > 1e-6 ? 1 : 0;	// Sweep direction
	x1 = *cpx;							// start point
	y1 = *cpy;
	if (rel) {							// end point
		x2 = *cpx + args[5];
		y2 = *cpy + args[6];
	} else {
		x2 = args[5];
		y2 = args[6];
	}

	dx = x1 - x2;
	dy = y1 - y2;
	d = sqrtf(dx*dx + dy*dy);
	if (d < 1e-6f || rx < 1e-6f || ry < 1e-6f) {
		// The arc degenerates to a line
		nsvg__lineTo(p, x2, y2);
		*cpx = x2;
		*cpy = y2;
		return;
	}

	sinrx = sinf(rotx);
	cosrx = cosf(rotx);

	// Convert to center point parameterization.
	// http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes
	// 1) Compute x1', y1'
	x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
	y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;
	d = nsvg__sqr(x1p)/nsvg__sqr(rx) + nsvg__sqr(y1p)/nsvg__sqr(ry);
	if (d > 1) {
		d = sqrtf(d);
		rx *= d;
		ry *= d;
	}
	// 2) Compute cx', cy'
	s = 0.0f;
	sa = nsvg__sqr(rx)*nsvg__sqr(ry) - nsvg__sqr(rx)*nsvg__sqr(y1p) - nsvg__sqr(ry)*nsvg__sqr(x1p);
	sb = nsvg__sqr(rx)*nsvg__sqr(y1p) + nsvg__sqr(ry)*nsvg__sqr(x1p);
	if (sa < 0.0f) sa = 0.0f;
	if (sb > 0.0f)
		s = sqrtf(sa / sb);
	if (fa == fs)
		s = -s;
	cxp = s * rx * y1p / ry;
	cyp = s * -ry * x1p / rx;

	// 3) Compute cx,cy from cx',cy'
	cx = (x1 + x2)/2.0f + cosrx*cxp - sinrx*cyp;
	cy = (y1 + y2)/2.0f + sinrx*cxp + cosrx*cyp;

	// 4) Calculate theta1, and delta theta.
	ux = (x1p - cxp) / rx;
	uy = (y1p - cyp) / ry;
	vx = (-x1p - cxp) / rx;
	vy = (-y1p - cyp) / ry;
	a1 = nsvg__vecang(1.0f,0.0f, ux,uy);	// Initial angle
	da = nsvg__vecang(ux,uy, vx,vy);		// Delta angle

//	if (vecrat(ux,uy,vx,vy) <= -1.0f) da = NSVG_PI;
//	if (vecrat(ux,uy,vx,vy) >= 1.0f) da = 0;

	if (fs == 0 && da > 0)
		da -= 2 * NSVG_PI;
	else if (fs == 1 && da < 0)
		da += 2 * NSVG_PI;

	// Approximate the arc using cubic spline segments.
	t[0] = cosrx; t[1] = sinrx;
	t[2] = -sinrx; t[3] = cosrx;
	t[4] = cx; t[5] = cy;

	// Split arc into max 90 degree segments.
	// The loop assumes an iteration per end point (including start and end), this +1.
	ndivs = (int)(fabsf(da) / (NSVG_PI*0.5f) + 1.0f);
	hda = (da / (float)ndivs) / 2.0f;
	// Fix for ticket #179: division by 0: avoid cotangens around 0 (infinite)
	if ((hda < 1e-3f) && (hda > -1e-3f))
		hda *= 0.5f;
	else
		hda = (1.0f - cosf(hda)) / sinf(hda);
	kappa = fabsf(4.0f / 3.0f * hda);
	if (da < 0.0f)
		kappa = -kappa;

	for (i = 0; i <= ndivs; i++) {
		a = a1 + da * ((float)i/(float)ndivs);
		dx = cosf(a);
		dy = sinf(a);
		nsvg__xformPoint(&x, &y, dx*rx, dy*ry, t); // position
		nsvg__xformVec(&tanx, &tany, -dy*rx * kappa, dx*ry * kappa, t); // tangent
		if (i > 0)
			nsvg__cubicBezTo(p, px+ptanx,py+ptany, x-tanx, y-tany, x, y);
		px = x;
		py = y;
		ptanx = tanx;
		ptany = tany;
	}

	*cpx = x2;
	*cpy = y2;
}

static void nsvg__parsePath(NSVGparser* p, const char** attr)
{
	const char* s = NULL;
	char cmd = '\0';
	float args[10];
	int nargs;
	int rargs = 0;
	char initPoint;
	float cpx, cpy, cpx2, cpy2;
	const char* tmp[4];
	char closedFlag;
	int i;
	char item[64];

	for (i = 0; attr[i]; i += 2) {
		if (strcmp(attr[i], "d") == 0) {
			s = attr[i + 1];
		} else {
			tmp[0] = attr[i];
			tmp[1] = attr[i + 1];
			tmp[2] = 0;
			tmp[3] = 0;
			nsvg__parseAttribs(p, tmp);
		}
	}

	if (s) {
		nsvg__resetPath(p);
		cpx = 0; cpy = 0;
		cpx2 = 0; cpy2 = 0;
		initPoint = 0;
		closedFlag = 0;
		nargs = 0;

		while (*s) {
			item[0] = '\0';
			if ((cmd == 'A' || cmd == 'a') && (nargs == 3 || nargs == 4))
				s = nsvg__getNextPathItemWhenArcFlag(s, item);
			if (!*item)
				s = nsvg__getNextPathItem(s, item);
			if (!*item) break;
			if (cmd != '\0' && nsvg__isCoordinate(item)) {
				if (nargs < 10)
					args[nargs++] = (float)nsvg__atof(item);
				if (nargs >= rargs) {
					switch (cmd) {
						case 'm':
						case 'M':
							nsvg__pathMoveTo(p, &cpx, &cpy, args, cmd == 'm' ? 1 : 0);
							// Moveto can be followed by multiple coordinate pairs,
							// which should be treated as linetos.
							cmd = (cmd == 'm') ? 'l' : 'L';
							rargs = nsvg__getArgsPerElement(cmd);
							cpx2 = cpx; cpy2 = cpy;
							initPoint = 1;
							break;
						case 'l':
						case 'L':
							nsvg__pathLineTo(p, &cpx, &cpy, args, cmd == 'l' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						case 'H':
						case 'h':
							nsvg__pathHLineTo(p, &cpx, &cpy, args, cmd == 'h' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						case 'V':
						case 'v':
							nsvg__pathVLineTo(p, &cpx, &cpy, args, cmd == 'v' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						case 'C':
						case 'c':
							nsvg__pathCubicBezTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 'c' ? 1 : 0);
							break;
						case 'S':
						case 's':
							nsvg__pathCubicBezShortTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 's' ? 1 : 0);
							break;
						case 'Q':
						case 'q':
							nsvg__pathQuadBezTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 'q' ? 1 : 0);
							break;
						case 'T':
						case 't':
							nsvg__pathQuadBezShortTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 't' ? 1 : 0);
							break;
						case 'A':
						case 'a':
							nsvg__pathArcTo(p, &cpx, &cpy, args, cmd == 'a' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						default:
							if (nargs >= 2) {
								cpx = args[nargs-2];
								cpy = args[nargs-1];
								cpx2 = cpx; cpy2 = cpy;
							}
							break;
					}
					nargs = 0;
				}
			} else {
				cmd = item[0];
				if (cmd == 'M' || cmd == 'm') {
					// Commit path.
					if (p->npts > 0)
						nsvg__addPath(p, closedFlag);
					// Start new subpath.
					nsvg__resetPath(p);
					closedFlag = 0;
					nargs = 0;
				} else if (initPoint == 0) {
					// Do not allow other commands until initial point has been set (moveTo called once).
					cmd = '\0';
				}
				if (cmd == 'Z' || cmd == 'z') {
					closedFlag = 1;
					// Commit path.
					if (p->npts > 0) {
						// Move current point to first point
						cpx = p->pts[0];
						cpy = p->pts[1];
						cpx2 = cpx; cpy2 = cpy;
						nsvg__addPath(p, closedFlag);
					}
					// Start new subpath.
					nsvg__resetPath(p);
					nsvg__moveTo(p, cpx, cpy);
					closedFlag = 0;
					nargs = 0;
				}
				rargs = nsvg__getArgsPerElement(cmd);
				if (rargs == -1) {
					// Command not recognized
					cmd = '\0';
					rargs = 0;
				}
			}
		}
		// Commit path.
		if (p->npts)
			nsvg__addPath(p, closedFlag);
	}

	nsvg__addShape(p);
}

static void nsvg__parseRect(NSVGparser* p, const char** attr)
{
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	float rx = -1.0f; // marks not set
	float ry = -1.0f;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "x") == 0) x = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "y") == 0) y = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "width") == 0) w = nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p));
			if (strcmp(attr[i], "height") == 0) h = nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p));
			if (strcmp(attr[i], "rx") == 0) rx = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p)));
			if (strcmp(attr[i], "ry") == 0) ry = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p)));
		}
	}

	if (rx < 0.0f && ry > 0.0f) rx = ry;
	if (ry < 0.0f && rx > 0.0f) ry = rx;
	if (rx < 0.0f) rx = 0.0f;
	if (ry < 0.0f) ry = 0.0f;
	if (rx > w/2.0f) rx = w/2.0f;
	if (ry > h/2.0f) ry = h/2.0f;

	if (w != 0.0f && h != 0.0f) {
		nsvg__resetPath(p);

		if (rx < 0.00001f || ry < 0.0001f) {
			nsvg__moveTo(p, x, y);
			nsvg__lineTo(p, x+w, y);
			nsvg__lineTo(p, x+w, y+h);
			nsvg__lineTo(p, x, y+h);
		} else {
			// Rounded rectangle
			nsvg__moveTo(p, x+rx, y);
			nsvg__lineTo(p, x+w-rx, y);
			nsvg__cubicBezTo(p, x+w-rx*(1-NSVG_KAPPA90), y, x+w, y+ry*(1-NSVG_KAPPA90), x+w, y+ry);
			nsvg__lineTo(p, x+w, y+h-ry);
			nsvg__cubicBezTo(p, x+w, y+h-ry*(1-NSVG_KAPPA90), x+w-rx*(1-NSVG_KAPPA90), y+h, x+w-rx, y+h);
			nsvg__lineTo(p, x+rx, y+h);
			nsvg__cubicBezTo(p, x+rx*(1-NSVG_KAPPA90), y+h, x, y+h-ry*(1-NSVG_KAPPA90), x, y+h-ry);
			nsvg__lineTo(p, x, y+ry);
			nsvg__cubicBezTo(p, x, y+ry*(1-NSVG_KAPPA90), x+rx*(1-NSVG_KAPPA90), y, x+rx, y);
		}

		nsvg__addPath(p, 1);

		nsvg__addShape(p);
	}
}

static void nsvg__parseCircle(NSVGparser* p, const char** attr)
{
	float cx = 0.0f;
	float cy = 0.0f;
	float r = 0.0f;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "cx") == 0) cx = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "cy") == 0) cy = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "r") == 0) r = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualLength(p)));
		}
	}

	if (r > 0.0f) {
		nsvg__resetPath(p);

		nsvg__moveTo(p, cx+r, cy);
		nsvg__cubicBezTo(p, cx+r, cy+r*NSVG_KAPPA90, cx+r*NSVG_KAPPA90, cy+r, cx, cy+r);
		nsvg__cubicBezTo(p, cx-r*NSVG_KAPPA90, cy+r, cx-r, cy+r*NSVG_KAPPA90, cx-r, cy);
		nsvg__cubicBezTo(p, cx-r, cy-r*NSVG_KAPPA90, cx-r*NSVG_KAPPA90, cy-r, cx, cy-r);
		nsvg__cubicBezTo(p, cx+r*NSVG_KAPPA90, cy-r, cx+r, cy-r*NSVG_KAPPA90, cx+r, cy);

		nsvg__addPath(p, 1);

		nsvg__addShape(p);
	}
}

static void nsvg__parseEllipse(NSVGparser* p, const char** attr)
{
	float cx = 0.0f;
	float cy = 0.0f;
	float rx = 0.0f;
	float ry = 0.0f;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "cx") == 0) cx = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "cy") == 0) cy = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "rx") == 0) rx = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p)));
			if (strcmp(attr[i], "ry") == 0) ry = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p)));
		}
	}

	if (rx > 0.0f && ry > 0.0f) {

		nsvg__resetPath(p);

		nsvg__moveTo(p, cx+rx, cy);
		nsvg__cubicBezTo(p, cx+rx, cy+ry*NSVG_KAPPA90, cx+rx*NSVG_KAPPA90, cy+ry, cx, cy+ry);
		nsvg__cubicBezTo(p, cx-rx*NSVG_KAPPA90, cy+ry, cx-rx, cy+ry*NSVG_KAPPA90, cx-rx, cy);
		nsvg__cubicBezTo(p, cx-rx, cy-ry*NSVG_KAPPA90, cx-rx*NSVG_KAPPA90, cy-ry, cx, cy-ry);
		nsvg__cubicBezTo(p, cx+rx*NSVG_KAPPA90, cy-ry, cx+rx, cy-ry*NSVG_KAPPA90, cx+rx, cy);

		nsvg__addPath(p, 1);

		nsvg__addShape(p);
	}
}

static void nsvg__parseLine(NSVGparser* p, const char** attr)
{
	float x1 = 0.0;
	float y1 = 0.0;
	float x2 = 0.0;
	float y2 = 0.0;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "x1") == 0) x1 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "y1") == 0) y1 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "x2") == 0) x2 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "y2") == 0) y2 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
		}
	}

	nsvg__resetPath(p);

	nsvg__moveTo(p, x1, y1);
	nsvg__lineTo(p, x2, y2);

	nsvg__addPath(p, 0);

	nsvg__addShape(p);
}

static void nsvg__parsePoly(NSVGparser* p, const char** attr, int closeFlag)
{
	int i;
	const char* s;
	float args[2];
	int nargs, npts = 0;
	char item[64];

	nsvg__resetPath(p);

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "points") == 0) {
				s = attr[i + 1];
				nargs = 0;
				while (*s) {
					s = nsvg__getNextPathItem(s, item);
					args[nargs++] = (float)nsvg__atof(item);
					if (nargs >= 2) {
						if (npts == 0)
							nsvg__moveTo(p, args[0], args[1]);
						else
							nsvg__lineTo(p, args[0], args[1]);
						nargs = 0;
						npts++;
					}
				}
			}
		}
	}

	nsvg__addPath(p, (char)closeFlag);

	nsvg__addShape(p);
}

static void nsvg__parseSVG(NSVGparser* p, const char** attr)
{
	int i;
	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "width") == 0) {
				p->image->width = nsvg__parseCoordinate(p, attr[i + 1], 0.0f, 0.0f);
			} else if (strcmp(attr[i], "height") == 0) {
				p->image->height = nsvg__parseCoordinate(p, attr[i + 1], 0.0f, 0.0f);
			} else if (strcmp(attr[i], "viewBox") == 0) {
				const char *s = attr[i + 1];
				char buf[64];
				s = nsvg__parseNumber(s, buf, 64);
				p->viewMinx = nsvg__atof(buf);
				while (*s && (nsvg__isspace(*s) || *s == '%' || *s == ',')) s++;
				if (!*s) return;
				s = nsvg__parseNumber(s, buf, 64);
				p->viewMiny = nsvg__atof(buf);
				while (*s && (nsvg__isspace(*s) || *s == '%' || *s == ',')) s++;
				if (!*s) return;
				s = nsvg__parseNumber(s, buf, 64);
				p->viewWidth = nsvg__atof(buf);
				while (*s && (nsvg__isspace(*s) || *s == '%' || *s == ',')) s++;
				if (!*s) return;
				s = nsvg__parseNumber(s, buf, 64);
				p->viewHeight = nsvg__atof(buf);
			} else if (strcmp(attr[i], "preserveAspectRatio") == 0) {
				if (strstr(attr[i + 1], "none") != 0) {
					// No uniform scaling
					p->alignType = NSVG_ALIGN_NONE;
				} else {
					// Parse X align
					if (strstr(attr[i + 1], "xMin") != 0)
						p->alignX = NSVG_ALIGN_MIN;
					else if (strstr(attr[i + 1], "xMid") != 0)
						p->alignX = NSVG_ALIGN_MID;
					else if (strstr(attr[i + 1], "xMax") != 0)
						p->alignX = NSVG_ALIGN_MAX;
					// Parse X align
					if (strstr(attr[i + 1], "yMin") != 0)
						p->alignY = NSVG_ALIGN_MIN;
					else if (strstr(attr[i + 1], "yMid") != 0)
						p->alignY = NSVG_ALIGN_MID;
					else if (strstr(attr[i + 1], "yMax") != 0)
						p->alignY = NSVG_ALIGN_MAX;
					// Parse meet/slice
					p->alignType = NSVG_ALIGN_MEET;
					if (strstr(attr[i + 1], "slice") != 0)
						p->alignType = NSVG_ALIGN_SLICE;
				}
			}
		}
	}
}

static void nsvg__parseGradient(NSVGparser* p, const char** attr, signed char type)
{
	int i;
	NSVGgradientData* grad = (NSVGgradientData*)malloc(sizeof(NSVGgradientData));
	if (grad == NULL) return;
	memset(grad, 0, sizeof(NSVGgradientData));
	grad->units = NSVG_OBJECT_SPACE;
	grad->type = type;
	if (grad->type == NSVG_PAINT_LINEAR_GRADIENT) {
		grad->linear.x1 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
		grad->linear.y1 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
		grad->linear.x2 = nsvg__coord(100.0f, NSVG_UNITS_PERCENT);
		grad->linear.y2 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
	} else if (grad->type == NSVG_PAINT_RADIAL_GRADIENT) {
		grad->radial.cx = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
		grad->radial.cy = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
		grad->radial.r = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
	}

	nsvg__xformIdentity(grad->xform);

	for (i = 0; attr[i]; i += 2) {
		if (strcmp(attr[i], "id") == 0) {
			strncpy(grad->id, attr[i+1], 63);
			grad->id[63] = '\0';
		} else if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "gradientUnits") == 0) {
				if (strcmp(attr[i+1], "objectBoundingBox") == 0)
					grad->units = NSVG_OBJECT_SPACE;
				else
					grad->units = NSVG_USER_SPACE;
			} else if (strcmp(attr[i], "gradientTransform") == 0) {
				nsvg__parseTransform(grad->xform, attr[i + 1]);
			} else if (strcmp(attr[i], "cx") == 0) {
				grad->radial.cx = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "cy") == 0) {
				grad->radial.cy = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "r") == 0) {
				grad->radial.r = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "fx") == 0) {
				grad->radial.fx = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "fy") == 0) {
				grad->radial.fy = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "x1") == 0) {
				grad->linear.x1 = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "y1") == 0) {
				grad->linear.y1 = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "x2") == 0) {
				grad->linear.x2 = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "y2") == 0) {
				grad->linear.y2 = nsvg__parseCoordinateRaw(attr[i + 1]);
			} else if (strcmp(attr[i], "spreadMethod") == 0) {
				if (strcmp(attr[i+1], "pad") == 0)
					grad->spread = NSVG_SPREAD_PAD;
				else if (strcmp(attr[i+1], "reflect") == 0)
					grad->spread = NSVG_SPREAD_REFLECT;
				else if (strcmp(attr[i+1], "repeat") == 0)
					grad->spread = NSVG_SPREAD_REPEAT;
			} else if (strcmp(attr[i], "xlink:href") == 0) {
				const char *href = attr[i+1];
				strncpy(grad->ref, href+1, 62);
				grad->ref[62] = '\0';
			}
		}
	}

	grad->next = p->gradients;
	p->gradients = grad;
}

static void nsvg__parseGradientStop(NSVGparser* p, const char** attr)
{
	NSVGattrib* curAttr = nsvg__getAttr(p);
	NSVGgradientData* grad;
	NSVGgradientStop* stop;
	int i, idx;

	curAttr->stopOffset = 0;
	curAttr->stopColor = 0;
	curAttr->stopOpacity = 1.0f;

	for (i = 0; attr[i]; i += 2) {
		nsvg__parseAttr(p, attr[i], attr[i + 1]);
	}

	// Add stop to the last gradient.
	grad = p->gradients;
	if (grad == NULL) return;

	grad->nstops++;
	grad->stops = (NSVGgradientStop*)realloc(grad->stops, sizeof(NSVGgradientStop)*grad->nstops);
	if (grad->stops == NULL) return;

	// Insert
	idx = grad->nstops-1;
	for (i = 0; i < grad->nstops-1; i++) {
		if (curAttr->stopOffset < grad->stops[i].offset) {
			idx = i;
			break;
		}
	}
	if (idx != grad->nstops-1) {
		for (i = grad->nstops-1; i > idx; i--)
			grad->stops[i] = grad->stops[i-1];
	}

	stop = &grad->stops[idx];
	stop->color = curAttr->stopColor;
	stop->color |= (unsigned int)(curAttr->stopOpacity*255) << 24;
	stop->offset = curAttr->stopOffset;
}

static void nsvg__startElement(void* ud, const char* el, const char** attr)
{
	NSVGparser* p = (NSVGparser*)ud;

	if (p->defsFlag) {
		// Skip everything but gradients and styles in defs
		if (strcmp(el, "linearGradient") == 0) {
			nsvg__parseGradient(p, attr, NSVG_PAINT_LINEAR_GRADIENT);
		} else if (strcmp(el, "radialGradient") == 0) {
			nsvg__parseGradient(p, attr, NSVG_PAINT_RADIAL_GRADIENT);
		} else if (strcmp(el, "stop") == 0) {
			nsvg__parseGradientStop(p, attr);
		} else if (strcmp(el, "style") == 0) {
			p->styleFlag = 1;
		}
		return;
	}

	if (strcmp(el, "g") == 0) {
		nsvg__pushAttr(p);
		nsvg__parseAttribs(p, attr);
	} else if (strcmp(el, "path") == 0) {
		if (p->pathFlag)	// Do not allow nested paths.
			return;
		nsvg__pushAttr(p);
		nsvg__parsePath(p, attr);
		nsvg__popAttr(p);
	} else if (strcmp(el, "rect") == 0) {
		nsvg__pushAttr(p);
		nsvg__parseRect(p, attr);
		nsvg__popAttr(p);
	} else if (strcmp(el, "circle") == 0) {
		nsvg__pushAttr(p);
		nsvg__parseCircle(p, attr);
		nsvg__popAttr(p);
	} else if (strcmp(el, "ellipse") == 0) {
		nsvg__pushAttr(p);
		nsvg__parseEllipse(p, attr);
		nsvg__popAttr(p);
	} else if (strcmp(el, "line") == 0)  {
		nsvg__pushAttr(p);
		nsvg__parseLine(p, attr);
		nsvg__popAttr(p);
	} else if (strcmp(el, "polyline") == 0)  {
		nsvg__pushAttr(p);
		nsvg__parsePoly(p, attr, 0);
		nsvg__popAttr(p);
	} else if (strcmp(el, "polygon") == 0)  {
		nsvg__pushAttr(p);
		nsvg__parsePoly(p, attr, 1);
		nsvg__popAttr(p);
	} else  if (strcmp(el, "linearGradient") == 0) {
		nsvg__parseGradient(p, attr, NSVG_PAINT_LINEAR_GRADIENT);
	} else if (strcmp(el, "radialGradient") == 0) {
		nsvg__parseGradient(p, attr, NSVG_PAINT_RADIAL_GRADIENT);
	} else if (strcmp(el, "stop") == 0) {
		nsvg__parseGradientStop(p, attr);
	} else if (strcmp(el, "defs") == 0) {
		p->defsFlag = 1;
	} else if (strcmp(el, "svg") == 0) {
		nsvg__parseSVG(p, attr);
	} else if (strcmp(el, "style") == 0) {
		p->styleFlag = 1;
	}
}

static void nsvg__endElement(void* ud, const char* el)
{
	NSVGparser* p = (NSVGparser*)ud;

	if (strcmp(el, "g") == 0) {
		nsvg__popAttr(p);
	} else if (strcmp(el, "path") == 0) {
		p->pathFlag = 0;
	} else if (strcmp(el, "defs") == 0) {
		p->defsFlag = 0;
	} else if (strcmp(el, "style") == 0) {
		p->styleFlag = 0;
	}
}

static char *nsvg__strndup(const char *s, size_t n)
{
	char *result = (char *)malloc(n + 1);
	if (result == NULL)
		return NULL;

	memcpy(result, s, n);
	result[n] = '\0';
	return result;
}

static void nsvg__content(void* ud, const char* s)
{
	NSVGparser* p = (NSVGparser*)ud;
	if (!p->styleFlag)
		return;

	// Parse all the styles inside the style block. Each style's content will be later processed using nsvg__parseStyle().
	// Note: We only support selector lists of simple class selectors (e.g. ".foo, .bar { ... }").
	while (*s) {
		NSVGstyleDeclaration* styles[NSVG_MAX_CLASSES];
		int nstyles = 0;
		const char* propsStart;
		const char* propsEnd;
		int i;

		// 1) Parse the selector list up to '{'. For each simple class selector ('.name'),
		//    allocate a new NSVGstyleDeclaration into the local staging array. Styles are
		//    only committed to p->styles in step 3 below, once their propertiesText has
		//    also been allocated successfully.
		while (*s && *s != '{') {
			const char* selStart;
			const char* selEnd;
			NSVGstyleDeclaration* style;

			while (*s && (nsvg__isspace(*s) || *s == ','))
				s++;
			if (!*s || *s == '{')
				break;

			selStart = s;
			while (*s && !nsvg__isspace(*s) && *s != ',' && *s != '{')
				s++;
			selEnd = s;

			if (*selStart != '.' || nstyles >= NSVG_MAX_CLASSES)
				continue; // unsupported selector, or staging array full
			selStart++; // strip leading '.'

			style = (NSVGstyleDeclaration*)malloc(sizeof(NSVGstyleDeclaration));
			if (style == NULL)
				continue;
			style->className = nsvg__strndup(selStart, (size_t)(selEnd - selStart));
			if (style->className == NULL) {
				free(style);
				continue;
			}
			style->propertiesText = NULL;
			style->next = NULL;
			styles[nstyles++] = style;
		}
		if (!*s) {
			// No '{' found - discard pending styles and stop.
			for (i = 0; i < nstyles; i++) {
				free(styles[i]->className);
				free(styles[i]);
			}
			break;
		}
		s++; // advance past '{'

		// 2) Find the end of the properties block (up to '}').
		propsStart = s;
		while (*s && *s != '}')
			s++;
		propsEnd = s;

		// 3) Allocate propertiesText for each pending style. Commit successful ones to
		//    p->styles (head-insertion); free any whose allocation fails.
		for (i = 0; i < nstyles; i++) {
			styles[i]->propertiesText = nsvg__strndup(propsStart, (size_t)(propsEnd - propsStart));
			if (styles[i]->propertiesText == NULL) {
				free(styles[i]->className);
				free(styles[i]);
			} else {
				styles[i]->next = p->styles;
				p->styles = styles[i];
			}
		}

		if (*s)
			s++; // advance past '}'
	}
}

static void nsvg__imageBounds(NSVGparser* p, float* bounds)
{
	NSVGshape* shape;
	shape = p->image->shapes;
	if (shape == NULL) {
		bounds[0] = bounds[1] = bounds[2] = bounds[3] = 0.0;
		return;
	}
	bounds[0] = shape->bounds[0];
	bounds[1] = shape->bounds[1];
	bounds[2] = shape->bounds[2];
	bounds[3] = shape->bounds[3];
	for (shape = shape->next; shape != NULL; shape = shape->next) {
		bounds[0] = nsvg__minf(bounds[0], shape->bounds[0]);
		bounds[1] = nsvg__minf(bounds[1], shape->bounds[1]);
		bounds[2] = nsvg__maxf(bounds[2], shape->bounds[2]);
		bounds[3] = nsvg__maxf(bounds[3], shape->bounds[3]);
	}
}

static float nsvg__viewAlign(float content, float container, int type)
{
	if (type == NSVG_ALIGN_MIN)
		return 0;
	else if (type == NSVG_ALIGN_MAX)
		return container - content;
	// mid
	return (container - content) * 0.5f;
}

static void nsvg__scaleGradient(NSVGgradient* grad, float tx, float ty, float sx, float sy)
{
	float t[6];
	nsvg__xformSetTranslation(t, tx, ty);
	nsvg__xformMultiply (grad->xform, t);

	nsvg__xformSetScale(t, sx, sy);
	nsvg__xformMultiply (grad->xform, t);
}

static void nsvg__scaleToViewbox(NSVGparser* p, const char* units)
{
	NSVGshape* shape;
	NSVGpath* path;
	float tx, ty, sx, sy, us, bounds[4], t[6], avgs;
	int i;
	float* pt;

	// Guess image size if not set completely.
	nsvg__imageBounds(p, bounds);

	if (p->viewWidth == 0) {
		if (p->image->width > 0) {
			p->viewWidth = p->image->width;
		} else {
			p->viewMinx = bounds[0];
			p->viewWidth = bounds[2] - bounds[0];
		}
	}
	if (p->viewHeight == 0) {
		if (p->image->height > 0) {
			p->viewHeight = p->image->height;
		} else {
			p->viewMiny = bounds[1];
			p->viewHeight = bounds[3] - bounds[1];
		}
	}
	if (p->image->width == 0)
		p->image->width = p->viewWidth;
	if (p->image->height == 0)
		p->image->height = p->viewHeight;

	tx = -p->viewMinx;
	ty = -p->viewMiny;
	sx = p->viewWidth > 0 ? p->image->width / p->viewWidth : 0;
	sy = p->viewHeight > 0 ? p->image->height / p->viewHeight : 0;
	// Unit scaling
	us = 1.0f / nsvg__convertToPixels(p, nsvg__coord(1.0f, nsvg__parseUnits(units)), 0.0f, 1.0f);

	// Fix aspect ratio
	if (p->alignType == NSVG_ALIGN_MEET) {
		// fit whole image into viewbox
		sx = sy = nsvg__minf(sx, sy);
		tx += nsvg__viewAlign(p->viewWidth*sx, p->image->width, p->alignX) / sx;
		ty += nsvg__viewAlign(p->viewHeight*sy, p->image->height, p->alignY) / sy;
	} else if (p->alignType == NSVG_ALIGN_SLICE) {
		// fill whole viewbox with image
		sx = sy = nsvg__maxf(sx, sy);
		tx += nsvg__viewAlign(p->viewWidth*sx, p->image->width, p->alignX) / sx;
		ty += nsvg__viewAlign(p->viewHeight*sy, p->image->height, p->alignY) / sy;
	}

	// Transform
	sx *= us;
	sy *= us;
	avgs = (sx+sy) / 2.0f;
	for (shape = p->image->shapes; shape != NULL; shape = shape->next) {
		shape->bounds[0] = (shape->bounds[0] + tx) * sx;
		shape->bounds[1] = (shape->bounds[1] + ty) * sy;
		shape->bounds[2] = (shape->bounds[2] + tx) * sx;
		shape->bounds[3] = (shape->bounds[3] + ty) * sy;
		for (path = shape->paths; path != NULL; path = path->next) {
			path->bounds[0] = (path->bounds[0] + tx) * sx;
			path->bounds[1] = (path->bounds[1] + ty) * sy;
			path->bounds[2] = (path->bounds[2] + tx) * sx;
			path->bounds[3] = (path->bounds[3] + ty) * sy;
			for (i =0; i < path->npts; i++) {
				pt = &path->pts[i*2];
				pt[0] = (pt[0] + tx) * sx;
				pt[1] = (pt[1] + ty) * sy;
			}
		}

		if (shape->fill.type == NSVG_PAINT_LINEAR_GRADIENT || shape->fill.type == NSVG_PAINT_RADIAL_GRADIENT) {
			nsvg__scaleGradient(shape->fill.gradient, tx,ty, sx,sy);
			memcpy(t, shape->fill.gradient->xform, sizeof(float)*6);
			nsvg__xformInverse(shape->fill.gradient->xform, t);
		}
		if (shape->stroke.type == NSVG_PAINT_LINEAR_GRADIENT || shape->stroke.type == NSVG_PAINT_RADIAL_GRADIENT) {
			nsvg__scaleGradient(shape->stroke.gradient, tx,ty, sx,sy);
			memcpy(t, shape->stroke.gradient->xform, sizeof(float)*6);
			nsvg__xformInverse(shape->stroke.gradient->xform, t);
		}

		shape->strokeWidth *= avgs;
		shape->strokeDashOffset *= avgs;
		for (i = 0; i < shape->strokeDashCount; i++)
			shape->strokeDashArray[i] *= avgs;
	}
}

static void nsvg__createGradients(NSVGparser* p)
{
	NSVGshape* shape;

	for (shape = p->image->shapes; shape != NULL; shape = shape->next) {
		if (shape->fill.type == NSVG_PAINT_UNDEF) {
			if (shape->fillGradient[0] != '\0') {
				float inv[6], localBounds[4];
				nsvg__xformInverse(inv, shape->xform);
				nsvg__getLocalBounds(localBounds, shape, inv);
				shape->fill.gradient = nsvg__createGradient(p, shape->fillGradient, localBounds, shape->xform, &shape->fill.type);
			}
			if (shape->fill.type == NSVG_PAINT_UNDEF) {
				shape->fill.type = NSVG_PAINT_NONE;
			}
		}
		if (shape->stroke.type == NSVG_PAINT_UNDEF) {
			if (shape->strokeGradient[0] != '\0') {
				float inv[6], localBounds[4];
				nsvg__xformInverse(inv, shape->xform);
				nsvg__getLocalBounds(localBounds, shape, inv);
				shape->stroke.gradient = nsvg__createGradient(p, shape->strokeGradient, localBounds, shape->xform, &shape->stroke.type);
			}
			if (shape->stroke.type == NSVG_PAINT_UNDEF) {
				shape->stroke.type = NSVG_PAINT_NONE;
			}
		}
	}
}

NSVGimage* nsvgParse(char* input, const char* units, float dpi)
{
	NSVGparser* p;
	NSVGimage* ret = 0;

	p = nsvg__createParser();
	if (p == NULL) {
		return NULL;
	}
	p->dpi = dpi;

	nsvg__parseXML(input, nsvg__startElement, nsvg__endElement, nsvg__content, p);

	// Create gradients after all definitions have been parsed
	nsvg__createGradients(p);

	// Scale to viewBox
	nsvg__scaleToViewbox(p, units);

	ret = p->image;
	p->image = NULL;

	nsvg__deleteParser(p);

	return ret;
}

NSVGimage* nsvgParseFromFile(const char* filename, const char* units, float dpi)
{
	FILE* fp = NULL;
	size_t size;
	char* data = NULL;
	NSVGimage* image = NULL;

	fp = fopen(filename, "rb");
	if (!fp) goto error;
	if (fseek(fp, 0, SEEK_END)) goto error;
	size = ftell(fp);
	if (size == -1l) goto error;
	if (fseek(fp, 0, SEEK_SET)) goto error;
	data = (char*)malloc(size+1);
	if (data == NULL) goto error;
	if (fread(data, 1, size, fp) != size) goto error;
	data[size] = '\0';	// Must be null terminated.
	fclose(fp);
	image = nsvgParse(data, units, dpi);
	free(data);

	return image;

error:
	if (fp) fclose(fp);
	if (data) free(data);
	if (image) nsvgDelete(image);
	return NULL;
}

NSVGpath* nsvgDuplicatePath(NSVGpath* p)
{
    NSVGpath* res = NULL;

    if (p == NULL)
        return NULL;

    res = (NSVGpath*)malloc(sizeof(NSVGpath));
    if (res == NULL) goto error;
    memset(res, 0, sizeof(NSVGpath));

    res->pts = (float*)malloc(p->npts*2*sizeof(float));
    if (res->pts == NULL) goto error;
    memcpy(res->pts, p->pts, p->npts * sizeof(float) * 2);
    res->npts = p->npts;

    memcpy(res->bounds, p->bounds, sizeof(p->bounds));

    res->closed = p->closed;

    return res;

error:
    if (res != NULL) {
        free(res->pts);
        free(res);
    }
    return NULL;
}

void nsvgDelete(NSVGimage* image)
{
	NSVGshape *snext, *shape;
	if (image == NULL) return;
	shape = image->shapes;
	while (shape != NULL) {
		snext = shape->next;
		nsvg__deletePaths(shape->paths);
		nsvg__deletePaint(&shape->fill);
		nsvg__deletePaint(&shape->stroke);
		free(shape);
		shape = snext;
	}
	free(image);
}

#endif // NANOSVG_IMPLEMENTATION

#endif // NANOSVG_H

#define NANOSVGRAST_IMPLEMENTATION
/*
 * Copyright (c) 2013-14 Mikko Mononen memon@inside.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * The polygon rasterization is heavily based on stb_truetype rasterizer
 * by Sean Barrett - http://nothings.org/
 *
 */

#ifndef NANOSVGRAST_H
#define NANOSVGRAST_H

// nanosvg.h is embedded above; no separate include needed.

#ifndef NANOSVGRAST_CPLUSPLUS
#ifdef __cplusplus
extern "C" {
#endif
#endif

typedef struct NSVGrasterizer NSVGrasterizer;

/* Example Usage:
	// Load SVG
	NSVGimage* image;
	image = nsvgParseFromFile("test.svg", "px", 96);

	// Create rasterizer (can be used to render multiple images).
	struct NSVGrasterizer* rast = nsvgCreateRasterizer();
	// Allocate memory for image
	unsigned char* img = malloc(w*h*4);
	// Rasterize
	nsvgRasterize(rast, image, 0,0,1, img, w, h, w*4);
*/

// Allocated rasterizer context.
NSVGrasterizer* nsvgCreateRasterizer(void);

// Rasterizes SVG image, returns RGBA image (non-premultiplied alpha)
//   r - pointer to rasterizer context
//   image - pointer to image to rasterize
//   tx,ty - image offset (applied after scaling)
//   scale - image scale
//   dst - pointer to destination image data, 4 bytes per pixel (RGBA)
//   w - width of the image to render
//   h - height of the image to render
//   stride - number of bytes per scaleline in the destination buffer
void nsvgRasterize(NSVGrasterizer* r,
				   NSVGimage* image, float tx, float ty, float scale,
				   unsigned char* dst, int w, int h, int stride);

// Deletes rasterizer context.
void nsvgDeleteRasterizer(NSVGrasterizer*);


#ifndef NANOSVGRAST_CPLUSPLUS
#ifdef __cplusplus
}
#endif
#endif

#ifdef NANOSVGRAST_IMPLEMENTATION

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define NSVG__SUBSAMPLES	5
#define NSVG__FIXSHIFT		10
#define NSVG__FIX			(1 << NSVG__FIXSHIFT)
#define NSVG__FIXMASK		(NSVG__FIX-1)
#define NSVG__MEMPAGE_SIZE	1024

typedef struct NSVGedge {
	float x0,y0, x1,y1;
	int dir;
	struct NSVGedge* next;
} NSVGedge;

typedef struct NSVGpoint {
	float x, y;
	float dx, dy;
	float len;
	float dmx, dmy;
	unsigned char flags;
} NSVGpoint;

typedef struct NSVGactiveEdge {
	int x,dx;
	float ey;
	int dir;
	struct NSVGactiveEdge *next;
} NSVGactiveEdge;

typedef struct NSVGmemPage {
	unsigned char mem[NSVG__MEMPAGE_SIZE];
	int size;
	struct NSVGmemPage* next;
} NSVGmemPage;

typedef struct NSVGcachedPaint {
	signed char type;
	char spread;
	float xform[6];
	unsigned int colors[256];
} NSVGcachedPaint;

struct NSVGrasterizer
{
	float px, py;

	float tessTol;
	float distTol;

	NSVGedge* edges;
	int nedges;
	int cedges;

	NSVGpoint* points;
	int npoints;
	int cpoints;

	NSVGpoint* points2;
	int npoints2;
	int cpoints2;

	NSVGactiveEdge* freelist;
	NSVGmemPage* pages;
	NSVGmemPage* curpage;

	unsigned char* scanline;
	int cscanline;

	unsigned char* bitmap;
	int width, height, stride;
};

NSVGrasterizer* nsvgCreateRasterizer(void)
{
	NSVGrasterizer* r = (NSVGrasterizer*)malloc(sizeof(NSVGrasterizer));
	if (r == NULL) goto error;
	memset(r, 0, sizeof(NSVGrasterizer));

	r->tessTol = 0.25f;
	r->distTol = 0.01f;

	return r;

error:
	nsvgDeleteRasterizer(r);
	return NULL;
}

void nsvgDeleteRasterizer(NSVGrasterizer* r)
{
	NSVGmemPage* p;

	if (r == NULL) return;

	p = r->pages;
	while (p != NULL) {
		NSVGmemPage* next = p->next;
		free(p);
		p = next;
	}

	if (r->edges) free(r->edges);
	if (r->points) free(r->points);
	if (r->points2) free(r->points2);
	if (r->scanline) free(r->scanline);

	free(r);
}

static NSVGmemPage* nsvg__nextPage(NSVGrasterizer* r, NSVGmemPage* cur)
{
	NSVGmemPage *newp;

	// If using existing chain, return the next page in chain
	if (cur != NULL && cur->next != NULL) {
		return cur->next;
	}

	// Alloc new page
	newp = (NSVGmemPage*)malloc(sizeof(NSVGmemPage));
	if (newp == NULL) return NULL;
	memset(newp, 0, sizeof(NSVGmemPage));

	// Add to linked list
	if (cur != NULL)
		cur->next = newp;
	else
		r->pages = newp;

	return newp;
}

static void nsvg__resetPool(NSVGrasterizer* r)
{
	NSVGmemPage* p = r->pages;
	while (p != NULL) {
		p->size = 0;
		p = p->next;
	}
	r->curpage = r->pages;
}

static unsigned char* nsvg__alloc(NSVGrasterizer* r, int size)
{
	unsigned char* buf;
	if (size > NSVG__MEMPAGE_SIZE) return NULL;
	if (r->curpage == NULL || r->curpage->size+size > NSVG__MEMPAGE_SIZE) {
		r->curpage = nsvg__nextPage(r, r->curpage);
	}
	buf = &r->curpage->mem[r->curpage->size];
	r->curpage->size += size;
	return buf;
}

static int nsvg__ptEquals(float x1, float y1, float x2, float y2, float tol)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx*dx + dy*dy < tol*tol;
}

static void nsvg__addPathPoint(NSVGrasterizer* r, float x, float y, int flags)
{
	NSVGpoint* pt;

	if (r->npoints > 0) {
		pt = &r->points[r->npoints-1];
		if (nsvg__ptEquals(pt->x,pt->y, x,y, r->distTol)) {
			pt->flags = (unsigned char)(pt->flags | flags);
			return;
		}
	}

	if (r->npoints+1 > r->cpoints) {
		r->cpoints = r->cpoints > 0 ? r->cpoints * 2 : 64;
		r->points = (NSVGpoint*)realloc(r->points, sizeof(NSVGpoint) * r->cpoints);
		if (r->points == NULL) return;
	}

	pt = &r->points[r->npoints];
	pt->x = x;
	pt->y = y;
	pt->flags = (unsigned char)flags;
	r->npoints++;
}

static void nsvg__appendPathPoint(NSVGrasterizer* r, NSVGpoint pt)
{
	if (r->npoints+1 > r->cpoints) {
		r->cpoints = r->cpoints > 0 ? r->cpoints * 2 : 64;
		r->points = (NSVGpoint*)realloc(r->points, sizeof(NSVGpoint) * r->cpoints);
		if (r->points == NULL) return;
	}
	r->points[r->npoints] = pt;
	r->npoints++;
}

static void nsvg__duplicatePoints(NSVGrasterizer* r)
{
	if (r->npoints > r->cpoints2) {
		r->cpoints2 = r->npoints;
		r->points2 = (NSVGpoint*)realloc(r->points2, sizeof(NSVGpoint) * r->cpoints2);
		if (r->points2 == NULL) return;
	}

	memcpy(r->points2, r->points, sizeof(NSVGpoint) * r->npoints);
	r->npoints2 = r->npoints;
}

static void nsvg__addEdge(NSVGrasterizer* r, float x0, float y0, float x1, float y1)
{
	NSVGedge* e;

	// Skip horizontal edges
	if (y0 == y1)
		return;

	if (r->nedges+1 > r->cedges) {
		r->cedges = r->cedges > 0 ? r->cedges * 2 : 64;
		r->edges = (NSVGedge*)realloc(r->edges, sizeof(NSVGedge) * r->cedges);
		if (r->edges == NULL) return;
	}

	e = &r->edges[r->nedges];
	r->nedges++;

	if (y0 < y1) {
		e->x0 = x0;
		e->y0 = y0;
		e->x1 = x1;
		e->y1 = y1;
		e->dir = 1;
	} else {
		e->x0 = x1;
		e->y0 = y1;
		e->x1 = x0;
		e->y1 = y0;
		e->dir = -1;
	}
}

static float nsvg__normalize(float *x, float* y)
{
	float d = sqrtf((*x)*(*x) + (*y)*(*y));
	if (d > 1e-6f) {
		float id = 1.0f / d;
		*x *= id;
		*y *= id;
	}
	return d;
}

static float nsvg__absf(float x) { return x < 0 ? -x : x; }
static float nsvg__roundf(float x) { return (x >= 0) ? floorf(x + 0.5) : ceilf(x - 0.5); }

static void nsvg__flattenCubicBez(NSVGrasterizer* r,
								  float x1, float y1, float x2, float y2,
								  float x3, float y3, float x4, float y4,
								  int level, int type)
{
	float x12,y12,x23,y23,x34,y34,x123,y123,x234,y234,x1234,y1234;
	float dx,dy,d2,d3;

	if (level > 10) return;

	x12 = (x1+x2)*0.5f;
	y12 = (y1+y2)*0.5f;
	x23 = (x2+x3)*0.5f;
	y23 = (y2+y3)*0.5f;
	x34 = (x3+x4)*0.5f;
	y34 = (y3+y4)*0.5f;
	x123 = (x12+x23)*0.5f;
	y123 = (y12+y23)*0.5f;

	dx = x4 - x1;
	dy = y4 - y1;
	d2 = nsvg__absf((x2 - x4) * dy - (y2 - y4) * dx);
	d3 = nsvg__absf((x3 - x4) * dy - (y3 - y4) * dx);

	if ((d2 + d3)*(d2 + d3) < r->tessTol * (dx*dx + dy*dy)) {
		nsvg__addPathPoint(r, x4, y4, type);
		return;
	}

	x234 = (x23+x34)*0.5f;
	y234 = (y23+y34)*0.5f;
	x1234 = (x123+x234)*0.5f;
	y1234 = (y123+y234)*0.5f;

	nsvg__flattenCubicBez(r, x1,y1, x12,y12, x123,y123, x1234,y1234, level+1, 0);
	nsvg__flattenCubicBez(r, x1234,y1234, x234,y234, x34,y34, x4,y4, level+1, type);
}

static void nsvg__flattenShape(NSVGrasterizer* r, NSVGshape* shape, float scale)
{
	int i, j;
	NSVGpath* path;

	for (path = shape->paths; path != NULL; path = path->next) {
		r->npoints = 0;
		// Flatten path
		nsvg__addPathPoint(r, path->pts[0]*scale, path->pts[1]*scale, 0);
		for (i = 0; i < path->npts-1; i += 3) {
			float* p = &path->pts[i*2];
			nsvg__flattenCubicBez(r, p[0]*scale,p[1]*scale, p[2]*scale,p[3]*scale, p[4]*scale,p[5]*scale, p[6]*scale,p[7]*scale, 0, 0);
		}
		// Close path
		nsvg__addPathPoint(r, path->pts[0]*scale, path->pts[1]*scale, 0);
		// Build edges
		for (i = 0, j = r->npoints-1; i < r->npoints; j = i++)
			nsvg__addEdge(r, r->points[j].x, r->points[j].y, r->points[i].x, r->points[i].y);
	}
}

enum NSVGpointFlags
{
	NSVG_PT_CORNER = 0x01,
	NSVG_PT_BEVEL = 0x02,
	NSVG_PT_LEFT = 0x04
};

static void nsvg__initClosed(NSVGpoint* left, NSVGpoint* right, NSVGpoint* p0, NSVGpoint* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float dx = p1->x - p0->x;
	float dy = p1->y - p0->y;
	float len = nsvg__normalize(&dx, &dy);
	float px = p0->x + dx*len*0.5f, py = p0->y + dy*len*0.5f;
	float dlx = dy, dly = -dx;
	float lx = px - dlx*w, ly = py - dly*w;
	float rx = px + dlx*w, ry = py + dly*w;
	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__buttCap(NSVGrasterizer* r, NSVGpoint* left, NSVGpoint* right, NSVGpoint* p, float dx, float dy, float lineWidth, int connect)
{
	float w = lineWidth * 0.5f;
	float px = p->x, py = p->y;
	float dlx = dy, dly = -dx;
	float lx = px - dlx*w, ly = py - dly*w;
	float rx = px + dlx*w, ry = py + dly*w;

	nsvg__addEdge(r, lx, ly, rx, ry);

	if (connect) {
		nsvg__addEdge(r, left->x, left->y, lx, ly);
		nsvg__addEdge(r, rx, ry, right->x, right->y);
	}
	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__squareCap(NSVGrasterizer* r, NSVGpoint* left, NSVGpoint* right, NSVGpoint* p, float dx, float dy, float lineWidth, int connect)
{
	float w = lineWidth * 0.5f;
	float px = p->x - dx*w, py = p->y - dy*w;
	float dlx = dy, dly = -dx;
	float lx = px - dlx*w, ly = py - dly*w;
	float rx = px + dlx*w, ry = py + dly*w;

	nsvg__addEdge(r, lx, ly, rx, ry);

	if (connect) {
		nsvg__addEdge(r, left->x, left->y, lx, ly);
		nsvg__addEdge(r, rx, ry, right->x, right->y);
	}
	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

#ifndef NSVG_PI
#define NSVG_PI (3.14159265358979323846264338327f)
#endif

static void nsvg__roundCap(NSVGrasterizer* r, NSVGpoint* left, NSVGpoint* right, NSVGpoint* p, float dx, float dy, float lineWidth, int ncap, int connect)
{
	int i;
	float w = lineWidth * 0.5f;
	float px = p->x, py = p->y;
	float dlx = dy, dly = -dx;
	float lx = 0, ly = 0, rx = 0, ry = 0, prevx = 0, prevy = 0;

	for (i = 0; i < ncap; i++) {
		float a = (float)i/(float)(ncap-1)*NSVG_PI;
		float ax = cosf(a) * w, ay = sinf(a) * w;
		float x = px - dlx*ax - dx*ay;
		float y = py - dly*ax - dy*ay;

		if (i > 0)
			nsvg__addEdge(r, prevx, prevy, x, y);

		prevx = x;
		prevy = y;

		if (i == 0) {
			lx = x; ly = y;
		} else if (i == ncap-1) {
			rx = x; ry = y;
		}
	}

	if (connect) {
		nsvg__addEdge(r, left->x, left->y, lx, ly);
		nsvg__addEdge(r, rx, ry, right->x, right->y);
	}

	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__bevelJoin(NSVGrasterizer* r, NSVGpoint* left, NSVGpoint* right, NSVGpoint* p0, NSVGpoint* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0 = p1->x - (dlx0 * w), ly0 = p1->y - (dly0 * w);
	float rx0 = p1->x + (dlx0 * w), ry0 = p1->y + (dly0 * w);
	float lx1 = p1->x - (dlx1 * w), ly1 = p1->y - (dly1 * w);
	float rx1 = p1->x + (dlx1 * w), ry1 = p1->y + (dly1 * w);

	nsvg__addEdge(r, lx0, ly0, left->x, left->y);
	nsvg__addEdge(r, lx1, ly1, lx0, ly0);

	nsvg__addEdge(r, right->x, right->y, rx0, ry0);
	nsvg__addEdge(r, rx0, ry0, rx1, ry1);

	left->x = lx1; left->y = ly1;
	right->x = rx1; right->y = ry1;
}

static void nsvg__miterJoin(NSVGrasterizer* r, NSVGpoint* left, NSVGpoint* right, NSVGpoint* p0, NSVGpoint* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0, rx0, lx1, rx1;
	float ly0, ry0, ly1, ry1;

	if (p1->flags & NSVG_PT_LEFT) {
		lx0 = lx1 = p1->x - p1->dmx * w;
		ly0 = ly1 = p1->y - p1->dmy * w;
		nsvg__addEdge(r, lx1, ly1, left->x, left->y);

		rx0 = p1->x + (dlx0 * w);
		ry0 = p1->y + (dly0 * w);
		rx1 = p1->x + (dlx1 * w);
		ry1 = p1->y + (dly1 * w);
		nsvg__addEdge(r, right->x, right->y, rx0, ry0);
		nsvg__addEdge(r, rx0, ry0, rx1, ry1);
	} else {
		lx0 = p1->x - (dlx0 * w);
		ly0 = p1->y - (dly0 * w);
		lx1 = p1->x - (dlx1 * w);
		ly1 = p1->y - (dly1 * w);
		nsvg__addEdge(r, lx0, ly0, left->x, left->y);
		nsvg__addEdge(r, lx1, ly1, lx0, ly0);

		rx0 = rx1 = p1->x + p1->dmx * w;
		ry0 = ry1 = p1->y + p1->dmy * w;
		nsvg__addEdge(r, right->x, right->y, rx1, ry1);
	}

	left->x = lx1; left->y = ly1;
	right->x = rx1; right->y = ry1;
}

static void nsvg__roundJoin(NSVGrasterizer* r, NSVGpoint* left, NSVGpoint* right, NSVGpoint* p0, NSVGpoint* p1, float lineWidth, int ncap)
{
	int i, n;
	float w = lineWidth * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float a0 = atan2f(dly0, dlx0);
	float a1 = atan2f(dly1, dlx1);
	float da = a1 - a0;
	float lx, ly, rx, ry;

	if (da < NSVG_PI) da += NSVG_PI*2;
	if (da > NSVG_PI) da -= NSVG_PI*2;

	n = (int)ceilf((nsvg__absf(da) / NSVG_PI) * (float)ncap);
	if (n < 2) n = 2;
	if (n > ncap) n = ncap;

	lx = left->x;
	ly = left->y;
	rx = right->x;
	ry = right->y;

	for (i = 0; i < n; i++) {
		float u = (float)i/(float)(n-1);
		float a = a0 + u*da;
		float ax = cosf(a) * w, ay = sinf(a) * w;
		float lx1 = p1->x - ax, ly1 = p1->y - ay;
		float rx1 = p1->x + ax, ry1 = p1->y + ay;

		nsvg__addEdge(r, lx1, ly1, lx, ly);
		nsvg__addEdge(r, rx, ry, rx1, ry1);

		lx = lx1; ly = ly1;
		rx = rx1; ry = ry1;
	}

	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__straightJoin(NSVGrasterizer* r, NSVGpoint* left, NSVGpoint* right, NSVGpoint* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float lx = p1->x - (p1->dmx * w), ly = p1->y - (p1->dmy * w);
	float rx = p1->x + (p1->dmx * w), ry = p1->y + (p1->dmy * w);

	nsvg__addEdge(r, lx, ly, left->x, left->y);
	nsvg__addEdge(r, right->x, right->y, rx, ry);

	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static int nsvg__curveDivs(float r, float arc, float tol)
{
	float da = acosf(r / (r + tol)) * 2.0f;
	int divs = (int)ceilf(arc / da);
	if (divs < 2) divs = 2;
	return divs;
}

static void nsvg__expandStroke(NSVGrasterizer* r, NSVGpoint* points, int npoints, int closed, int lineJoin, int lineCap, float lineWidth)
{
	int ncap = nsvg__curveDivs(lineWidth*0.5f, NSVG_PI, r->tessTol);	// Calculate divisions per half circle.
	NSVGpoint left = {0,0,0,0,0,0,0,0}, right = {0,0,0,0,0,0,0,0}, firstLeft = {0,0,0,0,0,0,0,0}, firstRight = {0,0,0,0,0,0,0,0};
	NSVGpoint* p0, *p1;
	int j, s, e;

	// Build stroke edges
	if (closed) {
		// Looping
		p0 = &points[npoints-1];
		p1 = &points[0];
		s = 0;
		e = npoints;
	} else {
		// Add cap
		p0 = &points[0];
		p1 = &points[1];
		s = 1;
		e = npoints-1;
	}

	if (closed) {
		nsvg__initClosed(&left, &right, p0, p1, lineWidth);
		firstLeft = left;
		firstRight = right;
	} else {
		// Add cap
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		nsvg__normalize(&dx, &dy);
		if (lineCap == NSVG_CAP_BUTT)
			nsvg__buttCap(r, &left, &right, p0, dx, dy, lineWidth, 0);
		else if (lineCap == NSVG_CAP_SQUARE)
			nsvg__squareCap(r, &left, &right, p0, dx, dy, lineWidth, 0);
		else if (lineCap == NSVG_CAP_ROUND)
			nsvg__roundCap(r, &left, &right, p0, dx, dy, lineWidth, ncap, 0);
	}

	for (j = s; j < e; ++j) {
		if (p1->flags & NSVG_PT_CORNER) {
			if (lineJoin == NSVG_JOIN_ROUND)
				nsvg__roundJoin(r, &left, &right, p0, p1, lineWidth, ncap);
			else if (lineJoin == NSVG_JOIN_BEVEL || (p1->flags & NSVG_PT_BEVEL))
				nsvg__bevelJoin(r, &left, &right, p0, p1, lineWidth);
			else
				nsvg__miterJoin(r, &left, &right, p0, p1, lineWidth);
		} else {
			nsvg__straightJoin(r, &left, &right, p1, lineWidth);
		}
		p0 = p1++;
	}

	if (closed) {
		// Loop it
		nsvg__addEdge(r, firstLeft.x, firstLeft.y, left.x, left.y);
		nsvg__addEdge(r, right.x, right.y, firstRight.x, firstRight.y);
	} else {
		// Add cap
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		nsvg__normalize(&dx, &dy);
		if (lineCap == NSVG_CAP_BUTT)
			nsvg__buttCap(r, &right, &left, p1, -dx, -dy, lineWidth, 1);
		else if (lineCap == NSVG_CAP_SQUARE)
			nsvg__squareCap(r, &right, &left, p1, -dx, -dy, lineWidth, 1);
		else if (lineCap == NSVG_CAP_ROUND)
			nsvg__roundCap(r, &right, &left, p1, -dx, -dy, lineWidth, ncap, 1);
	}
}

static void nsvg__prepareStroke(NSVGrasterizer* r, float miterLimit, int lineJoin)
{
	int i, j;
	NSVGpoint* p0, *p1;

	p0 = &r->points[r->npoints-1];
	p1 = &r->points[0];
	for (i = 0; i < r->npoints; i++) {
		// Calculate segment direction and length
		p0->dx = p1->x - p0->x;
		p0->dy = p1->y - p0->y;
		p0->len = nsvg__normalize(&p0->dx, &p0->dy);
		// Advance
		p0 = p1++;
	}

	// calculate joins
	p0 = &r->points[r->npoints-1];
	p1 = &r->points[0];
	for (j = 0; j < r->npoints; j++) {
		float dlx0, dly0, dlx1, dly1, dmr2, cross;
		dlx0 = p0->dy;
		dly0 = -p0->dx;
		dlx1 = p1->dy;
		dly1 = -p1->dx;
		// Calculate extrusions
		p1->dmx = (dlx0 + dlx1) * 0.5f;
		p1->dmy = (dly0 + dly1) * 0.5f;
		dmr2 = p1->dmx*p1->dmx + p1->dmy*p1->dmy;
		if (dmr2 > 0.000001f) {
			float s2 = 1.0f / dmr2;
			if (s2 > 600.0f) {
				s2 = 600.0f;
			}
			p1->dmx *= s2;
			p1->dmy *= s2;
		}

		// Clear flags, but keep the corner.
		p1->flags = (p1->flags & NSVG_PT_CORNER) ? NSVG_PT_CORNER : 0;

		// Keep track of left turns.
		cross = p1->dx * p0->dy - p0->dx * p1->dy;
		if (cross > 0.0f)
			p1->flags |= NSVG_PT_LEFT;

		// Check to see if the corner needs to be beveled.
		if (p1->flags & NSVG_PT_CORNER) {
			if ((dmr2 * miterLimit*miterLimit) < 1.0f || lineJoin == NSVG_JOIN_BEVEL || lineJoin == NSVG_JOIN_ROUND) {
				p1->flags |= NSVG_PT_BEVEL;
			}
		}

		p0 = p1++;
	}
}

static void nsvg__flattenShapeStroke(NSVGrasterizer* r, NSVGshape* shape, float scale)
{
	int i, j, closed;
	NSVGpath* path;
	NSVGpoint* p0, *p1;
	float miterLimit = shape->miterLimit;
	int lineJoin = shape->strokeLineJoin;
	int lineCap = shape->strokeLineCap;
	float lineWidth = shape->strokeWidth * scale;

	for (path = shape->paths; path != NULL; path = path->next) {
		// Flatten path
		r->npoints = 0;
		nsvg__addPathPoint(r, path->pts[0]*scale, path->pts[1]*scale, NSVG_PT_CORNER);
		for (i = 0; i < path->npts-1; i += 3) {
			float* p = &path->pts[i*2];
			nsvg__flattenCubicBez(r, p[0]*scale,p[1]*scale, p[2]*scale,p[3]*scale, p[4]*scale,p[5]*scale, p[6]*scale,p[7]*scale, 0, NSVG_PT_CORNER);
		}
		if (r->npoints < 2)
			continue;

		closed = path->closed;

		// If the first and last points are the same, remove the last, mark as closed path.
		p0 = &r->points[r->npoints-1];
		p1 = &r->points[0];
		if (nsvg__ptEquals(p0->x,p0->y, p1->x,p1->y, r->distTol)) {
			r->npoints--;
			p0 = &r->points[r->npoints-1];
			closed = 1;
		}

		if (shape->strokeDashCount > 0) {
			int idash = 0, dashState = 1;
			float totalDist = 0, dashLen, allDashLen, dashOffset;
			NSVGpoint cur;

			if (closed)
				nsvg__appendPathPoint(r, r->points[0]);

			// Duplicate points -> points2.
			nsvg__duplicatePoints(r);

			r->npoints = 0;
 			cur = r->points2[0];
			nsvg__appendPathPoint(r, cur);

			// Figure out dash offset.
			allDashLen = 0;
			for (j = 0; j < shape->strokeDashCount; j++)
				allDashLen += shape->strokeDashArray[j];
			if (shape->strokeDashCount & 1)
				allDashLen *= 2.0f;
			// Find location inside pattern
			dashOffset = fmodf(shape->strokeDashOffset, allDashLen);
			if (dashOffset < 0.0f)
				dashOffset += allDashLen;

			while (dashOffset > shape->strokeDashArray[idash]) {
				dashOffset -= shape->strokeDashArray[idash];
				idash = (idash + 1) % shape->strokeDashCount;
			}
			dashLen = (shape->strokeDashArray[idash] - dashOffset) * scale;

			for (j = 1; j < r->npoints2; ) {
				float dx = r->points2[j].x - cur.x;
				float dy = r->points2[j].y - cur.y;
				float dist = sqrtf(dx*dx + dy*dy);

				if ((totalDist + dist) > dashLen) {
					// Calculate intermediate point
					float d = (dashLen - totalDist) / dist;
					float x = cur.x + dx * d;
					float y = cur.y + dy * d;
					nsvg__addPathPoint(r, x, y, NSVG_PT_CORNER);

					// Stroke
					if (r->npoints > 1 && dashState) {
						nsvg__prepareStroke(r, miterLimit, lineJoin);
						nsvg__expandStroke(r, r->points, r->npoints, 0, lineJoin, lineCap, lineWidth);
					}
					// Advance dash pattern
					dashState = !dashState;
					idash = (idash+1) % shape->strokeDashCount;
					dashLen = shape->strokeDashArray[idash] * scale;
					// Restart
					cur.x = x;
					cur.y = y;
					cur.flags = NSVG_PT_CORNER;
					totalDist = 0.0f;
					r->npoints = 0;
					nsvg__appendPathPoint(r, cur);
				} else {
					totalDist += dist;
					cur = r->points2[j];
					nsvg__appendPathPoint(r, cur);
					j++;
				}
			}
			// Stroke any leftover path
			if (r->npoints > 1 && dashState) {
				nsvg__prepareStroke(r, miterLimit, lineJoin);
				nsvg__expandStroke(r, r->points, r->npoints, 0, lineJoin, lineCap, lineWidth);
			}
		} else {
			nsvg__prepareStroke(r, miterLimit, lineJoin);
			nsvg__expandStroke(r, r->points, r->npoints, closed, lineJoin, lineCap, lineWidth);
		}
	}
}

static int nsvg__cmpEdge(const void *p, const void *q)
{
	const NSVGedge* a = (const NSVGedge*)p;
	const NSVGedge* b = (const NSVGedge*)q;

	if (a->y0 < b->y0) return -1;
	if (a->y0 > b->y0) return  1;
	return 0;
}


static NSVGactiveEdge* nsvg__addActive(NSVGrasterizer* r, NSVGedge* e, float startPoint)
{
	 NSVGactiveEdge* z;

	if (r->freelist != NULL) {
		// Restore from freelist.
		z = r->freelist;
		r->freelist = z->next;
	} else {
		// Alloc new edge.
		z = (NSVGactiveEdge*)nsvg__alloc(r, sizeof(NSVGactiveEdge));
		if (z == NULL) return NULL;
	}

	float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
//	STBTT_assert(e->y0 <= start_point);
	// round dx down to avoid going too far
	if (dxdy < 0)
		z->dx = (int)(-nsvg__roundf(NSVG__FIX * -dxdy));
	else
		z->dx = (int)nsvg__roundf(NSVG__FIX * dxdy);
	z->x = (int)nsvg__roundf(NSVG__FIX * (e->x0 + dxdy * (startPoint - e->y0)));
//	z->x -= off_x * FIX;
	z->ey = e->y1;
	z->next = 0;
	z->dir = e->dir;

	return z;
}

static void nsvg__freeActive(NSVGrasterizer* r, NSVGactiveEdge* z)
{
	z->next = r->freelist;
	r->freelist = z;
}

static void nsvg__fillScanline(unsigned char* scanline, int len, int x0, int x1, int maxWeight, int* xmin, int* xmax)
{
	int i = x0 >> NSVG__FIXSHIFT;
	int j = x1 >> NSVG__FIXSHIFT;
	if (i < *xmin) *xmin = i;
	if (j > *xmax) *xmax = j;
	if (i < len && j >= 0) {
		if (i == j) {
			// x0,x1 are the same pixel, so compute combined coverage
			scanline[i] = (unsigned char)(scanline[i] + ((x1 - x0) * maxWeight >> NSVG__FIXSHIFT));
		} else {
			if (i >= 0) // add antialiasing for x0
				scanline[i] = (unsigned char)(scanline[i] + (((NSVG__FIX - (x0 & NSVG__FIXMASK)) * maxWeight) >> NSVG__FIXSHIFT));
			else
				i = -1; // clip

			if (j < len) // add antialiasing for x1
				scanline[j] = (unsigned char)(scanline[j] + (((x1 & NSVG__FIXMASK) * maxWeight) >> NSVG__FIXSHIFT));
			else
				j = len; // clip

			for (++i; i < j; ++i) // fill pixels between x0 and x1
				scanline[i] = (unsigned char)(scanline[i] + maxWeight);
		}
	}
}

// note: this routine clips fills that extend off the edges... ideally this
// wouldn't happen, but it could happen if the truetype glyph bounding boxes
// are wrong, or if the user supplies a too-small bitmap
static void nsvg__fillActiveEdges(unsigned char* scanline, int len, NSVGactiveEdge* e, int maxWeight, int* xmin, int* xmax, char fillRule)
{
	// non-zero winding fill
	int x0 = 0, w = 0;

	if (fillRule == NSVG_FILLRULE_NONZERO) {
		// Non-zero
		while (e != NULL) {
			if (w == 0) {
				// if we're currently at zero, we need to record the edge start point
				x0 = e->x; w += e->dir;
			} else {
				int x1 = e->x; w += e->dir;
				// if we went to zero, we need to draw
				if (w == 0)
					nsvg__fillScanline(scanline, len, x0, x1, maxWeight, xmin, xmax);
			}
			e = e->next;
		}
	} else if (fillRule == NSVG_FILLRULE_EVENODD) {
		// Even-odd
		while (e != NULL) {
			if (w == 0) {
				// if we're currently at zero, we need to record the edge start point
				x0 = e->x; w = 1;
			} else {
				int x1 = e->x; w = 0;
				nsvg__fillScanline(scanline, len, x0, x1, maxWeight, xmin, xmax);
			}
			e = e->next;
		}
	}
}

static float nsvg__clampf(float a, float mn, float mx) {
	if (isnan(a))
		return mn;
	return a < mn ? mn : (a > mx ? mx : a);
}

static unsigned int nsvg__RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
}

static unsigned int nsvg__lerpRGBA(unsigned int c0, unsigned int c1, float u)
{
	int iu = (int)(nsvg__clampf(u, 0.0f, 1.0f) * 256.0f);
	int r = (((c0) & 0xff)*(256-iu) + (((c1) & 0xff)*iu)) >> 8;
	int g = (((c0>>8) & 0xff)*(256-iu) + (((c1>>8) & 0xff)*iu)) >> 8;
	int b = (((c0>>16) & 0xff)*(256-iu) + (((c1>>16) & 0xff)*iu)) >> 8;
	int a = (((c0>>24) & 0xff)*(256-iu) + (((c1>>24) & 0xff)*iu)) >> 8;
	return nsvg__RGBA((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
}

static unsigned int nsvg__applyOpacity(unsigned int c, float u)
{
	int iu = (int)(nsvg__clampf(u, 0.0f, 1.0f) * 256.0f);
	int r = (c) & 0xff;
	int g = (c>>8) & 0xff;
	int b = (c>>16) & 0xff;
	int a = (((c>>24) & 0xff)*iu) >> 8;
	return nsvg__RGBA((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
}

static inline int nsvg__div255(int x)
{
    return ((x+1) * 257) >> 16;
}

static void nsvg__scanlineSolid(unsigned char* dst, int count, unsigned char* cover, int x, int y,
								float tx, float ty, float scale, NSVGcachedPaint* cache)
{

	if (cache->type == NSVG_PAINT_COLOR) {
		int i, cr, cg, cb, ca;
		cr = cache->colors[0] & 0xff;
		cg = (cache->colors[0] >> 8) & 0xff;
		cb = (cache->colors[0] >> 16) & 0xff;
		ca = (cache->colors[0] >> 24) & 0xff;

		for (i = 0; i < count; i++) {
			int r,g,b;
			int a = nsvg__div255((int)cover[0] * ca);
			int ia = 255 - a;
			// Premultiply
			r = nsvg__div255(cr * a);
			g = nsvg__div255(cg * a);
			b = nsvg__div255(cb * a);

			// Blend over
			r += nsvg__div255(ia * (int)dst[0]);
			g += nsvg__div255(ia * (int)dst[1]);
			b += nsvg__div255(ia * (int)dst[2]);
			a += nsvg__div255(ia * (int)dst[3]);

			dst[0] = (unsigned char)r;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)b;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
		}
	} else if (cache->type == NSVG_PAINT_LINEAR_GRADIENT) {
		// TODO: spread modes.
		// TODO: plenty of opportunities to optimize.
		float fx, fy, dx, gy;
		float* t = cache->xform;
		int i, cr, cg, cb, ca;
		unsigned int c;

		fx = ((float)x - tx) / scale;
		fy = ((float)y - ty) / scale;
		dx = 1.0f / scale;

		for (i = 0; i < count; i++) {
			int r,g,b,a,ia;
			gy = fx*t[1] + fy*t[3] + t[5];
			c = cache->colors[(int)nsvg__clampf(gy*255.0f, 0, 255.0f)];
			cr = (c) & 0xff;
			cg = (c >> 8) & 0xff;
			cb = (c >> 16) & 0xff;
			ca = (c >> 24) & 0xff;

			a = nsvg__div255((int)cover[0] * ca);
			ia = 255 - a;

			// Premultiply
			r = nsvg__div255(cr * a);
			g = nsvg__div255(cg * a);
			b = nsvg__div255(cb * a);

			// Blend over
			r += nsvg__div255(ia * (int)dst[0]);
			g += nsvg__div255(ia * (int)dst[1]);
			b += nsvg__div255(ia * (int)dst[2]);
			a += nsvg__div255(ia * (int)dst[3]);

			dst[0] = (unsigned char)r;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)b;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
			fx += dx;
		}
	} else if (cache->type == NSVG_PAINT_RADIAL_GRADIENT) {
		// TODO: spread modes.
		// TODO: plenty of opportunities to optimize.
		// TODO: focus (fx,fy)
		float fx, fy, dx, gx, gy, gd;
		float* t = cache->xform;
		int i, cr, cg, cb, ca;
		unsigned int c;

		fx = ((float)x - tx) / scale;
		fy = ((float)y - ty) / scale;
		dx = 1.0f / scale;

		for (i = 0; i < count; i++) {
			int r,g,b,a,ia;
			gx = fx*t[0] + fy*t[2] + t[4];
			gy = fx*t[1] + fy*t[3] + t[5];
			gd = sqrtf(gx*gx + gy*gy);
			c = cache->colors[(int)nsvg__clampf(gd*255.0f, 0, 255.0f)];
			cr = (c) & 0xff;
			cg = (c >> 8) & 0xff;
			cb = (c >> 16) & 0xff;
			ca = (c >> 24) & 0xff;

			a = nsvg__div255((int)cover[0] * ca);
			ia = 255 - a;

			// Premultiply
			r = nsvg__div255(cr * a);
			g = nsvg__div255(cg * a);
			b = nsvg__div255(cb * a);

			// Blend over
			r += nsvg__div255(ia * (int)dst[0]);
			g += nsvg__div255(ia * (int)dst[1]);
			b += nsvg__div255(ia * (int)dst[2]);
			a += nsvg__div255(ia * (int)dst[3]);

			dst[0] = (unsigned char)r;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)b;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
			fx += dx;
		}
	}
}

static void nsvg__rasterizeSortedEdges(NSVGrasterizer *r, float tx, float ty, float scale, NSVGcachedPaint* cache, char fillRule)
{
	NSVGactiveEdge *active = NULL;
	int y, s;
	int e = 0;
	int maxWeight = (255 / NSVG__SUBSAMPLES);  // weight per vertical scanline
	int xmin, xmax;

	for (y = 0; y < r->height; y++) {
		memset(r->scanline, 0, r->width);
		xmin = r->width;
		xmax = 0;
		for (s = 0; s < NSVG__SUBSAMPLES; ++s) {
			// find center of pixel for this scanline
			float scany = (float)(y*NSVG__SUBSAMPLES + s) + 0.5f;
			NSVGactiveEdge **step = &active;

			// update all active edges;
			// remove all active edges that terminate before the center of this scanline
			while (*step) {
				NSVGactiveEdge *z = *step;
				if (z->ey <= scany) {
					*step = z->next; // delete from list
//					NSVG__assert(z->valid);
					nsvg__freeActive(r, z);
				} else {
					z->x += z->dx; // advance to position for current scanline
					step = &((*step)->next); // advance through list
				}
			}

			// resort the list if needed
			for (;;) {
				int changed = 0;
				step = &active;
				while (*step && (*step)->next) {
					if ((*step)->x > (*step)->next->x) {
						NSVGactiveEdge* t = *step;
						NSVGactiveEdge* q = t->next;
						t->next = q->next;
						q->next = t;
						*step = q;
						changed = 1;
					}
					step = &(*step)->next;
				}
				if (!changed) break;
			}

			// insert all edges that start before the center of this scanline -- omit ones that also end on this scanline
			while (e < r->nedges && r->edges[e].y0 <= scany) {
				if (r->edges[e].y1 > scany) {
					NSVGactiveEdge* z = nsvg__addActive(r, &r->edges[e], scany);
					if (z == NULL) break;
					// find insertion point
					if (active == NULL) {
						active = z;
					} else if (z->x < active->x) {
						// insert at front
						z->next = active;
						active = z;
					} else {
						// find thing to insert AFTER
						NSVGactiveEdge* p = active;
						while (p->next && p->next->x < z->x)
							p = p->next;
						// at this point, p->next->x is NOT < z->x
						z->next = p->next;
						p->next = z;
					}
				}
				e++;
			}

			// now process all active edges in non-zero fashion
			if (active != NULL)
				nsvg__fillActiveEdges(r->scanline, r->width, active, maxWeight, &xmin, &xmax, fillRule);
		}
		// Blit
		if (xmin < 0) xmin = 0;
		if (xmax > r->width-1) xmax = r->width-1;
		if (xmin <= xmax) {
			nsvg__scanlineSolid(&r->bitmap[y * r->stride] + xmin*4, xmax-xmin+1, &r->scanline[xmin], xmin, y, tx,ty, scale, cache);
		}
	}

}

static void nsvg__unpremultiplyAlpha(unsigned char* image, int w, int h, int stride)
{
	int x,y;

	// Unpremultiply
	for (y = 0; y < h; y++) {
		unsigned char *row = &image[y*stride];
		for (x = 0; x < w; x++) {
			int r = row[0], g = row[1], b = row[2], a = row[3];
			if (a != 0) {
				row[0] = (unsigned char)(r*255/a);
				row[1] = (unsigned char)(g*255/a);
				row[2] = (unsigned char)(b*255/a);
			}
			row += 4;
		}
	}

	// Defringe
	for (y = 0; y < h; y++) {
		unsigned char *row = &image[y*stride];
		for (x = 0; x < w; x++) {
			int r = 0, g = 0, b = 0, a = row[3], n = 0;
			if (a == 0) {
				if (x-1 > 0 && row[-1] != 0) {
					r += row[-4];
					g += row[-3];
					b += row[-2];
					n++;
				}
				if (x+1 < w && row[7] != 0) {
					r += row[4];
					g += row[5];
					b += row[6];
					n++;
				}
				if (y-1 > 0 && row[-stride+3] != 0) {
					r += row[-stride];
					g += row[-stride+1];
					b += row[-stride+2];
					n++;
				}
				if (y+1 < h && row[stride+3] != 0) {
					r += row[stride];
					g += row[stride+1];
					b += row[stride+2];
					n++;
				}
				if (n > 0) {
					row[0] = (unsigned char)(r/n);
					row[1] = (unsigned char)(g/n);
					row[2] = (unsigned char)(b/n);
				}
			}
			row += 4;
		}
	}
}


static void nsvg__initPaint(NSVGcachedPaint* cache, NSVGpaint* paint, float opacity)
{
	int i, j;
	NSVGgradient* grad;

	cache->type = paint->type;

	if (paint->type == NSVG_PAINT_COLOR) {
		cache->colors[0] = nsvg__applyOpacity(paint->color, opacity);
		return;
	}

	grad = paint->gradient;

	cache->spread = grad->spread;
	memcpy(cache->xform, grad->xform, sizeof(float)*6);

	if (grad->nstops == 0) {
		for (i = 0; i < 256; i++)
			cache->colors[i] = 0;
	} else if (grad->nstops == 1) {
		unsigned int color = nsvg__applyOpacity(grad->stops[0].color, opacity);
		for (i = 0; i < 256; i++)
			cache->colors[i] = color;
	} else {
		unsigned int ca, cb = 0;
		float ua, ub, du, u;
		int ia, ib, count;

		ca = nsvg__applyOpacity(grad->stops[0].color, opacity);
		ua = nsvg__clampf(grad->stops[0].offset, 0, 1);
		ub = nsvg__clampf(grad->stops[grad->nstops-1].offset, ua, 1);
		ia = (int)(ua * 255.0f);
		ib = (int)(ub * 255.0f);
		for (i = 0; i < ia; i++) {
			cache->colors[i] = ca;
		}

		for (i = 0; i < grad->nstops-1; i++) {
			ca = nsvg__applyOpacity(grad->stops[i].color, opacity);
			cb = nsvg__applyOpacity(grad->stops[i+1].color, opacity);
			ua = nsvg__clampf(grad->stops[i].offset, 0, 1);
			ub = nsvg__clampf(grad->stops[i+1].offset, 0, 1);
			ia = (int)(ua * 255.0f);
			ib = (int)(ub * 255.0f);
			count = ib - ia;
			if (count <= 0) continue;
			u = 0;
			du = 1.0f / (float)count;
			for (j = 0; j < count; j++) {
				cache->colors[ia+j] = nsvg__lerpRGBA(ca,cb,u);
				u += du;
			}
		}

		for (i = ib; i < 256; i++)
			cache->colors[i] = cb;
	}

}

/*
static void dumpEdges(NSVGrasterizer* r, const char* name)
{
	float xmin = 0, xmax = 0, ymin = 0, ymax = 0;
	NSVGedge *e = NULL;
	int i;
	if (r->nedges == 0) return;
	FILE* fp = fopen(name, "w");
	if (fp == NULL) return;

	xmin = xmax = r->edges[0].x0;
	ymin = ymax = r->edges[0].y0;
	for (i = 0; i < r->nedges; i++) {
		e = &r->edges[i];
		xmin = nsvg__minf(xmin, e->x0);
		xmin = nsvg__minf(xmin, e->x1);
		xmax = nsvg__maxf(xmax, e->x0);
		xmax = nsvg__maxf(xmax, e->x1);
		ymin = nsvg__minf(ymin, e->y0);
		ymin = nsvg__minf(ymin, e->y1);
		ymax = nsvg__maxf(ymax, e->y0);
		ymax = nsvg__maxf(ymax, e->y1);
	}

	fprintf(fp, "<svg viewBox=\"%f %f %f %f\" xmlns=\"http://www.w3.org/2000/svg\">", xmin, ymin, (xmax - xmin), (ymax - ymin));

	for (i = 0; i < r->nedges; i++) {
		e = &r->edges[i];
		fprintf(fp ,"<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:#000;\" />", e->x0,e->y0, e->x1,e->y1);
	}

	for (i = 0; i < r->npoints; i++) {
		if (i+1 < r->npoints)
			fprintf(fp ,"<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:#f00;\" />", r->points[i].x, r->points[i].y, r->points[i+1].x, r->points[i+1].y);
		fprintf(fp ,"<circle cx=\"%f\" cy=\"%f\" r=\"1\" style=\"fill:%s;\" />", r->points[i].x, r->points[i].y, r->points[i].flags == 0 ? "#f00" : "#0f0");
	}

	fprintf(fp, "</svg>");
	fclose(fp);
}
*/

void nsvgRasterize(NSVGrasterizer* r,
				   NSVGimage* image, float tx, float ty, float scale,
				   unsigned char* dst, int w, int h, int stride)
{
	NSVGshape *shape = NULL;
	NSVGedge *e = NULL;
	NSVGcachedPaint cache;
	int i;
    int j;
    unsigned char paintOrder;

	r->bitmap = dst;
	r->width = w;
	r->height = h;
	r->stride = stride;

	if (w > r->cscanline) {
		r->cscanline = w;
		r->scanline = (unsigned char*)realloc(r->scanline, w);
		if (r->scanline == NULL) return;
	}

	for (i = 0; i < h; i++)
		memset(&dst[i*stride], 0, w*4);

	for (shape = image->shapes; shape != NULL; shape = shape->next) {
		if (!(shape->flags & NSVG_FLAGS_VISIBLE))
			continue;

        for (j = 0; j < 3; j++) {
            paintOrder = (shape->paintOrder >> (2 * j)) & 0x03;

            if (paintOrder == NSVG_PAINT_FILL && shape->fill.type != NSVG_PAINT_NONE) {
                nsvg__resetPool(r);
                r->freelist = NULL;
                r->nedges = 0;

                nsvg__flattenShape(r, shape, scale);

                // Scale and translate edges
                for (i = 0; i < r->nedges; i++) {
                    e = &r->edges[i];
                    e->x0 = tx + e->x0;
                    e->y0 = (ty + e->y0) * NSVG__SUBSAMPLES;
                    e->x1 = tx + e->x1;
                    e->y1 = (ty + e->y1) * NSVG__SUBSAMPLES;
                }

                // Rasterize edges
                if (r->nedges != 0)
                    qsort(r->edges, r->nedges, sizeof(NSVGedge), nsvg__cmpEdge);

                // now, traverse the scanlines and find the intersections on each scanline, use non-zero rule
                nsvg__initPaint(&cache, &shape->fill, shape->opacity);

                nsvg__rasterizeSortedEdges(r, tx,ty,scale, &cache, shape->fillRule);
            }
            if (paintOrder == NSVG_PAINT_STROKE && shape->stroke.type != NSVG_PAINT_NONE && (shape->strokeWidth * scale) > 0.01f) {
                nsvg__resetPool(r);
                r->freelist = NULL;
                r->nedges = 0;

                nsvg__flattenShapeStroke(r, shape, scale);

    //			dumpEdges(r, "edge.svg");

                // Scale and translate edges
                for (i = 0; i < r->nedges; i++) {
                    e = &r->edges[i];
                    e->x0 = tx + e->x0;
                    e->y0 = (ty + e->y0) * NSVG__SUBSAMPLES;
                    e->x1 = tx + e->x1;
                    e->y1 = (ty + e->y1) * NSVG__SUBSAMPLES;
                }

                // Rasterize edges
                if (r->nedges != 0)
                    qsort(r->edges, r->nedges, sizeof(NSVGedge), nsvg__cmpEdge);

                // now, traverse the scanlines and find the intersections on each scanline, use non-zero rule
                nsvg__initPaint(&cache, &shape->stroke, shape->opacity);

                nsvg__rasterizeSortedEdges(r, tx,ty,scale, &cache, NSVG_FILLRULE_NONZERO);
            }
        }
	}

	nsvg__unpremultiplyAlpha(dst, w, h, stride);

	r->bitmap = NULL;
	r->width = 0;
	r->height = 0;
	r->stride = 0;
}

#endif // NANOSVGRAST_IMPLEMENTATION

#endif // NANOSVGRAST_H


// Windhawk provides the declarations of its API functions (Wh_Log,
// Wh_GetIntSetting, ...) automatically, no extra include is needed.

namespace {

constexpr UINT kTrayCallbackMsg = WM_APP + 1;
constexpr UINT kMsgShutdown = WM_APP + 2;
constexpr UINT kMsgSettingsChanged = WM_APP + 3;
constexpr UINT kMsgShellChange = WM_APP + 4;
constexpr UINT_PTR kTimerCoalesce = 1;
constexpr UINT_PTR kTimerFallback = 2;
// Slow fallback refresh in case a shell change notification is missed.
constexpr UINT kFallbackIntervalMs = 60000;
// Coalesce bursts of shell notifications (a big delete sends many).
constexpr UINT kCoalesceMs = 300;
constexpr UINT kThreadReadyTimeoutMs = 5000;
constexpr UINT kThreadExitTimeoutMs = 5000;
// Ignore repeated open requests within this window (a double-click in
// single-click mode produces two WM_LBUTTONUP).
constexpr DWORD kOpenDebounceMs = 800;
constexpr UINT kTrayIconId = 1;
constexpr int kCmdOpen = 100;
constexpr int kCmdEmpty = 101;

// Stable identity of our tray icon. Windows uses it to remember the icon's
// notification-area settings and to avoid duplicates. Regenerated for the
// tool-mod version because the host process changed.
constexpr GUID kTrayIconGuid = {
    0xc85e4c89, 0xf405, 0x4999,
    {0x87, 0x1f, 0x27, 0x5a, 0xd8, 0xbf, 0x16, 0xaf}};

constexpr WCHAR kWindowClass[] = L"WindhawkRecycleBinTray";

HINSTANCE g_hInst = nullptr;
HANDLE g_thread = nullptr;
HANDLE g_readyEvent = nullptr;
HWND g_hwnd = nullptr;
UINT g_taskbarCreatedMsg = 0;
ULONG g_shellNotifyId = 0;
HICON g_systemEmpty = nullptr;
HICON g_systemFull = nullptr;
HICON g_customEmpty = nullptr;
HICON g_customFull = nullptr;
// Effective icons: custom ones win over system ones.
HICON g_iconEmpty = nullptr;
HICON g_iconFull = nullptr;
HICON g_currentIcon = nullptr;
// True when Windows uses a light taskbar (then dark icons are used).
bool g_lightTaskbar = false;
bool g_confirmBeforeEmpty = true;
bool g_singleClickToOpen = false;
DWORD g_lastOpenTick = 0;

// RAII wrapper for Wh_GetStringSetting.
class StringSetting {
 public:
    explicit StringSetting(PCWSTR s) : m_s(s) {}
    ~StringSetting() {
        if (m_s)
            Wh_FreeStringSetting(m_s);
    }
    StringSetting(const StringSetting&) = delete;
    StringSetting& operator=(const StringSetting&) = delete;
    PCWSTR get() const { return m_s ? m_s : L""; }

 private:
    PCWSTR m_s;
};

void LoadSettings() {
    g_confirmBeforeEmpty = !!Wh_GetIntSetting(L"confirmBeforeEmpty");
    g_singleClickToOpen = !!Wh_GetIntSetting(L"singleClickToOpen");
}

void FormatSize(ULONGLONG bytes, PWSTR buffer, size_t bufferChars) {
    static const WCHAR* const units[] = {L"B", L"KB", L"MB", L"GB", L"TB"};
    double value = (double)bytes;
    int unit = 0;
    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        unit++;
    }
    if (unit == 0)
        swprintf_s(buffer, bufferChars, L"%llu %s", bytes, units[unit]);
    else
        swprintf_s(buffer, bufferChars, L"%.1f %s", value, units[unit]);
}

void FormatCount(ULONGLONG count, PWSTR buffer, size_t bufferChars) {
    swprintf_s(buffer, bufferChars, L"%llu %s", count,
               count == 1 ? L"item" : L"items");
}

struct RecycleBinState {
    bool full = false;
    ULONGLONG items = 0;
    ULONGLONG size = 0;
};

RecycleBinState QueryRecycleBin() {
    RecycleBinState state;
    SHQUERYRBINFO info = {};
    info.cbSize = sizeof(info);
    if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &info))) {
        state.items = (ULONGLONG)info.i64NumItems;
        state.size = (ULONGLONG)info.i64Size;
        state.full = info.i64NumItems > 0;
    }
    return state;
}

void LoadSystemIcons() {
    SHSTOCKICONINFO sii = {};
    sii.cbSize = sizeof(sii);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_RECYCLER, SHGSI_ICON | SHGSI_SMALLICON,
                                     &sii)) &&
        sii.hIcon) {
        g_systemEmpty = sii.hIcon;
    }

    sii = {};
    sii.cbSize = sizeof(sii);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_RECYCLERFULL,
                                     SHGSI_ICON | SHGSI_SMALLICON, &sii)) &&
        sii.hIcon) {
        g_systemFull = sii.hIcon;
    }

    // Fallback to the classic shell32.dll bin icons if the stock lookup failed.
    if (!g_systemEmpty)
        g_systemEmpty = ExtractIconW(nullptr, L"shell32.dll", 31);
    if (!g_systemFull)
        g_systemFull = ExtractIconW(nullptr, L"shell32.dll", 32);
}

bool PathEndsWithNoCase(PCWSTR path, PCWSTR suffix) {
    size_t pathLen = wcslen(path);
    size_t suffixLen = wcslen(suffix);
    if (suffixLen > pathLen)
        return false;
    return _wcsicmp(path + pathLen - suffixLen, suffix) == 0;
}

bool ReadWholeFile(PCWSTR path, BYTE** outData, DWORD* outSize) {
    *outData = nullptr;
    *outSize = 0;
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    bool ok = false;
    LARGE_INTEGER fileSize = {};
    if (GetFileSizeEx(hFile, &fileSize) && fileSize.QuadPart > 0 &&
        fileSize.QuadPart <= 4 * 1024 * 1024) {
        DWORD size = (DWORD)fileSize.QuadPart;
        BYTE* data = (BYTE*)malloc(size + 1);
        if (data) {
            DWORD read = 0;
            if (ReadFile(hFile, data, size, &read, nullptr) && read == size) {
                data[size] = 0;
                *outData = data;
                *outSize = size;
                ok = true;
            } else {
                free(data);
            }
        }
    }
    CloseHandle(hFile);
    return ok;
}

HICON CreateHiconFromRgba(const unsigned char* rgba, int size) {
    BITMAPV5HEADER bi = {};
    bi.bV5Size = sizeof(bi);
    bi.bV5Width = size;
    bi.bV5Height = -size;  // top-down
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;

    void* bits = nullptr;
    HBITMAP hColor = CreateDIBSection(nullptr, (BITMAPINFO*)&bi,
                                      DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hColor || !bits)
        return nullptr;

    // NanoSVG outputs RGBA; DIB expects 0xAARRGGBB.
    const unsigned char* src = rgba;
    DWORD* dst = (DWORD*)bits;
    for (int i = 0; i < size * size; i++) {
        dst[i] = ((DWORD)src[3] << 24) | ((DWORD)src[0] << 16) |
                 ((DWORD)src[1] << 8) | (DWORD)src[2];
        src += 4;
    }

    HBITMAP hMask = CreateBitmap(size, size, 1, 1, nullptr);
    if (hMask) {
        // All-opaque mask: the alpha channel of the 32-bit color bitmap
        // controls transparency.
        HDC hdc = CreateCompatibleDC(nullptr);
        if (hdc) {
            HBITMAP hOld = (HBITMAP)SelectObject(hdc, hMask);
            PatBlt(hdc, 0, 0, size, size, BLACKNESS);
            SelectObject(hdc, hOld);
            DeleteDC(hdc);
        }
    }
    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = hColor;
    ii.hbmMask = hMask;
    HICON hIcon = CreateIconIndirect(&ii);
    DeleteObject(hColor);
    if (hMask)
        DeleteObject(hMask);
    return hIcon;
}

// Box-downscale srcSize -> dstSize (square RGBA) with an alpha contrast
// boost so thin SVG strokes stay visible at tray size.
void DownscaleRgba(const unsigned char* src, int srcSize, unsigned char* dst,
                   int dstSize) {
    int factor = srcSize / dstSize;
    for (int y = 0; y < dstSize; y++) {
        for (int x = 0; x < dstSize; x++) {
            unsigned int r = 0, g = 0, b = 0, a = 0;
            for (int ky = 0; ky < factor; ky++) {
                for (int kx = 0; kx < factor; kx++) {
                    const unsigned char* p =
                        src + ((y * factor + ky) * srcSize + (x * factor + kx)) *
                                  4;
                    r += p[0];
                    g += p[1];
                    b += p[2];
                    a += p[3];
                }
            }
            unsigned int n = (unsigned int)factor * factor;
            unsigned char* d = dst + (y * dstSize + x) * 4;
            d[0] = (unsigned char)(r / n);
            d[1] = (unsigned char)(g / n);
            d[2] = (unsigned char)(b / n);
            float af = (a / (float)n) / 255.0f;
            float boosted = powf(af < 0 ? 0 : (af > 1 ? 1 : af), 0.55f);
            d[3] = (unsigned char)(boosted * 255.0f + 0.5f);
        }
    }
}

// Rasterizes an SVG file to an HICON of the given size (rendered 4x and
// downscaled for crisp strokes).
HICON RasterizeSvgIcon(PCWSTR svgPath, int size) {
    BYTE* data = nullptr;
    DWORD dataSize = 0;
    if (!ReadWholeFile(svgPath, &data, &dataSize)) {
        Wh_Log(L"Failed to read SVG file: %s", svgPath);
        return nullptr;
    }

    NSVGimage* image = nsvgParse((char*)data, "px", 96.0f);
    free(data);
    if (!image || image->width <= 0 || image->height <= 0) {
        Wh_Log(L"Failed to parse SVG: %s", svgPath);
        if (image)
            nsvgDelete(image);
        return nullptr;
    }

    const int renderSize = size * 4;
    unsigned char* pixels =
        (unsigned char*)malloc((size_t)renderSize * renderSize * 4);
    HICON hIcon = nullptr;
    if (pixels) {
        NSVGrasterizer* rast = nsvgCreateRasterizer();
        if (rast) {
            float longest =
                image->width > image->height ? image->width : image->height;
            float scale = (float)renderSize / longest;
            float tx = (renderSize - image->width * scale) / 2.0f;
            float ty = (renderSize - image->height * scale) / 2.0f;
            memset(pixels, 0, (size_t)renderSize * renderSize * 4);
            nsvgRasterize(rast, image, tx, ty, scale, pixels, renderSize,
                          renderSize, renderSize * 4);
            nsvgDeleteRasterizer(rast);

            unsigned char* small =
                (unsigned char*)malloc((size_t)size * size * 4);
            if (small) {
                DownscaleRgba(pixels, renderSize, small, size);
                hIcon = CreateHiconFromRgba(small, size);
                free(small);
            }
        }
        free(pixels);
    }
    nsvgDelete(image);

    if (hIcon)
        Wh_Log(L"Rasterized SVG icon: %s", svgPath);
    else
        Wh_Log(L"Failed to rasterize SVG: %s", svgPath);
    return hIcon;
}

HICON LoadIconFromFile(PCWSTR path) {
    if (!path || !*path)
        return nullptr;

    WCHAR expanded[MAX_PATH];
    DWORD len = ExpandEnvironmentStringsW(path, expanded, ARRAYSIZE(expanded));
    if (len == 0 || len > ARRAYSIZE(expanded))
        return nullptr;

    int size = GetSystemMetrics(SM_CXSMICON);
    if (size < 16)
        size = 16;

    HICON icon = nullptr;
    if (PathEndsWithNoCase(expanded, L".svg"))
        icon = RasterizeSvgIcon(expanded, size);
    else
        icon = (HICON)LoadImageW(nullptr, expanded, IMAGE_ICON, size, size,
                                  LR_LOADFROMFILE);
    if (icon)
        Wh_Log(L"Loaded custom icon: %s", expanded);
    else
        Wh_Log(L"Failed to load custom icon: %s", expanded);
    return icon;
}

void ResolveIcons() {
    // Custom icons win over system icons.
    g_iconEmpty = g_customEmpty ? g_customEmpty : g_systemEmpty;
    g_iconFull = g_customFull ? g_customFull : g_systemFull;
}

void FreeCustomIcons() {
    if (g_customEmpty) {
        DestroyIcon(g_customEmpty);
        g_customEmpty = nullptr;
    }
    if (g_customFull) {
        DestroyIcon(g_customFull);
        g_customFull = nullptr;
    }
}

// True when Windows is set to a light theme (light taskbar).
bool IsLightTaskbar() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return status == ERROR_SUCCESS && value != 0;
}

void LoadCustomIcons() {
    g_lightTaskbar = IsLightTaskbar();
    FreeCustomIcons();

    PCWSTR emptySetting = L"customEmptyIconPath";
    PCWSTR fullSetting = L"customFullIconPath";
    if (g_lightTaskbar) {
        emptySetting = L"customEmptyIconPathLight";
        fullSetting = L"customFullIconPathLight";
    }

    StringSetting emptyPath(Wh_GetStringSetting(emptySetting));
    StringSetting fullPath(Wh_GetStringSetting(fullSetting));
    g_customEmpty = LoadIconFromFile(emptyPath.get());
    g_customFull = LoadIconFromFile(fullPath.get());

    if (g_lightTaskbar && (!g_customEmpty || !g_customFull)) {
        // Fall back to the main custom icons when light-taskbar ones are
        // not set.
        StringSetting emptyPath2(Wh_GetStringSetting(L"customEmptyIconPath"));
        StringSetting fullPath2(Wh_GetStringSetting(L"customFullIconPath"));
        if (!g_customEmpty)
            g_customEmpty = LoadIconFromFile(emptyPath2.get());
        if (!g_customFull)
            g_customFull = LoadIconFromFile(fullPath2.get());
    }

    ResolveIcons();
    // Force the tray icon to refresh with the new icons.
    g_currentIcon = nullptr;
}

void AddTrayIcon() {
    if (!g_hwnd)
        return;

    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = kTrayIconId;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_GUID;
    nid.uCallbackMessage = kTrayCallbackMsg;
    nid.hIcon = g_currentIcon ? g_currentIcon : g_iconEmpty;
    nid.guidItem = kTrayIconGuid;
    wcscpy_s(nid.szTip, L"Recycle Bin");
    Shell_NotifyIconW(NIM_ADD, &nid);

    NOTIFYICONDATAW nidVer = {};
    nidVer.cbSize = sizeof(nidVer);
    nidVer.hWnd = g_hwnd;
    nidVer.uID = kTrayIconId;
    nidVer.guidItem = kTrayIconGuid;
    nidVer.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &nidVer);

    Wh_Log(L"Tray icon added");
}

void UpdateTrayIcon() {
    if (!g_hwnd)
        return;

    RecycleBinState state = QueryRecycleBin();
    HICON icon = state.full ? g_iconFull : g_iconEmpty;

    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = kTrayIconId;
    nid.guidItem = kTrayIconGuid;
    nid.uFlags = NIF_GUID | NIF_TIP | NIF_SHOWTIP;

    if (icon && icon != g_currentIcon) {
        g_currentIcon = icon;
        nid.uFlags |= NIF_ICON;
        nid.hIcon = icon;
    }

    WCHAR tip[128];
    if (state.full) {
        WCHAR sizeText[32];
        WCHAR countText[32];
        FormatSize(state.size, sizeText, ARRAYSIZE(sizeText));
        FormatCount(state.items, countText, ARRAYSIZE(countText));
        swprintf_s(tip, ARRAYSIZE(tip), L"Recycle Bin\n%s (%s)", countText,
                   sizeText);
    } else {
        wcscpy_s(tip, L"Recycle Bin (empty)");
    }
    wcscpy_s(nid.szTip, tip);

    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void RemoveTrayIcon() {
    if (!g_hwnd)
        return;

    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = kTrayIconId;
    nid.guidItem = kTrayIconGuid;
    nid.uFlags = NIF_GUID;
    Shell_NotifyIconW(NIM_DELETE, &nid);

    Wh_Log(L"Tray icon removed");
}

void OpenRecycleBin() {
    // Open the Recycle Bin namespace directly instead of starting an extra
    // explorer.exe. COM is initialized on this thread.
    ShellExecuteW(nullptr, L"open", L"shell:RecycleBinFolder", nullptr,
                  nullptr, SW_SHOWNORMAL);
}

void OpenRecycleBinDebounced() {
    DWORD now = GetTickCount();
    if (now - g_lastOpenTick < kOpenDebounceMs)
        return;
    g_lastOpenTick = now;
    OpenRecycleBin();
}

void EmptyRecycleBin() {
    DWORD flags = SHERB_NOPROGRESSUI | SHERB_NOSOUND;
    if (!g_confirmBeforeEmpty)
        flags |= SHERB_NOCONFIRMATION;
    if (SUCCEEDED(SHEmptyRecycleBinW(nullptr, nullptr, flags)))
        UpdateTrayIcon();
}

void ShowContextMenuAt(POINT pt) {
    RecycleBinState state = QueryRecycleBin();

    HMENU menu = CreatePopupMenu();
    if (!menu)
        return;

    WCHAR header[128];
    if (state.full) {
        WCHAR sizeText[32];
        WCHAR countText[32];
        FormatSize(state.size, sizeText, ARRAYSIZE(sizeText));
        FormatCount(state.items, countText, ARRAYSIZE(countText));
        swprintf_s(header, ARRAYSIZE(header), L"%s (%s)", countText, sizeText);
    } else {
        wcscpy_s(header, L"Recycle Bin is empty");
    }
    AppendMenuW(menu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, header);
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kCmdOpen, L"Open");
    AppendMenuW(menu, MF_STRING, kCmdEmpty, L"Empty Recycle Bin");

    // The standard trick so the menu dismisses correctly.
    SetForegroundWindow(g_hwnd);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
                   pt.x, pt.y, 0, g_hwnd, nullptr);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
}

// Menu position for keyboard invocation: the center of the icon.
POINT GetIconAnchorPoint() {
    POINT pt = {};
    NOTIFYICONIDENTIFIER nii = {};
    nii.cbSize = sizeof(nii);
    nii.hWnd = g_hwnd;
    nii.uID = kTrayIconId;
    nii.guidItem = kTrayIconGuid;
    RECT rc = {};
    if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &rc))) {
        pt.x = (rc.left + rc.right) / 2;
        pt.y = (rc.top + rc.bottom) / 2;
    } else {
        GetCursorPos(&pt);
    }
    return pt;
}

void RegisterRecycleBinNotify() {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (!SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0, nullptr,
                                          &pidl))) {
        Wh_Log(L"SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder) failed");
        return;
    }

    SHChangeNotifyEntry entry = {pidl, TRUE};
    g_shellNotifyId = SHChangeNotifyRegister(
        g_hwnd, SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_CREATE | SHCNE_DELETE | SHCNE_MKDIR | SHCNE_RMDIR |
            SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEDIR |
            SHCNE_UPDATEITEM | SHCNE_UPDATEIMAGE,
        kMsgShellChange, 1, &entry);
    CoTaskMemFree(pidl);

    if (g_shellNotifyId)
        Wh_Log(L"Shell change notifications registered");
    else
        Wh_Log(L"SHChangeNotifyRegister failed");
}

void UnregisterRecycleBinNotify() {
    if (g_shellNotifyId) {
        SHChangeNotifyDeregister(g_shellNotifyId);
        g_shellNotifyId = 0;
    }
}

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam,
                             LPARAM lParam) {
    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        // The taskbar was recreated: re-add the icon. Also reload icons in
        // case the display scaling changed.
        Wh_Log(L"TaskbarCreated, re-adding the icon");
        LoadCustomIcons();
        AddTrayIcon();
        UpdateTrayIcon();
        return 0;
    }

    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerCoalesce) {
                KillTimer(hWnd, kTimerCoalesce);
                UpdateTrayIcon();
            } else if (wParam == kTimerFallback) {
                UpdateTrayIcon();
            }
            return 0;

        case WM_SETTINGCHANGE:
            // The taskbar follows the system theme; "ImmersiveColorSet"
            // broadcasts exactly when it flips.
            if (lParam && wcscmp((PCWSTR)lParam, L"ImmersiveColorSet") == 0) {
                bool light = IsLightTaskbar();
                if (light != g_lightTaskbar) {
                    Wh_Log(L"Taskbar theme changed, reloading icons");
                    LoadCustomIcons();
                    UpdateTrayIcon();
                }
            }
            return 0;

        case WM_DISPLAYCHANGE:
            // Display scaling may have changed: rebuild icons at the new size.
            Wh_Log(L"Display change, reloading icons");
            LoadCustomIcons();
            UpdateTrayIcon();
            return 0;

        case WM_CONTEXTMENU:
            // Keyboard-invoked context menu (Shift+F10 / Menu key).
            ShowContextMenuAt(GetIconAnchorPoint());
            return 0;

        case kTrayCallbackMsg: {
            // NOTIFYICON_VERSION_4: LOWORD(lParam) is the event, wParam
            // carries the anchor point.
            UINT event = LOWORD(lParam);
            switch (event) {
                case WM_LBUTTONDBLCLK:
                    if (!g_singleClickToOpen)
                        OpenRecycleBin();
                    break;
                case WM_LBUTTONUP:
                    if (g_singleClickToOpen)
                        OpenRecycleBinDebounced();
                    break;
                case WM_RBUTTONUP:
                case WM_CONTEXTMENU: {
                    POINT pt = {GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam)};
                    ShowContextMenuAt(pt);
                    break;
                }
                case NIN_SELECT:
                case NIN_KEYSELECT:
                    // Keyboard activation (Win+B, arrows, Enter).
                    OpenRecycleBin();
                    break;
            }
            return 0;
        }

        case kMsgShellChange: {
            // Free the notification block, then coalesce bursts of events
            // with a short one-shot timer before refreshing.
            PIDLIST_ABSOLUTE* ppidl = nullptr;
            LONG lEvent = 0;
            if (SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam,
                                         &ppidl, &lEvent)) {
                SHChangeNotification_Unlock((HANDLE)wParam);
            }
            SetTimer(hWnd, kTimerCoalesce, kCoalesceMs, nullptr);
            return 0;
        }

        case kMsgSettingsChanged:
            LoadSettings();
            LoadCustomIcons();
            UpdateTrayIcon();
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case kCmdOpen:
                    OpenRecycleBin();
                    break;
                case kCmdEmpty:
                    EmptyRecycleBin();
                    break;
            }
            return 0;

        case kMsgShutdown:
            RemoveTrayIcon();
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI TrayThreadProc(LPVOID) {
    // COM STA: ShellExecuteW may involve shell extensions.
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed");
        CoUninitialize();
        return 1;
    }

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    // A hidden top-level window (not message-only) so it receives the
    // TaskbarCreated broadcast and WM_SETTINGCHANGE.
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kWindowClass, L"", WS_POPUP, 0,
                             0, 0, 0, nullptr, nullptr, g_hInst, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"CreateWindowExW failed");
        UnregisterClassW(kWindowClass, g_hInst);
        CoUninitialize();
        return 1;
    }

    LoadSettings();
    LoadSystemIcons();
    LoadCustomIcons();
    RegisterRecycleBinNotify();
    AddTrayIcon();
    UpdateTrayIcon();
    SetTimer(g_hwnd, kTimerFallback, kFallbackIntervalMs, nullptr);

    // The window exists and the icon is up: init can proceed.
    SetEvent(g_readyEvent);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    KillTimer(g_hwnd, kTimerCoalesce);
    KillTimer(g_hwnd, kTimerFallback);
    UnregisterRecycleBinNotify();
    FreeCustomIcons();
    if (g_systemEmpty)
        DestroyIcon(g_systemEmpty);
    if (g_systemFull)
        DestroyIcon(g_systemFull);
    g_systemEmpty = g_systemFull = nullptr;
    g_iconEmpty = g_iconFull = g_currentIcon = nullptr;
    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
    UnregisterClassW(kWindowClass, g_hInst);

    CoUninitialize();
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Recycle Bin tray: tool init");
    g_hInst = GetModuleHandleW(nullptr);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    // Wait until the tray thread created its window, so g_hwnd is always
    // known from here on.
    if (WaitForSingleObject(g_readyEvent, kThreadReadyTimeoutMs) !=
        WAIT_OBJECT_0) {
        Wh_Log(L"Tray thread did not signal ready in time");
    }
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // Do the work on the tray thread: it owns the window, the icons and the
    // timers.
    if (g_hwnd)
        PostMessageW(g_hwnd, kMsgSettingsChanged, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L"Recycle Bin tray: tool uninit");
    if (g_hwnd)
        PostMessageW(g_hwnd, kMsgShutdown, 0, 0);
    if (g_thread) {
        if (WaitForSingleObject(g_thread, kThreadExitTimeoutMs) !=
            WAIT_OBJECT_0) {
            // Dedicated tool process: ending the process is acceptable here.
            Wh_Log(L"Tray thread did not exit in time");
            ExitProcess(1);
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    g_hwnd = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;
void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}
BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }
    LocalFree(argv);
    if (isExcluded) {
        return FALSE;
    }
    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }
        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }
        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolModProcess) {
        return FALSE;
    }
    g_isToolModProcessLauncher = true;
    return TRUE;
}
void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }
    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }
    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                      1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }
    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }
    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModSettingsChanged();
}
void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}
