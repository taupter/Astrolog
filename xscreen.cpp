/*
** Astrolog (Version 7.80) File: xscreen.cpp
**
** IMPORTANT NOTICE: Astrolog and all chart display routines and anything
** not enumerated below used in this program are Copyright (C) 1991-2025 by
** Walter D. Pullen (Astara@msn.com, http://www.astrolog.org/astrolog.htm).
** Permission is granted to freely use, modify, and distribute these
** routines provided these credits and notices remain unmodified with any
** altered or distributed versions of the program.
**
** The main ephemeris databases and calculation routines are from the
** library SWISS EPHEMERIS and are programmed and copyright 1997-2008 by
** Astrodienst AG. Use of that source code is subject to license for Swiss
** Ephemeris Free Edition at https://www.astro.com/swisseph/swephinfo_e.htm.
** This copyright notice must not be changed or removed by any user of this
** program.
**
** Additional ephemeris databases and formulas are from the calculation
** routines in the program PLACALC and are programmed and Copyright (C)
** 1989,1991,1993 by Astrodienst AG and Alois Treindl (alois@astro.ch). The
** use of that source code is subject to regulations made by Astrodienst
** Zurich, and the code is not in the public domain. This copyright notice
** must not be changed or removed by any user of this program.
**
** The original planetary calculation routines used in this program have
** been copyrighted and the initial core of this program was mostly a
** conversion to C of the routines created by James Neely as listed in
** 'Manual of Computer Programming for Astrologers', by Michael Erlewine,
** available from Matrix Software.
**
** Atlas composed using data from https://www.geonames.org/ licensed under a
** Creative Commons Attribution 4.0 License. Time zone changes composed using
** public domain TZ database: https://data.iana.org/time-zones/tz-link.html
**
** The PostScript code within the core graphics routines are programmed
** and Copyright (C) 1992-1993 by Brian D. Willoughby (brianw@sounds.wa.com).
**
** More formally: This program is free software; you can redistribute it
** and/or modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version. This program is
** distributed in the hope that it will be useful and inspiring, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details, a copy of which is in the
** LICENSE.HTM file included with Astrolog, and at http://www.gnu.org
**
** Initial programming 8/28-30/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Last code change made 6/19/2025.
*/

#include "astrolog.h"


#ifdef GRAPH
/*
******************************************************************************
** Astrolog Icon.
******************************************************************************
*/

#ifdef X11
// This information used to define Astrolog's X Windows icon (ringed planet
// with moons) is similar to the output format used by the bitmap program.
// You could extract this section and run "xsetroot -bitmap" on it.

#define icon_width 48
#define icon_height 48
CONST uchar icon_bits[] = {
  0x0e,0x00,0x00,0x00,0x10,0x00,0x7f,0x00,0x00,0x00,0x00,0x00,0xff,0x01,0x00,
  0x02,0x00,0x00,0xff,0x43,0x00,0x00,0x00,0x00,0xfe,0x0f,0x00,0x00,0x02,0x00,
  0x9e,0x1f,0x00,0x00,0x00,0x10,0x1e,0x7c,0xc0,0x03,0x00,0x38,0x3c,0xf8,0x7e,
  0x7d,0x00,0x10,0x3c,0xe0,0xab,0xaa,0x00,0x00,0x38,0xc0,0x5f,0x05,0x03,0x00,
  0x70,0x60,0xb5,0x12,0x04,0x00,0xf0,0xf8,0x5f,0x85,0x18,0x0e,0xe0,0xf8,0xaa,
  0x2a,0x90,0x33,0xc0,0xfd,0x5f,0x05,0xe0,0x47,0xc0,0xbf,0xb5,0x52,0xc0,0x4b,
  0x80,0xff,0x5f,0x05,0xc0,0x85,0x00,0xff,0xaa,0x2a,0x80,0x83,0x80,0xff,0x5f,
  0x05,0x00,0x87,0x80,0x7f,0xb5,0x92,0x00,0x4b,0x80,0xff,0x5f,0x05,0x00,0x45,
  0x80,0xff,0xaa,0x2a,0x00,0x33,0x80,0xff,0x5f,0x05,0x00,0x0f,0xc0,0xff,0xb5,
  0x52,0x00,0x02,0xc0,0xff,0x5f,0x05,0x00,0x02,0xc1,0xff,0xab,0x2a,0x00,0x02,
  0xe0,0xff,0x5f,0x05,0x00,0x02,0x38,0x5f,0xbf,0x92,0x00,0x01,0x58,0xfe,0x5f,
  0x05,0x00,0x81,0xbc,0xfc,0xbe,0x2a,0x00,0x01,0x7c,0xfc,0x7f,0x05,0x00,0x01,
  0x3c,0xbc,0xf5,0x52,0x00,0x01,0x58,0xfe,0xff,0x05,0x80,0x00,0xb8,0xff,0xea,
  0x2b,0xc0,0x01,0xe0,0xfe,0xdf,0x0f,0xc0,0x23,0x00,0x5c,0xb5,0x9f,0xa0,0x03,
  0x00,0xf8,0x5f,0x3d,0x10,0x07,0x00,0xf8,0xaa,0xfa,0x18,0x0f,0x08,0xe0,0x5f,
  0xf5,0x05,0x0e,0x1c,0xc0,0xb5,0xea,0x03,0x1c,0x08,0x00,0x5f,0x85,0x07,0x3c,
  0x00,0x00,0xbe,0x7e,0x1f,0x3c,0x00,0x10,0xc0,0x03,0x3e,0x78,0x00,0x00,0x00,
  0x00,0xf8,0x79,0x00,0x00,0x00,0x00,0xf0,0x7f,0x00,0x00,0x20,0x00,0xc0,0xff,
  0x00,0x00,0x00,0x00,0x82,0xff,0x00,0x00,0x00,0x00,0x00,0xfe,0x40,0x00,0x00,
  0x00,0x00,0x70};
#endif


/*
******************************************************************************
** Interactive Screen Graphics Routines.
******************************************************************************
*/

// Set up the color palette of RGB values used by the program for its 16 main
// color indexes. There's a default palette of bright colors used with black
// backgrounds, and an optional set of adjusted colors that looks better with
// white backgrounds. Also, if the user customizes a palette slot with the
// -YXK switch, then leave it alone and don't change it.

void InitColorPalette(int n)
{
  CONST KV *rgbbmpNew, *rgbbmpOld;
  int i;

  rgbbmpNew = (n < 1 || !gs.fAltPalette ? rgbbmpDef : rgbbmpDef2);
  rgbbmpOld = (n < 1 || !gs.fAltPalette ? rgbbmpDef2 : rgbbmpDef);
  for (i = 0; i < cColor; i++)
    if (n < 0 || rgbbmp[i] == rgbbmpOld[i])
      rgbbmp[i] = rgbbmpNew[i];
}


// Set up all the colors used by the program, i.e. the foreground and
// background colors, and all the colors in the object arrays, based on
// whether or not are in monochrome and/or reverse video mode.

void InitColorsX()
{
  int i;
  flag fInverse = gs.fInverse;
#ifdef X11
  char sz[cchSzDef];
  Colormap cmap;
  XColor xcol;
  KV kv;

  if (!gi.fFile) {
    cmap = XDefaultColormap(gi.disp, gi.screen);

    // Allocate colors from the present X11 colormap. Given RGB color strings,
    // allocate these colors and determine their values.

    for (i = 0; i < cColor; i++) {
      kv = rgbbmp[i];
      sprintf(sz, "#%02x%02x%02x", RgbR(kv), RgbG(kv), RgbB(kv));
      XParseColor(gi.disp, cmap, sz, &xcol);
      XAllocColor(gi.disp, cmap, &xcol);
      rgbind[i] = xcol.pixel;
    }
  }
  if (!gi.fFile) {
    XSetBackground(gi.disp, gi.gc,   rgbind[gi.kiOff]);
    XSetForeground(gi.disp, gi.pmgc, rgbind[gi.kiOff]);
  }
#endif

#ifdef WIN
  // Don't print on a black background unless user really wants that.
  if (wi.hdcPrint != NULL && us.fSmartSave)
    fInverse = fTrue;
#endif

  gi.kiOn   = kMainA[!fInverse];
  gi.kiOff  = kMainA[fInverse];
  gi.kiLite = gs.fColor ? kMainA[2+fInverse] : gi.kiOn;
  gi.kiGray = gs.fColor ? kMainA[3-fInverse] : gi.kiOn;
  for (i = 0; i <= 8; i++)
    kMainB[i]    = gs.fColor ? kMainA[i]    : gi.kiOn;
  for (i = 0; i <= cRainbow; i++)
    kRainbowB[i] = gs.fColor ? kRainbowA[i] : gi.kiOn;
  for (i = 0; i < cElem; i++)
    kElemB[i]    = gs.fColor ? kElemA[i]    : gi.kiOn;
  for (i = 0; i <= cAspect; i++)
    kAspB[i]     = gs.fColor ? kAspA[i]     : gi.kiOn;
  for (i = 0; i <= cObj; i++)
    kObjB[i]     = gs.fColor ? kObjA[i]     : gi.kiOn;
  for (i = 0; i <= cRay+1; i++)
    kRayB[i]     = gs.fColor ? kRayA[i]     : gi.kiOn;
}


#ifdef WCLI
// Window event processor for the Windows CLI version. Most event processing
// happens inside the InteractX() message loop.

LRESULT API WndProcWCLI(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
  HDC hdc;
  HPEN hpen, hpenOld;
  int x, y;

  wi.hwnd = hwnd;
  switch (wMsg) {

  // The window has been resized. Change the chart size if need be.
  case WM_SIZE:
    wi.xClient = gs.xWin = LOWORD(lParam);
    wi.yClient = gs.yWin = HIWORD(lParam);
    if (!wi.fNotManual) {
      gi.xWinResize = gs.xWin; gi.yWinResize = gs.yWin;
    }
    wi.fDoResize = fTrue;
    break;

  // All or part of the window needs to be redrawn. Will do so later.
  case WM_PAINT:
    wi.fDoRedraw = fTrue;
    break;

  // The mouse has been left clicked or dragged over the window.
  case WM_LBUTTONDOWN:
  case WM_MOUSEMOVE:
    x = WLo(lParam);
    y = WHi(lParam);
    if (wMsg == WM_MOUSEMOVE) {

      // Dragging with right mouse down rotates and tilts globes.
      if ((wParam & MK_RBUTTON) != 0 && us.fGraphics && (fMap ||
        gi.nMode == gMidpoint || gi.nMode == gLocal || gi.nMode == gSphere ||
        gi.nMode == gGlobe || gi.nMode == gPolar || gi.nMode == gTelescope)) {
        gs.rRot += (real)(x-WLo(wi.lParamRC)) * rDegHalf / (real)gs.xWin *
          (gi.nMode == gLocal || gi.nMode == gTelescope ? -gi.zViewRatio :
          1.0);
        gs.rTilt += (real)(y-WHi(wi.lParamRC)) * rDegHalf / (real)gs.yWin *
          (gi.nMode == gLocal || gi.nMode == gTelescope ? gi.zViewRatio :
          (gi.nMode == gGlobe ? -1.0 : 1.0));
        while (gs.rRot >= rDegMax)
          gs.rRot -= rDegMax;
        while (gs.rRot < 0.0)
          gs.rRot += rDegMax;
        while (gs.rTilt > rDegQuad)
          gs.rTilt = rDegQuad;
        while (gs.rTilt < -rDegQuad)
          gs.rTilt = -rDegQuad;
        if (gi.nMode == gMidpoint || gi.nMode == gTelescope) {
          if (gi.nMode == gMidpoint && gs.objTrack >= 0)
            gs.rRot = planet[gs.objTrack];
          gs.objTrack = -1;
        }
        wi.lParamRC = lParam;
        wi.fDoRedraw = fTrue;
        break;
      }

      // Treat dragging with left mouse down as a Shift+left click.
      if ((wParam & MK_LBUTTON) == 0 ||
        (wParam & MK_SHIFT) || (wParam & MK_CONTROL))
        break;
      wParam = MK_SHIFT;
    }

    // Alt+click on a world map chart means relocate the chart there.
    if (wMsg == WM_LBUTTONDOWN && GetKeyState(VK_MENU) < 0) {
      if (fMap && !gs.fConstel && !gs.fMollewide) {
        Lon = rDegHalf -
          Mod((real)(x-gi.xOffset) / (real)gs.xWin*rDegMax - gs.rRot);
        if (Lon < -rDegHalf)
          Lon = -rDegHalf;
        else if (Lon > rDegHalf)
          Lon = rDegHalf;
        Lat = rDegQuad-(real)(y-gi.yOffset)/(real)gs.yWin*rDegHalf;
        if (Lat < -rDegQuad)
          Lat = -rDegQuad;
        else if (Lat > rDegQuad)
          Lat = rDegQuad;
        wi.xMouse = -1;
        ciCore = ciMain;
        wi.fDoCast = fTrue;
      }
      break;
    }
    hdc = GetDC(hwnd);
    hpen = (HPEN)CreatePen(PS_SOLID, !gs.fThick ? 0 : 2,
      (COLORREF)rgbbmp[gi.kiPen]);
    hpenOld = (HPEN)SelectObject(hdc, hpen);

    // Ctrl+click means draw a rectangle. Ctrl+Shift+click does ellipse.
    if (wParam & MK_CONTROL) {
      SelectObject(hdc, GetStockObject(NULL_BRUSH));
      if (wParam & MK_SHIFT)
        Ellipse(hdc, wi.xMouse, wi.yMouse, x, y);
      else
        Rectangle(hdc, wi.xMouse, wi.yMouse, x, y);

    // Shift+click means draw a line from the last to current position.
    } else if (wParam & MK_SHIFT) {
      if (wi.xMouse >= 0) {
        MoveTo(hdc, wi.xMouse, wi.yMouse);
        LineTo(hdc, x, y);
        if (wMsg == WM_MOUSEMOVE) {
          wi.xMouse = x; wi.yMouse = y;
        }
      }

    // A simple click means set a pixel and remember that location.
    } else {
      SetPixel(hdc, x, y, (COLORREF)rgbbmp[gi.kiPen]);
      wi.xMouse = x; wi.yMouse = y;
    }
    SelectObject(hdc, hpenOld);
    DeleteObject(hpen);
    ReleaseDC(hwnd, hdc);
    break;

  // The mouse has been right clicked on the window.
  case WM_RBUTTONDOWN:
    if (us.fGraphics) {
      if (fMap || gi.nMode == gLocal || gi.nMode == gSphere ||
        gi.nMode == gGlobe || gi.nMode == gPolar || gi.nMode == gTelescope)
        wi.lParamRC = lParam;
    }
    break;

  default:
    return DefWindowProc(hwnd, wMsg, wParam, lParam);
  }
  return fFalse;
}
#endif


#ifdef ISG
// This routine opens up and initializes a window and prepares it to be drawn
// upon, and gets various information about the display, too.

void BeginX()
{
#ifdef X11
  gi.fBmp = fFalse;        // Astrolog can't draw 24 bit color bitmaps on X11.
  gi.disp = XOpenDisplay(gs.szDisplay);
  if (gi.disp == NULL) {
    PrintError("Can't open display.");
    Terminate(tcFatal);
  }
  gi.screen = DefaultScreen(gi.disp);
  bg = BlackPixel(gi.disp, gi.screen);
  fg = WhitePixel(gi.disp, gi.screen);
  hint.x = gi.xOffset; hint.y = gi.yOffset;
  hint.width = gs.xWin; hint.height = gs.yWin;
  hint.min_width = BITMAPX1; hint.min_height = BITMAPY1;
  hint.max_width = BITMAPX;  hint.max_height = BITMAPY;
  hint.flags = PPosition | PSize | PMaxSize | PMinSize;
  gi.depth = DefaultDepth(gi.disp, gi.screen);
  if (gi.depth < 5) {
    gi.fMono = fTrue;      // Is this a monochrome display?
    gs.fColor = fFalse;
  }
  gi.root = RootWindow(gi.disp, gi.screen);
  if (gs.fRoot)
    gi.wind = gi.root;     // If -XB in effect, then draw on the root window.
  else
    gi.wind = XCreateSimpleWindow(gi.disp, DefaultRootWindow(gi.disp),
      hint.x, hint.y, hint.width, hint.height, 5, fg, bg);
  gi.pmap = XCreatePixmap(gi.disp, gi.wind, gs.xWin, gs.yWin, gi.depth);
  gi.icon = XCreateBitmapFromData(gi.disp, DefaultRootWindow(gi.disp),
    (char *)icon_bits, icon_width, icon_height);
  if (!gs.fRoot)
    XSetStandardProperties(gi.disp, gi.wind, szAppName, szAppName, gi.icon,
      (char **)xkey, 0, &hint);

  // There are two graphics workareas. One is what the user currently sees in
  // the window, and the other is what is currently being drawn on. When done,
  // can quickly copy it to the viewport for a smooth look.

  gi.gc = XCreateGC(gi.disp, gi.wind, 0, 0);
  XSetGraphicsExposures(gi.disp, gi.gc, 0);
  gi.pmgc = XCreateGC(gi.disp, gi.wind, 0, 0);
  InitColorsX();                                  // Go set up colors.
  if (!gs.fRoot)
    XSelectInput(gi.disp, gi.wind, KeyPressMask | StructureNotifyMask |
      ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask);
  XMapRaised(gi.disp, gi.wind);
  XSync(gi.disp, 0);
  XFillRectangle(gi.disp, gi.pmap, gi.pmgc, 0, 0, gs.xWin, gs.yWin);
#endif // X11

#ifdef WIN
  if (wi.fChartWindow && (wi.xClient != gs.xWin ||
    wi.yClient != gs.yWin) && wi.hdcPrint == hdcNil)
    ResizeWindowToChart();
  gi.xOffset = NMultDiv(wi.xClient - gs.xWin, wi.xScroll, nScrollDiv);
  gi.yOffset = NMultDiv(wi.yClient - gs.yWin, wi.yScroll, nScrollDiv);
  SetWindowOrg(wi.hdc, -gi.xOffset, -gi.yOffset);
  SetWindowExt(wi.hdc, wi.xClient, wi.yClient);
  SetMapMode(wi.hdc, MM_ANISOTROPIC);
  SelectObject(wi.hdc, GetStockObject(NULL_PEN));
  SelectObject(wi.hdc, GetStockObject(NULL_BRUSH));
  if (!gs.fJetTrail || wi.hdcPrint != hdcNil)
    PatBlt(wi.hdc, -gi.xOffset, -gi.yOffset, wi.xClient, wi.yClient,
      gs.fInverse ? WHITENESS : BLACKNESS);
  InitColorsX();
#endif // WIN

#ifdef WCLI
  WNDCLASS wndclass;
  if (!wi.fWndclass) {
    wi.fWndclass = fTrue;
    wi.hinst = GetModuleHandle(NULL);
    ClearB((pbyte)&wndclass, sizeof(WNDCLASS));
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
    wndclass.lpfnWndProc = WndProcWCLI;
    wndclass.hInstance = wi.hinst;
    wndclass.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wndclass.lpszClassName = szAppName;
    if (!RegisterClass(&wndclass)) {
      PrintError("The window class could not be registered.");
      Terminate(tcFatal);
    }
  }
  wi.hwndMain = CreateWindow(
    szAppName,
    szAppNameCore " " szVersionCore,
    WS_CAPTION |
    WS_SYSMENU |
    WS_MINIMIZEBOX |
    WS_MAXIMIZEBOX |
    WS_THICKFRAME |
    WS_VSCROLL |
    WS_HSCROLL |
    WS_CLIPCHILDREN |
    WS_OVERLAPPED,
    CW_USEDEFAULT, CW_USEDEFAULT,
    CW_USEDEFAULT, CW_USEDEFAULT,
    (HWND)NULL,
    (HMENU)NULL,
    wi.hinst,
    (LPSTR)NULL);
  if (wi.hwndMain == (HWND)NULL) {
    PrintError("The window could not be created.");
    Terminate(tcFatal);
  }
  wi.hwnd = wi.hwndMain;
  ResizeWindowToChart();
  ShowWindow(wi.hwndMain, SW_SHOW);
  ShowScrollBar(wi.hwnd, SB_BOTH, fFalse);
  gi.xOffset = gi.yOffset = 0;
  InitColorsX();
#endif // WCLI
}


// Animate the current chart based on the given values indicating how much
// to update by. Update and recast the current chart info appropriately.
// Note animation mode for comparison charts will update the second chart.

void Animate(int mode, int toadd)
{
  if (((gi.nMode == gAstroGraph || gi.nMode == gSphere) && gs.fAnimMap) ||
    ((gi.nMode == gWorldMap || gi.nMode == gGlobe || gi.nMode == gPolar) &&
    (gs.fAlt || gs.fAnimMap))) {
    gs.rRot += (real)toadd;
    if (gs.rRot >= rDegMax)     // For animating map displays, add in
      gs.rRot -= rDegMax;       // appropriate degree value.
    else if (gs.rRot < 0.0)
      gs.rRot += rDegMax;
    return;
  }

  mode = NAbs(mode);
  if (mode == iAnimNow) {
#ifdef TIME
    // For the continuous chart update to present moment animation mode, go
    // get whatever time it is now.
    FInputData(szNowCore);
#else
    mode = 1;
    goto LNotNow;
#endif
  } else {    // Otherwise add on appropriate time vector to chart info.
#ifndef TIME
LNotNow:
#endif
    if (us.nRel == rcDual || us.nRel <= rcTransit)
      ciCore = ciTwin;
    else if (us.fProgress || us.fTransit || us.fTransitInf || us.fTransitGra)
      ciCore = ciTran;
    else
      ciCore = ciMain;
    AddTime(&ciCore, mode, toadd);
  }
  if (us.nRel == rcDual || us.nRel <= rcTransit) {
    ciTwin = ciCore;
    ciCore = ciMain;
  } else if (us.fProgress || us.fTransit || us.fTransitInf || us.fTransitGra) {
    ciTran = ciCore;
    ciCore = ciMain;
    if (us.fProgress)
      is.JDp = MdytszToJulian(MonT, DayT, YeaT, TimT, ciDefa.dst, ciDefa.zon);
  } else
    ciMain = ciCore;
  if (us.nRel)
    CastRelation();
  else
    CastChart(0);
}


#ifndef WIN
// This routine exits graphics mode, prompts the user for a set of command
// switches, processes them, and returns to the previous graphics with the
// new settings in effect, allowing one to change most any setting without
// having to lose their graphics state or fall back to a -Q loop.

void CommandLineX()
{
  char szCommandLine[cchSzMax], *rgsz[MAXSWITCHES];
  int argc, fT, fPause = fFalse;

  ciCore = ciMain;
  fT = us.fLoop; us.fLoop = fTrue;
  argc = NPromptSwitches(szCommandLine, rgsz);
  is.cchRow = 0;
  is.fSzInteract = fTrue;
  if (!FProcessSwitches(argc, rgsz))
    fPause = fTrue;
  else {
    is.fMult = fFalse;
    FPrintTables();
    if (is.fMult) {
      ClearB((pbyte)&us.fCredit,
        (int)((pbyte)&us.fLoop - (pbyte)&us.fCredit));
      fPause = fTrue;
    }
  }

  is.fSzInteract = fFalse;
  us.fLoop = fT;
  ciMain = ciCore;
  InitColorsX();
}
#endif // WIN


// Given two chart size values, adjust them such that the chart will look
// "square". This rounds the higher value down and checks certain conditions.

void SquareX(int *x, int *y, flag fForce)
{
  if (!fForce && !fSquare)    // Unless really want to force a square, realize
    return;                   // that some charts look better rectangular.
  if (*x > *y)
    *x = *y;
  else
    *y = *x;
  if (fSidebar)      // Take into account chart's sidebar, if any.
    *x += xSideT;
}


#ifdef WINANY
// Change the pixel size of the window so its internal drawable area is the
// dimensions of the current graphics chart. Both the upper left and lower
// right corners of the window may change depending on the scroll position.

void ResizeWindowToChart()
{
  HDC hdc;
  RECT rcOld, rcCli, rcNew;
  int xScr, yScr;

  if (!us.fGraphics)
    return;
  if (gs.xWin == 0)
    gs.xWin = DEFAULTX;
  if (gs.yWin == 0)
    gs.yWin = DEFAULTY;
  hdc = GetDC(wi.hwnd);
  xScr = GetDeviceCaps(hdc, HORZRES);
  yScr = GetDeviceCaps(hdc, VERTRES);
  ReleaseDC(wi.hwnd, hdc);
  GetWindowRect(wi.hwnd, &rcOld);
  GetClientRect(wi.hwnd, &rcCli);
  rcNew.left = rcOld.left + gi.xOffset;
  rcNew.top  = rcOld.top  + gi.yOffset;
  rcNew.right = rcNew.left + gs.xWin + (gi.nMode == 0 ? (SIDESIZE *
    gi.nScaleText) >> 1 : 0) + (rcOld.right - rcOld.left - rcCli.right);
  rcNew.bottom = rcNew.top + gs.yWin +
    (rcOld.bottom - rcOld.top - rcCli.bottom);
  if (rcNew.right > xScr)
    OffsetRect(&rcNew, xScr - rcNew.right, 0);
  if (rcNew.bottom > yScr)
    OffsetRect(&rcNew, 0, yScr - rcNew.bottom);
  if (rcNew.left < 0)
    OffsetRect(&rcNew, -rcNew.left, 0);
  if (rcNew.top < 0)
    OffsetRect(&rcNew, 0, -rcNew.top);
  wi.fNotManual = fTrue;
  MoveWindow(wi.hwnd, rcNew.left, rcNew.top,
    rcNew.right - rcNew.left, rcNew.bottom - rcNew.top, fTrue);
  wi.fNotManual = fFalse;
}
#endif


#ifndef WIN
// This routine gets called after graphics are brought up and displayed on
// the screen. It loops, processing key presses, mouse clicks, etc, that the
// window receives, until the user specifies they want to exit the program.

void InteractX()
{
#ifdef X11
  char sz[cchSzDef];
  XEvent xevent;
  KeySym keysym;
#endif
#ifdef WCLI
  HBITMAP hbmpOld;
  HDC hdcWin;
  PAINTSTRUCT ps;
  MSG msg;
  int nMsg;
#endif
  int fAutosize = fFalse, fResize = fFalse, fRedraw = fTrue, fNoChart = fFalse,
    fBreak = fFalse, fCast = fFalse, mousex = -1, mousey = -1,
    buttonx = -1, buttony = -1, length, key, i;

  neg(gs.nAnim);
  while (!fBreak) {
    gi.nScale = gs.nScale/100;
    gi.nScaleText = gs.nScaleText/50;
#ifdef WCLI
    if (wi.fDoResize) {
      wi.fDoResize = fFalse;
      fResize = fTrue;
    }
    if (wi.fDoRedraw) {
      wi.fDoRedraw = fFalse;
      fRedraw = fTrue;
    }
    if (wi.fDoCast) {
      wi.fDoCast = fFalse;
      fCast = fTrue;
    }
#endif

    // Some chart windows, like the world maps and aspect grids, should always
    // be a certain size, so correct if a resize was attempted.

    if (fMap) {
      length = nDegMax*gi.nScale;
      if (gs.xWin != length) {
        gs.xWin = length;
        fResize = fTrue;
      }
      length = nDegHalf*gi.nScale;
      if (gs.yWin != length) {
        gs.yWin = length;
        fResize = fTrue;
      }
    } else if (gi.nMode == gGrid) {
      length = (gi.nGridCell + (us.nRel <= rcDual))*CELLSIZE*gi.nScale + 1;
      if (gs.xWin != length) {
        gs.xWin = length;
        fResize = fTrue;
      }
      if (gs.yWin != length) {
        gs.yWin = length;
        fResize = fTrue;
      }

    // Make sure the window isn't too large or too small.

    } else {
      if (gs.fKeepSquare && fSquare) {
        if (fSidebar)
          gs.xWin -= (SIDESIZE * gi.nScaleText) >> 1;
        if (gs.xWin != gs.yWin) {
          i = Min(gs.xWin, gs.yWin);
          i = Max(i, BITMAPX1);
          gs.xWin = gs.yWin = i;
          fResize = fTrue;
        }
        if (fSidebar)
          gs.xWin += (SIDESIZE * gi.nScaleText) >> 1;
      }
      if (gs.xWin < BITMAPX1) {
        gs.xWin = BITMAPX1;
        fResize = fTrue;
      } else if (gs.xWin > BITMAPX) {
        gs.xWin = BITMAPX;
        fResize = fTrue;
      }
      if (gs.yWin < BITMAPY1) {
        gs.yWin = BITMAPY1;
        fResize = fTrue;
      } else if (gs.yWin > BITMAPY) {
        gs.yWin = BITMAPY;
        fResize = fTrue;
      }
    }

    // Negative animation jump rate means first time with this rate.

    if (gs.nAnim < 0)
      neg(gs.nAnim);

    // Physically resize window if we've changed the size parameters.

    if (fAutosize) {
      fAutosize = fFalse;
      gs.xWin = gi.xWinResize; gs.yWin = gi.yWinResize;
      fResize = fTrue;
    }
    if (fResize) {
      fResize = fFalse;
#ifdef X11
      XResizeWindow(gi.disp, gi.wind, gs.xWin, gs.yWin);
      XFreePixmap(gi.disp, gi.pmap);
      gi.pmap = XCreatePixmap(gi.disp, gi.wind, gs.xWin, gs.yWin, gi.depth);
#endif
#ifdef WCLI
      ResizeWindowToChart();
#endif
      fRedraw = fTrue;
    }

    // Recast chart if the chart information has changed any.

    if (fCast) {
      fCast = fFalse;
      ciCore = ciMain;
      if (us.nRel)
        CastRelation();
      else
        CastChart(0);
      fRedraw = fTrue;
    }
    if (gs.nAnim && !gi.fPause)
      fRedraw = fTrue;

    // Update the screen if anything has changed since last time around.

    if (fRedraw && (!gi.fPause || gs.nAnim)) {
      fRedraw = fFalse;

      // If we're in animation mode, change the chart info appropriately.
      if (gs.nAnim && !gi.fPause)
        Animate(gs.nAnim, gi.nDir);

      // Clear the screen and set up a buffer to draw in.
#ifdef X11
      if (!gs.fJetTrail)
        XFillRectangle(gi.disp, gi.pmap, gi.pmgc, 0, 0, gs.xWin, gs.yWin);
#endif
#ifdef WCLI
      InvalidateRect(wi.hwnd, NULL, fFalse);
      ClearB((pbyte)&ps, sizeof(PAINTSTRUCT));
      hdcWin = BeginPaint(wi.hwnd, &ps);
      wi.hdc = CreateCompatibleDC(hdcWin);
      wi.hbmp = CreateCompatibleBitmap(hdcWin, wi.xClient, wi.yClient);
      hbmpOld = (HBITMAP)SelectObject(wi.hdc, wi.hbmp);
      if (gs.fJetTrail)
        BitBlt(wi.hdc, 0, 0, wi.xClient, wi.yClient, hdcWin, 0, 0, SRCCOPY);
      SetWindowOrg(wi.hdc, 0, 0);
      SetWindowExt(wi.hdc, gs.xWin, gs.yWin);
      SetMapMode(wi.hdc, MM_ANISOTROPIC);
      SelectObject(wi.hdc, GetStockObject(NULL_PEN));
      SelectObject(wi.hdc, GetStockObject(NULL_BRUSH));
      if (!gs.fJetTrail)
        PatBlt(wi.hdc, 0, 0, gs.xWin, gs.yWin,
          gs.fInverse ? WHITENESS : BLACKNESS);
#endif
      if (fNoChart)
        fNoChart = fFalse;
      else
        DrawChartX();

      // Make the drawn chart visible in the current screen buffer.
#ifdef X11
      XSync(gi.disp, 0);
      XCopyArea(gi.disp, gi.pmap, gi.wind, gi.gc,
        0, 0, gs.xWin, gs.yWin, 0, 0);
#endif
#ifdef WCLI
      BitBlt(hdcWin, 0, 0, wi.xClient, wi.yClient,
        wi.hdc, 0, 0, SRCCOPY);
      SelectObject(wi.hdc, hbmpOld);
      DeleteObject(wi.hbmp);
      DeleteDC(wi.hdc);
      EndPaint(wi.hwnd, &ps);
#endif
#ifdef EXPRESS
      // Notify AstroExpression the screen has just been redrawn.
      if (!us.fExpOff && FSzSet(us.szExpDisp3))
        ParseExpression(us.szExpDisp3);
#endif
    } // if

    // Now process what's on the event queue, i.e. any keys pressed, etc.

#ifdef X11
    if (XEventsQueued(gi.disp, QueuedAfterFlush /*QueuedAfterReading*/) ||
      !gs.nAnim || gi.fPause) {
      XNextEvent(gi.disp, &xevent);

      // Restore what's on window if a part of it gets uncovered.
      if (xevent.type == Expose && xevent.xexpose.count == 0) {
        XSync(gi.disp, 0);
        XCopyArea(gi.disp, gi.pmap, gi.wind, gi.gc,
          0, 0, gs.xWin, gs.yWin, 0, 0);
      }
      switch (xevent.type) {

      // Check for a manual resize of window by user.
      case ConfigureNotify:
        gi.xWinResize = gs.xWin = xevent.xconfigure.width;
        gi.yWinResize = gs.yWin = xevent.xconfigure.height;
        XFreePixmap(gi.disp, gi.pmap);
        gi.pmap = XCreatePixmap(gi.disp, gi.wind, gs.xWin, gs.yWin, gi.depth);
        fRedraw = fTrue;
        break;
      case MappingNotify:
        XRefreshKeyboardMapping((XMappingEvent *)&xevent);
        break;

      // Process any mouse buttons the user pressed.
      case ButtonPress:
        mousex = xevent.xbutton.x; mousey = xevent.xbutton.y;
        if (xevent.xbutton.button == Button1) {
          DrawColor(gi.kiLite);
          DrawPoint(mousex, mousey);
          XSync(gi.disp, 0);
          XCopyArea(gi.disp, gi.pmap, gi.wind, gi.gc,
            0, 0, gs.xWin, gs.yWin, 0, 0);
        } else if (xevent.xbutton.button == Button2 && (gi.nMode ==
          gAstroGraph || gi.nMode == gWorldMap) && gs.rRot == 0.0) {
          Lon = rDegHalf -
            (real)(xevent.xbutton.x-1)/(real)(gs.xWin-2)*rDegMax;
          Lat = rDegQuad -
            (real)(xevent.xbutton.y-1)/(real)(gs.yWin-2)*181.0;
          sprintf(sz, "Mouse is at %s.", SzLocation(Lon, Lat));
          PrintProgress(sz);
        } else if (xevent.xbutton.button == Button3)
          fBreak = fTrue;
        break;

      // Check for user dragging any of the mouse buttons across window.
      case MotionNotify:
        DrawColor(gi.kiPen);
        DrawLine(mousex, mousey, xevent.xbutton.x, xevent.xbutton.y);
        XSync(gi.disp, 0);
        XCopyArea(gi.disp, gi.pmap, gi.wind, gi.gc,
          0, 0, gs.xWin, gs.yWin, 0, 0);
        mousex = xevent.xbutton.x; mousey = xevent.xbutton.y;
        break;

      // Process any keys user pressed in window.
      case KeyPress:
        length = XLookupString((XKeyEvent *)&xevent, xkey, 10, &keysym, 0);
        if (length == 1) {
          key = xkey[0];
#endif // X11

#ifdef WCLI
      if (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        nMsg = LOWORD(msg.message);
        if (nMsg != WM_CHAR && nMsg != WM_KEYDOWN) {
          switch (nMsg) {
          case WM_SIZE:
            wi.xClient = gs.xWin = LOWORD(msg.lParam);
            wi.yClient = gs.yWin = HIWORD(msg.lParam);
            fResize = fTrue;
            break;
          case WM_PAINT:
            fRedraw = fTrue;
            break;
          default:
            DispatchMessage(&msg);
          }
        } else {
          key = (int)msg.wParam;
          if (nMsg == WM_KEYDOWN)
            key = FBetween(key, VK_F1, VK_F12) ? key - VK_F1 + 201 : 0;
#endif // WCLI

#ifdef EXPRESS
          // May want to adjust current key if AstroExpression says to do so.
          if (!us.fExpOff && FSzSet(us.szExpKey)) {
            ExpSetN(iLetterZ, key);
            ParseExpression(us.szExpKey);
            key = NExpGet(iLetterZ);
          }
#endif
          switch (key) {
          case -1:                    // In case ~XQ returns -1
            break;
          case -2:
            fResize = fCast = fTrue;  // Special ~XQ return value
            break;
          case ' ':
            fRedraw = fTrue;
            break;
          case 'p':
            inv(gi.fPause);
            break;
          case 'r':
            neg(gi.nDir);
            break;
          case 'x':
            inv(gs.fInverse);
            InitColorPalette(gs.fInverse);
            InitColorsX();
            fRedraw = fTrue;
            break;
          case 'm':
            if (!gi.fMono) {
              inv(gs.fColor);
              InitColorsX();
              fRedraw = fTrue;
            }
            break;
#ifdef X11
          case 'B':
            XSetWindowBackgroundPixmap(gi.disp, gi.root, gi.pmap);
            XClearWindow(gi.disp, gi.root);
            break;
#endif
          case 't':
            inv(gs.fText);
            fRedraw = fTrue;
            break;
          case 'i':
            inv(gs.fAlt);
            fRedraw = fTrue;
            break;
          case 'b':
            inv(gs.fBorder);
            fRedraw = fTrue;
            break;
          case 'q':
            inv(gs.fThick);
            fRedraw = fTrue;
            break;
          case 'l':
            inv(gs.fLabel);
            fRedraw = fTrue;
            break;
          case 'k':
            inv(gs.fLabelAsp);
            fRedraw = fTrue;
            break;
          case 'j':
            inv(gs.fJetTrail);
            break;
          case '<':
            if (gs.nScale > 100) {
              gs.nScale -= 100;
              fResize = fTrue;
            }
            break;
          case '>':
            if (gs.nScale < MAXSCALE) {
              gs.nScale += 100;
              fResize = fTrue;
            }
            break;
          case '[':
            if (gs.rTilt > -rDegQuad) {
              gs.rTilt = gs.rTilt > -rDegQuad ?
                gs.rTilt-(real)NAbs(gi.nDir) : -rDegQuad;
              fRedraw = fTrue;
            }
            if (gi.nMode == gTelescope)
              gs.objTrack = -1;
            break;
          case ']':
            if (gs.rTilt < rDegQuad) {
              gs.rTilt = gs.rTilt < rDegQuad ?
                gs.rTilt+(real)NAbs(gi.nDir) : rDegQuad;
              fRedraw = fTrue;
            }
            if (gi.nMode == gTelescope)
              gs.objTrack = -1;
            break;
          case '{':
            if (gi.nMode == gMidpoint || gi.nMode == gTelescope) {
              if (gi.nMode == gMidpoint && gs.objTrack >= 0)
                gs.rRot = planet[gs.objTrack];
              gs.objTrack = -1;
            }
            gs.rRot += (real)NAbs(gi.nDir);
            if (gs.rRot >= rDegMax)
              gs.rRot -= rDegMax;
            fRedraw = fTrue;
            break;
          case '}':
            if (gi.nMode == gMidpoint || gi.nMode == gTelescope) {
              if (gi.nMode == gMidpoint && gs.objTrack >= 0)
                gs.rRot = planet[gs.objTrack];
              gs.objTrack = -1;
            }
            gs.rRot -= (real)NAbs(gi.nDir);
            if (gs.rRot < 0.0)
              gs.rRot += rDegMax;
            fRedraw = fTrue;
            break;
          case 'Q':
            SquareX(&gs.xWin, &gs.yWin, fTrue);
            fResize = fTrue;
            break;
          case 'R':
            for (i = oChi; i <= oVes; i++)
              inv(ignore[i]);
            for (i = oSou; i <= oEP; i++)
              inv(ignore[i]);
            AdjustRestrictions();
            fCast = fTrue;
            break;
          case 'C':
            inv(us.fCusp);
            for (i = cuspLo; i <= cuspHi; i++)
              ignore[i] = !us.fCusp || !ignore[i];
            AdjustRestrictions();
            fCast = fTrue;
            break;
          case 'u':
            inv(us.fUranian);
            for (i = uranLo; i <= uranHi; i++)
              ignore[i] = !us.fUranian || !ignore[i];
            AdjustRestrictions();
            fCast = fTrue;
            break;
          case 'y':
            inv(us.fDwarf);
            for (i = dwarfLo; i <= dwarfHi; i++)
              ignore[i] = !us.fDwarf || !ignore[i];
            AdjustRestrictions();
            fCast = fTrue;
            break;
          case '`':
            inv(us.fMoons);
            for (i = moonsLo; i <= moonsHi; i++)
              ignore[i] = !us.fMoons || !ignore[i];
            AdjustRestrictions();
            fCast = fTrue;
            break;
          case '~':
            inv(us.fCOB);
            for (i = cobLo; i <= cobHi; i++)
              ignore[i] = !us.fCOB || !ignore[i];
            AdjustRestrictions();
            fCast = fTrue;
            break;
          case 'U':
            inv(us.fStar);
            for (i = starLo; i <= starHi; i++)
              ignore[i] = !us.fStar || !ignore[i];
            AdjustRestrictions();
            fCast = fTrue;
            break;
          case 'c':
            us.nRel = us.nRel ? rcNone : rcDual;
            fCast = fTrue;
            break;
          case 's':
            inv(us.fSidereal);
            fCast = fTrue;
            break;
          case 'h':
            inv(us.objCenter);
            fCast = fTrue;
            break;
          case 'a':
            inv(us.fHouse3D);
            fCast = fTrue;
            break;
          case 'g':
            inv(us.fDecan);
            fCast = fTrue;
            break;
          case 'f':
            inv(us.fFlip);
            fCast = fTrue;
            break;
          case 'z':
            inv(us.fIndian);
            fRedraw = fTrue;
            break;
          case '+':
            Animate(gs.nAnim, NAbs(gi.nDir));
            fCast = fTrue;
            break;
          case '-':
            Animate(gs.nAnim, -NAbs(gi.nDir));
            fCast = fTrue;
            break;
          case 'o':
            ciSave = ciMain;
            break;
          case 'O':
            ciMain = ciSave;
            fCast = fTrue;
            break;
#ifdef TIME
          case 'n':
            Animate(10, 0);
            ciMain = ciCore;
            fRedraw = fTrue;
            break;
#endif
          case 'N':                        // The continuous update animation.
            gs.nAnim = gs.nAnim ? 0 : -iAnimNow;
            break;

          // These are the nine different "add time to chart" animations.
          case '!': gs.nAnim = -1; break;
          case '@': gs.nAnim = -2; break;
          case '#': gs.nAnim = -3; break;
          case '$': gs.nAnim = -4; break;
          case '%': gs.nAnim = -5; break;
          case '^': gs.nAnim = -6; break;
          case '&': gs.nAnim = -7; break;
          case '*': gs.nAnim = -8; break;
          case '(': gs.nAnim = -9; break;

          // Should we go switch to a new chart type?
          case 'V': gi.nMode = gWheel;      fAutosize = fTrue; break;
          case 'A': gi.nMode = gGrid;       fRedraw   = fTrue; break;
          case 'Z': gi.nMode = gHorizon;    fAutosize = fTrue; break;
          case 'S': gi.nMode = gOrbit;      fAutosize = fTrue; break;
          case 'H': gi.nMode = gSector;     fAutosize = fTrue; break;
          case 'K': gi.nMode = gCalendar;   fAutosize = fTrue; break;
          case 'J': gi.nMode = gDisposit;   fAutosize = fTrue; break;
          case 'L': gi.nMode = gAstroGraph; fRedraw   = fTrue; break;
          case 'E': gi.nMode = gEphemeris;  fAutosize = fTrue; break;
          case 'I': gi.nMode = gRising;     fAutosize = fTrue; break;
          case 'M': gi.nMode = gMoons;      fAutosize = fTrue; break;
          case 'X': gi.nMode = gSphere;     fAutosize = fTrue; break;
          case 'W': gi.nMode = gWorldMap;   fRedraw   = fTrue; break;
          case 'G': gi.nMode = gGlobe;      fAutosize = fTrue; break;
          case 'P': gi.nMode = gPolar;      fAutosize = fTrue; break;
          case 'T': gi.nMode = gTelescope;  fAutosize = fTrue; break;
#ifdef BIORHYTHM
          case 'Y':                 // Should we switch to biorhythm chart?
            us.nRel = rcBiorhythm;
            gi.nMode = gBiorhythm;
            fCast = fTrue;
            break;
#endif
          case '=':
            inv(gs.fIndianWheel);
            fRedraw = fTrue;
            break;
#ifdef CONSTEL
          case 'F':
            if (gi.nMode != gHorizon && gi.nMode != gSphere && gi.nMode !=
              gGlobe && gi.nMode != gPolar && gi.nMode != gTelescope)
              gi.nMode = gWorldMap;
            inv(gs.fConstel);
            fRedraw = fTrue;
            break;
#endif
          case 'd':
            inv(gs.fHouseExtra);
            fRedraw = fTrue;
            break;
          case 'e':
            inv(gs.fEquator);
            fRedraw = fTrue;
            break;
#ifndef X11          // Astrolog can't draw 24 bit color bitmaps on X11.
          case 'w':
            inv(gi.fBmp);
            fRedraw = fTrue;
            break;
#endif
          case '0':
            inv(us.fPrimeVert);
            inv(us.fCalendarYear);
            inv(us.nEphemYears);
            inv(gs.fMollewide);
            gi.nMode = (gi.nMode == gWheel ? gHouse :
              (gi.nMode == gHouse ? gWheel : gi.nMode));
            fRedraw = fTrue;
            break;
          case 'v': case '?':
            length = us.nScrollRow;
            us.nScrollRow = 0;
            PrintL();
            if (key == 'v') {
              is.fMult = fFalse;
              PrintChart(us.fProgress);
            } else
              DisplayKeysX();
            us.nScrollRow = length;
            break;
          case chReturn:
            CommandLineX();
            fResize = fCast = fTrue;
            break;
          case chDelete:
            fRedraw = fNoChart = fTrue;
            break;
          case 'z'-'`': gi.kiPen = kBlack;   break;
          case 'e'-'`': gi.kiPen = kMaroon;  break;
          case 'f'-'`': gi.kiPen = kDkGreen; break;
          case 'o'-'`': gi.kiPen = kOrange;  break;
          case 'n'-'`': gi.kiPen = kDkBlue;  break;
          case 'u'-'`': gi.kiPen = kPurple;  break;
          case 'k'-'`': gi.kiPen = kDkCyan;  break;
          case 'l'-'`': gi.kiPen = kLtGray;  break;
          case 'd'-'`': gi.kiPen = kDkGray;  break;
          case 'r'-'`': gi.kiPen = kRed;     break;
          case 'g'-'`': gi.kiPen = kGreen;   break;
          case 'y'-'`': gi.kiPen = kYellow;  break;
          case 'b'-'`': gi.kiPen = kBlue;    break;
          case 'v'-'`': gi.kiPen = kMagenta; break;  // Ctrl+m is Enter
          case 'j'-'`': gi.kiPen = kCyan;    break;
          case 'a'-'`': gi.kiPen = kWhite;   break;
          case chEscape: case chBreak:
            fBreak = fTrue;
            break;
          default:
            if (FBetween(key, '1', '9')) {
              // Process numbers 1-9 signifying animation rate.
              gi.nDir = (gi.nDir > 0 ? 1 : -1)*(key-'0');
              break;
            } else if (FBetween(key, 201, 248)) {
              if (is.rgszMacro != NULL && is.rgszMacro[key-200]) {
                is.fSzInteract = fTrue;
                FProcessCommandLine(is.rgszMacro[key-200]);
                is.fSzInteract = fFalse;
                fResize = fCast = fTrue;
                break;
              }
            }
            putchar(chBell);    // Any key not bound will sound a beep.
          } // switch
        } // if
#ifdef X11
      default:
        ;
      } // switch
    } // if
#endif
#ifdef WCLI
    } // if
#endif
  } // while
}


// This is called right before program termination to get rid of the window.

void EndX()
{
#ifdef X11
  XFreeGC(gi.disp, gi.gc);
  XFreeGC(gi.disp, gi.pmgc);
  XFreePixmap(gi.disp, gi.pmap);
  XDestroyWindow(gi.disp, gi.wind);
  XCloseDisplay(gi.disp);
#endif
#ifdef WCLI
  UnregisterClass(szAppName, wi.hinst);
#endif
}
#endif // ISG
#endif // WIN


/*
******************************************************************************
** Main Graphics Processing.
******************************************************************************
*/

#ifdef SWISS
// Process an instance of the -YXU command switch.

flag FProcessYXU(CONST char *szLin, CONST char *szLnk, flag fAdd)
{
  char *pch;

  // Allocate or extend allocation of star name list.
  pch = (char *)PAllocate((fAdd ? CchSz(gs.szStarsLin) + 1 : 0) +
    CchSz(szLin) + 1, "star name list");
  if (pch == NULL)
    return fFalse;
  if (fAdd)
    sprintf(pch, "%s;%s", gs.szStarsLin, szLin);
  else
    sprintf(pch, "%s", szLin);
  DeallocatePIf(gs.szStarsLin);
  gs.szStarsLin = pch;

  // Allocate or extend allocation of star link list.
  pch = (char *)PAllocate((fAdd ? CchSz(gs.szStarsLnk) + 1 : 0) +
    CchSz(szLnk) + 2, "star link list");
  if (pch == NULL)
    return fFalse;
  if (fAdd)
    sprintf(pch, "%s;%s", gs.szStarsLnk, szLnk);
  else
    sprintf(pch, "%s", szLnk);
  DeallocatePIf(gs.szStarsLnk);
  gs.szStarsLnk = pch;

  // Count total number of star names present, and reserve that many slots.
  gi.cStarsLin = *gs.szStarsLin != chNull;
  for (pch = gs.szStarsLin; *pch; pch++)
    if (*pch == chSep || *pch == chSep2)
      gi.cStarsLin++;
  if (gi.rges != NULL) {
    DeallocateP(gi.rges);
    gi.rges = NULL;
  }
  gi.rges = RgAllocate(gi.cStarsLin, ES, "extra stars");
  if (gi.rges == NULL)
    gi.cStarsLin = 0;
  return fTrue;
}
#endif


// Process one command line switch passed to the program dealing with the
// graphics features. This is just like the processing of each switch in the
// main program, however here each switch has been prefixed with an 'X'.

int NProcessSwitchesX(int argc, char **argv, int pos,
  flag fOr, flag fAnd, flag fNot)
{
  int darg = 0, i, j;
  real rT;
  char ch1, ch2;

  ch1 = argv[0][pos+1];
  ch2 = ch1 == chNull ? chNull : argv[0][pos+2];
  switch (argv[0][pos]) {
  case chNull:
    break;

  case 'b':
    if (us.fNoWrite || is.fSzInteract) {
      ErrorArgv("Xb");
      return tcError;
    }
    ch1 = ChCap(ch1);
    if (ch1 == 'B')
      gi.fBmp = fFalse;
    else if (ch1 == 'W') {
      ch1 = 'B';
      gi.fBmp = fTrue;
    }
    if (FValidBmpmode(ch1))
      gs.chBmpMode = ch1;
    gs.ft = FSwitchF2(gs.ft == ftBmp) * ftBmp;
    break;

#ifdef PS
  case 'p':
    if (us.fNoWrite || is.fSzInteract) {
      ErrorArgv("Xp");
      return tcError;
    }
    gs.ft = FSwitchF2(gs.ft == ftPS) * ftPS;
    gs.fPSComplete = (ch1 == '0');
    break;
#endif

  case 'M':
    if (FBetween(ch1, '1', '0' + cRing)) {
      i = (ch1 - '0') + (ch2 == '0');
      if (FErrorArgc("XM", argc, i))
        return tcError;
      for (j = 1; j <= i; j++)
        FCloneSz(argv[j], &szWheelX[(ch2 == '0' && j >= i) ? 0 : j]);
      darg += i;
      break;
    }
#ifdef META
    if (us.fNoWrite || is.fSzInteract) {
      ErrorArgv("XM");
      return tcError;
    }
    if (ch1 == '0') {
      gs.nFontAll = FSwitchF(gs.nFontAll > 0) * gi.nFontPrev;
      gs.nFontTxt = gs.nFontAll / 100000;
      gs.nFontSig = (gs.nFontAll / 10000) % 10;
      gs.nFontHou = (gs.nFontAll / 1000) % 10;
      gs.nFontObj = (gs.nFontAll / 100) % 10;
      gs.nFontAsp = (gs.nFontAll / 10) % 10;
      gs.nFontNak = gs.nFontAll % 10;
    }
    gs.ft = FSwitchF2(gs.ft == ftWmf) * ftWmf;
#endif
    break;

#ifdef WIRE
  case '3':
    if (us.fNoWrite || is.fSzInteract) {
      ErrorArgv("X3");
      return tcError;
    }
    gs.ft = FSwitchF2(gs.ft == ftWire) * ftWire;
    break;
#endif

  case 'o':
    if (us.fNoWrite || is.fSzInteract) {
      ErrorArgv("Xo");
      return tcError;
    }
    if (FErrorArgc("Xo", argc, 1))
      return tcError;
    if (gs.ft == ftNone)
      gs.ft = ftBmp;
    FCloneSz(argv[1], &gi.szFileOut);
    darg++;
    break;

#ifdef X11
  case 'B':
    if (is.fSzInteract) {
      ErrorArgv("XB");
      return tcError;
    }
    SwitchF(gs.fRoot);
    break;
#endif

  case 'I':
    if (ch1 == '0') {
      if (FErrorArgc("XI0", argc, 2))
        return tcError;
      rT = RFromSz(argv[1]);
      i = NFromSz(argv[2]);
      if (FErrorValR("XI0", !FValidBackPct(rT), rT, 1))
        return tcError;
      if (FErrorValN("XI0", !FValidBackOrient(i), i, 2))
        return tcError;
      gs.rBackPct = rT;
      gs.nBackOrient = i;
      darg += 2;
      break;
    } else if (ch1 == 'W') {
      if (FErrorArgc("XIW", argc, 1))
        return tcError;
      FLoadBmp(argv[1], &gi.bmpWorld, fFalse);
      darg++;
      break;
    }
    if (FErrorArgc("XI", argc, 1))
      return tcError;
    FLoadBmp(argv[1], &gi.bmpBack, fFalse);
    darg++;
    break;

  case 'm':
    SwitchF(gs.fColor);
    break;

  case 'r':
    SwitchF(gs.fInverse);
    InitColorPalette(gs.fInverse);
    break;

  case 'w':
    if (FErrorArgc("Xw", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (argc > 2 && ((j = NFromSz(argv[2])) || argv[2][0] == '0')) {
      argc--; argv++;
      darg++;
    } else
      j = i;
    if (FErrorValN("Xw", !FValidGraphX(i), i, 1))
      return tcError;
    if (FErrorValN("Xw", !FValidGraphY(j), j, 2))
      return tcError;
    gs.xWin = i; gs.yWin = j;
    darg++;
    break;

  case 's':
    if (FErrorArgc("Xs", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (i < 100)
      i *= 100;
    if (FErrorValN("Xs", !FValidScale(i), i, 0))
      return tcError;
    gs.nScale = i;
    gi.nScale = gs.nScale/100;   // Refresh so -Xs within -XM2 works
    darg++;
    break;

  case 'S':
    if (FErrorArgc("XS", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (i < 100)
      i *= 100;
    if (FErrorValN("XS", !FValidScaleText(i), i, 0))
      return tcError;
    gs.nScaleText = i;
    gi.nScaleText = gs.nScaleText/50;    // Refresh so changing -XS works
    gi.nScaleTextT2 = gi.nScaleText * gi.nScaleT;
    gi.nScaleTextT = gi.nScaleTextT2 >> 1;
    darg++;
    break;

  case 'Q':
    SwitchF(gs.fKeepSquare);
    break;

  case 'i':
    SwitchF(gs.fAlt);
    break;

  case 't':
    SwitchF(gs.fText);
    break;

  case 'u':
    SwitchF(gs.fBorder);
    break;

  case 'x':
    if (ch1 == '0') {
      SwitchF(gs.fAntialias);
      break;
    }
    SwitchF(gs.fThick);
    break;

  case 'l':
    SwitchF(gs.fLabel);
    break;

  case 'A':
    SwitchF(gs.fLabelAsp);
    break;

  case 'j':
    SwitchF(gs.fJetTrail);
    break;

  case 'e':
    SwitchF(gs.fEquator);
    break;

  case 'U':
    if (ch1 == 'x') {
      SwitchF(gs.fAllExo);
      break;
    }
    SwitchF(gs.fAllStar);
    if (FBetween(ch1, '0', '3'))
      gs.nAllStar = (ch1 - '0');
    break;

  case 'E':
    if (FErrorArgc("XE", argc, 2))
      return tcError;
    if (FBetween(ch1, '0', '3'))
      gs.nAstLabel = (ch1 - '0');
    gs.nAstLo = NFromSz(argv[1]);
    gs.nAstHi = NFromSz(argv[2]);
    darg += 2;
    break;

#ifdef ATLAS
  case 'L':
    SwitchF(gs.fLabelCity);
    if (FBetween(ch1, '1', '5'))
      gs.nLabelCity = (ch1 - '0');
    break;
#endif

  case 'C':
    SwitchF(gs.fHouseExtra);
    break;

  case '1':
    if (fAnd) {
      gs.objLeft = 0;
      break;
    }
    if (FErrorArgc("X1", argc, 1))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("X1", !FItem(i), i, 0))
      return tcError;
    gs.objLeft = i+1;
    darg++;
    break;

  case '2':
    if (fAnd) {
      gs.objLeft = 0;
      break;
    }
    if (FErrorArgc("X2", argc, 1))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("X2", !FItem(i), i, 0))
      return tcError;
    gs.objLeft = -i-1;
    darg++;
    break;

#ifdef X11
  case 'd':
    if (is.fSzInteract) {
      ErrorArgv("Xd");
      return tcError;
    }
    if (FErrorArgc("Xd", argc, 1))
      return tcError;
    FCloneSz(argv[1], &gs.szDisplay);
    darg++;
    break;
#endif

  case 'v':
    if (ch1 == '0') {
      SwitchF(gs.fDoSidebar);
      break;
    }
    if (FErrorArgc("Xv", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("Xv", !FBetween(i, 0, 3), i, 0))
      return tcError;
    gs.nDecaFill = i;
    darg++;
    break;

  case 'J':
    SwitchF(gs.fIndianWheel);
    break;

  case '8':
    SwitchF(gs.fMoonWheel);
    break;

  case 'X':
    if (argc > 1 && ((rT = RFromSz(argv[1])) || argv[1][0] == '0')) {
      darg++;
      if (FErrorValR("XX", !FValidRotation(rT), rT, 1))
        return tcError;
      gs.rRot = rT;
      if (argc > 2 && ((rT = RFromSz(argv[2])) || argv[2][0] == '0')) {
        darg++;
        if (FErrorValR("XX", !FValidTilt(rT), rT, 2))
          return tcError;
        gs.rTilt = rT;
      }
    }
    gi.nMode = FSwitchF2(gi.nMode == gSphere) * gSphere;
    if (ch1 == '0')
      SwitchF(gs.fSouth);
    break;

  case 'W':
    if (argc > 1 && ((rT = RFromSz(argv[1])) || argv[1][0] == '0')) {
      darg++;
      if (FErrorValR("XW", !FValidRotation(rT), rT, 0))
        return tcError;
      gs.rRot = rT;
    }
    gi.nMode = FSwitchF2(gi.nMode == gWorldMap) * gWorldMap;
    if (ch1 == '0')
      SwitchF(gs.fMollewide);
    is.fHaveInfo |= gs.fAlt;
    break;

  case 'G':
    if (argc > 1 && ((rT = RFromSz(argv[1])) || argv[1][0] == '0')) {
      darg++;
      if (FErrorValR("XG", !FValidRotation(rT), rT, 1))
        return tcError;
      gs.rRot = rT;
      if (argc > 2 && ((rT = RFromSz(argv[2])) || argv[2][0] == '0')) {
        darg++;
        if (FErrorValR("XG", !FValidTilt(rT), rT, 2))
          return tcError;
        gs.rTilt = rT;
      }
    }
    gi.nMode = FSwitchF2(gi.nMode == gGlobe) * gGlobe;
    if (ch1 == '0')
      SwitchF(gs.fSouth);
    is.fHaveInfo |= gs.fAlt;
    break;

  case 'P':
    if (argc > 1 && ((rT = RFromSz(argv[1])) || argv[1][0] == '0')) {
      darg++;
      if (FErrorValR("XP", !FValidRotation(rT), rT, 0))
        return tcError;
    } else
      rT = 0.0;
    gs.rRot = rT;
    gi.nMode = FSwitchF2(gi.nMode == gPolar) * gPolar;
    if (ch1 == '0')
      SwitchF(gs.fSouth);
    else if (ch1 == 'v')
      SwitchF(gs.fPrintMap);
    is.fHaveInfo |= gs.fAlt;
    break;

  case 'Z':
    if (argc > 1 && (i = NParseSz(argv[1], pmObject)) != -1) {
      darg++;
      if (!FValidObj(i))
        i = -1;
      gs.objTrack = i;
      if (fAnd)
        break;
    }
    gi.nMode = FSwitchF2(gi.nMode == gTelescope) * gTelescope;
    break;

#ifdef CONSTEL
  case 'F':
    if (gi.nMode != gHorizon && gi.nMode != gSphere && gi.nMode != gGlobe &&
      gi.nMode != gPolar && gi.nMode != gTelescope)
      gi.nMode = FSwitchF2(gi.nMode == gWorldMap) * gWorldMap;
    SwitchF(gs.fConstel);
    is.fHaveInfo |= gs.fAlt;
    break;
#endif

#ifdef ISG
  case 'k':
    if (FErrorArgc("Xk", argc, 1))
      return tcError;
    gi.kiPen = NParseSz(argv[1], pmColor);
    darg++;
    break;

  case 'n':
    if (ch1 == 'p') {
      SwitchF(gi.fPause);
      break;
    } else if (ch1 == 'f') {
      if (FErrorArgc("Xnf", argc, 1))
        return tcError;
      i = NFromSz(argv[1]);
      if (FErrorValN("Xnf", !FBetween(i, 1, 9), i, 0))
        return tcError;
      gi.nDir = i;
      darg++;
      break;
    }
    if (argc > 1 && (i = NFromSz(argv[1])))
      darg++;
    else
      i = 10;
    if (FErrorValN("Xn", !FBetween(i, 1, 13), i, 0))
      return tcError;
    gs.nAnim = (fOr || fNot ? i : -i);
    break;

  case 'N':
    SwitchF(gs.fAnimMap);
    break;
#endif

  default:
    ErrorSwitch(argv[0]);
    return tcError;
  }
  // 'darg' contains the value to be added to argc after returning.
  return darg;
}


// Process one command line switch passed to the program dealing with more
// obscure graphics options. This is structured very much like the function
// NProcessSwitchesX(), except here each switch begins with 'YX'.

int NProcessSwitchesRareX(int argc, char **argv, int pos,
  flag fOr, flag fAnd, flag fNot)
{
  int darg = 0, i, j;
  real rT;
  char ch1;
#ifdef SWISS
  flag fAdd;
#ifdef CONSTEL
  CONST char **ppch;
#endif
#endif // SWISS

  ch1 = argv[0][pos+1];
  switch (argv[0][pos]) {

  // No longer implemented, but still skip 2 parameters and do nothing for
  // compatibility with old astrolog.as files.
  case chNull:
    if (FErrorArgc("YX", argc, 2))
      return tcError;
    darg += 2;
    break;

  case 'G':
    if (FErrorArgc("YXG", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    switch (ch1) {
    case 'c':
      if (FErrorValN("YXGc", !FBetween(i, 1, 2), i, 0))
        return tcError;
      gs.nGlyphCap = i;
      break;
    case 'u':
      if (FErrorValN("YXGu", !FBetween(i, 1, 2), i, 0))
        return tcError;
      gs.nGlyphUra = i;
      break;
    case 'p':
      if (FErrorValN("YXGp", !FBetween(i, 1, 3), i, 0))
        return tcError;
      gs.nGlyphPlu = i;
      break;
    case 'l':
      if (FErrorValN("YXGl", !FBetween(i, 1, 2), i, 0))
        return tcError;
      gs.nGlyphLil = i;
      break;
    case 'v':
      if (FErrorValN("YXGv", !FBetween(i, 1, 2), i, 0))
        return tcError;
      gs.nGlyphVer = i;
      break;
    case 'e':
      if (FErrorValN("YXGe", !FBetween(i, 1, 2), i, 0))
        return tcError;
      gs.nGlyphEri = i;
      break;
    default:
      if (FErrorValN("YXG", !FValidGlyphs(i), i, 0))
        return tcError;
      if (FBetween(i/100000,   1, 2)) gs.nGlyphCap = i/100000;
      if (FBetween(i/10000%10, 1, 2)) gs.nGlyphUra = i/10000%10;
      if (FBetween(i/1000%10,  1, 3)) gs.nGlyphPlu = i/1000%10;
      if (FBetween(i/100%10,   1, 2)) gs.nGlyphLil = i/100%10;
      if (FBetween(i/10%10,    1, 2)) gs.nGlyphVer = i/10%10;
      if (FBetween(i%10,       1, 2)) gs.nGlyphEri = i%10;
      break;
    }
    darg++;
    break;

  case 'D':
    if (FErrorArgc("YXD", argc, 3 - (ch1 == '1' || ch1 == 'D')))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("YXD", !FItem(i), i, 1))
      return tcError;
    if (ch1 == 'D') {
      j = NParseSz(argv[2], pmObject);
      if (FErrorValN("YXDD", !FItem(j), j, 2))
        return tcError;
      FCloneSz(szDrawObject[j], (char **)&szDrawObject[i]);
      FCloneSz(szDrawObject2[j], (char **)&szDrawObject2[i]);
    } else {
      FCloneSzCore(argv[2][0] ? argv[2] : szDrawObjectDef[i],
        (char **)&szDrawObject[i], szDrawObject[i] == szDrawObjectDef[i]);
      FCloneSzCore(
        ch1 == '1' ? "" : (argv[3][0] ? argv[3] : szDrawObjectDef2[i]),
        (char **)&szDrawObject2[i], szDrawObject2[i] == szDrawObjectDef2[i]);
    }
    darg += 3 - (ch1 == '1' || ch1 == 'D');
    break;

  case 'A':
    if (FErrorArgc("YXA", argc, 3 - (ch1 == '1')))
      return tcError;
    i = NParseSz(argv[1], pmAspect);
    if (FErrorValN("YXA", !FAspect3(i), i, 0))
      return tcError;
    FCloneSzCore(argv[2][0] ? argv[2] : szDrawAspectDef[i],
      (char **)&szDrawAspect[i], szDrawAspect[i] == szDrawAspectDef[i]);
    FCloneSzCore(
      ch1 == '1' ? "" : (argv[3][0] ? argv[3] : szDrawAspectDef2[i]),
      (char **)&szDrawAspect2[i], szDrawAspect2[i] == szDrawAspectDef2[i]);
    darg += 3 - (ch1 == '1');
    break;

  case 'v':
    if (FErrorArgc("YXv", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("YXv", !FValidDecaType(i), i, 1))
      return tcError;
    gs.nDecaType = i;
    darg++;
    if (argc > 2 && FNumCh(argv[2][0])) {
      i = NFromSz(argv[2]);
      if (FErrorValN("YXv", !FValidDecaSize(i), i, 2))
        return tcError;
      gs.nDecaSize = i;
      darg++;
      if (argc > 3 && FNumCh(argv[3][0])) {
        i = NFromSz(argv[3]);
        if (FErrorValN("YXv", !FValidDecaLine(i), i, 3))
          return tcError;
        gs.nDecaLine = i;
        darg++;
      }
    }
    break;

  case 't':
    if (FErrorArgc("YXt", argc, 1))
      return tcError;
    FCloneSz(argv[1], &gs.szSidebar);
    darg++;
    break;

  case 'g':
    if (FErrorArgc("YXg", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("YXg", !FValidGrid(i), i, 0))
      return tcError;
    gs.nGridCell = i;
    darg++;
    break;

  case 'S':
    if (FErrorArgc("YXS", argc, 1))
      return tcError;
    rT = RFromSz(argv[1]);
    if (FErrorValR("YXS", !FValidZoom(rT), rT, 0))
      return tcError;
    gs.rspace = rT;
    darg++;
    break;

  case 'j':
    if (FErrorArgc("YXj", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (ch1 != '0') {
      gs.cspace = i;
      if (gi.rgspace != NULL) {
        DeallocateP(gi.rgspace);
        gi.rgspace = NULL;
      }
      gi.cspace = gi.ispace = 0;
    } else
      gs.zspace = i;
    darg++;
    break;

  case '7':
    if (FErrorArgc("YX7", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("YX7", !FValidEsoteric(i), i, 0))
      return tcError;
    gs.nRayWidth = i;
    darg++;
    break;

  case 'k':
    SwitchF(gs.fColorSign);
    if (ch1 == '0')
      SwitchF(gs.fColorHouse);
    break;

  case 'K':
    if (ch1 == '0') {
      SwitchF(gs.fAltPalette);
      InitColorPalette(gs.fInverse);
      break;
    }
    if (FErrorArgc("YXK", argc, 2))
      return tcError;
    i = NParseSz(argv[1], pmColor);
    if (FErrorValN("YXK", !FValidColor(i), i, 0))
      return tcError;
    rgbbmp[i] = NParseSz(argv[2], pmRGB);
    darg += 2;
    break;

  case 'e':
    SwitchF(gs.fEcliptic);
    break;

  case 'a':
    if (FErrorArgc("YXa", argc, 1))
      return tcError;
    gs.nDashMax = NFromSz(argv[1]);
    darg++;
    break;

  case 'W':
    gs.nTriangles = NFromSz(argv[1]);
    darg++;
    break;

#ifdef SWISS
  case 'U':
    if (ch1 != '1') {
      if (FErrorArgc("YXU", argc, 2))
        return tcError;
      fAdd = (ch1 == '0' && FSzSet(gs.szStarsLin) && FSzSet(gs.szStarsLnk));
      if (!FProcessYXU(argv[1], argv[2], fAdd))
        return tcError;
      darg += 2;
    } else {
#ifdef CONSTEL
      for (ppch = szDrawConstelLine; *ppch != NULL; ppch += 2) {
        if (!FProcessYXU(ppch[0], ppch[1], ppch != szDrawConstelLine))
          return tcError;
      }
#endif
    }
    break;
#endif // SWISS

  case 'f':
    if (FErrorArgc("YXf", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    switch (ch1) {
    case 't':
      if (FErrorValN("YXft", !FBetween(i, 0, 9) || rgszFontAllow[0][i] == '-',
        i, 0))
        return tcError;
      gs.nFontTxt = i;
      break;
    case 's':
      if (FErrorValN("YXfs", !FBetween(i, 0, 9) || rgszFontAllow[1][i] == '-',
        i, 0))
        return tcError;
      gs.nFontSig = i;
      break;
    case 'h':
      if (FErrorValN("YXfh", !FBetween(i, 0, 9) || rgszFontAllow[2][i] == '-',
        i, 0))
        return tcError;
      gs.nFontHou = i;
      break;
    case 'o':
      if (FErrorValN("YXfo", !FBetween(i, 0, 9) || rgszFontAllow[3][i] == '-',
        i, 0))
        return tcError;
      gs.nFontObj = i;
      break;
    case 'a':
      if (FErrorValN("YXfa", !FBetween(i, 0, 9) || rgszFontAllow[4][i] == '-',
        i, 0))
        return tcError;
      gs.nFontAsp = i;
      break;
    case 'n':
      if (FErrorValN("YXfn", !FBetween(i, 0, 9) || rgszFontAllow[5][i] == '-',
        i, 0))
        return tcError;
      gs.nFontNak = i;
      break;
    default:
      if (FErrorValN("YXf", !FBetween(i, 0, 999999), i, 0))
        return tcError;
      gs.nFontTxt = i/100000;
      gs.nFontSig = i/10000%10;
      gs.nFontHou = i/1000%10;
      gs.nFontObj = i/100%10;
      gs.nFontAsp = i/10%10;
      gs.nFontNak = i%10;
      break;
    }
    gs.nFontAll = gs.nFontTxt*100000 + gs.nFontSig*10000 +
      gs.nFontHou*1000 + gs.nFontObj*100 + gs.nFontAsp*10 + gs.nFontNak;
    if (gs.nFontAll != 0)
      gi.nFontPrev = gs.nFontAll;
    darg++;
    break;

#ifdef PS
  case 'p':
    if (ch1 == '0') {
      if (FErrorArgc("YXp0", argc, 2))
        return tcError;
      gs.xInch = RFromSz(argv[1]);
      gs.yInch = RFromSz(argv[2]);
      darg += 2;
      break;
    }
    if (FErrorArgc("YXp", argc, 1))
      return tcError;
    gs.nOrient = NFromSz(argv[1]);
    darg++;
    break;
#endif

  default:
    ErrorSwitch(argv[0]);
    return tcError;
  }
  // 'darg' contains the value to be added to argc after returning.
  return darg;
}


// Figure out what graphics mode a graphics chart should be generated in,
// based on various command switches in effect, e.g. -L combined with -X,
// -g combined with -X, and so on.

int DetectGraphicsChartMode()
{
  int nMode;

  if (us.fWheel)                   nMode = gHouse;
  else if (us.fGrid || us.fAspect) nMode = gGrid;
  else if (us.fMidpoint)           nMode = gMidpoint;
  else if (us.fHorizon)            nMode = gHorizon;
  else if (us.fOrbit)              nMode = gOrbit;
  else if (us.fSector)             nMode = gSector;
  else if (us.fInfluence)          nMode = gDisposit;
  else if (us.fEsoteric)           nMode = gEsoteric;
  else if (us.fAstroGraph)         nMode = gAstroGraph;
  else if (us.fCalendar)           nMode = gCalendar;
  else if (us.fEphemeris)          nMode = gEphemeris;
  else if (us.fHorizonSearch)      nMode = gRising;
  else if (us.fAtlasNear)          nMode = gLocal;
  else if (us.fMoonChart)          nMode = gMoons;
  else if (us.fInDayGra)           nMode = gTraTraGra;
  else if (us.fTransitGra)         nMode = gTraNatGra;
  else if (us.nRel == rcBiorhythm) nMode = gBiorhythm;
  else                             nMode = gWheel;

  return nMode;
}


// This is the main interface to all the graphics features. This routine is
// called from the main program if any of the -X switches were specified,
// and it sets up for and goes and generates the appropriate graphics chart.
// Return fTrue if successfull, or fFalse if some error occurred.

flag FActionX()
{
  int i, n;

  gi.fFile = (gs.ft != ftNone);
#ifdef PS
  gi.fEps = !gs.fPSComplete;
#endif
  if (gi.nMode == 0)
    gi.nMode = DetectGraphicsChartMode();

  gi.nScaleT = gs.ft == ftPS ? PSMUL : (gs.ft == ftWmf ? METAMUL :
    (gs.ft == ftWire ? WIREMUL : 1));
#ifdef WIN
  if (wi.hdcPrint != hdcNil)
    gi.nScaleT = METAMUL;
#endif
  gi.nScale = gs.nScale/100;
  gi.nScaleText = gs.nScaleText/50;
  gi.nScaleTextT2 = gi.nScaleText * gi.nScaleT;
  gi.nScaleTextT = gi.nScaleTextT2 >> 1;

  // Determine the pixel size the graphics chart is to have.

  if (gi.nMode == gGrid) {
    if (us.nRel <= rcDual && us.fMidpoint && !us.fAspList)
      us.fGridMidpoint = fTrue;
    if (gs.nGridCell > 0)
      gi.nGridCell = gs.nGridCell;
    else
      for (gi.nGridCell = i = 0; i <= is.nObj; i++)
        gi.nGridCell += FProper(i);
    gs.xWin = gs.yWin =
      (gi.nGridCell + (us.nRel <= rcDual))*CELLSIZE*gi.nScale + 1;
  } else if (gs.fKeepSquare && fSquare) {
#ifdef WIN
    if (wi.hdcPrint == hdcNil) {
      if (fSidebar)
        gs.xWin -= (SIDESIZE * gi.nScaleText) >> 1;
#endif
      n = Min(gs.xWin, gs.yWin);
      gs.xWin = gs.yWin = n;
#ifdef WIN
      if (fSidebar)
        gs.xWin += (SIDESIZE * gi.nScaleText) >> 1;
    }
#endif
  } else if (fMap) {
    gs.xWin = nDegMax*gi.nScale;
    gs.yWin = nDegHalf*gi.nScale;
  }
#ifdef WIN
  if (fSidebar)
    gs.xWin -= (SIDESIZE * gi.nScaleText) >> 1;
#endif

  if (gi.fFile) {
    if (!BeginFileX())
      if (gs.ft == ftPS) {
        gs.ft = ftNone; gi.fFile = fFalse;
        return fFalse;
      }
    if (gs.xWin == 0)
      gs.xWin = DEFAULTX;
    if (gs.yWin == 0)
      gs.yWin = DEFAULTY;
    if (fSidebar)
      gs.xWin += (SIDESIZE * gi.nScaleText) >> 1;
    if (gs.xWin > BITMAPX)
      gs.xWin = BITMAPX;
    if (gs.yWin > BITMAPY)
      gs.yWin = BITMAPY;
    if (gs.ft == ftBmp) {
      if (!gi.fBmp) {
        gi.cbBmpRow = (gs.xWin + 1) >> 1;
        if ((gi.bm = PAllocate(gi.cbBmpRow * gs.yWin, "bitmap")) == NULL)
          return fFalse;
      } else {
        if (!FAllocateBmp(&gi.bmp, gs.xWin, gs.yWin))
          return fFalse;
      }
    }
#ifdef PS
    else if (gs.ft == ftPS)
      PsBegin();
#endif
#ifdef META
    else if (gs.ft == ftWmf) {
      for (gi.cbMeta = MAXMETA; gi.cbMeta > 0 &&
        (gi.bm = PAllocate(gi.cbMeta, "metafile")) == NULL;
        gi.cbMeta -= MAXMETA/8)
        PrintWarning("Attempting to get maximum memory for metafile.");
      if (gi.cbMeta <= 0)
        return fFalse;
      gs.xWin   *= METAMUL;  // Increase chart sizes and scales behind the
      gs.yWin   *= METAMUL;  // scenes to make graphics look smoother.
      gs.nScale *= METAMUL;
    }
#endif
#ifdef WIRE
    else {
      gs.xWin   *= WIREMUL;  // Increase chart sizes and scales behind the
      gs.yWin   *= WIREMUL;  // scenes to make graphics look smoother.
      gs.nScale *= WIREMUL;
      for (gi.cbWire = MAXMETA; gi.cbWire > 0 &&
        (gi.bm = PAllocate(gi.cbWire, "wireframe")) == NULL;
        gi.cbWire -= MAXMETA/8)
        PrintWarning("Attempting to get maximum memory for wireframe.");
      if (gi.cbWire <= 0)
        return fFalse;
      gi.pwWireCur = (word *)gi.bm;
      gi.cWire = gi.zDefault = 0;
      gi.kiInFile = -1;
    }
#endif
    InitColorsX();
  }
#ifdef ISG
  else {
    if (gs.xWin == 0 || gs.yWin == 0) {
      if (gs.xWin == 0)
        gs.xWin = DEFAULTX;
      if (gs.yWin == 0)
        gs.yWin = DEFAULTY;
      SquareX(&gs.xWin, &gs.yWin, fFalse);
    } else if (fSidebar)
      gs.xWin += (SIDESIZE * gi.nScaleText) >> 1;
    BeginX();
  }
#endif // ISG

  if (gi.fFile || gs.fRoot)    // Go draw the graphic chart.
    DrawChartX();
  if (gi.fFile) {    // Write bitmap to file if in that mode.
    EndFileX();
    if ((gs.ft == ftBmp && !gi.fBmp) || gs.ft == ftWmf || gs.ft == ftWire) {
      DeallocateP(gi.bm);
      gi.bm = NULL;
    }
  }
#ifdef ISG
  else {
#ifdef X11
    if (gs.fRoot) {                                           // Process -XB.
      XSetWindowBackgroundPixmap(gi.disp, gi.root, gi.pmap);
      XClearWindow(gi.disp, gi.root);

      // If -Xn in effect with -XB, then enter loop where continuously
      // calculate and animate chart, displaying on the root window.
      while (gs.nAnim) {
        Animate(gs.nAnim, 1);
        if (!gs.fJetTrail)
          XFillRectangle(gi.disp, gi.pmap, gi.pmgc, 0, 0, gs.xWin, gs.yWin);
        DrawChartX();
        XSetWindowBackgroundPixmap(gi.disp, gi.root, gi.pmap);
        XClearWindow(gi.disp, gi.root);
      }
    } else
#endif
#ifndef WIN
      InteractX();    // Window's up; process commands given to window now.
    EndX();
#else
    DrawChartX();
#endif
  }
#endif // ISG
  return fTrue;
}
#endif // GRAPH

/* xscreen.cpp */
