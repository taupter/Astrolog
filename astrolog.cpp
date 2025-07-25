/*
** Astrolog (Version 7.80) File: astrolog.cpp
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


/*
******************************************************************************
** Program Dispatch Procedures.
******************************************************************************
*/

// Initialize the Ansi color arrays with the color to print each object in.

void InitColors(void)
{
  int *rgObjRuler = ruler1, i, k;

  // Figure out which rulership set to use for "Element" color.
  if (ignore7[rrStd]) {
    if      (!ignore7[rrExa]) rgObjRuler = exalt;
    else if (!ignore7[rrEso]) rgObjRuler = rgObjEso1;
    else if (!ignore7[rrHie]) rgObjRuler = rgObjHie1;
    else if (!ignore7[rrRay]) rgObjRuler = NULL;
  }

  // Determine and assign the color of each planet.
  for (i = 0; i <= oNorm; i++) {
    k = kObjU[i];
    if (k == kRay || rgObjRuler == NULL)
      k = kRayA[rgObjRay[i]];
    else if (k == kElement)
      k = kElemA[(rgObjRuler[i]-1) & 3];
    else if (k == kPlanet)
      k = kObjA[!FBetween(i, cobLo, cobHi) ? ObjOrbit(i) :
        oJup + (i - cobLo)] ^ 8;
    kObjA[i] = k;
  }

  // Determine and assign the color of each star.
  EnsureStarBright();
  k = kObjU[starLo];
  for (i = starLo; i <= starHi; i++)
    kObjA[i] = (k >= kNull ? KStarA(rStarBright[i-starLo+1]) : k);
}


// This is the dispatch procedure for the entire program. After all the
// command switches have been processed, this routine is called to actually
// call the various routines to generate and display the charts.

void Action(void)
{
  char sz[cchSzMax];
  int cSequenceLine = us.cSequenceLine, iList, iList2, iLine, i;
  flag fDoList, fHTML, fHTMLClip;

  // If the -os switch is in effect, open a file and set a global to
  // internally 'redirect' all screen output to.

  if (is.szFileScreen) {
    is.S = fopen(is.szFileScreen, "w");
    if (is.S == NULL) {
      sprintf(sz, "File %s can not be created.", is.szFileScreen);
      PrintError(sz);
      is.S = stdout;
    }
  } else
    is.S = stdout;
  is.cchRow = is.cchCol = is.cchColMax = 0;

  // If the -kh switch is in effect, start outputting a new HTML file.

  fHTML = us.fTextHTML && !us.fGraphics && is.S != stdout;
  if (fHTML) {
    fHTMLClip = is.nHTML < 0;
    is.nHTML = 2;
    if (fHTMLClip)
      PrintSz("Version:0.9\n"
        "StartHTML:00000161\n"
        "EndHTML:00010000\n"
        "StartFragment:00000196\n"
        "EndFragment:00010000\n");
    sprintf(sz, "<html>\n<head><meta charset=\"UTF-8\"><title>Astrolog %s"
      "</title></head>\n<body>", szVersionCore); PrintSz(sz);
    if (fHTMLClip)
      PrintSz("\n<!--StartFragment -->\n");
    PrintSz("<font face=\"Courier\">");
    is.nHTML = 3;
  } else
    is.nHTML = 0;

  if (us.nCharsetOut == ccUTF8 && is.S != stdout)
    fprintf(is.S, "%c%c%c", 0xef, 0xbb, 0xbf);

  // If the -5e switch is in effect, loop over all charts in chart list.

  fDoList = (us.nListAll > 0 && !us.fGraphics && is.cci > 0 &&
    !(us.nListAll == 3 && is.cci < 2));
  iList = (us.nListAll == 3); iList2 = 0;
LNextList:
  if (fDoList) {
    is.iciIndex1 = iList; is.iciIndex2 = iList2;
    if (us.nListAll == 1)
      ciCore = ciMain = is.rgci[iList];
    else if (us.nListAll == 2)
      ciTwin = is.rgci[iList];
    else {
      ciCore = ciMain = is.rgci[iList];
      ciTwin = is.rgci[iList2];
    }
  }
  iLine = 0;

LNextLine:
  if (iLine < cSequenceLine && is.rgszLine[iLine] != NULL)
    FProcessCommandLine(is.rgszLine[iLine]);
  is.fMult = fFalse;
  is.fNoEphFile = fFalse;
  InitColors();
  AnsiColor(kDefault);

  // First adjust the restriction status of the cusps, Uranians, stars, and so
  // on, based on whether -C, -u, and -U switches are in effect.

  if (!us.fCusp)
    for (i = cuspLo; i <= cuspHi; i++)
      ignore[i] = ignore2[i] = fTrue;
  if (!us.fUranian)
    for (i = uranLo; i <= uranHi; i++)
      ignore[i] = ignore2[i] = fTrue;
  if (!us.fDwarf)
    for (i = dwarfLo; i <= dwarfHi; i++)
      ignore[i] = ignore2[i] = fTrue;
  if (!us.fMoons)
    for (i = moonsLo; i <= moonsHi; i++)
      ignore[i] = ignore2[i] = fTrue;
  if (!us.fCOB)
    for (i = cobLo; i <= cobHi; i++)
      ignore[i] = ignore2[i] = fTrue;
  if (!us.fStar)
    for (i = starLo; i <= starHi; i++)
      ignore[i] = ignore2[i] = fTrue;

  if (FPrintTables())    // Print out any generic tables specified.
    goto LDone;          // If nothing else to do, then exit right away.
  if (is.fMult) {
    PrintL2();
    is.fMult = fFalse;
  }

  // Here either do a normal chart or some kind of relationship chart.

  if (!us.nRel) {
#ifndef WIN
    // If chart info not in memory yet, then prompt the user for it.
    if (!is.fHaveInfo && !FInputData(szTtyCore))
      return;
    ciMain = ciCore;
    CastChart(1);
#else
    ciMain = ciCore;
    if (wi.fCast || cSequenceLine > 0 || fDoList) {
      wi.fCast = fFalse;
      CastChart(1);
    }
#endif
  } else {
    ciMain = ciCore;
    CastRelation();
  }
#ifndef WIN
  ciSave = ciMain;
#endif

#ifdef GRAPH
  if (us.fGraphics) {
    // If in -X graphics mode, go make a graphics chart.
    FActionX();
    iLine = cSequenceLine;    // Once any graphics drawn, stop looping!
  } else
#endif
  {
    // If not in graphics mode, print a text only chart on screen.
#ifdef GRAPH
    if (gs.fInverse) {
      SwapN(kBlackA, kWhiteA);
      SwapN(kLtGrayA, kDkGrayA);
      AnsiColor(kDefault);
    }
#endif
#ifdef EXPRESS
  // Notify AstroExpression a chart is about to be drawn.
  if (!us.fExpOff && FSzSet(us.szExpDisp1))
    ParseExpression(us.szExpDisp1);
#endif
    PrintChart(is.fProgress);
#ifdef EXPRESS
  // Notify AstroExpression a chart has just been drawn.
  if (!us.fExpOff && FSzSet(us.szExpDisp2))
    ParseExpression(us.szExpDisp2);
#endif
#ifdef GRAPH
    if (gs.fInverse) {
      SwapN(kBlackA, kWhiteA);
      SwapN(kLtGrayA, kDkGrayA);
    }
#endif
  }

LDone:
  iLine++;
  if (iLine < cSequenceLine) {
    if (!us.fGraphics)
      PrintL2();
    goto LNextLine;
  }
  if (fDoList) {
    if (us.nListAll >= 3) {
      iList2++;
      if (iList2 < (us.nListAll >= 4 ? is.cci : iList)) {
        PrintL2();
        goto LNextList;
      }
      iList2 = 0;
    }
    iList++;
    if (iList < is.cci) {
      PrintL2();
      goto LNextList;
    }
    is.iciIndex1 = is.iciIndex2 = -1;
  }

  if (fHTML) {           // If -kh switch in effect, end the HTML file.
    is.nHTML = 2;
    PrintSz("</font>\n</font>");
    if (fHTMLClip)
      PrintSz("\n<!--EndFragment-->\n");
    PrintSz("</body>\n</html>\n");
    is.nHTML = 0;
  }

  if (us.fWriteFile)     // If -o switch in effect, then write the chart
    FOutputData();       // information to a file.

  if (is.S != stdout)    // If were internally directing chart display to a
    fclose(is.S);        // file as with the -os switch, close it here.
}


#ifndef WIN
// Reset a few variables to their default values they have upon startup of the
// program. We don't reset all variables, just the most volatile ones. This is
// called when in the -Q loop to reset things like which charts to display,
// but leave setups such as object restrictions and orbs alone.

void InitVariables(void)
{
  us.fInterpret = us.fProgress = is.fHaveInfo = is.fMult = fFalse;
  us.nRel = rcNone;
  FCloneSz(NULL, &is.szFileScreen);
  ClearB((pbyte)&us.fListing, (int)((pbyte)&us.fLoop - (pbyte)&us.fListing));
}
#endif


/*
******************************************************************************
** Command Switch Procedures.
******************************************************************************
*/

// Given a string representing a command line (e.g. a macro string), go parse
// it into its various switches and parameters, then go process them and
// change program settings. Basically a wrapper for other functions.

flag FProcessCommandLine(CONST char *szLine)
{
  char szCommandLine[cchSzLine], *rgsz[MAXSWITCHES];
  int argc, cb;
  flag fT = fFalse;
  FILE *fileT;

  if (szLine == NULL || *szLine == chNull)
    return fTrue;
  cb = CchSz(szLine)+1;

  // Check for filename on command line.
  if (!FChSwitch(szLine[0])) {
    fileT = fopen(szLine, "r");
    if (fileT != NULL) {
      fclose(fileT);
      sprintf(szCommandLine, "-i \"%s\"", szLine);
      fT = fTrue;
    }
  }

  // Parse and process the command line.
  if (!fT)
    CopyRgb((byte *)szLine, (byte *)szCommandLine, cb);
  argc = NParseCommandLine(szCommandLine, rgsz);
  fT = FProcessSwitches(argc, rgsz);
  if (!fT) {
    sprintf(szCommandLine, "Failed to parse command line: %s", szLine);
    PrintWarning(szCommandLine);
  }
  return fT;
}


// Given a string representing a command line, convert it to an "argv" format
// of an array of strings, one for each switch or parameter, i.e. exactly like
// the format of the command line as given when the program starts.

int NParseCommandLine(char *szLine, char **argv)
{
  int argc = 1, fSpace = fTrue;
  char *pch = szLine, chQuote = chNull;

  // Split the entered line up into its individual switch strings.
  while ((uchar)*pch >= ' ' || *pch == chTab) {
    if (*pch == ' ' || *pch == chTab) {
      if (fSpace)
        // Skip over the current run of spaces between strings.
        ;
      else {
        // First space after a string, end parameter here.
        if (chQuote == chNull) {
          *pch = chNull;
          fSpace = fTrue;
        }
      }
    } else {
      if (fSpace) {
        // First character after run of spaces, begin parameter here.
        if (argc >= MAXSWITCHES-1) {
          PrintWarning("Too many parameters! Rest of line ignored.");
          break;
        }
        chQuote = (*pch == '"' || *pch == '\'') ? *pch : chNull;
        argv[argc++] = pch + (chQuote != chNull);
        fSpace = fFalse;
      } else {
        // Skip over the current string.
        if (chQuote != chNull && *pch == chQuote && pch[1] < '@') {
          *pch = chNull;
          fSpace = fTrue;
        }
      }
    }
    pch++;
  }
  argv[0] = (char *)szAppNameCore;
  argv[argc] = NULL;               // Set last string in switch array to Null.
  return argc;
}


#ifndef WIN
// This routine is called by the main program to interactively prompt the user
// for command switches and parameters, entered in the same format as they
// would be on a command line. This needs to be called with certain systems
// which don't allow passing of a command line to the program, or when -Q is
// in effect. The result of this routine is returned to the main program which
// then processes it as done with a real command line.

int NPromptSwitches(char *line, char *argv[MAXSWITCHES])
{
  FILE *fileSav;
  char sz[cchSzDef];

  fileSav = is.S; is.S = stdout;
  is.cchRow = 0;
  AnsiColor(kWhiteA);
  sprintf(sz, "** %s version %s ", szAppName, szVersionCore); PrintSz(sz);
  sprintf(sz, "(See '%cHc' switch for copyrights and credits.) **\n",
    chSwitch); PrintSz(sz);
  AnsiColor(kDefault);
  PrintSz("Enter all parameter options below. ");
  sprintf(sz, "(Enter '%cH' for help. Enter '.' to exit.)\n", chSwitch);
  PrintSz(sz);
  is.S = fileSav;
  InputString("Input command line", line);
  PrintL();
  return NParseCommandLine(line, argv);
}
#endif


// This subprocedure is like FProcessSwitches() below, except that it only
// processes one switch, namely one of the obscure -Y types.

int NProcessSwitchesRare(int argc, char **argv, int pos,
  flag fOr, flag fAnd, flag fNot)
{
  int darg = 0, i, j, k, l;
  real r;
  char ch1, ch2 = chNull;
  pbyte pb;
  int *lpn;
  real *lpr;
#ifdef MATRIX
  OE oe;
#endif
#ifdef SWISS
  char szName[cchSzDef], *pch;
#endif

  ch1 = argv[0][pos+1];
  if (ch1 != chNull)
    ch2 = argv[0][pos+2];

  switch (argv[0][pos]) {
  case chNull:
    SwitchF(us.fSwitchRare);
    break;

  case 'T':
    SwitchF(us.fTruePos);
    break;

  case 'V':
    SwitchF(us.fTopoPos);
    break;

  case 'f':
    SwitchF(us.fRefract);
    break;

  case 'h':
    SwitchF(us.fBarycenter);
    break;

  case 'm':
    SwitchF(us.fMoonMove);
    break;

  case 's':
    if (argc > 1 && (r = RParseSz(argv[1], pmOffset)) != rLarge) {
      if (FErrorValR("Ys", !FValidOffset(r), r, 0))
        return fFalse;
      us.rZodiacOffsetAll = r;
      darg++;
    }
    SwitchF(us.fSidereal2);
    break;

  case 'n':
    if (ch1 == '0')
      SwitchF(us.fNoNutation);
    else if (ch1 == 'n')
      SwitchF(us.fNaturalNode);
    else
      SwitchF(us.fTrueNode);
    break;

  case 'u':
    if (ch1 == '0')
      SwitchF(us.fEclipseAny);
    SwitchF(us.fEclipse);
    break;

  case 'd':
    SwitchF(us.fEuroDate);
    break;

  case 't':
    SwitchF(us.fEuroTime);
    break;

  case 'v':
    SwitchF(us.fEuroDist);
    break;

  case 'r':
    SwitchF(us.fRound);
    break;

  case 'w':
    if (FErrorArgc("Yw", argc, 1))
      return tcError;
    r = RFromSz(argv[1]);
    if (FErrorValR("Yw", r < 0.0, r, 0))
      return tcError;
    us.rStation = r;
    darg++;
    break;

  case 'C':
    SwitchF(us.fSmartCusp);
    break;

  case 'O':
    SwitchF(us.fSmartSave);
    break;

  case '8':
    SwitchF(us.fClip80);
    break;

  case 'a':
    if (ch1 == 'o') {
      i = (ch2 - '0');
      if (!FBetween(i, 0, 3))
        i = 0;
      us.nCharsetOut = i;
    } else {
      i = (ch1 - '0');
      if (!FBetween(i, 0, 3))
        i = 0;
      us.nCharset = i;
    }
    break;

  case 'Q':
    if (FErrorArgc("YQ", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("YQ", i < 0, i, 0))
      return tcError;
    us.nScrollRow = i;
    darg++;
    break;

  case 'q':
    i = (ch1 - '0');
    if (!FBetween(i, 0, 9))
      i = 0;
    if (FErrorArgc("Yq", argc, i))
      return tcError;
    us.cSequenceLine = i;
    for (i = 0; i < us.cSequenceLine; i++)
      FCloneSz(argv[i+1], &is.rgszLine[i]);
    darg += i;
    break;

  case 'i':
    if (FErrorArgc("Yi", argc, 1))
      return tcError;
    i = (ch1 - '0');
    if (!FBetween(i, 0, 9))
      i = 0;
    FCloneSz(argv[1], &us.rgszPath[i]);
    is.fSwissPathSet = fFalse;
    darg++;
    break;

  case 'o':
    SwitchF(us.fWriteOld);
    break;

  case 'c':
    SwitchF(us.fHouseAngle);
    for (i = 0; i < 4; i++)
      FCloneSzCore(us.fHouseAngle ? szObjName[objMax + i] :
        szObjName[oAsc + i*3], (char **)&szObjDisp[oAsc + i*3],
        szObjDisp[oAsc + i*3] == szObjName[oAsc + i*3]);
    break;

  case 'p':
    SwitchF(us.fPolarAsc);
    break;

  case 'z':
    if (ch1 == '0' && fAnd) {
      us.rDeltaT = rInvalid;
      break;
    } else if (ch1 == '1') {
      SwitchF(us.fOffsetOnly);
      break;
    }
    if (FErrorArgc("Yz", argc, 1))
      return tcError;
    if (ch1 == '0')
      us.rDeltaT = RFromSz(argv[1]);
    else if (ch1 == 'O')
      us.rObjAddition = RFromSz(argv[1]);
    else if (ch1 == 'C')
      us.rCuspAddition = RFromSz(argv[1]);
    else
      us.lTimeAddition = NFromSz(argv[1]);
    darg++;
    break;

  case '1':
    if (FErrorArgc("Y1", argc, 2))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("Y1", !FItem(i), i, 1))
      return tcError;
    j = NParseSz(argv[2], pmObject);
    if (FErrorValN("Y1", !FItem(j), j, 2))
      return tcError;
    us.fObjRotWhole = (ch1 == '0' && !fAnd);
    us.objRot1 = i;
    us.objRot2 = j;
    darg += 2;
    break;

  case 'Z':
    if (FErrorArgc("YZ", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("YZ", !FBetween(i, 0, 7), i, 0))
      return tcError;
    us.nHorizon = i;
    darg++;
    break;

  case 'l':
    if (FErrorArgc("Yl", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("Yl", !FSector(i), i, 0))
      return tcError;
    SwitchF(pluszone[i]);
    darg++;
    break;

#ifdef ARABIC
  case 'P':
    if (FErrorArgc("YP", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("YP", !FBetween(i, -1, 1), i, 0))
      return tcError;
    us.nArabicNight = i;
    darg++;
    break;
#endif

  case 'b':
    if (FErrorArgc("Yb", argc, 1))
      return tcError;
    i = NFromSz(argv[1]);
    if (FErrorValN("Yb", !FValidBioday(i), i, 0))
      return tcError;
    us.nBioday = i;
    darg++;
    break;

#ifdef SWISS
  case 'e':
    if (FErrorArgc("Ye", argc, 2))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("Ye", !FCust(i), i, 1))
      return tcError;
    i -= custLo;
    j = (ch1 == 'b') + (ch1 == 'O')*2 + (ch1 == 'm')*3 + (ch1 == 'j')*4;
    if (j > 0)
      ch1 = ch2;
    k = (j == 2 ? NParseSz(argv[2], pmObject) : NFromSz(argv[2]));
    if (FErrorValN("Ye", k < (int)(j != 2) && j < 4, k, 2))
      return tcError;
    rgObjSwiss[i] = k;
    rgTypSwiss[i] = j;
    rgPntSwiss[i] =
      (ch1 == 'n') + (ch1 == 's')*2 + (ch1 == 'p')*3 + (ch1 == 'a')*4;
    rgFlgSwiss[i] = 0;
    if (j <= 1)
      SwissGetObjName(szName, j <= 0 ? -k : k);
    else {
      if (j == 3)
        k = ObjMoons(k);
      sprintf(szName, "%s", FItem(k) ? szObjName[k] : szObjUnknown);
    }
    k = rgPntSwiss[i];
    if (k > 0) {
      for (pch = szName; *pch; pch++)
        ;
      sprintf(szName + Min(3, pch-szName), "%s",
        k == 1 ? "Nor" : (k == 2 ? "Sou" : (k == 3 ? "Per" : "Api")));
    }
    FCloneSzCore(szName, (char **)&szObjDisp[i + custLo],
      szObjDisp[i + custLo] == szObjName[i + custLo]);
#ifdef GRAPH
    FCloneSzCore("t", (char **)&szDrawObject[i + custLo],
      szDrawObject[i + custLo] == szDrawObjectDef[i + custLo]);
    FCloneSzCore("", (char **)&szDrawObject2[i + custLo],
      szDrawObject2[i + custLo] == szDrawObjectDef2[i + custLo]);
#endif
    k = pos + 1;
    do {
      ch2 = argv[0][k++];
      rgFlgSwiss[i] |= (ch2 == 'H') + (ch2 == 'S')*2 + (ch2 == 'B')*4 +
        (ch2 == 'N')*8 + (ch2 == 'T')*16 + (ch2 == 'V')*32;
    } while (ch2);
    darg += 2;
    break;
#endif

#ifdef MATRIX
  case 'E':
    if (FErrorArgc("YE", argc, 17))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("YE", !FHelio(i), i, 1))
      return tcError;
    oe.sma = RFromSz(argv[2]);
    oe.ec0 = atof(argv[3]);  oe.ec1 = atof(argv[4]);  oe.ec2 = atof(argv[5]);
    oe.in0 = atof(argv[6]);  oe.in1 = atof(argv[7]);  oe.in2 = atof(argv[8]);
    oe.ap0 = atof(argv[9]);  oe.ap1 = atof(argv[10]); oe.ap2 = atof(argv[11]);
    oe.an0 = atof(argv[12]); oe.an1 = atof(argv[13]); oe.an2 = atof(argv[14]);
    oe.ma0 = atof(argv[15]); oe.ma1 = atof(argv[16]); oe.ma2 = atof(argv[17]);
    rgoe[IoeFromObj(i)] = oe;
    darg += 17;
    break;
#endif

  case 'U':
    if (ch1 == 'b') {
      SwitchF(us.fStarMagDist);
      if (ch2 == '0')
        SwitchF(us.fStarMagAbs);
      break;
    } else if (ch1 == 'x') {
      if (FErrorArgc("YUx", argc, 1))
        return tcError;
      FCloneSz(argv[1], &us.szExoList);
      darg++;
      break;
    }
    if (FErrorArgc("YU", argc, 2))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("YU", !FStar(i), i, 1))
      return tcError;
    FCloneSz(argv[2], &szStarCustom[i-oNorm]);
    rStarBrightDef[0] = -1.0;                    // Recompute brightness
    darg += 2;
    break;

  case 'R':
    if (ch1 == 'd') {
      if (FErrorArgc("YRd", argc, 1))
        return tcError;
      us.nSignDiv = NFromSz(argv[1]);
      darg++;
      break;
    } else if (ch1 == 'h') {
      SwitchF(us.fIgnoreAuto);
      break;
    } else if (ch1 == 'U') {
      if (FErrorArgc("YRU", argc, 1))
        return tcError;
      us.fStarsList = (ch2 == '0');
      FCloneSz(argv[1], &us.szStarsList);
      darg++;
      break;
    } else if (ch1 == 'o') {
      InitRestrictions(fTrue);
      break;
    } else if (ch1 == 'i') {
      InitRestrictions(fFalse);
      AdjustRestrictions();
      break;
    }
    if (FErrorArgc("YR", argc, 2 + (ch1 == 'Z')*2 + (ch1 == '7')*3))
      return tcError;
    i = NParseSz(argv[1], pmObject); j = NParseSz(argv[2], pmObject);
    if (ch1 == '0') {
      us.fIgnoreSign = i != 0;
      us.fIgnoreDir  = j != 0;
      darg += 2;
      break;
    } else if (ch1 == '1') {
      us.fIgnoreDiralt = i != 0;
      us.fIgnoreDirlen = j != 0;
      darg += 2;
      break;
    } else if (ch1 == '2') {
      us.fIgnoreAlt0   = i != 0;
      us.fIgnoreDisequ = j != 0;
      darg += 2;
      break;
    } else if (ch1 == 'Z') {
      ignorez[arAsc] = i != 0;
      ignorez[arMC]  = j != 0;
      ignorez[arDes] = NFromSz(argv[3]) != 0;
      ignorez[arIC]  = NFromSz(argv[4]) != 0;
      darg += 4;
      break;
    } else if (ch1 == '7') {
      ignore7[rrStd] = i != 0;
      ignore7[rrEso] = j != 0;
      ignore7[rrHie] = NFromSz(argv[3]) != 0;
      ignore7[rrExa] = NFromSz(argv[4]) != 0;
      ignore7[rrRay] = NFromSz(argv[5]) != 0;
      if (!ignore7[rrRay])
        EnsureRay();
      darg += 5;
      break;
    }
    if (FErrorValN("YR", !FItem(i), i, 1))
      return tcError;
    if (FErrorValN("YR", !FItem(j) || j < i, j, 2))
      return tcError;
    if (FErrorArgc("YR", argc, 3+j-i))
      return tcError;
    pb = ch1 == 'T' ? ignore2 : ignore;
    for (k = i; k <= j; k++)
      pb[k] = NFromSz(argv[3+k-i]) != 0;
    darg += 3+j-i;
    RedoRestrictions();
    break;

  case 'A':
    if (ch1 == 'D') {
      if (FErrorArgc("YAD", argc, 4))
        return tcError;
      i = NParseSz(argv[1], pmAspect);
      if (FErrorValN("YAD", !FAspect2(i), i, 1))
        return tcError;
      FCloneSzCore(CchSz(argv[2]) >= 3 ? argv[2] : szAspectName[i],
        (char **)&szAspectDisp[i], szAspectDisp[i] == szAspectName[i]);
      FCloneSzCore(CchSz(argv[3]) >= 3 ? argv[3] : szAspectAbbrev[i],
        (char **)&szAspectAbbrevDisp[i],
        szAspectAbbrevDisp[i] == szAspectAbbrev[i]);
      FCloneSzCore(CchSz(argv[4]) >= 3 ? argv[4] : szAspectGlyph[i],
        (char **)&szAspectGlyphDisp[i],
        szAspectGlyphDisp[i] == szAspectGlyph[i]);
      darg += 4;
      break;
    }
    if (FErrorArgc("YA", argc, 2))
      return tcError;
    k = ch1 == 'm' || ch1 == 'd' ? pmObject : pmAspect;
    i = NParseSz(argv[1], k); j = NParseSz(argv[2], k);
    k = ch1 == 'm' || ch1 == 'd' ? oNorm+1 : cAspect;
    if (FErrorValN("YA", !FBetween(i, (int)(ch1 == 'o' || ch1 == 'a'), k),
      i, 1))
      return tcError;
    if (FErrorValN("YA", !FBetween(j, 0, k) || j < i, j, 2))
      return tcError;
    if (FErrorArgc("YA", argc, 3+j-i))
      return tcError;
    lpr = ch1 == 'o' ? rAspOrb : (ch1 == 'm' ? rObjOrb :
      (ch1 == 'd' ? rObjAdd : rAspAngle));
    for (k = i; k <= j; k++)
      lpr[k] = RFromSz(argv[3+k-i]);
    darg += 3+j-i;
    break;

  case 'j':
    if (FErrorArgc("Yj", argc, 2 + 2*(ch1 == '0') + 4*(ch1 == '7')))
      return tcError;
    if (ch1 == '0') {
      rObjInf[oNorm1+1]  = RFromSz(argv[1]);
      rObjInf[oNorm1+2]  = RFromSz(argv[2]);
      rHouseInf[cSign+1] = RFromSz(argv[3]);
      rHouseInf[cSign+2] = RFromSz(argv[4]);
      darg += 4;
      break;
    } else if (ch1 == '7') {
      rObjInf[oNorm1+3]  = RFromSz(argv[1]);
      rObjInf[oNorm1+4]  = RFromSz(argv[2]);
      rObjInf[oNorm1+5]  = RFromSz(argv[3]);
      rHouseInf[cSign+3] = RFromSz(argv[4]);
      rHouseInf[cSign+4] = RFromSz(argv[5]);
      rHouseInf[cSign+5] = RFromSz(argv[6]);
      darg += 6;
      break;
    }
    k = ch1 == 'C' ? pmSign : (ch1 == 'A' ? pmAspect : pmObject);
    i = NParseSz(argv[1], k); j = NParseSz(argv[2], k);
    k = ch1 == 'C' ? cSign : (ch1 == 'A' ? cAspect : oNorm1);
    if (FErrorValN("Yj", !FBetween(i, 0, k), i, 1))
      return tcError;
    if (FErrorValN("Yj", !FBetween(j, 0, k) || j < i, j, 2))
      return tcError;
    if (FErrorArgc("Yj", argc, 3+j-i))
      return tcError;
    lpr = ch1 == 'C' ? rHouseInf : (ch1 == 'A' ? rAspInf :
      (ch1 == 'T' ? rTransitInf : rObjInf));
    for (k = i; k <= j; k++)
      lpr[k] = RFromSz(argv[3+k-i]);
    darg += 3+j-i;
    break;

  case 'J':
    if (FErrorArgc("YJ", argc, 3 - (ch1 == '0')))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("YJ", !FNorm(i), i, 1))
      return tcError;
    j = NParseSz(argv[2], pmSign);
    if (FErrorValN("YJ", !FBetween(j, 0, cSign), j, 2))
      return tcError;
    if (ch1 != '0') {
      k = NParseSz(argv[3], pmSign);
      if (FErrorValN("YJ", !FBetween(k, 0, cSign), k, 3))
        return tcError;
    }
    if (ch1 == chNull) {
      ruler1[i] = j;
      ruler2[i] = k;
      if (FBetween(i, 1, oPlu)) {
        if (j != 0)
          rules[j] = i;
        if (k != 0)
          rules[k] = i;
      }
    } else if (ch1 == '0') {
      exalt[i] = j;
    } else if (ch2 != '0') {
      rgObjEso1[i] = j;
      rgObjEso2[i] = k;
      if (FBetween(i, 0, oPlu) || i == oVul) {
        if (j != 0)
          rgSignEso1[j] = i;
        if (k != 0)
          rgSignEso1[k] = i;
      }
    } else {
      rgObjHie1[i] = j;
      rgObjHie2[i] = k;
      if (FBetween(i, 0, oPlu) || i == oVul) {
        if (j != 0)
          rgSignHie1[j] = i;
        if (k != 0)
          rgSignHie1[k] = i;
      }
    }
    darg += 3 - (ch1 == '0');
    break;

  case '7':
    if (FErrorArgc("Y7", argc, 2))
      return tcError;
    k = ch1 == 'O' ? pmObject : (ch1 == 'C' ? pmSign : 0);
    i = NParseSz(argv[1], k); j = NParseSz(argv[2], k);
    k = ch1 == 'O' ? oNorm : (ch1 == 'C' ? cSign : 0);
    if (FErrorValN("Y7", !FBetween(i, (int)(ch1 == 'C'), k), i, 1))
      return tcError;
    if (FErrorValN("Y7", !FBetween(j, (int)(ch1 == 'C'), k) || j < i, j, 2))
      return tcError;
    if (FErrorArgc("Y7", argc, 3+j-i))
      return tcError;
    lpn = ch1 == 'O' ? rgObjRay : (ch1 == 'C' ? rgSignRay : NULL);
    for (k = i; k <= j; k++) {
      l = NFromSz(argv[3+k-i]);
      if (FErrorValN("Y7",
        !FBetween(l, (int)(ch1 == 'C'), ch1 != 'C' ? 7 : 1234567), l, 3+k-i))
        return tcError;
      lpn[k] = l;
    }
    darg += 3+j-i;
    break;

#ifdef INTERPRET
  case 'I':
    if (FErrorArgc("YI", argc, 2))
      return tcError;
    i = NParseSz(argv[1],
      ch1 == 'A' ? pmAspect : (ch1 == chNull ? pmObject : pmSign));
    j = ch1 == 'A' ? cAspect : (ch1 == chNull ? (int)cObj : (int)cSign);
    if (FErrorValN("YI", !FBetween(i, (int)(ch1 != chNull), j), i, 1))
      return tcError;
    if (ch1 == 'A' && ch2 == '0')
      ch1 = '0';
    switch (ch1) {
    case 'A':    FCloneSzCore(argv[2], (char **)&szInteract[i],
      szInteract[i]  == szInteractDef[i]);  break;
    case '0':    FCloneSzCore(argv[2], (char **)&szTherefore[i],
      szTherefore[i] == szThereforeDef[i]); break;
    case chNull: FCloneSzCore(argv[2], (char **)&szMindPart[i],
      szMindPart[i]  == szMindPartDef[i]);  break;
    case 'C':    FCloneSzCore(argv[2], (char **)&szLifeArea[i],
      szLifeArea[i]  == szLifeAreaDef[i]);  break;
    case 'v':    FCloneSzCore(argv[2], (char **)&szDesire[i],
      szDesire[i]    == szDesireDef[i]);    break;
    default:     FCloneSzCore(argv[2], (char **)&szDesc[i],
      szDesc[i]      == szDescDef[i]);      break;
    }
    darg += 2;
    break;
#endif

  case 'k':
    if (ch1 == 'U') {
      if (FErrorArgc("YkU", argc, 1))
        return tcError;
      FCloneSz(argv[1], &us.szStarsColor);
      darg++;
      break;
    }
    if (ch1 == 'E') {
      if (FErrorArgc("YkE", argc, 1))
        return tcError;
      FCloneSz(argv[1], &us.szAstColor);
      darg++;
      break;
    }
    if (FErrorArgc("Yk", argc, 2 + 2*(ch1 == 'C')))
      return tcError;
    if (ch1 == 'C') {
      kElemA[eFir] = NParseSz(argv[1], pmColor) & (cColor-1);
      kElemA[eEar] = NParseSz(argv[2], pmColor) & (cColor-1);
      kElemA[eAir] = NParseSz(argv[3], pmColor) & (cColor-1);
      kElemA[eWat] = NParseSz(argv[4], pmColor) & (cColor-1);
      darg += 4;
      break;
    }
    k = ch1 == 'O' ? pmObject : (ch1 == 'A' ? pmAspect : 0);
    i = NParseSz(argv[1], k); j = NParseSz(argv[2], k);
    k = ch1 == 'O' ? starLo : (ch1 == 'A' ? cAspect :
      (ch1 == '0' || ch1 == '7' ? cRainbow : 8));
    if (FErrorValN("Yk",
      !FBetween(i, (int)(ch1 != chNull && ch1 != 'O'), k), i, 1))
      return tcError;
    if (FErrorValN("Yk",
      !FBetween(j, (int)(ch1 != chNull && ch1 != 'O'), k) || j < i, j, 2))
      return tcError;
    if (FErrorArgc("Yk", argc, 3+j-i))
      return tcError;
    lpn = ch1 == 'O' ? kObjU : (ch1 == 'A' ? kAspA : (ch1 == '7' ? kRayA :
      (ch1 == '0' ? kRainbowA : kMainA)));
    for (k = i; k <= j; k++)
      lpn[k] = NParseSz(argv[3+k-i], pmColor) & (ch1 != 'O' ? cColor-1 : ~0);
    darg += 3+j-i;
    break;

  case 'D':
    if (FErrorArgc("YD", argc, 2))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("YD", !FItem(i), i, 1))
      return tcError;
    FCloneSzCore(CchSz(argv[2]) >= 2 ? argv[2] : szObjName[i],
      (char **)&szObjDisp[i], szObjDisp[i] == szObjName[i]);
    darg += 2;
    break;

  case 'S':
    if (FErrorArgc("YS", argc, 2))
      return tcError;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("YS", !FNorm(i), i, 1))
      return tcError;
    r = RParseSz(argv[2], pmDist);
    if (FErrorValR("YS", r < 0.0, r, 2))
      return tcError;
    rObjDiam[i] = r;
    darg += 2;
    break;

  case 'F':
    if (FErrorArgc("YF", argc, 8))
      return tcError;
    is.fHaveInfo = fTrue;
    i = NParseSz(argv[1], pmObject);
    if (FErrorValN("YF", !FItem(i), i, 1))
      return tcError;
    r = Mod((real)(NFromSz(argv[2]) +
      (NParseSz(argv[3], pmSign)-1)*30) + RFromSz(argv[4])/60.0);
    planet[i] = r;
    if (FCusp(i) && i != oAsc && i != oMC) {
      chouse[i-(cuspLo-1)] = r;
      if (i == oDes)
        chouse[sAri] = Mod(chouse[sLib] + rDegHalf);
      else if (i == oNad)
        chouse[sCap] = Mod(chouse[sCan] + rDegHalf);
    }
    j = NFromSz(argv[5]);
    r = (j < 0 ? -1.0 : 1.0)*((real)NAbs(j) + RFromSz(argv[6])/60.0);
    planetalt[i] = Mod((r + rDegQuad) * 2.0) / 2.0 - rDegQuad;
    ret[i] = RFromSz(argv[7]);
    if (i <= oNorm)
      SphToRec(RFromSz(argv[8]), planet[i], planetalt[i],
        &space[i].x, &space[i].y, &space[i].z);
    MM = -1;    // Assume a chart position file is being loaded.
    darg += 8;
    break;

#ifdef GRAPH
  case 'X':
    return NProcessSwitchesRareX(argc, argv, pos+1, fOr, fAnd, fNot);
#endif

  case 'B':
#ifndef WIN
    putchar(chBell);
#else
    MessageBeep((UINT)-1);
#endif
    break;

  case '0':
    SwitchF(us.fNoDisplay);
    break;

  case '5':
    if (ch1 == 'i') {
      if (FErrorArgc("Y5i", argc, 1))
        return tcError;
      FCloneSz(argv[1], &us.szADB);
      darg++;
      break;
    } else if (ch1 == 'I') {
      if (FErrorArgc("Y5I", argc, 2))
        return tcError;
      i = NFromSz(argv[1]);
      if (FErrorValN("Y5I", i < 0, i, 1))
        return tcError;
      us.iExpADB = i;
      us.cExpADB = NFromSz(argv[2]);
      darg += 2;
      break;
    }
    i = 1 + (ch1 == '2') + (ch1 == '3')*2 + (ch1 == '4')*3;
    FEnumerateCIList(i);
    break;

  case 'Y':
    if (ch1 == 't') {
      if (FErrorArgc("YYt", argc, 1))
        return tcError;
      if (!us.fGraphics)
        PrintSzFormat(argv[1], fFalse);
      darg++;
      break;
    } else if (ch1 == 'T') {
      if (FErrorArgc("YYT", argc, 1))
        return tcError;
      PrintSzFormat(argv[1], fTrue);
      darg++;
      break;
    } 
#ifdef INTRPRET
    else if (ch1 == 'I') {
      if (FErrorArgc("YYI", argc, 1))
        return tcError;
      FieldWord(argv[1]);
      darg++;
      break;
    }
#endif
#ifdef ATLAS
    i = ch1 - '0';
    if (FErrorArgc("YY", argc, 1 + (i == 1 || i == 2)))
      return tcError;
    if (is.fileIn == NULL) {
      PrintError("Switch only allowed in file context.");
      return tcError;
    }
    ch2 = getc(is.fileIn);
    if (ch2 >= ' ')
      ungetc(ch2, is.fileIn);
    if (i <= 0) {
      if (!FLoadAtlas(is.fileIn, NFromSz(argv[1])))
        return tcError;
    } else if (i == 1) {
      if (!FLoadZoneRules(is.fileIn, NFromSz(argv[1]), NFromSz(argv[2])))
        return tcError;
    } else if (i == 2) {
      if (!FLoadZoneChanges(is.fileIn, NFromSz(argv[1]), NFromSz(argv[2])))
        return tcError;
    } else if (i >= 3) {
      if (!FLoadZoneLinks(is.fileIn, NFromSz(argv[1])))
        return tcError;
    }
    darg += 1 + (i == 1 || i == 2);
    break;
#endif

  default:
    ErrorSwitch(argv[0]);
    return tcError;
  }
  return darg;    // Return the value to be added to argc.
}


// Process a command switch line passed to the program. Read each entry in
// the argument list and set all the program modes and charts to display.

flag FProcessSwitches(int argc, char **argv)
{
  int ich, i, j, k;
  flag fNot, fOr, fAnd;
  real rT;
  char ch1, ch2, *pch;
  CI ci;
#ifdef EXPRESS
  char **ppch;
#endif

  argc--; argv++;
  while (argc) {
    ch1 = argv[0][0];
    fNot = fOr = fAnd = fFalse;
    switch (ch1) {
    case '=': fOr  = fTrue; break;
    case '_': fAnd = fTrue; break;
    case ':':               break;
    default:  fNot = fTrue; break;
    }
    ich = 1 + FChSwitch(ch1);    // Leading dash?
    ch1 = argv[0][ich];
    ch2 = (ch1 == chNull ? chNull : argv[0][ich+1]);
    switch (argv[0][ich-1]) {

    case 'H':
      if (ch1 == 'c')
        SwitchF(us.fCredit);
      else if (ch1 == 'Y')
        SwitchF(us.fSwitchRare);
#ifdef ISG
      else if (ch1 == 'X')
        SwitchF(us.fKeyGraph);
#endif
      else if (ch1 == 'C')
        SwitchF(us.fSign);
      else if (ch1 == 'O')
        SwitchF(us.fObject);
      else if (ch1 == 'A')
        SwitchF(us.fAspect);
      else if (ch1 == 'F')
        SwitchF(us.fConstel);
      else if (ch1 == 'S')
        SwitchF(us.fOrbitData);
      else if (ch1 == '7')
        SwitchF(us.fRay);
      else if (ch1 == 'I')
        SwitchF(us.fMeaning);
      else if (ch1 == 'e') {
        SwitchF(us.fCredit); SwitchF(us.fSwitch); SwitchF(us.fSwitchRare);
        SwitchF(us.fKeyGraph); SwitchF(us.fSign); SwitchF(us.fObject);
        SwitchF(us.fAspect); SwitchF(us.fConstel); SwitchF(us.fOrbitData);
        SwitchF(us.fRay); SwitchF(us.fMeaning);
      } else
        SwitchF(us.fSwitch);
      break;

    case 'Q':
      if (ch1 == '0')
        SwitchF(us.fLoopInit);
      SwitchF(us.fLoop);
      break;

    case 'M':
      if (FBetween(ch1, '1', '0' + cRing)) {
        i = (ch1 - '0') + (ch2 == '0');
        if (FErrorArgc("M", argc, i))
          return fFalse;
        for (j = 1; j <= i; j++)
          FCloneSz(argv[j], &szWheel[(ch2 == '0' && j >= i) ? 0 : j]);
        argc -= i; argv += i;
        break;
      }
      i = (ch1 == '0');
      if (FErrorArgc("M", argc, 1+i))
        return fFalse;
      j = NFromSz(argv[1]);
      if (FErrorValN("M", !FValidMacro(j), j, 1))
        return fFalse;
      if (i) {
        if (FEnsureMacro(j+1))
          FCloneSz(argv[2], &is.rgszMacro[j]);
      } else if (j < is.cszMacro)
        FProcessCommandLine(is.rgszMacro[j]);
      argc -= 1+i; argv += 1+i;
      break;

    case 'Y':
      i = NProcessSwitchesRare(argc, argv, ich, fOr, fAnd, fNot);
      if (i < 0)
        return fFalse;
      argc -= i; argv += i;
      break;

    // Switches which determine the type of chart to display:

    case 'v':
      if (ch1 == '0') {
        SwitchF(us.fVelocity);
        break;
      } else if (ch1 == '3') {
        SwitchF(us.fListDecan);
        if (argc > 1 && ((i = NFromSz(argv[1])) > 0 || FNumCh(argv[1][0]) ||
          argv[1][0] == '~')) {
          argc--; argv++;
          if (FErrorValN("v3", !FValidDecan(i), i, 0))
            return fFalse;
          if (i <= 0)
            us.fListDecan = fFalse;
          else
            us.nDecanType = i;
        }
        break;
      }
      SwitchF(us.fListing);
      break;

    case 'w':
      if (ch1 == '0')
        SwitchF(us.fWheelReverse);
      if (argc > 1 && (i = NFromSz(argv[1]))) {
        argc--; argv++;
        if (FErrorValN("w", !FValidWheel(i), i, 0))
          return fFalse;
        us.nWheelRows = i;
      }
      SwitchF(us.fWheel);
      break;

    case 'g':
      if (ch1 == '0' || ch2 == '0')
        SwitchF(us.fGridConfig);
      else if (ch1 == 'm' || ch2 == 'm')
        SwitchF(us.fGridMidpoint);
      if (ch1 == 'p')
        SwitchF(us.fParallel);
      else if (ch1 == 'd')
        SwitchF(us.fDistance);
      else if (ch1 == 'a')
        us.nAppSep = FSwitchF(us.nAppSep);
      else if (ch1 == 'x')
        us.nAppSep = FSwitchF(us.nAppSep) * 2;
      else if (ch1 == 's') {
        if (FErrorArgc("gs", argc, 1))
          return fFalse;
        i = NFromSz(argv[1]);
        if (FErrorValN("gs", !FValidAppSep(i), i, 0))
          return fFalse;
        us.nAppSep = i;
        argc--; argv++;
      }
#ifdef X11
      else if (ch1 == 'e') {
        if (FErrorArgc("geometry", argc, 1))
          return fFalse;
        gs.xWin = NFromSz(argv[1]);
        if (argc > 2 && (gs.yWin = NFromSz(argv[2]))) {
          argc--; argv++;
        } else
          gs.yWin = gs.xWin;
        if (FErrorValN("geometry", !FValidGraphX(gs.xWin), gs.xWin, 1))
          return fFalse;
        if (FErrorValN("geometry", !FValidGraphY(gs.yWin), gs.yWin, 2))
          return fFalse;
        argc--; argv++;
        break;
      }
#endif
      SwitchF(us.fGrid);
      break;

    case 'a':
      SwitchF(us.fAspList);
      if (ch1 == '0') {
        SwitchF(us.fAspSummary);
        ch1 = ch2;
      }
      if (ch1 == 'p') {
        SwitchF(us.fParallel);
        ch1 = ch2;
      } else if (ch1 == 'd') {
        SwitchF(us.fDistance);
        ch1 = ch2;
      } else if (ch1 == 'a') {
        us.nAppSep = FSwitchF(us.nAppSep);
        ch1 = ch2;
      } else if (ch1 == 'x') {
        us.nAppSep = FSwitchF(us.nAppSep) * 2;
        ch1 = ch2;
      } else if (ch1 == 's') {
        if (FErrorArgc("as", argc, 1))
          return fFalse;
        i = NFromSz(argv[1]);
        if (FErrorValN("as", !FValidAppSep(i), i, 0))
          return fFalse;
        us.nAppSep = i;
        argc--; argv++;
      }
      switch (ch1) {
      case 'j': us.nAspectSort = asj; break;
      case 'o': us.nAspectSort = aso; break;
      case 'n': us.nAspectSort = asn; break;
      case 'O': us.nAspectSort = asO; break;
      case 'P': us.nAspectSort = asP; break;
      case 'A': us.nAspectSort = asA; break;
      case 'C': us.nAspectSort = asC; break;
      case 'D': us.nAspectSort = asD; break;
      case 'm': us.nAspectSort = asM; break;
      }
      break;

    case 'm':
      if (ch1 == '0' || ch2 == '0')
        SwitchF(us.fMidSummary);
      if (ch1 == 'a')
        SwitchF(us.fMidAspect);
      SwitchF(us.fMidpoint);
      break;

    case 'Z':
      if (ch1 == 'd') {
        if (ch2 == 'm' || ch2 == 'y' || ch2 == 'Y') {
          if (ch2 == 'y')
            us.nEphemYears = 1;
          else if (ch2 == 'Y') {
            if (FErrorArgc("ZdY", argc, 1))
              return fFalse;
            i = NFromSz(argv[1]);
            if (FErrorValN("ZdY", i < 1, i, 0))
              return fFalse;
            us.nEphemYears = i;
            argc--; argv++;
          }
          SwitchF(us.fInDayMonth);
          us.fInDayYear = us.fInDayMonth && (ch2 != 'm');
        }
        SwitchF(us.fHorizonSearch);
        break;
      }
      if (ch1 == '0')
        SwitchF(us.fPrimeVert);
      SwitchF(us.fHorizon);
      break;

    case 'S':
      SwitchF(us.fOrbit);
      break;

    case 'l':
      if (ch1 == '0')
        SwitchF(us.fSectorApprox);
      SwitchF(us.fSector);
      break;

    case 'j':
      if (ch1 == '0')
        SwitchF(us.fInfluenceSign);
      SwitchF(us.fInfluence);
      break;

    case '7':
      if (FErrorSubswitch("7", ch1, ch1 != chNull))
        return fFalse;
      SwitchF(us.fEsoteric);
      break;

    case 'L':
      if (ch1 == '0')
        SwitchF(us.fLatitudeCross);
      if (argc > 1 && (i = NFromSz(argv[1]))) {
        argc--; argv++;
        if (FErrorValN("L", !FValidAstrograph(i), i, 1))
          return fFalse;
        us.nAstroGraphStep = i;
        if (argc > 1 && ((i = NFromSz(argv[1])) != 0 || FNumCh(argv[1][0]))) {
          argc--; argv++;
          if (FErrorValN("L0", !FValidAstrograph2(i), i, 2))
            return fFalse;
          us.nAstroGraphDist = i;
        }
      }
      SwitchF(us.fAstroGraph);
      break;

    case 'K':
      if (ch1 == 'y')
        SwitchF(us.fCalendarYear);
      SwitchF(us.fCalendar);
      break;

    case 'd':
      if (ch1 == 'p') {
        us.nProgress = (ch2 == '0') + 2*(ch2 == '1');
        if (us.nProgress)
          ch2 = argv[0][++ich + 1];
        i = (ch2 == 'y') + 2*(ch2 == 'Y');
#ifdef TIME
        j = i < 2 && (argv[0][ich+i+1] == 'n');
#else
        j = fFalse;
#endif
        if (!j && FErrorArgc("dp", argc, 2-(i&1)))
          return fFalse;
        is.fProgress = us.fInDayMonth = fTrue;
        DstT = ciDefa.dst; ZonT = ciDefa.zon;
        LonT = ciDefa.lon; LatT = ciDefa.lat;
#ifdef TIME
        if (j)
          GetTimeNow(&MonT, &DayT, &YeaT, &TimT, DstT, ZonT);
#endif
        if (i) {
          us.fInDayYear = fTrue;
          if (!j)
            YeaT = NParseSz(argv[1], pmYea);
          us.nEphemYears = i == 2 ? NFromSz(argv[2]) : 1;
        } else {
          if (!j) {
            MonT = NParseSz(argv[1], pmMon);
            YeaT = NParseSz(argv[2], pmYea);
            if (FErrorValN("dp", !FValidMon(MonT), MonT, 1))
              return fFalse;
          }
        }
        if (FErrorValN("dp", !FValidYea(YeaT), YeaT, i ? 1 : 2))
          return fFalse;
        if (!j) {
          i = 2-(i&1);
          argc -= i; argv += i;
        }
      } else if (ch1 == 'm' || ch1 == 'y' || ch1 == 'Y') {
        is.fProgress = fFalse;
        if (ch1 == 'y')
          us.nEphemYears = 1;
        else if (ch1 == 'Y') {
          if (FErrorArgc("dY", argc, 1))
            return fFalse;
          i = NFromSz(argv[1]);
          if (FErrorValN("dY", i < 1, i, 1))
            return fFalse;
          us.nEphemYears = i;
          argc--; argv++;
        }
        SwitchF(us.fInDayMonth);
        us.fInDayYear = us.fInDayMonth && (ch1 != 'm');
      }
#ifdef X11
      else if (ch1 == 'i') {    // -display switch for X
        if (FErrorArgc("display", argc, 1))
          return fFalse;
        FCloneSz(argv[1], &gs.szDisplay);
        argc--; argv++;
        break;
      }
#endif
      else if (argc > 1 && (i = NFromSz(argv[1]))) {
        if (FErrorValN("d", !FValidDivision(i), i, 0))
          return fFalse;
        us.nDivision = i;
        argc--; argv++;
      }
      SwitchF(us.fInDay);
      break;

    case 'D':
      SwitchF(us.fInDayInf);
      break;

    case 'E':
      j = ch1 == '0' || ch2 == '0';
      if (FErrorArgc("E", argc, (ch1 == 'Y') + j))
        return fFalse;
      SwitchF(us.fEphemeris);
      if (ch1 == 'y')
        us.nEphemYears = us.fEphemeris ? 1 : 0;
      else if (ch1 == 'Y') {
        i = NFromSz(argv[1]);
        if (FErrorValN("EY", i < 1, i, 1))
          return fFalse;
        us.nEphemYears = i;
        argc--; argv++;
      }
      if (j) {
        ch1 = argv[1][0];
        if (ch1) {
          us.nEphemRate = (ch1 == 'n' ? -2 : (ch1 == 'h' ? -1 :
            (ch1 == 'm' ? 1 : (ch1 == 'y' ? 2 : 0))));
          i = NFromSz(&argv[1][1]);
          us.nEphemFactor = Max(i, 1);
        }
        argc--; argv++;
      }
      break;

    case '8':
      if (ch1 == '0')
        SwitchF(us.fMoonChartSep);
      else if (FErrorSubswitch("8", ch1, ch1 != chNull))
        return fFalse;
      SwitchF(us.fMoonChart);
      break;

    case 'e':
      SwitchF(us.fListing); SwitchF(us.fWheel);
      SwitchF(us.fGrid); SwitchF(us.fAspList); SwitchF(us.fMidpoint);
      SwitchF(us.fHorizon); SwitchF(us.fOrbit); SwitchF(us.fSector);
      SwitchF(us.fInfluence); SwitchF(us.fEsoteric); SwitchF(us.fAstroGraph);
      SwitchF(us.fCalendar); SwitchF(us.fHorizonSearch);
      SwitchF(us.fInDay); SwitchF(us.fInDayInf); SwitchF(us.fInDayGra);
      SwitchF(us.fEphemeris); SwitchF(us.fArabic);
      SwitchF(us.fMoonChart); SwitchF(us.fExoTransit);
      break;

    case 't':
      SwitchF(us.fTransit);
      ZonT = ciDefa.zon; DstT = ciDefa.dst;
      LonT = ciDefa.lon; LatT = ciDefa.lat;
      if (ch1 == 'p') {
        us.nProgress = (ch2 == '0') + 2*(ch2 == '1');
        if (us.nProgress)
          ich++;
        is.fProgress = fTrue;
        ch1 = argv[0][++ich];
      } else
        is.fProgress = fFalse;
      if (ch1 == 'r') {
        SwitchF(is.fReturn);
        ch1 = argv[0][++ich];
      }
      i = (ch1 == 'y') + 2*(ch1 == 'Y') - (ch1 == 'd');
      if (i != 0)
        ch1 = argv[0][++ich];
      us.fInDayMonth = (i >= 0);
      us.fInDayYear = (i >= 1);
#ifdef TIME
      if (ch1 == 'n') {
        GetTimeNow(&MonT, &DayT, &YeaT, &TimT, DstT, ZonT);
        if (i >= 2) {
          if (FErrorArgc("tYn", argc, 1))
            return fFalse;
          us.nEphemYears = NFromSz(argv[1]);
          argc--; argv++;
        }
        break;
      }
#endif
      if (FErrorArgc("t", argc, 2 - (i == 1) + (i < 0)))
        return fFalse;
      YeaT = NParseSz(argv[2 - (i > 0) + (i < 0)], pmYea);
      if (FErrorValN("t", !FValidYea(YeaT), YeaT, 2 - (i > 0) + (i < 0)))
        return fFalse;
      if (i <= 0) {
        MonT = NParseSz(argv[1], pmMon);
        if (FErrorValN("t", !FValidMon(MonT), MonT, 1))
          return fFalse;
      }
      if (i < 0) {
        DayT = NParseSz(argv[2], pmDay);
        if (FErrorValN("td", !FValidDay(DayT, MonT, YeaT), DayT, 2))
          return fFalse;
      }
      if (i > 1)
        us.nEphemYears = NFromSz(argv[2]);
      i = 2 - (i == 1) + (i < 0);
      argc -= i; argv += i;
      break;

    case 'T':
      SwitchF(us.fTransitInf);
      ZonT = ciDefa.zon; DstT = ciDefa.dst;
      LonT = ciDefa.lon; LatT = ciDefa.lat;
      i = (ch1 == 't');
      if (i > 0)
        ch1 = argv[0][++ich];
      if (ch1 == 'p') {
        is.fProgress = fTrue;
        ch1 = argv[0][++ich];
      } else
        is.fProgress = fFalse;
      if (ch1 == 'r') {
        SwitchF(is.fReturn);
        ch1 = argv[0][++ich];
      }
#ifdef TIME
      if (ch1 == 'n') {
        GetTimeNow(&MonT, &DayT, &YeaT, &TimT, DstT, ZonT);
        break;
      }
#endif
      if (FErrorArgc("T", argc, 3 + i))
        return fFalse;
      MonT = NParseSz(argv[1], pmMon);
      DayT = NParseSz(argv[2], pmDay);
      YeaT = NParseSz(argv[3], pmYea);
      TimT = i > 0 ? RParseSz(argv[4], pmTim) : 0.0;
      if (FErrorValN("T", !FValidMon(MonT), MonT, 1))
        return fFalse;
      else if (FErrorValN("T", !FValidYea(YeaT), YeaT, 3))
        return fFalse;
      else if (FErrorValN("T", !FValidDay(DayT, MonT, YeaT), DayT, 2))
        return fFalse;
      else if (i > 0 && FErrorValR("Tt", !FValidTim(TimT), TimT, 4))
        return fFalse;
      argc -= 3+i; argv += 3+i;
      break;

    case 'B':
      SwitchF(us.fInDayGra);
      if (ch1 == 'p') {
        is.fProgress = fTrue;
        ch1 = argv[0][++ich];
      } else
        is.fProgress = fFalse;
      if (ch1 == 'm' || ch1 == 'y' || ch1 == 'Y') {
        if (ch1 == 'y')
          us.nEphemYears = 1;
        else if (ch1 == 'Y')
          us.nEphemYears = 5;
        SwitchF(us.fInDayMonth);
        us.fInDayYear = us.fInDayMonth && (ch1 != 'm');
      }
      if (ch1 == '0' || ch2 == '0')
        SwitchF(us.fGraphAll);
      break;

    case 'V':
      SwitchF(us.fTransitGra);
      ZonT = ciDefa.zon; DstT = ciDefa.dst;
      LonT = ciDefa.lon; LatT = ciDefa.lat;
      if (ch1 == 'p') {
        is.fProgress = fTrue;
        ch1 = argv[0][++ich];
      } else
        is.fProgress = fFalse;
      if (ch1 == 'r') {
        SwitchF(is.fReturn);
        ch1 = argv[0][++ich];
      }
      if (i = (ch1 == 'd') + 2*(ch1 == 'm') + 3*(ch1 == 'y') + 4*(ch1 == 'Y'))
        ch1 = argv[0][++ich];
      if (i < 1)
        i = 2;
      if (ch1 == '0') {
        SwitchF(us.fGraphAll);
        ch1 = argv[0][++ich];
      }
      if (i >= 2) {
        if (i == 3)
          us.nEphemYears = 1;
        else if (i == 4)
          us.nEphemYears = 5;
        SwitchF(us.fInDayMonth);
        if (i >= 3)
          us.fInDayYear = us.fInDayMonth;
      }
#ifdef TIME
      if (ch1 == 'n') {
        GetTimeNow(&MonT, &DayT, &YeaT, &TimT, DstT, ZonT);
        if (i >= 3)
          us.fInDayYear = us.fInDayMonth;
        break;
      }
#endif
      j = i < 2 ? 3 : (i == 2 ? 2 : 1);
      if (FErrorArgc("V", argc, j))
        return fFalse;
      if (i == 1) {
        MonT = NParseSz(argv[1], pmMon);
        DayT = NParseSz(argv[2], pmDay);
        YeaT = NParseSz(argv[3], pmYea);
      } else if (i == 2) {
        MonT = NParseSz(argv[1], pmMon);
        YeaT = NParseSz(argv[2], pmYea);
        DayT = 1;
      } else {
        YeaT = NParseSz(argv[1], pmYea);
        MonT = DayT = 1;
      }
      if (FErrorValN("V", !FValidMon(MonT), MonT, 1))
        return fFalse;
      if (FErrorValN("V", !FValidYea(YeaT), YeaT, j))
        return fFalse;
      if (FErrorValN("V", !FValidDay(DayT, MonT, YeaT), DayT, 2))
        return fFalse;
      argc -= j; argv += j;
      break;

#ifdef ARABIC
    case 'P':
      if (argc > 1 && (i = NFromSz(argv[1]))) {
        argc--; argv++;
        if (FErrorValN("P", !FValidPart(i), i, 0))
          return fFalse;
        us.nArabicParts = i;
      }
      if (ch1 == 'i' || ch1 == 'z' || ch1 == 'n' || ch1 == 'f') {
        us.nArabicSort = (ch1 != 'i' ? ch1 : 0);
        ch1 = ch2;
      }
      SwitchF(us.fArabic);
      if (ch1 == '0')
        SwitchF(us.fArabicFlip);
      break;
#endif

    case 'N':
      if (argc > 1 && (i = NFromSz(argv[1]))) {
        argc--; argv++;
        us.nAtlasList = i;
      }
      if (ch1 == 'z')
        SwitchF(us.fZoneChange);
      else if (ch1 == 'l')
        SwitchF(us.fAtlasNear);
      else
        SwitchF(us.fAtlasLook);
      break;

    case 'I':
      if (argc > 1 && (i = NFromSz(argv[1]))) {
        argc--; argv++;
        if (FErrorValN("I", !FValidScreen(i), i, 0))
          return fFalse;
        us.nScreenWidth = i;
      }
      SwitchF(us.fInterpret);
      break;

    // Switches which affect how the chart parameters are obtained:

#ifdef TIME
    case 'n':
      ci = ciCore;
      FInputData(szNowCore);
      if (ch1 == 'd')
        TT = 0.0;
      else if (ch1 == 'm') {
        DD = 1; TT = 0.0;
      } else if (ch1 == 'y') {
        MM = DD = 1; TT = 0.0;
      } else
        ch2 = ch1;
      if (FBetween(ch2, '1', '0' + cRing)) {
        *rgpci[ch2 - '0'] = ciCore;
        ciCore = ci;
      }
      break;
#endif

    case 'z':
      if (ch1 == '0') {
        if (argc <= 1 || RParseSz(argv[1], pmDst) == rLarge) {
          i = (ciDefa.dst != 0.0);
          SwitchF(i);
          SS = ciDefa.dst = (i ? 1.0 : 0.0);
        } else {
          rT = RParseSz(argv[1], pmDst);
          if (FErrorValR("z0", !FValidDst(rT), rT, 0))
            return fFalse;
          SS = ciDefa.dst = rT;
          argc--; argv++;
        }
        break;
      } else if (ch1 == 'l') {
        if (FErrorArgc("zl", argc, 2))
          return fFalse;
        rT = RParseSz(argv[1], pmLon);
        if (FErrorValR("zl", !FValidLon(rT), rT, 1))
          return fFalse;
        OO = ciDefa.lon = rT;
        rT = RParseSz(argv[2], pmLat);
        if (FErrorValR("zl", !FValidLat(rT), rT, 2))
          return fFalse;
        AA = ciDefa.lat = rT;
        argc -= 2; argv += 2;
        break;
      } else if (ch1 == 'v') {
        if (FErrorArgc("zv", argc, 1))
          return fFalse;
        us.elvDef = RParseSz(argv[1], pmElv);
        argc--; argv++;
        break;
      } else if (ch1 == 'f') {
        if (FErrorArgc("zf", argc, 1))
          return fFalse;
        us.tmpDef = RParseSz(argv[1], pmTmp);
        argc--; argv++;
        break;
      } else if (ch1 == 'j') {
        if (FErrorArgc("zj", argc, 2))
          return fFalse;
        ciDefa.nam = SzClone(argv[1]);
        ciDefa.loc = SzClone(argv[2]);
        argc -= 2; argv += 2;
        break;
      } else if (ch1 == 't') {
        if (FErrorArgc("zt", argc, 1))
          return fFalse;
        rT = RParseSz(argv[1], pmTim);
        if (FErrorValR("zt", !FValidTim(rT), rT, 0))
          return fFalse;
        TT = rT;
        argc--; argv++;
        break;
      } else if (ch1 == 'd') {
        if (FErrorArgc("zd", argc, 1))
          return fFalse;
        i = NParseSz(argv[1], pmDay);
        if (FErrorValN("zd", !FValidDay(i, MM, YY), i, 0))
          return fFalse;
        DD = i;
        argc--; argv++;
        break;
      } else if (ch1 == 'm') {
        if (FErrorArgc("zm", argc, 1))
          return fFalse;
        i = NParseSz(argv[1], pmMon);
        if (FErrorValN("zm", !FValidMon(i), i, 0))
          return fFalse;
        MM = i;
        argc--; argv++;
        break;
      } else if (ch1 == 'y') {
        if (FErrorArgc("zy", argc, 1))
          return fFalse;
        i = NParseSz(argv[1], pmYea);
        if (FErrorValN("zy", !FValidYea(i), i, 0))
          return fFalse;
        YY = i;
        argc--; argv++;
        break;
      } else if (ch1 == 'i') {
        if (FErrorArgc("zi", argc, 2))
          return fFalse;
        ciCore.nam = SzClone(argv[1]);
        ciCore.loc = SzClone(argv[2]);
        argc -= 2; argv += 2;
        break;
      }
#ifdef ATLAS
      else if (ch1 == 'L') {
        if (FErrorArgc("zL", argc, 1))
          return fFalse;
        if (!DisplayAtlasLookup(argv[1], 0, &i))
          PrintWarning("City doesn't match anything in atlas.");
        else {
          ciDefa.lon = OO; ciDefa.lat = AA;
        }
        argc--; argv++;
        break;
      } else if (ch1 == 'N') {
        if (FErrorArgc("zN", argc, 1))
          return fFalse;
        if (!DisplayAtlasLookup(argv[1], 0, &i))
          PrintWarning("City doesn't match anything in atlas.");
        else if (!DisplayTimezoneChanges(is.rgae[i].izn, 0, &ciCore))
          PrintWarning("Couldn't get time zone data!");
        else {
          ciDefa.dst = SS; ciDefa.zon = ZZ;
          ciDefa.lon = OO; ciDefa.lat = AA;
          is.fDst = (SS > 0.0);
        }
        argc--; argv++;
        break;
      }
#endif
      if (argc <= 1 || RParseSz(argv[1], pmZon) == rLarge)
        ZZ -= 1.0;
      else {
        rT = RParseSz(argv[1], pmZon);
        if (FErrorValR("z", !FValidZon(rT), rT, 0))
          return fFalse;
        ZZ = ciDefa.zon = rT;
        argc--; argv++;
      }
      break;

    case 'q':
      i = (ch1 == 'y' || ch1 == 'j' || ch1 == 'L') + 2*(ch1 == 'm') +
        3*(ch1 == 'd') + 7*(ch1 == 'a') + 8*(ch1 == 'b') + 10*(ch1 == 'c');
      if (i <= 0) {
        i = 4;
        ch2 = ch1;
      }
      if (FErrorArgc("q", argc, i))
        return fFalse;
      is.fHaveInfo = fTrue;
      ci = ciCore;
      if (ch1 == 'j') {
        is.JD = RFromSz(argv[1]) + rRound;
        TT = RFract(is.JD);
        JulianToMdy(is.JD - TT, &MM, &DD, &YY);
        TT *= 24.0;
        SS = ZZ = 0.0; OO = ciDefa.lon; AA = ciDefa.lat;
      } else if (ch1 == 'L') {
        j = NFromSz(argv[1]);
        if (FErrorValN("qL", !FValidList(j), j, 0))
          return fFalse;
        ciCore = is.rgci[j];
        is.iciCur = j;
      } else {
        MM = i > 1 ? NParseSz(argv[1], pmMon) : 1;
        DD = i > 2 ? NParseSz(argv[2], pmDay) : 1;
        YY = NParseSz(argv[3 - (i < 3) - (i < 2)], pmYea);
        TT = i > 3 ? RParseSz(argv[4], pmTim) : (i < 3 ? 0.0 : 12.0);
        SS = i > 7 ? RParseSz(argv[5], pmDst) : (i > 6 ? 0.0 : ciDefa.dst);
        ZZ = i > 6 ? RParseSz(argv[5 + (i > 7)], pmZon) : ciDefa.zon;
        OO = i > 6 ? RParseSz(argv[6 + (i > 7)], pmLon) : ciDefa.lon;
        AA = i > 6 ? RParseSz(argv[7 + (i > 7)], pmLat) : ciDefa.lat;
        if (FErrorValN("q", !FValidMon(MM), MM, 1))
          return fFalse;
        else if (FErrorValN("q", !FValidDay(DD, MM, YY), DD, 2))
          return fFalse;
        else if (FErrorValN("q", !FValidYea(YY), YY, 3 - (i < 3) - (i < 2)))
          return fFalse;
        else if (FErrorValR("q", !FValidTim(TT), TT, 4))
          return fFalse;
        else if (FErrorValR("q", !FValidDst(SS), SS, 5))
          return fFalse;
        else if (FErrorValR("q", !FValidZon(ZZ), ZZ, 5 + (i > 7)))
          return fFalse;
        else if (FErrorValR("q", !FValidLon(OO), OO, 6 + (i > 7)))
          return fFalse;
        else if (FErrorValR("q", !FValidLat(AA), AA, 7 + (i > 7)))
          return fFalse;
        if (i > 9) {
          ciCore.nam = SzClone(argv[9]);
          ciCore.loc = SzClone(argv[10]);
        }
      }
      if (FBetween(ch2, '1', '0' + cRing)) {
        *rgpci[ch2 - '0'] = ciCore;
        ciCore = ci;
      } else if (ch2 == 'D') {
        ciDefa = ciCore;
        ciCore = ci;
      } else if (ch2 == 't') {
        ciTran = ciCore;
        ciCore = ci;
      } else if (ch2 == 's') {
        ciSave = ciCore;
        ciCore = ci;
      } else if (ch2 == 'g') {
        ciGreg = ciCore;
        ciCore = ci;
      } else if (ch2 == 'l')
        FAppendCIList(&ciCore);
      argc -= i; argv += i;
      break;

    case 'i':
      if (ch1 == 'x') {
        SwapTemp(ciCore, ciTwin, ci);
        break;
      }
      if (us.fNoRead) {
        ErrorArgv("i");
        return tcError;
      }
      if (FErrorArgc("i", argc, 1))
        return fFalse;
      if (ch1 == 'd') {
        OpenDir(argv[1]);
        argc--; argv++;
        break;
      }
      ci = ciCore;
      if (!FInputData(argv[1]))
        return fFalse;
      if (ch1 == 'l' || ch2 == 'l')
        FAppendCIList(&ciCore);
      if (FBetween(ch1, '1', '0' + cRing)) {
        *rgpci[ch1 - '0'] = ciCore;
        ciCore = ci;
        *rgpcp[ch1 - '0'] = cp0;
      } else if (ch1 == 'D') {
        ciDefa = ciCore;
        ciCore = ci;
      } else if (ch1 == 't') {
        ciTran = ciCore;
        ciCore = ci;
        is.JDp = MdytszToJulian(MonT, DayT, YeaT, TimT,
          ciDefa.dst, ciDefa.zon);
      } else if (ch1 == 's') {
        ciSave = ciCore;
        ciCore = ci;
      } else if (ch1 == 'g') {
        ciGreg = ciCore;
        ciCore = ci;
      }
      argc--; argv++;
      break;

    case '>':
      ch1 = 's';
      // Fall through

    case 'o':
      if (us.fNoWrite) {
        ErrorArgv("o");
        return tcError;
      }
      if (FErrorArgc("o", argc, 1))
        return fFalse;
      if (ch1 == 's') {
        FCloneSz(argv[1], &is.szFileScreen);
        argc--; argv++;
        break;
      } else if (ch1 == '0' || ch1 == 'd' || ch1 == 'l' ||
        ch1 == 'a' || ch1 == 'q' || ch1 == 'x')
        us.nWriteFormat = FSwitchF2(us.nWriteFormat == ch1) * ch1;
      SwitchF(us.fWriteFile);
      FCloneSz(argv[1], &is.szFileOut);
      argc--; argv++;
      // Save extra lines to be output as comments to file
      if (is.rgszComment != NULL) {
        for (i = 0; i < is.cszComment; i++)
          DeallocatePIf(is.rgszComment[i]);
        DeallocateP(is.rgszComment);
        is.rgszComment = NULL;
      }
      for (is.cszComment = 0; argc - is.cszComment > 1 &&
        !FChSwitch(argv[1 + is.cszComment][0]); is.cszComment++)
        ;
      if (is.cszComment > 0) {
        is.rgszComment = RgAllocate(is.cszComment, char *, "comment list");
        ClearB((pbyte)is.rgszComment, is.cszComment * sizeof(char *));
        for (i = 0; i < is.cszComment; i++)
          FCloneSz(argv[1 + i], &is.rgszComment[i]);
        argc -= is.cszComment; argv += is.cszComment;
      }
      break;

    // Switches which affect what information is used in a chart:

    case 'R':
      if (ch1 == 'A') {
        if (ch2 == '0')
          for (i = 1; i <= cAspect; i++)
            ignorea[i] = fTrue;
        else if (ch2 == '1')
          for (i = 1; i <= cAspect; i++)
            ignorea[i] = fFalse;
        while (argc > 1 && (i = NParseSz(argv[1], pmAspect)))
          if (FErrorValN("RA", !FAspect(i), i, 0))
            return fFalse;
          else {
            SwitchF(ignorea[i]);
            argc--; argv++;
          }
        for (us.nAsp = cAspect; us.nAsp > 0 && ignorea[us.nAsp]; us.nAsp--)
          ;
        break;
      }
      if (ch1 == 'O') {
        if (FErrorArgc("RO", argc, 1))
          return fFalse;
        i = NParseSz(argv[1], pmObject);
        if (FErrorValN("RO", !FBetween(i, -1, cObj), i, 0))
          return fFalse;
        us.objRequire = i;
        argc--; argv++;
        break;
      }
      if (ch1 == 'T') {
        pch = (char *)ignore2;
        ch1 = argv[0][++ich];
      } else
        pch = (char *)ignore;
      if (ch1 == '0')
        for (i = 0; i <= cObj; i++)
          pch[i] = fTrue;
      else if (ch1 == '1')
        for (i = 0; i <= cObj; i++)
          pch[i] = fFalse;
      else if (ch1 == 'C')
        for (i = cuspLo; i <= cuspHi; i++)
          SwitchF(pch[i]);
      else if (ch1 == 'u' && ch2 == '0')      // Must be before Uranian check
        for (i = dwarfLo; i <= dwarfHi; i++)
          SwitchF(pch[i]);
      else if (ch1 == 'u')
        for (i = uranLo; i <= uranHi; i++)
          SwitchF(pch[i]);
      else if (ch1 == '8')
        for (i = moonsLo; i <= moonsHi; i++)
          SwitchF(pch[i]);
      else if (ch1 == 'b')
        for (i = cobLo; i <= cobHi; i++)
          SwitchF(pch[i]);
      else if (ch1 == 'U')
        for (i = starLo; i <= starHi; i++)
          SwitchF(pch[i]);
      else if (argc <= 1 || NParseSz(argv[1], pmObject) < 0)
        for (i = oChi; i <= oEP; i++)
          if (i != oNod)
            SwitchF(pch[i]);
      while (argc > 1 && (i = NParseSz(argv[1], pmObject)) >= 0)
        if (FErrorValN("R", !FItem(i), i, 0))
          return fFalse;
        else {
          if (ch1 != 'C' && ch1 != 'u' && ch1 != 'U')
            SwitchF(pch[i]);
          else
            inv(pch[i]);
          argc--; argv++;
        }
      RedoRestrictions();
      break;

    case 'C':
      j = us.fCusp;
      SwitchF(us.fCusp);
      if (j != us.fCusp)
        for (i = cuspLo; i <= cuspHi; i++)
          ignore[i] = j;
      AdjustRestrictions();
      break;

    case 'u':
      if (ch1 == '0') {
        j = us.fDwarf;
        SwitchF(us.fDwarf);
        if (j != us.fDwarf)
          for (i = dwarfLo; i <= dwarfHi; i++)
            ignore[i] = j;
      } else if (ch1 == '8') {
        j = us.fMoons;
        SwitchF(us.fMoons);
        if (j != us.fMoons)
          for (i = moonsLo; i <= moonsHi; i++)
            ignore[i] = j;
      } else if (ch1 == 'b') {
        j = us.fCOB;
        SwitchF(us.fCOB);
        if (j != us.fCOB)
          for (i = cobLo; i <= cobHi; i++)
            ignore[i] = j;
      } else {
        j = us.fUranian;
        SwitchF(us.fUranian);
        if (j != us.fUranian)
          for (i = uranLo; i <= uranHi; i++)
            ignore[i] = j;
      }
      AdjustRestrictions();
      break;

    case 'U':
      if (ch1 == 'x') {
        SwitchF(us.fExoTransit);
        if (ch2 == 'd' || ch2 == 'm' || ch2 == 'y' || ch2 == 'Y') {
          us.fParallel = fTrue;
          us.fInDayMonth = (ch2 != 'd');
          us.fInDayYear = us.fInDayMonth && (ch2 == 'y' || ch2 == 'Y');
          us.nEphemYears = (ch2 == 'Y' ? 5 : 1);
        }
        break;
      }
      j = us.fStar;
      if (ch1 == 'i' || ch1 == 'z' || ch1 == 'l' || ch1 == 'n' ||
        ch1 == 'b' || ch1 == 'd' || ch1 == 'v')
        us.nStarSort = (ch1 != 'i' ? ch1 : 0);
      SwitchF(us.fStar);
      if (j != us.fStar)
        for (i = starLo; i <= starHi; i++)
          ignore[i] = j;
      AdjustRestrictions();
      break;

    case 'A':
      if (ch1 == '3')
        SwitchF(us.fAspect3D);
      else if (ch1 == 'p')
        SwitchF(us.fAspectLat);
      else if (ch1 == 'P')
        SwitchF(us.fParallel2);
      else if (ch1 != 'o' && ch1 != 'm' && ch1 != 'd' && ch1 != 'a') {
        if (FErrorArgc("A", argc, 1))
          return fFalse;
        i = NParseSz(argv[1], pmAspect);
        if (FErrorValN("A", !FValidAspect(i), i, 0))
          return fFalse;
        for (j = us.nAsp + 1; j <= i; j++)
          ignorea[j] = fFalse;
        for (j = i + 1; j <= cAspect; j++)
          ignorea[j] = fTrue;
        us.nAsp = i;
        argc--; argv++;
      } else {
        if (FErrorArgc("A", argc, 2))
          return fFalse;
        i = NParseSz(argv[1], ch1 == 'o' || ch1 == 'a' ? pmAspect : pmObject);
        if (FErrorValN("A", i < (int)(ch1 == 'o' || ch1 == 'a') ||
          i > (ch1 == 'o' || ch1 == 'a' ? cAspect : oNorm+1), i, 1))
          return fFalse;
        rT = RParseSz(argv[2], 0);
        if (FErrorValR("A", rT < -rDegMax || rT > rDegMax, rT, 2))
          return fFalse;
        if (ch1 == 'o')
          rAspOrb[i] = rT;
        else if (ch1 == 'm')
          rObjOrb[i] = rT;
        else if (ch1 == 'd')
          rObjAdd[i] = rT;
        else
          rAspAngle[i] = rT;
        argc -= 2; argv += 2;
      }
      break;

    // Switches which affect how a chart is computed:

    case 'b':
      if (ch1 == '0') {
        SwitchF(us.fSeconds);
        break;
      } else if (ch1 == '1') {
        SwitchF(us.fSecond1K);
        break;
      } else if (ch1 == 'j')
        us.nSwissEph = FSwitchF(us.nSwissEph == 2) * 2;
      else if (ch1 == 's')
        us.nSwissEph = FSwitchF(us.nSwissEph == 1);
      else if (ch1 == 'p' && !us.fNoPlacalc)
        SwitchF(us.fPlacalcPla);
      else if (ch1 == 'm' && !us.fNoPlacalc)
        SwitchF(us.fMatrixPla);
      else if (ch1 == 'a')
        SwitchF(us.fPlacalcAst);
      else if (ch1 == 'U')
        SwitchF(us.fMatrixStar);
      else if (ch1 == 'J' && !us.fNoNetwork)
        us.nSwissEph = FSwitchF(us.nSwissEph == 3) * 3;
      SwitchF(us.fEphemFiles);
      break;

    case 'c':
      if (ch1 == '3') {
        if (argc > 1 && ((i = NFromSz(argv[1])) != 0 || FNumCh(argv[1][0]) ||
          argv[1][0] == '~')) {
          if (FErrorValN("c3", !FValidMethod(i), i, 0))
            return fFalse;
          argc--; argv++;
          if (i > 0)
            us.nHouse3D = i;
          else {
            us.fHouse3D = fFalse;
            break;
          }
        }
        SwitchF(us.fHouse3D);
        break;
      }
#ifdef WIN
      if (argc <= 1 && wi.fSaverCfg)
        return fFalse;
#endif
      if (FErrorArgc("c", argc, 1))
        return fFalse;
      i = NParseSz(argv[1], pmSystem);
      if (FErrorValN("c", !FValidSystem(i), i, 0))
        return fFalse;
      us.nHouseSystem = i;
      argc--; argv++;
      break;

    case 's':
      if (argc > 1 && (rT = RParseSz(argv[1], pmOffset)) != rLarge) {
        if (FErrorValR("s", !FValidOffset(rT), rT, 0))
          return fFalse;
        argc--; argv++;
        us.rZodiacOffset = rT;
      }
      if (ch1 == 'r') {
        if (ch2 != '0')
          SwitchF(us.fEquator);
        SwitchF(us.fEquator2);
      } else if (ch1 == 'z')
        us.nDegForm = dfZod;
      else if (ch1 == 'h')
        us.nDegForm = dfHM;
      else if (ch1 == 'd')
        us.nDegForm = df360;
      else if (ch1 == 'n')
        us.nDegForm = dfNak;
#ifdef WSETUP
      else if (ch1 == 'e')    // -setup switch for Windows
        i = FCreateProgramGroup(fFalse) && FCreateDesktopIcon() &&
          FRegisterExtensions();
#endif
      else
        SwitchF(us.fSidereal);
      break;

    case 'h':
      if (argc > 1 && (i = NParseSz(argv[1], pmObject)) >= 0) {
        argc--; argv++;
      } else
        i = FSwitchF(us.objCenter != 0);
      if (FErrorValN("h", !FValidCenter(i), i, 0))
        return fFalse;
      SetCentric(i);
      break;

    case 'p':
      us.nProgress = (ch1 == '0') + 2*(ch1 == '1');
      if (us.nProgress)
        ch1 = argv[0][++ich];
      if (fAnd) {
        us.fProgress = fFalse;
        break;
      } else if (ch1 == 'd') {
        if (FErrorArgc("pd", argc, 1))
          return fFalse;
        rT = RFromSz(argv[1]);
        if (FErrorValR("pd", rT == 0.0, rT, 0))
          return fFalse;
        us.rProgDay = rT;
        argc--; argv++;
        break;
      } else if (ch1 == 'C') {
        if (FErrorArgc("pC", argc, 1))
          return fFalse;
        rT = RFromSz(argv[1]);
        if (FErrorValR("pC", rT == 0.0, rT, 0))
          return fFalse;
        us.rProgCusp = rT;
        argc--; argv++;
        break;
      }
      SwitchF(us.fProgress);
#ifdef TIME
      if (ch1 == 'n') {
        GetTimeNow(&MonT, &DayT, &YeaT, &TimT, ciDefa.dst, ciDefa.zon);
        is.JDp = MdytszToJulian(MonT, DayT, YeaT, TimT,
          ciDefa.dst, ciDefa.zon);
        break;
      }
#endif
      i = 3 + (ch1 == 't');
      if (FErrorArgc("p", argc, i))
        return fFalse;
      MonT = NParseSz(argv[1], pmMon);
      DayT = NParseSz(argv[2], pmDay);
      YeaT = NParseSz(argv[3], pmYea);
      TimT = ch1 == 't' ? RParseSz(argv[4], pmTim) : 0.0;
      if (FErrorValN("p", !FValidMon(MonT), MonT, 1))
        return fFalse;
      else if (FErrorValN("p", !FValidDay(DayT, MonT, YeaT), DayT, 2))
        return fFalse;
      else if (FErrorValN("p", !FValidYea(YeaT), YeaT, 3))
        return fFalse;
      else if (ch1 == 't' && FErrorValR("p", !FValidTim(TimT), TimT, 4))
        return fFalse;
      is.JDp = MdytszToJulian(MonT, DayT, YeaT, TimT, ciDefa.dst, ciDefa.zon);
      argc -= i; argv += i;
      break;

    case 'x':
      if (FErrorArgc("x", argc, 1))
        return fFalse;
      i = ChCap(argv[1][0]) == 'D';
      rT = RFromSz(argv[1] + i);
      if (i != 0 && rT != 0.0)
        rT = rDegMax / rT;
      if (FErrorValR("x", !FValidHarmonic(rT), rT, 0))
        return fFalse;
      us.rHarmonic = rT;
      argc--; argv++;
      break;

    case '1':
      if (FErrorSubswitch("1", ch1, ch1 != chNull && ch1 != '0'))
        return fFalse;
      if (argc > 1 && (i = NParseSz(argv[1], pmObject)) >= 0) {
        argc--; argv++;
      } else
        i = oSun;
      if (FErrorValN("1", !FItem(i), i, 0))
        return fFalse;
      us.fSolarWhole = (ch1 == '0' && !fAnd);
      us.objOnAsc = fAnd ? 0 : i+1;
      break;

    case '2':
      if (FErrorSubswitch("2", ch1, ch1 != chNull && ch1 != '0'))
        return fFalse;
      if (argc > 1 && (i = NParseSz(argv[1], pmObject)) >= 0) {
        argc--; argv++;
      } else
        i = oSun;
      if (FErrorValN("2", !FItem(i), i, 0))
        return fFalse;
      us.fSolarWhole = (ch1 == '0' && !fAnd);
      us.objOnAsc = fAnd ? 0 : -(i+1);
      break;

    case '3':
      if (FErrorSubswitch("3", ch1, ch1 != chNull))
        return fFalse;
      SwitchF(us.fDecan);
      break;

    case '4':
      if (FErrorSubswitch("4", ch1, ch1 != chNull))
        return fFalse;
      if (argc > 1 && (i = NFromSz(argv[1])) >= 0) {
        argc--; argv++;
      } else
        i = 1;
      if (FErrorValN("4", !FValidDwad(i), i, 0))
        return fFalse;
      us.nDwad = fAnd ? 0 : i;
      break;

    case 'f':
      SwitchF(us.fFlip);
      break;

    case 'G':
      SwitchF(us.fGeodetic);
      break;

    case 'J':
      SwitchF(us.fIndian);
      break;

    case '9':
      if (FErrorSubswitch("9", ch1, ch1 != chNull))
        return fFalse;
      SwitchF(us.fNavamsa);
      break;

    case 'F':
      if (FErrorArgc("F", argc, !fAnd ? 3 : 1))
        return fFalse;
      i = NParseSz(argv[1], pmObject);
      if (FErrorValN("F", !FItem(i), i, 1))
        return fFalse;
      if (fAnd) {
        force[i] = 0.0;
        argc--; argv++;
        break;
      }
      if (ch1 != 'm') {
        force[i] = ZD(NParseSz(argv[2], pmSign), RFromSz(argv[3]));
        if (FErrorValR("F", force[i] < 0.0 || force[i] >= rDegMax,
          force[i], 0))
          return fFalse;
        force[i] += rDegMax;
      } else {
        j = NParseSz(argv[2], pmObject);
        if (FErrorValN("Fm", !FItem(j), j, 2))
          return fFalse;
        k = NParseSz(argv[3], pmObject);
        if (FErrorValN("Fm", !FItem(k), k, 3))
          return fFalse;
        force[i] = (real)-(j * cObj + k + 1);
      }
      AdjustRestrictions();
      argc -= 3; argv += 3;
      break;

    case chNull:
      if (ich <= 1)
        break;
      // Fall thorugh

    case '+':
    case '-':
      pch = &argv[0][ich-1];    // Save because argv++ changes it
      if (argc > 1 && ((rT = RFromSz(argv[1])) != 0.0 || FNumCh(argv[1][0]) ||
        argv[1][0] == '~')) {
        argc--; argv++;
      } else
        rT = 1.0;
      if (*pch != '+')
        neg(rT);
      i = (int)rT;
      if (ch1 == 't') {
        i /= 24;
        rT -= (real)(i*24);
        TT += rT;
        AddTime(&ciCore, 3, 0);
      } else if (ch1 == 'm') {
        AddTime(&ciCore, 5, i%12);
        AddTime(&ciCore, 6, i/12);
        break;
      } else if (ch1 == 'y') {
        AddTime(&ciCore, 6, i);
        break;
      }
      j = MdyToJulian(MM, DD + i, YY);
      JulianToMdy((real)j - 0.5, &MM, &DD, &YY);
      break;

    // Switches for relationship and comparison charts:

    case 'r':
      if (ch1 == 'P') {
        if (FErrorArgc("r", argc, 1))
          return fFalse;
        j = NFromSz(argv[1]);
        if (FErrorValN("rP", !FBetween(j, 2, cRing), j, 0))
          return fFalse;
        SwitchF(rgfProg[j]);
        argc--; argv++;
        break;
      }
      if (fAnd) {
        us.nRel = rcNone;
        break;
      } else if (FBetween(ch1, '1', '0' + cRing)) {
        us.nRel = -(int)(ch1-'1');
        break;
      }
      i = 2 + 2*((ch1 == 'c' || ch1 == 'm') && ch2 == '0');
      if (FErrorArgc("r", argc, i))
        return fFalse;
      if (ch1 == 'c')
        us.nRel = rcComposite;
      else if (ch1 == 'm')
        us.nRel = rcMidpoint;
      else if (ch1 == 'd')
        us.nRel = rcDifference;
#ifdef BIORHYTHM
      else if (ch1 == 'b')
        us.nRel = rcBiorhythm;
#endif
      else if (ch1 == '0')
        us.nRel = rcDual;
      else if (ch1 == 't')
        us.nRel = rcTransit;
      else if (ch1 == 'p') {
        us.nRel = rcProgress;
        us.nProgress = (ch2 == '0') + 2*(ch2 == '1');
      } else
        us.nRel = rcSynastry;
      ci = ciCore;
      ciCore = ciTwin;
      if (!FInputData(argv[2]))
        return fFalse;
      cp2 = cp0;
      ciTwin = ciCore;
      ciCore = ci;
      if (!FInputData(argv[1]))
        return fFalse;
      cp1 = cp0;
      if (i > 2) {
        us.nRatio1 = NFromSz(argv[3]);
        us.nRatio2 = NFromSz(argv[4]);
        if (us.nRatio1 == us.nRatio2)
          us.nRatio1 = us.nRatio2 = 1;
      }
      argc -= i; argv += i;
      break;

#ifdef TIME
    case 'y':
      if (FErrorArgc("y", argc, 1))
        return fFalse;
      if (ch1 == 'd')
        us.nRel = rcDifference;
#ifdef BIORHYTHM
      else if (ch1 == 'b')
        us.nRel = rcBiorhythm;
#endif
      else if (ch1 == 't')
        us.nRel = rcTransit;
      else if (ch1 == 'p') {
        us.nRel = rcProgress;
        us.nProgress = (ch2 == '0') + 2*(ch2 == '1');
      } else
        us.nRel = rcDual;
      if (!FInputData(szNowCore))
        return fFalse;
      ciTwin = ciCore;
      if (!FInputData(argv[1]))
        return fFalse;
      argc--; argv++;
      break;
#endif

    case '5':
      if (ch1 == 'e' || ch1 == 'Y') {
        i = 1 + (ch2 == '2') + (ch2 == '3')*2 + (ch2 == '4')*3;
        if (ch1 == 'e')
          us.nListAll = FSwitchF(us.nListAll == i) * i;
        else
          FEnumerateCIList(i);   // -5Y does the same as -Y5
      } else if (ch1 == 'd')
        FSortCIList(0);
      else if (ch1 == 'x')
        FSortCIList(1);
      else if (ch1 == 'y')
        FSortCIList(2);
      else if (ch1 == 'n')
        FSortCIList(3);
      else if (ch1 == 'l')
        FSortCIList(4);
      else if (ch1 == 's')
        FSortCIList(5);
      else if (ch1 == '0')
        is.cci = 0;
      else if (ch1 == 'f') {
        if (FErrorArgc("5f", argc, 2))
          return fFalse;
        FilterCIList(argv[1], argv[2]);
        argc -= 2; argv += 2;
        break;
      } else if (ch1 == chNull)
        SwitchF(us.fListAuto);
      else {
        FErrorSubswitch("5", ch1, fTrue);
        return fFalse;
      }
      break;

    // Switches to access graphics options:

    case 'k':
      if (ch1 == 'h') {
        SwitchF(us.fTextHTML);
        break;
      }
      if (ch1 == '1') {     // Undocumented subswitch.
        us.fAnsiColor = 2;
        us.fAnsiChar  = 1;
      } else {
        if (ch1 != '0')
          SwitchF(us.fAnsiColor);
        SwitchF(us.fAnsiChar);
      }
      break;

#ifdef GRAPH
    case 'X':
      if (us.fNoGraphics) {
        ErrorArgv("X");
        return fFalse;
      }
      i = NProcessSwitchesX(argc, argv, ich, fOr, fAnd, fNot);
      if (i < 0)
        return fFalse;
      SwitchF2(us.fGraphics);
      argc -= i; argv += i;
      break;

#ifdef WIN
    case 'W':
      i = NProcessSwitchesW(argc, argv, ich, fOr, fAnd, fNot);
      if (i < 0)
        return fFalse;
      argc -= i; argv += i;
      break;
#endif
#endif // GRAPH

    case '0':
      if (fAnd)    // _0 should do nothing.
        break;
      while (ch1 != chNull) {
        switch (ch1) {
        case 'o': us.fNoWrite    = fTrue; break;
        case 'i': us.fNoRead     = fTrue; break;
        case 'q': us.fNoQuit     = fTrue; break;
        case 'X': us.fNoGraphics = fTrue; break;
        case 'b': us.fNoPlacalc  = fTrue; break;
        case 'n': us.fNoNetwork  = fTrue; break;
        case '~': us.fNoExp      = fTrue; break;
        default: FErrorSubswitch("0", ch1, fTrue); return fFalse;
        }
        ch1 = argv[0][++ich];
      }
      break;

    case '?':    // Common command line usage does the same as -H.
      SwitchF(us.fSwitch);
      break;

    case ';':    // The -; switch means don't process the rest of the line.
      return fTrue;

    case '@':    // The -@ switch is just a system flag indicator no-op.
      break;

    case '.':                // "-." is usually used to exit the -Q loop.
      Terminate(tcForce);

#ifdef EXPRESS
    case '~':
      if (ch1 == '0') {
        SwitchF(us.fExpOff);
        break;
      }
      i = 1 + (ch1 == 'M' || ch1 == '2');
      if (FErrorArgc("~", argc, i))
        return fFalse;
      ppch = NULL;
      if (ch1 == 'g')
        ppch = &us.szExpConfig;
      else if (ch1 == 'a')
        ppch = (ch2 != '0' ? &us.szExpAspList : &us.szExpAspSumm);
      else if (ch1 == 'm')
        ppch = (ch2 != 'a' ? &us.szExpMid : &us.szExpMidAsp);
      else if (ch1 == 'j')
        ppch = (ch2 != '0' ? &us.szExpInf : &us.szExpInf0);
      else if (ch1 == '7')
        ppch = &us.szExpEso;
      else if (ch1 == 'L')
        ppch = &us.szExpCross;
      else if (ch1 == 'E')
        ppch = &us.szExpEph;
      else if (ch1 == 'Z' && ch2 == 'd')
        ppch = &us.szExpRis;
      else if (ch1 == 'd')
        ppch = (ch2 != 'v' ? &us.szExpDay : &us.szExpVoid);
      else if (ch1 == 't')
        ppch = &us.szExpTra;
      else if (ch1 == 'P')
        ppch = &us.szExpPart;
      else if (ch1 == 'O')
        ppch = &us.szExpObj;
      else if (ch1 == 'C')
        ppch = &us.szExpHou;
      else if (ch1 == 'A')
        ppch = &us.szExpAsp;
      else if (ch1 == 'p')
        ppch = (ch2 != '0' ? &us.szExpProg : &us.szExpProg0);
      else if (ch1 == 'k' && ch2 == 'O')
        ppch = &us.szExpColObj;
      else if (ch1 == 'k' && ch2 == 'A')
        ppch = &us.szExpColAsp;
      else if (ch1 == 'k' && ch2 == 'v')
        ppch = &us.szExpColFill;
      else if (ch1 == 'F' && ch2 == chNull)
        ppch = &us.szExpFontSig;
      else if (ch1 == 'F' && ch2 == 'C')
        ppch = &us.szExpFontHou;
      else if (ch1 == 'F' && ch2 == 'O')
        ppch = &us.szExpFontObj;
      else if (ch1 == 'F' && ch2 == 'A')
        ppch = &us.szExpFontAsp;
      else if (ch1 == 'F' && ch2 == 'N')
        ppch = &us.szExpFontNak;
      else if (ch1 == 'v')
        ppch = (ch2 != '3' ? &us.szExpSort : &us.szExpDecan);
      else if (ch1 == 's' && ch2 == 'd')
        ppch = &us.szExpDegree;
      else if (ch1 == 'U' && ch2 == 'x')
        ppch = &us.szExpExo;
      else if (ch1 == 'U')
        ppch = (ch2 != '0' ? &us.szExpStar : &us.szExpAst);
      else if (ch1 == 'I' && ch2 == 'v')
        ppch = &us.szExpIntV;
      else if (ch1 == 'I' && ch2 == 'V')
        ppch = &us.szExpIntV2;
      else if (ch1 == 'I' && ch2 == 'a')
        ppch = &us.szExpIntA;
      else if (ch1 == 'I' && ch2 == 'A')
        ppch = &us.szExpIntA2;
      else if (ch1 == 'X' && ch2 == 'L')
        ppch = &us.szExpCity;
      else if (ch1 == 'X' && ch2 == 't')
        ppch = &us.szExpSidebar;
      else if (ch1 == 'X' && ch2 == 'Q')
        ppch = &us.szExpKey;
      else if (ch1 == 'W' && ch2 == 'Q')
        ppch = &us.szExpMenu;
      else if (ch1 == 'q' && ch2 == '1')
        ppch = &us.szExpCast1;
      else if (ch1 == 'q' && ch2 == '2')
        ppch = &us.szExpCast2;
      else if (ch1 == 'Q' && ch2 == '1')
        ppch = &us.szExpDisp1;
      else if (ch1 == 'Q' && ch2 == '2')
        ppch = &us.szExpDisp2;
      else if (ch1 == 'Q' && ch2 == '3')
        ppch = &us.szExpDisp3;
      else if (ch1 == '5' && ch2 == 's')
        ppch = &us.szExpListS;
      else if (ch1 == '5' && ch2 == 'f')
        ppch = &us.szExpListF;
      else if (ch1 == '5' && ch2 == 'Y')
        ppch = &us.szExpListY;
      else if (ch1 == '5' && ch2 == 'i')
        ppch = &us.szExpADB;
      else if (ch1 == 'M') {
        j = NFromSz(argv[1]);
        if (FErrorValN("~M", j < 0, j, 1))
          return fFalse;
        ExpSetMacro(j, argv[2]);
      } else if (ch1 == '1')
        ParseExpression(argv[1]);
      else if (ch1 == '2') {
        j = NFromSz(argv[1]);
        if (FErrorValN("~2", j < 0, j, 1))
          return fFalse;
        ExpSetString(j, argv[2], ch2 == '0');
      } else if (FErrorSubswitch("~", ch1, ch1 != chNull))
        return fFalse;
      else
        ShowParseExpression(argv[1]);
      if (ppch != NULL)
        FCloneSz(argv[1], ppch);
      argc -= i; argv += i;
      break;
#endif

    default:
      ErrorSwitch(argv[0]);
      return fFalse;
    }
    argc--; argv++;
  }
  return fTrue;
}


/*
******************************************************************************
** Main Program.
******************************************************************************
*/

// Store or recall the current state of restrictions, as done with the -YRo
// and -YRi switches.

void InitRestrictions(flag fStore)
{
  if (fStore) {
    CopyRgb(ignore,  ignoreMem,  sizeof(ignore));
    CopyRgb(ignore2, ignore2Mem, sizeof(ignore2));
    CopyRgb(ignorea, ignoreaMem, sizeof(ignorea));
    CopyRgb(ignorez, ignorezMem, sizeof(ignorez));
    CopyRgb(ignore7, ignore7Mem, sizeof(ignore7));
    ignorefMem[0] = us.fIgnoreSign;   ignorefMem[1] = us.fIgnoreDir;
    ignorefMem[2] = us.fIgnoreDiralt; ignorefMem[3] = us.fIgnoreDirlen;
    ignorefMem[4] = us.fIgnoreAlt0;   ignorefMem[5] = us.fIgnoreDisequ;
  } else {
    CopyRgb(ignoreMem,  ignore,  sizeof(ignore));
    CopyRgb(ignore2Mem, ignore2, sizeof(ignore2));
    CopyRgb(ignoreaMem, ignorea, sizeof(ignorea));
    CopyRgb(ignorezMem, ignorez, sizeof(ignorez));
    CopyRgb(ignore7Mem, ignore7, sizeof(ignore7));
    us.fIgnoreSign   = ignorefMem[0]; us.fIgnoreDir    = ignorefMem[1];
    us.fIgnoreDiralt = ignorefMem[2]; us.fIgnoreDirlen = ignorefMem[3];
    us.fIgnoreAlt0   = ignorefMem[4]; us.fIgnoreDisequ = ignorefMem[5];
  }
}


// Initialize program variables and tables that aren't done so at compile
// time. Called once when the program starts from main() or WinMain().

void InitProgram()
{
#ifdef WIN
  char sz[cchSzMax], *pch;
#endif
  int i;

  Assert(starHi == cObj && cObj == objMax-1);
  SetCI(ciDefa, MM, DD, YY, TT, 0, DEFAULT_ZONE, DEFAULT_LONG, DEFAULT_LAT);
  is.S = stdout;
  ClearB((pbyte)szStarCustom, sizeof(szStarCustom));
  InitRestrictions(fTrue);
  for (i = 0; i < objMax; i++) {
    szObjDisp[i] = szObjName[i];
    rgobjList[i] = i;
  }
  for (i = 1; i <= cAspect2; i++) {
    szAspectDisp[i]       = szAspectName[i];
    szAspectAbbrevDisp[i] = szAspectAbbrev[i];
    szAspectGlyphDisp[i]  = szAspectGlyph[i];
  }
#ifdef INTERPRET
  for (i = 0; i < objMax; i++)
    szMindPart[i] = szMindPartDef[i];
  for (i = 0; i <= cSign; i++) {
    szDesc[i]     = szDescDef[i];
    szDesire[i]   = szDesireDef[i];
    szLifeArea[i] = szLifeAreaDef[i];
  }
  for (i = 0; i <= cAspect; i++) {
    szInteract[i]  = szInteractDef[i];
    szTherefore[i] = szThereforeDef[i];
  }
#endif
#ifdef SWISS
  for (i = 0; i < cCust; i++) {
    rgObjSwiss[i] = rgObjSwissDef[i];
    rgTypSwiss[i] = rgTypSwissDef[i];
  }
  ClearB((pbyte)rgPntSwiss, sizeof(rgPntSwiss));
  ClearB((pbyte)rgFlgSwiss, sizeof(rgFlgSwiss));
#endif
#ifdef GRAPH
  InitColorPalette(-1);
  for (i = 0; i < objMaxG; i++) {
    szDrawObject[i]  = szDrawObjectDef[i];
    szDrawObject2[i] = szDrawObjectDef2[i];
  }
  for (i = 1; i <= cAspect3; i++) {
    szDrawAspect[i]  = szDrawAspectDef[i];
    szDrawAspect2[i] = szDrawAspectDef2[i];
  }
#endif
#ifdef WIN
  GetModuleFileName(wi.hinst, sz, cchSzMax);
  for (pch = sz; *pch; pch++)
    ;
  if (pch - sz > 4 && FEqSz(pch - 4, ".scr"))
    wi.fSaverExt = fTrue;
  // Ensure _graphicschart enum aligns with rgcmdMode array.
  Assert(rgcmdMode[gWheel]      == cmdChartList);
  Assert(rgcmdMode[gHouse]      == cmdChartWheel);
  Assert(rgcmdMode[gGrid]       == cmdChartGrid);
  Assert(rgcmdMode[gMidpoint]   == cmdChartMidpoint);
  Assert(rgcmdMode[gHorizon]    == cmdChartHorizon);
  Assert(rgcmdMode[gOrbit]      == cmdChartOrbit);
  Assert(rgcmdMode[gSector]     == cmdChartSector);
  Assert(rgcmdMode[gCalendar]   == cmdChartCalendar);
  Assert(rgcmdMode[gDisposit]   == cmdChartInfluence);
  Assert(rgcmdMode[gEsoteric]   == cmdChartEsoteric);
  Assert(rgcmdMode[gAstroGraph] == cmdChartAstroGraph);
  Assert(rgcmdMode[gEphemeris]  == cmdChartEphemeris);
  Assert(rgcmdMode[gRising]     == cmdChartRising);
  Assert(rgcmdMode[gLocal]      == cmdChartLocal);
  Assert(rgcmdMode[gTraTraGra]  == cmdTransit);
  Assert(rgcmdMode[gTraNatGra]  == cmdTransit);
  Assert(rgcmdMode[gMoons]      == cmdChartMoons);
  Assert(rgcmdMode[gExo]        == cmdChartExo);
  Assert(rgcmdMode[gSphere]     == cmdChartSphere);
  Assert(rgcmdMode[gWorldMap]   == cmdChartMap);
  Assert(rgcmdMode[gGlobe]      == cmdChartGlobe);
  Assert(rgcmdMode[gPolar]      == cmdChartPolar);
  Assert(rgcmdMode[gTelescope]  == cmdChartTelescope);
  Assert(rgcmdMode[gBiorhythm]  == 0/*cmdRelBiorhythm*/);
  Assert(rgcmdMode[gAspect]     == cmdChartAspect);
  Assert(rgcmdMode[gArabic]     == cmdChartArabic);
  Assert(rgcmdMode[gTraTraTim]  == cmdTransit);
  Assert(rgcmdMode[gTraTraInf]  == cmdTransit);
  Assert(rgcmdMode[gTraNatTim]  == cmdTransit);
  Assert(rgcmdMode[gTraNatInf]  == cmdTransit);
  Assert(rgcmdMode[gSign]       == cmdHelpSign);
  Assert(rgcmdMode[gObject]     == cmdHelpObject);
  Assert(rgcmdMode[gHelpAsp]    == cmdHelpAspect);
  Assert(rgcmdMode[gConstel]    == cmdHelpConstellation);
  Assert(rgcmdMode[gPlanet]     == cmdHelpPlanetInfo);
  Assert(rgcmdMode[gRay]        == cmdHelpRay);
  Assert(rgcmdMode[gMeaning]    == cmdHelpMeaning);
  Assert(rgcmdMode[gSwitch]     == cmdHelpSwitch);
  Assert(rgcmdMode[gObscure]    == cmdHelpObscure);
  Assert(rgcmdMode[gKeystroke]  == cmdHelpKeystroke);
  Assert(rgcmdMode[gCredit]     == cmdHelpCredit);
#endif
}


// Program is about to exit, so free all memory that was allocated.

void FinalizeProgram(flag fSkip)
{
  char sz[cchSzDef];
  int i;

  DeallocatePIf(grid);
  for (i = 0; i < 10; i++)
    DeallocatePIf(us.rgszPath[i]);
  DeallocatePIf(us.szADB);
  DeallocatePIf(us.szStarsColor);
  DeallocatePIf(us.szAstColor);
  DeallocatePIf(us.szStarsList);
  DeallocatePIf(us.szExoList);
  DeallocatePIf(is.rgci);
  if (is.rgexod != NULL) {
    for (i = 0; i < is.cexod; i++)
      DeallocatePIf(is.rgexod[i].sz);
    DeallocateP(is.rgexod);
  }
  DeallocatePIf(is.szFileOut);
  DeallocatePIf(is.szFileScreen);
  for (i = 0; i < us.cSequenceLine; i++)
    DeallocatePIf(is.rgszLine[i]);
  if (is.rgszMacro != NULL) {
    for (i = 0; i < is.cszMacro; i++)
      DeallocatePIf(is.rgszMacro[i]);
    DeallocateP(is.rgszMacro);
  }
  if (is.rgszComment != NULL) {
    for (i = 0; i < is.cszComment; i++)
      DeallocatePIf(is.rgszComment[i]);
    DeallocateP(is.rgszComment);
  }
  for (i = 0; i < objMax; i++)
    if (szObjDisp[i] != szObjName[i])
      DeallocateP((char *)szObjDisp[i]);
  for (i = 1; i <= cAspect2; i++) {
    if (szAspectDisp[i] != szAspectName[i])
      DeallocateP((char *)szAspectDisp[i]);
    if (szAspectAbbrevDisp[i] != szAspectAbbrev[i])
      DeallocateP((char *)szAspectAbbrevDisp[i]);
    if (szAspectGlyphDisp[i] != szAspectGlyph[i])
      DeallocateP((char *)szAspectGlyphDisp[i]);
  }
  for (i = 1; i <= cStar; i++)
    DeallocatePIf(szStarCustom[i]);
  for (i = 0; i <= cRing; i++)
    DeallocatePIf(szWheel[i]);
#ifdef ATLAS
  DeallocatePIf(is.rgae);
  DeallocatePIf(is.rgzc);
  DeallocatePIf(is.rgrun);
  DeallocatePIf(is.rgrue);
  DeallocatePIf(is.rgzonCol);
#endif
#ifdef INTERPRET
  for (i = 0; i < objMax; i++)
    if (szMindPart[i] != szMindPartDef[i])
      DeallocateP((char *)szMindPart[i]);
  for (i = 0; i <= cSign; i++) {
    if (szDesc[i] != szDescDef[i])
      DeallocateP((char *)szDesc[i]);
    if (szDesire[i] != szDesireDef[i])
      DeallocateP((char *)szDesire[i]);
    if (szLifeArea[i] != szLifeArea[i])
      DeallocateP((char *)szLifeArea[i]);
  }
  for (i = 0; i <= cAspect; i++) {
    if (szInteract[i] != szInteractDef[i])
      DeallocateP((char *)szInteract[i]);
    if (szTherefore[i] != szThereforeDef[i])
      DeallocateP((char *)szTherefore[i]);
  }
#endif
#ifdef EXPRESS
  ExpFinalize();
  DeallocatePIf(us.szExpConfig);
  DeallocatePIf(us.szExpAspList);
  DeallocatePIf(us.szExpAspSumm);
  DeallocatePIf(us.szExpMid);
  DeallocatePIf(us.szExpMidAsp);
  DeallocatePIf(us.szExpInf);
  DeallocatePIf(us.szExpInf0);
  DeallocatePIf(us.szExpEso);
  DeallocatePIf(us.szExpCross);
  DeallocatePIf(us.szExpEph);
  DeallocatePIf(us.szExpRis);
  DeallocatePIf(us.szExpDay);
  DeallocatePIf(us.szExpVoid);
  DeallocatePIf(us.szExpTra);
  DeallocatePIf(us.szExpPart);
  DeallocatePIf(us.szExpObj);
  DeallocatePIf(us.szExpHou);
  DeallocatePIf(us.szExpAsp);
  DeallocatePIf(us.szExpProg);
  DeallocatePIf(us.szExpProg0);
  DeallocatePIf(us.szExpColObj);
  DeallocatePIf(us.szExpColAsp);
  DeallocatePIf(us.szExpColFill);
  DeallocatePIf(us.szExpFontSig);
  DeallocatePIf(us.szExpFontHou);
  DeallocatePIf(us.szExpFontObj);
  DeallocatePIf(us.szExpFontAsp);
  DeallocatePIf(us.szExpFontNak);
  DeallocatePIf(us.szExpSort);
  DeallocatePIf(us.szExpDecan);
  DeallocatePIf(us.szExpDegree);
  DeallocatePIf(us.szExpStar);
  DeallocatePIf(us.szExpAst);
  DeallocatePIf(us.szExpExo);
  DeallocatePIf(us.szExpIntV);
  DeallocatePIf(us.szExpIntV2);
  DeallocatePIf(us.szExpIntA);
  DeallocatePIf(us.szExpIntA2);
  DeallocatePIf(us.szExpCity);
  DeallocatePIf(us.szExpSidebar);
  DeallocatePIf(us.szExpKey);
  DeallocatePIf(us.szExpMenu);
  DeallocatePIf(us.szExpCast1);
  DeallocatePIf(us.szExpCast2);
  DeallocatePIf(us.szExpDisp1);
  DeallocatePIf(us.szExpDisp2);
  DeallocatePIf(us.szExpDisp3);
  DeallocatePIf(us.szExpListS);
  DeallocatePIf(us.szExpListF);
  DeallocatePIf(us.szExpListY);
  DeallocatePIf(us.szExpADB);
#endif
#ifdef GRAPH
  DeallocatePIf(gi.bm);
  DeallocatePIf(gi.bmp.rgb);
  DeallocatePIf(gi.bmpBack.rgb);
  DeallocatePIf(gi.bmpBack2.rgb);
  DeallocatePIf(gi.bmpWorld.rgb);
  DeallocatePIf(gi.bmpRising.rgb);
  DeallocatePIf(gi.rgspace);
  DeallocatePIf(gi.rgConstel);
  DeallocatePIf(gi.szFileOut);
  DeallocatePIf(gs.szSidebar);
  for (i = 0; i <= cRing; i++)
    DeallocatePIf(szWheelX[i]);
  for (i = 0; i < objMax; i++) {
    if (szDrawObject[i] != szDrawObjectDef[i])
      DeallocateP((char *)szDrawObject[i]);
    if (szDrawObject2[i] != szDrawObjectDef2[i])
      DeallocateP((char *)szDrawObject2[i]);
  }
  for (i = 1; i <= cAspect2; i++) {
    if (szDrawAspect[i] != szDrawAspectDef[i])
      DeallocateP((char *)szDrawAspect[i]);
    if (szDrawAspect2[i] != szDrawAspectDef2[i])
      DeallocateP((char *)szDrawAspect2[i]);
  }
#ifdef SWISS
  DeallocatePIf(gi.rges);
  DeallocatePIf(gs.szStarsLin);
  DeallocatePIf(gs.szStarsLnk);
  DeallocatePIf(is.rgesSort);
#endif
#endif // GRAPH
#ifdef X11
  DeallocatePIf(gs.szDisplay);
#endif
#ifdef WINANY
  DeallocatePIf(wi.bmpWin.rgb);
#endif
#ifdef WIN
  if (ofn.lpstrFile != szFileName)
    DeallocatePIf(ofn.lpstrFile);
#endif
  if (fSkip)
    return;
  if (is.cAlloc != 0) {
    sprintf(sz, "Number of memory allocations not freed before exiting: %d",
      is.cAlloc);
    PrintWarning(sz);
  }
#ifdef DEBUG
  else if (is.cbAllocSize != 0) {
    sprintf(sz, "Number of memory bytes not freed before exiting: %d",
      is.cbAllocSize);
    PrintWarning(sz);
  }
#endif
}


#ifndef WIN
// The main program, the starting point for Astrolog, follows. This routine
// basically consists of a loop, inside which we read a command line, and go
// process it, before actually calling a routine to display astrology.

#ifdef SWITCHES
int main(int argc, char **argv)
{
#else
int main()
{
  int argc;
  char **argv;
#endif
  char szCommandLine[cchSzMax], *rgsz[MAXSWITCHES];
#ifdef BETA
  char szBeta[cchSzMax];
#endif

  // Read in info from the astrolog.as file.
  InitProgram();
#ifdef SWITCHES
  is.szProgName = argv[0];
#endif
  FProcessSwitchFile(DEFAULT_INFOFILE, NULL);
  ciTran = ciHexa = ciFive = ciFour = ciThre = ciTwin = ciMain = ciCore;
#ifdef BETA
  sprintf(szBeta, "This is a beta version of %s %s! "
    "That means changes are still being made and testing is not complete. "
    "If this is being run after %s %d, %d, "
    "it should be replaced with the finished release.\n\n",
    szAppName, szVersionCore, szMonth[ciSave.mon], ciSave.day, ciSave.yea);
  FieldWord(szBeta);
#endif

LBegin:
  if (us.fNoSwitches) {                             // Go prompt for switches
    argc = NPromptSwitches(szCommandLine, rgsz);    // if don't have them.
    argv = rgsz;
  }
  is.szProgName = argv[0];
  if (FProcessSwitches(argc, argv)) {
    if (!us.fNoSwitches && us.fLoopInit) {
      us.fNoSwitches = fTrue;
      goto LBegin;
    }
    Action();
  }
  if (us.fLoop || us.fNoQuit) {  // If -Q in effect loop back and get switch
    PrintL2();                   // info for another chart to display.
    InitVariables();
    us.fLoop = us.fNoSwitches = fTrue;
    goto LBegin;
  }
  Terminate(tcOK);    // The only standard place to exit Astrolog is here.
  return 0;
}
#endif // WIN

/* astrolog.cpp */
