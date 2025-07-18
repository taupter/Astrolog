/*
** Astrolog (Version 7.80) File: general.cpp
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
** General Procedures.
******************************************************************************
*/

// Swap two floating point values.

void SwapR(real *d1, real *d2)
{
  real temp;

  temp = *d1; *d1 = *d2; *d2 = temp;
}


// Return the length of a string (not counting the null terminator).

int CchSz(CONST char *sz)
{
  CONST char *pch = sz;

  while (*pch)
    pch++;
  return (int)(pch - sz);
}


// Return the length of a string (not counting the null terminator). Like
// CchSz() but treat UTF8 byte sequences as a single character when necessary.

int CwchSz(CONST char *sz)
{
  CONST char *pch;
  int cwch, dwch;

  if (us.nCharset < ccUTF8)
    return CchSz(sz);
  cwch = 0;
  for (pch = sz; *pch; pch += dwch, cwch++)
    dwch = UTF8ToWch((uchar *)pch, NULL);
  return cwch;
}


// Compare two strings case sensitively. Return 0 if equal, negative number if
// first less than second, and positive number if first greater than second.

int NCompareSz(CONST char *sz1, CONST char *sz2)
{
  if (sz1 == NULL || sz2 == NULL)
    return (sz1 == sz2);
  while (*sz1 && *sz1 == *sz2)
    sz1++, sz2++;
  return (int)*sz1 - *sz2;
}


// Compare two strings case insensitively. Return 0 if equal, negative number
// if first less than second, and positive if first greater than second.

int NCompareSzI(CONST char *sz1, CONST char *sz2)
{
  if (sz1 == NULL || sz2 == NULL)
    return (sz1 == sz2);
  while (*sz1 && ChCap(*sz1) == ChCap(*sz2))
    sz1++, sz2++;
  return (int)ChCap(*sz1) - ChCap(*sz2);
}


// Return whether two ranges of characters are equal. Either string ending
// prematurely with a zero terminator makes the strings not equal.

flag FEqRgch(CONST char *rgch1, CONST char *rgch2, int cch, flag fInsensitive)
{
  int ich;

  if (!fInsensitive) {
    for (ich = 0; ich < cch; ich++) {
      if (rgch1[ich] == chNull || rgch1[ich] != rgch2[ich])
        return fFalse;
    }
  } else {
    for (ich = 0; ich < cch; ich++) {
      if (rgch1[ich] == chNull || ChCap(rgch1[ich]) != ChCap(rgch2[ich]))
        return fFalse;
    }
  }
  return fTrue;
}


// Return whether the first string matches the second, case insensitively.
// The first string may be truncated, but the first three chars must match.

flag FMatchSz(CONST char *sz1, CONST char *sz2)
{
  CONST char *szStart = sz1;

  while (*sz1 && ChCap(*sz1) == ChCap(*sz2))
    sz1++, sz2++;
  return *sz1 == chNull && (*sz2 == chNull || sz1 - szStart >= 3);
}


// Return whether the first string matches any string in the second, case
// sensitively. The second string is subdivided by comma or semicolon
// characters. Return offset into string, and optionally the index into list.

CONST char *SzInList(CONST char *sz1, CONST char *sz2, int *pisz)
{
  CONST char *szStart = sz1;
  int isz = 0;

  loop {
    // Compate string to current string in string list.
    for (sz1 = szStart; *sz1 && *sz1 == *sz2; sz1++, sz2++)
      ;
    if (*sz2 == chNull || (*sz2 == chSep || *sz2 == chSep2)) {
      if (*sz1 == chNull) {
        // Match if reached end of string and of current string in list.
        if (pisz != NULL)
          *pisz = isz;
        return sz2 + (*sz2 == chSep || *sz2 == chSep2);
      }
    } else {
      // Skip ahead to start of next string in string list.
      while (*sz2 && !(*sz2 == chSep || *sz2 == chSep2))
        sz2++;
    }
    if (*sz2 == chSep || *sz2 == chSep2)
      sz2++;
    else
      break;    // If no separator, then end of string list reached.
    isz++;
  }
  if (pisz != NULL)
    *pisz = -1;
  return NULL;
}


// Set a given number of bytes to zero given a starting pointer.

void ClearB(pbyte pb, int cb)
{
  while (cb-- > 0)
    *pb++ = 0;
}


// Copy a given number of bytes from one location to another.

void CopyRgb(CONST byte *pbSrc, byte *pbDst, int cb)
{
  while (cb-- > 0)
    *pbDst++ = *pbSrc++;
}


// Copy a range of characters and zero terminate it. If there are too many
// characters to fit in the destination buffer, the string is truncated.

void CopyRgchToSz(CONST char *pch, int cch, char *sz, int cchMax)
{
  cch = Min(cch, cchMax-1);
  CopyRgb((pbyte)pch, (pbyte)sz, cch);
  sz[cch] = chNull;
}


// Determine the sign of a number: -1 if value negative, +1 if value positive,
// and 0 if it's zero.

real RSgn(real r)
{
  return r == 0.0 ? 0.0 : RSgn2(r);
}


// Given an x and y coordinate, return the angle formed by a line from the
// origin to this coordinate. This is just converting from rectangular to
// polar coordinates, however this doesn't involve the radius here.

real RAngle(real x, real y)
{
  real a;

  if (x != 0.0) {
    if (y != 0.0)
      a = RAtn(y/x);
    else
      a = x < 0.0 ? rPi : 0.0;
  } else
    a = y < 0.0 ? -rPiHalf : rPiHalf;
  if (a < 0.0)
    a += rPi;
  if (y < 0.0)
    a += rPi;
  return a;
}


// Like RAngle() but return the angle between two 3D vectors instead.

real VAngle(CONST PT3R *v1, CONST PT3R *v2)
{
  real angle, len1, len2;

  len1 = PtLen((*v1));
  len2 = PtLen((*v2));
  if (len1 != 0.0 && len2 != 0.0) {
    angle = PtDot((*v1), (*v2))/len1/len2;
    if (angle == 0.0)
      return rPiHalf;
    else if (angle <= -1.0)
      return rPi;
    angle = RAtn(RSqr(1.0 - Sq(angle)) / angle);
    if (angle >= 0.0)
      return angle;
    else
      return angle + rPi;
  } else
    return rPiHalf;
}


// Modulus function for floating point values, in which we bring the given
// parameter to within the range of 0 to 360.

real Mod(real d)
{
  if (d >= rDegMax)         // In most cases, value is only slightly
    d -= rDegMax;           // out of range, so can test for it and
  else if (d < 0.0)         // avoid the more complicated arithmetic.
    d += rDegMax;
  if (d >= 0 && d < rDegMax)
    return d;
  return (d - RFloor(d/rDegMax)*rDegMax);
}


// Integer division, like the "/" operator but always rounds result down.

long Dvd(long x, long y)
{
  long z;

  if (y == 0)
    return x;
  z = x / y;
  if (((x >= 0) == (y >= 0)) || x-z*y == 0)
    return z;
  return z - 1;
}


// Lookup a string within a table (case insensitively) returning the index
// that goes with the matched string, or -1 if the string is not found.

int SzLookup(CONST StrLook *rgStrLook, CONST char *sz)
{
  CONST char *pch1, *pch2;
  int irg;

  for (irg = 0; rgStrLook[irg].isz >= 0; irg++) {
    for (pch1 = sz, pch2 = rgStrLook[irg].sz;
      *pch1 && ChCap(*pch1) == ChCap(*pch2); pch1++, pch2++)
      ;
    if (*pch1 == chNull && (*pch2 == chNull || pch1 - sz >= 3))
      return rgStrLook[irg].isz;
  }
  return -1;
}


// Return whether a zero terminated string is a substring of another string,
// case insensitively.

flag FEqSzSubI(CONST char *sz1, CONST char *sz2)
{
  while (*sz1 && ChCap(*sz1) == ChCap(*sz2))
    sz1++, sz2++;
  return *sz1 == chNull;
}


// Set a string to a floating point value, with at most 'n' significant
// fractional digits, and dropping trailing '0' characters.

void FormatR(char *sz, real r, int n)
{
  char szT[cchSzDef], *pch;
  int x = n/100, y = NAbs(n%100);

  // If n > 100, use 100's place as minimum field length for entire number.
  if (x != 0)
    sprintf(szT, "%%%d.%df", NAbs(x) + y + 1, y);
  else
    sprintf(szT, "%%.%df", NAbs(n));
  sprintf(sz, szT, r);
  for (pch = sz; *pch; pch++)
    ;
  while (pch > sz && *(--pch) == '0')    // Drop off any trailing 0 digits.
    ;
  // Positive n means ensure at least one fractional digit.
  pch[n > 0 ? 1 + (*pch == '.') : (*pch != '.')] = chNull;
}


// Blend two RGB colors along the specified proportion between them. Returned
// color ranges from the first color (ratio = 0) to the second (ratio = 1).

KV KvBlend(KV kv1, KV kv2, real rRatio)
{
  return Rgb((int)((real)(RgbR(kv2) - RgbR(kv1)) * rRatio) + RgbR(kv1),
    (int)((real)(RgbG(kv2) - RgbG(kv1)) * rRatio) + RgbG(kv1),
    (int)((real)(RgbB(kv2) - RgbB(kv1)) * rRatio) + RgbB(kv1));
}


#define rHueMax  rDegMax
#define rHueHalf rDegHalf
#define rHue13   120.0
#define rHue23   240.0
#define rHue16   60.0

// Return a RGB color of the rainbow given a number 0-360, in which 0 is red,
// 120 is green, and 240 is blue.

KV KvHue(real deg)
{
  int nR, nG, nB;
  real rDiff;

  while (deg >= rHueMax)
    deg -= rHueMax;
  while (deg < 0.0)
    deg += rHueMax;
  rDiff = RAbs(deg - rHueHalf);
  if (rDiff > rHue13)
    nR = 255;
  else if (rDiff < rHue16)
    nR = 0;
  else
    nR = NMultDiv((int)(rDiff - rHue16), 255, rHue16);
  rDiff = RAbs(deg - rHue13);
  if (rDiff < rHue16)
    nG = 255;
  else if (rDiff > rHue13)
    nG = 0;
  else
    nG = NMultDiv((int)(rHue13 - rDiff), 255, rHue16);
  rDiff = RAbs(deg - rHue23);
  if (rDiff < rHue16)
    nB = 255;
  else if (rDiff > rHue13)
    nB = 0;
  else
    nB = NMultDiv((int)(rHue13 - rDiff), 255, rHue16);
  return Rgb(nR, nG, nB);
}


CONST int rgnHue2[cSign+1] =
  {0, 15, 30, 45, 60, 75, 120, 180, 210, 240, 270, 315, 360};

// Return a RGB color of the rainbow given a number 0-360, in which 0 is red,
// 120 is yellow, and 240 is blue. This aligns with paint based RYB primary
// colors, instead of light based RGB colors as in KvHue().

KV KvHue2(real deg)
{
  int sig;

  while (deg >= rHueMax)
    deg -= rHueMax;
  while (deg < 0.0)
    deg += rHueMax;
  sig = SFromZ(deg)-1;
  deg = (real)rgnHue2[sig] +
    (deg - (real)(sig*30)) / 30.0 * (real)(rgnHue2[sig+1] - rgnHue2[sig]);
  return KvHue(deg);
}


/*
******************************************************************************
** General Astrology Procedures.
******************************************************************************
*/

// A similar modulus function: Convert an integer to value from 1-12.

int Mod12(int i)
{
  while (i > cSign)
    i -= cSign;
  while (i < 1)
    i += cSign;
  return i;
}


// Convert an inputed fractional degrees/minutes value to a true decimal
// degree quantity. For example, the user enters the decimal value "10.30"
// to mean 10 degrees and 30 minutes; this will return 10.5, i.e. 10 degrees
// and 30 minutes expressed as a floating point degree value.

real DecToDeg(real d)
{
  return RSgn(d)*(RFloor(RAbs(d))+RFract(RAbs(d))*100.0/60.0);
}


// This is the inverse of the above function. Given a true decimal value for
// a zodiac degree, adjust it so the degrees are in the integer part and the
// minute expressed as hundredths, e.g. 10.5 degrees -> 10.30

real DegToDec(real d)
{
  return RSgn(d)*(RFloor(RAbs(d))+RFract(RAbs(d))*60.0/100.0);
}


// Return the shortest distance between two degrees in the zodiac. This is
// normally their difference, but we have to check if near the Aries point.

real MinDistance(real deg1, real deg2)
{
  real r;

  r = RAbs(deg1-deg2);
  return r <= rDegHalf ? r : rDegMax - r;
}


// This is just like the above routine, except the min distance value returned
// will either be positive or negative based on whether the second value is
// ahead or behind the first one in a circular zodiac.

real MinDifference(real deg1, real deg2)
{
  real r;

  r = deg2 - deg1;
  if (RAbs(r) < rDegHalf)
    return r;
  return r >= 0 ? r - rDegMax : r + rDegMax;
}


// Return the degree of the midpoint between two zodiac positions, making sure
// we return the true midpoint closest to the positions in question.

real Midpoint(real deg1, real deg2)
{
  real mid;

  mid = (deg1+deg2)/2.0;
  return MinDistance(deg1, mid) < rDegQuad ? mid : Mod(mid+rDegHalf);
}


// Return the minimum great circle distance between two sets of spherical
// coordinates. This is like MinDistance() but takes latitude into account.

real SphDistance(real lon1, real lat1, real lon2, real lat2)
{
  real dLon, r;

  dLon = RAbs(lon1 - lon2);
  r = RAcosD(RSinD(lat1)*RSinD(lat2) + RCosD(lat1)*RCosD(lat2)*RCosD(dLon));
  return r;
}


// Given two pairs of coordinates on a sphere, return coordinates at some
// proportion (0.0-1.0) along the great circle path between them.

void SphRatio(real lon1, real lat1, real lon2, real lat2, real rRatio,
  real *lon, real *lat)
{
  real x1, y1, z1, x2, y2, z2, x, y, z, len, ang, adj, ang2;

  SphToRec(1.0, lon1, lat1, &x1, &y1, &z1);
  SphToRec(1.0, lon2, lat2, &x2, &y2, &z2);
  if (rRatio != 0.5) {
    // Bisecting an arc is easy, however other proportions require extra math.
    len = RLength3(x2 - x1, y2 - y1, z2 - z1) / 2.0;
    ang = RAsinD(len);
    adj = 1.0 / RTanD(ang);
    rRatio = (rRatio - 0.5) / 0.5;
    ang2 = rRatio * ang;
    rRatio = adj * RTanD(ang2);
    rRatio = (rRatio / 2.0) + 0.5;
  }
  x = x1 + (x2 - x1) * rRatio;
  y = y1 + (y2 - y1) * rRatio;
  z = z1 + (z2 - z1) * rRatio;
  RecToSph3(x, y, z, lon, lat);
}


// Given a planet and sign, determine whether: The planet rules the sign or
// is in detriment in the sign, the planet exalts in sign or is in fall /
// debilitated in sign, the planet esoterically and hierarchically and ray
// rules or is in detriment in the sign, and return an appropriate string.

char *Dignify(int obj, int sign)
{
  static char szDignify[7];
  int sign2 = Mod12(sign+6), ray, ich;

  sprintf(szDignify, "-_____");
  if (obj > oNorm)
    goto LExit;

  // Check standard rulerships.
  if (!ignore7[rrStd]) {
    if (ruler1[obj] == sign || ruler2[obj] == sign)
      szDignify[rrStd+1] = 'R';
    else if (ruler1[obj] == sign2 || ruler2[obj] == sign2)
      szDignify[rrStd+1] = 'd';
  }
  if (!ignore7[rrExa]) {
    if (exalt[obj] == sign)
      szDignify[rrExa+1] = 'X';
    else if (exalt[obj] == sign2)
      szDignify[rrExa+1] = 'f';
  }

  // Check esoteric rulerships.
  if (!ignore7[rrEso]) {
    if (rgObjEso1[obj] == sign || rgObjEso2[obj] == sign)
      szDignify[rrEso+1] = 'S';
    else if (rgObjEso1[obj] == sign2 || rgObjEso2[obj] == sign2)
      szDignify[rrEso+1] = 's';
  }
  if (!ignore7[rrHie]) {
    if (rgObjHie1[obj] == sign || rgObjHie2[obj] == sign)
      szDignify[rrHie+1] = 'H';
    else if (rgObjHie1[obj] == sign2 || rgObjHie2[obj] == sign2)
      szDignify[rrHie+1] = 'h';
  }
  if (!ignore7[rrRay]) {
    ray = rgObjRay[obj];
    if (ray > 0) {
      if (rgSignRay2[sign][ray] > 0)
        szDignify[rrRay+1] = 'Y';
      else if (rgSignRay2[sign2][ray] > 0)
        szDignify[rrRay+1] = 'z';
    }
  }

LExit:
  // Put "most significant" rulership state present in the first character.
  // Order: Standard rulership, exaltation, esoteric, Hierarchical, Ray.
  for (ich = 1; ich <= rrMax; ich += (ich == 1 ? 3 :
    (ich == 4 ? -2 : (ich == 3 ? 2 : 1)))) {
    if (szDignify[ich] != '_') {
      szDignify[0] = szDignify[ich];
      break;
    }
  }
  return szDignify;
}


// Process the list of each sign's rays, creating a grid based on it
// indicating whether each ray applies to a sign, and its proportion.

void EnsureRay()
{
  int i, j, c, n;

  for (i = 1; i <= cSign; i++) {
    for (j = 1; j <= cRay; j++)
      rgSignRay2[i][j] = 0;
    c = 0;
    n = rgSignRay[i];
    while (n) {
      j = n % 10;
      n /= 10;
      if (!FBetween(j, 1, cRay))
        continue;
      rgSignRay2[i][j] = 1;
      c++;
    }
    for (j = 1; j <= cRay; j++)
      rgSignRay2[i][j] *= 420 / c;
  }
}


// Initialize table of star brightnesses. Usually only called once before
// first star accessed, but may be redone if computation method changes.

void EnsureStarBright()
{
  int i;
  real rMode;

  rMode = FCmSwissStar() ? 1.0 : 0.0;
  if (rStarBrightDef[0] != rMode) {
    rStarBrightDef[0] = rMode;

    // Matrix formulas have star brightnesses in a simple table.
    for (i = 1; i <= cStar; i++) {
#ifdef MATRIX
      rStarBrightDef[i] = rStarBrightMatrix[i];
#else
      rStarBrightDef[i] = 1.0;
#endif
      rStarBright[i] = rStarBrightDef[i];
      // No distance data, so assume each star is 100 LY away.
      rStarBrightDistDef[i] = cp0.dist[oNorm+i] = 100.0 * rLYToAU;
    }

#ifdef SWISS
    // Swiss Ephemeris reads star brightnesses from an external file.
    if (FCmSwissStar())
      SwissComputeStars(0.0, fTrue);
#endif
  }
}


// Determine the number of days in a particular month. The year is needed too,
// because have to check for leap years in the case of February.

int DayInMonth(int month, int year)
{
  int d;

  if (month == mSep || month == mApr || month == mJun || month == mNov)
    d = 30;
  else if (month != mFeb)
    d = 31;
  else {
    d = 28;
    if (year % 4 == 0 &&
      (year % 100 != 0 || year % 400 == 0 || year <= yeaJ2G))
      d++;
  }
  return d;
}


// Return the actual number of days in a particular month. Normally, this is
// the same as the above routine which determines the index of the last day of
// the month, but the values can differ when changing between calendar systems
// (Julian to Gregorian) in which a month can skip over days.

int DaysInMonth(int month, int year)
{
  int d;

  d = DayInMonth(month, year);
  if (ciGreg.yea == yeaJ2G && ciGreg.mon == monJ2G && ciGreg.day == dayJ2G2 &&
    year == yeaJ2G && month == monJ2G)
    d -= (dayJ2G2 - dayJ2G1 - 1);
  return d;
}


// Return the day of the week (Sunday is 0) of the specified given date.

int DayOfWeek(int month, int day, int year)
{
  int d;

  d = (MdyToJulian(month, day, year) + 1) % 7;
  return d < 0 ? d+7 : d;
}


// Given a day, and the month and year it falls in, add a number of days to
// it and return the new day index. As month changes are not checked for here,
// this is mostly just adding the offset to the day, however need to check for
// calendar changes for when days in a month may be skipped.

int AddDay(int month, int day, int year, int delta)
{
  int d;

  d = day + delta;
  if (ciGreg.yea == yeaJ2G && ciGreg.mon == monJ2G && ciGreg.day == dayJ2G2 &&
    year == yeaJ2G && month == monJ2G) {       // Check for Julian to
    if (d > dayJ2G1 && d < dayJ2G2)            // Gregorian crossover.
      d += NSgn(delta)*(dayJ2G2-dayJ2G1-1);
  }
  return d;
}


// Add a certain amount of time to the hour/day/month/year quantity that
// defines a particular chart. This is used by the chart animation feature.
// This can add or subtract anywhere from 1 to 9 seconds, minutes, hours,
// days, months, years, decades, centuries, or millenia in any one call.
// This is mainly just addition to the appropriate quantity, but have to
// check for overflows, e.g. Dec 30 + 3 days = Jan 2 of next year.

void AddTime(CI *pci, int mode, int toadd)
{
  int d, h;
  real m;

  if (mode < 1 || mode == iAnimNow)
    mode = iAnimDay;

  h = (int)RFloor(pci->tim);
  m = RFract(pci->tim)*60.0;
  if (m < 60.0 && m + 1.0/rLarge >= 60.0)  // Avoid roundoff error.
    m = 60.0;
  if (mode == 13)
    m += 1.0/60000.0*(real)toadd;          // Add milliseconds.
  else if (mode == 12)
    m += 1.0/6000.0*(real)toadd;           // Add 1/100th seconds.
  else if (mode == 11)
    m += 1.0/600.0*(real)toadd;            // Add 1/10th seconds.
  else if (mode == 1)
    m += 1.0/60.0*(real)toadd;             // Add seconds.
  else if (mode == 2)
    m += (real)toadd;                      // Add minutes.

  // Add hours, either naturally or if minute value overflowed.

  if (m >= 60.0 || m < 0.0 || mode == 3) {
    if (m >= 60.0) {
      m -= 60.0; toadd = NSgn2(toadd);
    } else if (m < 0.0) {
      m += 60.0; toadd = -NSgn2(-toadd);
    }
    h += toadd;
  }

  // Add days, either naturally or if hour value overflowed.

  if (h >= 24 || h < 0 || mode == 4) {
    if (h >= 24) {
      h -= 24; toadd = NSgn2(toadd);
    } else if (h < 0) {
      h += 24; toadd = -NSgn2(-toadd);
    }
    pci->day = AddDay(pci->mon, pci->day, pci->yea, toadd);
  }

  // Add months, either naturally or if day value overflowed.

  d = DayInMonth(pci->mon, pci->yea);
  if (pci->day > d || pci->day < 1 || mode == 5) {
    if (pci->day > d) {
      pci->day -= d; toadd = NSgn2(toadd);
    } else if (pci->day < 1) {
      pci->day += DayInMonth(Mod12(pci->mon - 1), pci->yea);
      toadd = -NSgn2(-toadd);
    }
    pci->mon += toadd;
  }

  // Add years, either naturally or if month value overflowed.

  if (pci->mon > 12 || pci->mon < 1 || mode == 6) {
    if (pci->mon > 12) {
      pci->mon -= 12; toadd = NSgn2(toadd);
    } else if (pci->mon < 1) {
      pci->mon += 12; toadd = NSgn2(toadd);
    }
    pci->yea += toadd;
  }
  if (mode == 7)
    pci->yea += 10 * toadd;      // Add decades.
  else if (mode == 8)
    pci->yea += 100 * toadd;     // Add centuries.
  else if (mode == 9)
    pci->yea += 1000 * toadd;    // Add millenia.

  pci->tim = (real)h + m/60.0;   // Recalibrate hour time.
  d = DayInMonth(pci->mon, pci->yea);
  if (pci->day > d)              // Truncate day in new month if illegal.
    pci->day = d;
}


// Given a set of chart information, return the offset of its time in hours
// before UTC. This is normally (time zone - daylight offset), however check
// for special case time zones and Daylight autodetection.

real GetOffsetCI(CONST CI *pci)
{
  real zon = pci->zon, dst = pci->dst;

  if (zon == zonLMT)
    zon = pci->lon / 15.0;
  else if (zon == zonLAT)
    zon = pci->lon / 15.0 - SwissLatLmt(is.JD);
  dst = DstReal(dst);
  return zon - dst;
}


// Given an aspect and two objects making that aspect with each other, return
// the maximum orb allowed for such an aspect. Normally this only depends on
// the aspect itself, but some objects require narrow orbs, and some allow
// wider orbs, so check for these cases.

real GetOrb(int obj1, int obj2, int asp)
{
  real orb, r;

  orb = rAspOrb[asp];
  r = rObjOrb[Min(obj1, oNorm1)];
  orb = Min(orb, r);
  r = rObjOrb[Min(obj2, oNorm1)];
  orb = Min(orb, r);
  orb += rObjAdd[Min(obj1, oNorm1)];
  orb += rObjAdd[Min(obj2, oNorm1)];
  return orb;
}


// Return an aspect's name, checking whether parallel aspects are on.

CONST char *SzAspect(int asp)
{
  if (us.fParallel && asp <= aOpp)
    asp += cAspect;
  return szAspectDisp[asp];
}


// Return the three letter abbreviation for an aspect.

CONST char *SzAspectAbbrev(int asp)
{
  if (us.fParallel && asp <= aOpp)
    asp += cAspect;
  return szAspectAbbrevDisp[asp];
}


// Restriction settings have changed in an arbitrary fashion (such as by
// running an unknown command line). Set the various "is category enabled"
// flags based on whether any of their objects are unrestricted.

void RedoRestrictions()
{
  int i;
  flag f;

  for (f = fFalse, i = cuspLo; i <= cuspHi; i++)
    if (!ignore[i] || !ignore2[i]) {
      f = fTrue;
      break;
    }
  us.fCusp = f;
  for (f = fFalse, i = uranLo; i <= uranHi; i++)
    if (!ignore[i] || !ignore2[i]) {
      f = fTrue;
      break;
    }
  us.fUranian = f;
  for (f = fFalse, i = dwarfLo; i <= dwarfHi; i++)
    if (!ignore[i] || !ignore2[i]) {
      f = fTrue;
      break;
    }
  us.fDwarf = f;
  for (f = fFalse, i = moonsLo; i <= moonsHi; i++)
    if (!ignore[i] || !ignore2[i]) {
      f = fTrue;
      break;
    }
  us.fMoons = f;
  for (f = fFalse, i = cobLo; i <= cobHi; i++)
    if (!ignore[i] || !ignore2[i]) {
      f = fTrue;
      break;
    }
  us.fCOB = f;
  for (f = fFalse, i = starLo; i <= starHi; i++)
    if (!ignore[i] || !ignore2[i]) {
      f = fTrue;
      break;
    }
  us.fStar = f;
  AdjustRestrictions();
}


// Set the central planet (e.g. geocentric or heliocentric).

void SetCentric(int obj)
{
  if (!us.fIgnoreAuto && ignore[us.objCenter] && !ignore[obj]) {
    // If -YRh switch in effect, might auto(un)restrict central object.
    inv(ignore[us.objCenter]);
    inv(ignore[obj]);
  }
  us.objCenter = obj;
}


// Return the planet or other object that an object orbits, if any.

int ObjOrbit(int obj)
{
  if (FGeo(obj))
    return oEar;
  if (FBetween(obj, oMer, cPlanet) || obj == oEar)
    return oSun;
  if (FCust(obj)) {
#ifdef SWISS
    // Check if this object has been redefined to be related to Earth's Moon.
    if (rgTypSwiss[obj - custLo] == 2 && FGeo(rgObjSwiss[obj - custLo]))
      return oEar;
    // Check if this object has been redefined to be a planetary moon.
    if (rgTypSwiss[obj - custLo] == 3 && rgObjSwiss[obj - custLo] % 100 < 99)
      return rgObjSwiss[obj - custLo] / 100 + 1;
    // Check if this is a JPL Horizons object for a planetary moon.
    if (rgTypSwiss[obj - custLo] == 4) {
      if (rgObjSwiss[obj - custLo] % 100 < 99 &&
        FBetween(rgObjSwiss[obj - custLo], 401, 998))
        return rgObjSwiss[obj - custLo] / 100 + 1;
      // Check if this is a JPL Horizons object for a Dwarf planet moon.
      if (rgObjSwiss[obj - custLo] == 120136199)  // Dysnomia
        return oEri;
      if (rgObjSwiss[obj - custLo] == 120136108)  // Hi'iaka
        return oHau;
      if (rgObjSwiss[obj - custLo] == 220136108)  // Namaka
        return oHau;
      if (rgObjSwiss[obj - custLo] == 120050000)  // Weywot
        return oQua;
      if (rgObjSwiss[obj - custLo] == 120090482)  // Vanth
        return oOrc;
      if (rgObjSwiss[obj - custLo] == 120120347)  // Actaea
        return oOrc;  // Hack: Since Salacia not present by default
    }
#endif
    return oSun;
  }
  return -1;
}


// Map a planetary moon ephemeris number to an Astrolog object index.

int ObjMoons(int i)
{
  int pla, moo, obj;

  pla = i/100; moo = i - pla*100;
  if (FBetween(pla, 4, 9) && FBetween(moo, 1, 8)) {
    obj = nMooMap[pla-4][moo-1];
    if (obj >= 0)
      obj += moonsLo;
  } else if (FBetween(pla, 5, 9) && moo == 99)
    obj = cobLo + (pla-5);
  else
    obj = -1;
  return obj;
}


// Return the diameter of an object, in km.

real RObjDiam(int obj)
{
#ifdef SWISS
  int i;

  // Check if this object has been redefined to be a planetary moon.
  if (FCust(obj) && rgTypSwiss[obj - custLo] == 3) {
    i = rgObjSwiss[obj - custLo];
    obj = ObjMoons(i);
  }
#endif
  if (!FNorm(obj))
    return 0.0;
  // If planet Center of Body (COB) is present, barycenter size should be 0.
  if (FBetween(obj, oJup, oPlu) && (!ignore[obj - oJup + cobLo]
#ifdef SWISS
    || (!ignore[oVul] && rgTypSwiss[oVul - custLo] == 3 &&
    rgObjSwiss[oVul - custLo] == (obj - oJup + 5)*100 + 99)
#endif
    ))
    return 0.0;
  return rObjDiam[obj];
}


/*
******************************************************************************
** String Procedures.
******************************************************************************
*/

// Print a string on the screen. A seemingly simple operation, however
// keep track of what column are printing at after each newline so can
// automatically clip at the appropriate point, and keep track of the row
// printing at too, so can prompt before screen scrolling.

void PrintSz(CONST char *sz)
{
  char szInput[cchSzDef], *pch;
  wchar wch;
  int ch, dch;
  flag fWantIBM = fFalse;
#ifndef WIN
  int fT;
#endif

  if (us.fNoDisplay)
    return;
#ifdef WINANY
  if (is.S == stdout)
    fWantIBM = fTrue;
#endif
  for (pch = (char *)sz; *pch; pch++) {
    ch = (uchar)*pch;
    if (ch != '\n') {
      if (is.nHTML != 2) {
        is.cchCol++;
        if (us.fClip80 && is.cchCol >= us.nScreenWidth)  // Clip if needed.
          continue;
      }
    } else {
      if (is.cchCol > is.cchColMax)
        is.cchColMax = is.cchCol;
      is.cchRow++;
      is.cchCol = 0;
    }
#ifdef WIN
    if (is.S == stdout) {
      // Windows text mode screen text should be output in IBM character set.
      if (us.nCharset >= ccUTF8) {
        pch += (UTF8ToWch((uchar *)pch, &wch) - 1);
        if (us.nCharsetOut != ccLatin)
          ch = ChIBMFromWch(wch);
        else
          ch = ChLatinFromWch(wch);
      } else if (us.nCharset == ccLatin) {
        if (us.nCharsetOut != ccLatin) {
          wch = WchFromChLatin(ch);
          ch = ChIBMFromWch(wch);
        }
      }
      if ((byte)ch >= ' ') {
        szInput[0] = ch; szInput[1] = chNull;
        TextOut(wi.hdc, (is.cchCol - 1 - wi.xScroll * 10) * wi.xChar + 4,
          (is.cchRow - wi.yScroll * 10) * wi.yChar, szInput, 1);
      }
    } else
#endif
    if (is.nHTML == 1) {
      // HTML text can handle Unicode characters.
      if (us.nCharset > ccNone) {
        if (us.nCharset >= ccUTF8)
          pch += (UTF8ToWch((uchar *)pch, &wch) - 1);
        else if (us.nCharset == ccLatin)
          wch = WchFromChLatin(ch);
        else if (us.nCharset == ccIBM)
          wch = WchFromChIBM(ch);
        ch = wch;
      }
      if (ch == '<')
        fprintf(is.S, "&lt;");
      else if (ch == '>')
        fprintf(is.S, "&gt;");
      else if (ch == '&')
        fprintf(is.S, "&amp;");
      else if (ch == '\"')
        fprintf(is.S, "&quot;");
      else if (ch == ' ' && (pch <= sz || *(pch+1) <= ' '))
        fprintf(is.S, "&nbsp;");
      else if (ch == '\n')
        fprintf(is.S, "<br>\n");
      else if (ch >= 128)
        fprintf(is.S, "&#%d;", ch);
      else
        putc(ch, is.S);
    } else {
      // Text output should be IBM chars for Windows console, else Latin.
      dch = 0;
      if (us.nCharset > ccNone) {
        if (us.nCharset >= ccUTF8) {
          pch += (UTF8ToWch((uchar *)pch, &wch) - 1);
          if (us.nCharsetOut == ccUTF8)
            dch = WchToUTF8(wch, szInput);
          else if (fWantIBM || us.nCharsetOut == ccIBM)
            ch = ChIBMFromWch(wch);
          else
            ch = ChLatinFromWch(wch);
        } else if (us.nCharset == ccLatin) {
          if (us.nCharsetOut == ccUTF8) {
            wch = WchFromChLatin(ch);
            dch = WchToUTF8(wch, szInput);
          } else if (fWantIBM || us.nCharsetOut == ccIBM) {
            wch = WchFromChLatin(ch);
            ch = ChIBMFromWch(wch);
          }
        } else {
          if (us.nCharsetOut == ccUTF8) {
            wch = WchFromChIBM(ch);
            dch = WchToUTF8(wch, szInput);
          } else if (!(fWantIBM || us.nCharsetOut == ccIBM)) {
            wch = WchFromChIBM(ch);
            ch = ChLatinFromWch(wch);
          }
        }
      }
      if (dch < 1)
        putc(ch, is.S);
      else
        fprintf(is.S, "%s", szInput);
    }
#ifndef WIN
    if (ch == '\n' && is.S == stdout &&
      us.nScrollRow > 0 && is.cchRow >= us.nScrollRow) {

      // If have printed 'n' rows, stop and wait for a line to be entered.

      fT = us.fAnsiColor;
      us.fAnsiColor = fFalse;
      InputString("Press return to continue scrolling", szInput);
      us.fAnsiColor = fT;
      is.cchRow = 0;

      // One can actually give a few simple commands before hitting return.

      if (szInput[0] == 'q' || szInput[0] == '.')
        Terminate(tcForce);
      else if (szInput[0] == '8')
        inv(us.fClip80);
      else if (szInput[0] == 'Q')
        us.nScrollRow = 0;
      else if (szInput[0] == 'k') {
        if (us.fAnsiColor)
          AnsiColor(kDefault);
        inv(us.fAnsiColor); inv(us.fAnsiChar);
      }
    }
#else
    if (ch == '\n' && is.S == stdout && wi.hdcPrint != hdcNil &&
      is.cchRow >= us.nScrollRow) {

      // If writing to the printer, start a new page when appropriate.

      is.cchRow = 0;
      EndPage(wi.hdcPrint);
      StartPage(wi.hdcPrint);
      // StartPage clobbers all the DC settings.
      SetMapMode(wi.hdcPrint, MM_ANISOTROPIC);      // For SetViewportExt
      SetViewportOrg(wi.hdcPrint, 0, 0);
      SetViewportExt(wi.hdcPrint, GetDeviceCaps(wi.hdcPrint, HORZRES),
        GetDeviceCaps(wi.hdcPrint, VERTRES));
      SetWindowOrg(wi.hdcPrint, 0, 0);
      SetWindowExt(wi.hdcPrint, wi.xClient, wi.yClient);
      SetBkMode(wi.hdcPrint, TRANSPARENT);
      SelectObject(wi.hdcPrint, wi.hfont);
    }
#endif
  }
}


// Print a single character on the screen.

void PrintCh(char ch)
{
  char sz[2];

  sz[0] = ch; sz[1] = chNull;    // Treat char as a string of length one.
  PrintSz(sz);                   // Then call above to print the string.
}


// A simple procedure used throughout the program: Print a particular
// character on the screen 'n' times.

void PrintTab(char ch, int cch)
{
  while (cch-- > 0)
    PrintCh(ch);
}


// Like PrintCh() but ensure line drawing characters aren't converted to
// something else, if the character encoding is set differently.

void PrintCh2(char ch)
{
  int nCharsetSav = us.nCharset;

  us.nCharset = (us.nCharset > ccNone ? ccIBM : ccNone);
  PrintCh(ch);
  us.nCharset = nCharsetSav;
}


// Like PrintTab() but ensure line drawing characters aren't converted to
// something else, if the character encoding is set differently.

void PrintTab2(char ch, int cch)
{
  int nCharsetSav = us.nCharset;

  us.nCharset = (us.nCharset > ccNone ? ccIBM : ccNone);
  PrintTab(ch, cch);
  us.nCharset = nCharsetSav;
}


// Print a string on the screen. Unlike the normal PrintSz(), here still
// go to the standard output even if text is being sent to a file with -os.

void PrintSzScreen(CONST char *sz)
{
  FILE *fileT;

  fileT = is.S;
  is.S = stdout;
  PrintSz(sz);
  is.S = fileT;
}


// Print a string, however also expand escape sequences like "\n" to a
// newline, "\A" to AstroExpression custom variable @a, and "\a" to the
// AstroExpression custom string indexed by custom variable @a.

void PrintSzFormat(CONST char *sz, flag fPopup)
{
  char szFormat[cchSzLine], *pch2;
  CONST char *pch;

  for (pch = sz, pch2 = szFormat; *pch; pch++, pch2++) {
    *pch2 = *pch;
    if (*pch == '\\' && (pch[1] == '\\' || pch[1] == 'n')) {
      if (pch[1] == 'n')
        *pch2 = '\n';
      pch++;
    }
#ifdef EXPRESS
    else if (*pch == '\\') {
      if (FCapCh(pch[1])) {
        pch2 = PchFormatExpression(pch2, pch[1] - '@') - 1;
        pch++;
        continue;
      } else if (FUncapCh(pch[1])) {
        pch2 = PchFormatString(pch2, pch[1] - '`') - 1;
        pch++;
        continue;
      }
    }
#endif
  }
  *pch2 = chNull;
#ifdef WIN
  if (fPopup)
    PrintNotice(szFormat);
  else
#endif
  PrintSz(szFormat);
}


// Print a partial progress message given a string. This is meant to be used
// in the middle of long operations such as creating and saving files.

void PrintProgress(CONST char *sz)
{
#ifndef WIN
  // Progress messages are ignored in the Windows version.
  AnsiColor(kYellowA);
  fprintf(stderr, "%s\n", sz);
  AnsiColor(kDefault);
#endif
}


// Print a general user message given a string. This is just like the warning
// displayer below, except print in a different color.

void PrintNotice(CONST char *sz)
{
#ifndef WIN
  AnsiColor(kYellowA);
  fprintf(stderr, "%s\n", sz);
  AnsiColor(kDefault);
#else
  char szT[cchSzDef];

  if (wi.fNoPopup)
    return;
  sprintf(szT, "%s Notice", szAppName);
  MessageBox(wi.hwndMain, sz, szT, MB_ICONINFORMATION);
#endif
}


// Print a warning message given a string. This is called in non-fatal cases
// in which normal execution is resumed after printing the string.

void PrintWarning(CONST char *sz)
{
#ifndef WIN
  AnsiColor(kRedA);
  fprintf(stderr, "%s\n", sz);
  AnsiColor(kDefault);
#else
  char szT[cchSzDef];

  if (wi.fNoPopup)
    return;
  sprintf(szT, "%s Warning", szAppName);
  MessageBox(wi.hwndMain, sz, szT, MB_ICONSTOP);
#endif
}


// Print an error message. This is called in more serious cases which halt
// running of the current chart sequence, which may terminate the program,
// but isn't a fatal error in that can still fall back to the -Q loop.

void PrintError(CONST char *sz)
{
#ifndef WIN
  AnsiColor(kRedA);
  fprintf(stderr, "%s: %s\n", szAppName, sz);
  AnsiColor(kDefault);
  Terminate(tcError);
#else
  char szT[cchSzDef];

  if (wi.fNoPopup)
    return;
  sprintf(szT, "%s Error", szAppName);
  MessageBox(wi.hwndMain, sz, szT, MB_ICONEXCLAMATION);
#endif
}


// Print error message for missing parameters to a command switch.

flag FErrorArgc(CONST char *szOpt, int carg, int cargMax)
{
  char sz[cchSzDef];

  carg--;
  if (carg >= cargMax)
    return fFalse;
  sprintf(sz, "Too few parameters to switch %c%s (%d given, %d required)",
    chSwitch, szOpt, carg, cargMax);
  PrintError(sz);
  return fTrue;
}


// Print error message for out of range integer parameter to a command switch.

flag FErrorValN(CONST char *szOpt, flag f, int nVal, int nPar)
{
  char sz[cchSzMax], szPar[cchSzDef];

  if (!f)
    return fFalse;
  if (nPar <= 0)
    szPar[0] = chNull;
  else
    sprintf(szPar, "parameter #%d of ", nPar);
  sprintf(sz, "Value %d passed to %sswitch %c%s out of range.\n",
    nVal, szPar, chSwitch, szOpt);
  PrintError(sz);
  return fTrue;
}


// Print error message for out of range real parameter to a command switch.

flag FErrorValR(CONST char *szOpt, flag f, real rVal, int nPar)
{
  char sz[cchSzMax], szPar[cchSzDef], szVal[cchSzDef];

  if (!f)
    return fFalse;
  if (nPar <= 0)
    szPar[0] = chNull;
  else
    sprintf(szPar, "parameter #%d of ", nPar);
  if (rVal != rLarge) {
    FormatR(szVal, rVal, -6);
    sprintf(sz, "Value %s passed to %sswitch %c%s out of range.\n",
      szVal, szPar, chSwitch, szOpt);
  } else
    sprintf(sz, "Bad value passed to %sswitch %c%s\n",
      szPar, chSwitch, szOpt);
  PrintError(sz);
  return fTrue;
}


// Print error message for a disallowed command switch.

void ErrorArgv(CONST char *szOpt)
{
  char sz[cchSzDef];

  sprintf(sz, "The switch %c%s is not allowed now.\n", chSwitch, szOpt);
  PrintError(sz);
}


// Print error message for a completely unknown command switch.

void ErrorSwitch(CONST char *szOpt)
{
  char sz[cchSzDef];

  sprintf(sz, "Unknown switch '%s'", szOpt);
  PrintError(sz);
}


// Print error message for a unknown command subswitch.

flag FErrorSubswitch(CONST char *szOpt, char chSub, flag f)
{
  char sz[cchSzDef];

  if (!f)
    return fFalse;
  sprintf(sz, "Unknown subswitch '%c%s%c' to command switch '%c%s'",
    chSwitch, szOpt, chSub, chSwitch, szOpt);
  PrintError(sz);
  return fTrue;
}


#ifdef PLACALC
// Print error messages dealing with ephemeris file access.

void ErrorEphem(CONST char *sz, long l)
{
  char szT[cchSzDef];

  if (is.fNoEphFile)
    return;
  if (l < 0)
    sprintf(szT, "Ephemeris file %s not found.\n", sz);
  else
    sprintf(szT, "Seek error in file %s at position %ld.\n", sz, l);
  is.fNoEphFile = fTrue;
  PrintWarning(szT);
}
#endif


// Set an Ansi or MS Windows text color.

void AnsiColor(int k)
{
  char sz[cchSzDef];
  int cchSav;
#ifdef GRAPH
  KV kv;
#endif

#ifdef WIN
  if (is.S == stdout) {
    if (k < 0)
      k = kLtGrayA;
    SetTextColor(wi.hdc, (COLORREF)rgbbmp[us.fAnsiColor ? k : kLtGrayA]);
    return;
  }
#endif

  // Special case: If passed the "color" Reverse, and Ansi color is not only
  // on but set to a value > 1, then enter reverse video mode.

  if (!us.fAnsiColor || (k == kReverse && us.fAnsiColor < 2))
    return;
  cchSav = is.cchCol;
  is.cchCol = 0;
  if (is.nHTML <= 0) {
    sprintf(sz, "%c[", chEscape);
    PrintSz(sz);
    if (k == kDefault)
      PrintCh('0');
    else if (k == kReverse) {
      PrintCh('7');
    } else {
      sprintf(sz, "%c;%d", k > 7 ? '1' : '0', 30 + (k & 7));
      PrintSz(sz);
    }
    PrintCh('m');
  } else {
    if (is.nHTML != 3) {
      is.nHTML = 2;
      PrintSz("</font>");
    } else
      is.nHTML = 2;
    if (k < 0)
      k = kLtGrayA;
    PrintSz("<font color=\"");
#ifdef GRAPH
    if (rgbbmp[k] == rgbbmpDef[k])
      PrintSz(szColorHTML[k]);
    else {
      kv = rgbbmp[k];
      sprintf(sz, "#%06x", Rgb(RgbB(kv), RgbG(kv), RgbR(kv)));
      PrintSz(sz);
    }
#else
    PrintSz(szColorHTML[k]);
#endif
    PrintSz("\">");
    is.nHTML = 1;
  }
  is.cchCol = cchSav;
}


// Print a zodiac position on the screen. This basically just prints the
// string returned from SzZodiac() below, except also take care of color.

void PrintZodiac(real deg)
{
  if (us.fRound) {
    if (us.nDegForm == dfZod)
      deg = Mod(deg +
        VSeconds(rRound/60.0, rRound/3600.0, rRound/3600.0/1000.0));
    else if (us.nDegForm == dfHM)
      deg = Mod(deg +
        VSeconds(rRound/4.0, rRound/4.0/60.0, rRound/4.0/60.0/1000.0));
  }
  AnsiColor(kSignA(SFromZ(deg)));
  PrintSz(SzZodiac(deg));
  AnsiColor(kDefault);
}


// Determine and return the best character to use for the degree symbol.

char ChDeg()
{
  char chDeg;

#ifdef GRAPH
  // In graphics charts, always use the Latin encoding for degree.
  if (us.fGraphics && us.fAnsiChar)
    chDeg = (us.nCharset == ccIBM ? chDegI : chDegL);
  else
#endif
#ifdef WIN
    if (!us.fGraphics && gs.nFontTxt > 0)
      chDeg = chDegL;
  else
#endif
    // Otherwise degree depends on the current output codepage environment.
    chDeg = (us.fAnsiChar > 1 ? chDegT : chDegC);
  return chDeg;
}


// Return a letter representing retrogradation status. That means "R" for
// retrograde, or potentially "S" for stationary if the velocity is near 0.

char ChRet(real dir)
{
  if (RAbs(dir) < us.rStation)
    return 'S';
  if (dir < 0.0)
    return chRet;
  return ' ';
}


// Given a zodiac position, return a string containing it as it's formatted
// for display to the user.

char *SzZodiac(real deg)
{
  static char szZod[16];
  int sign, d, m;
  real s;

  switch (us.nDegForm) {
  case dfZod:
    // Normally, format the position in degrees/sign/minutes format.

    sign = (int)deg / 30;
    d = (int)deg - sign*30;
    m = (int)(RFract(deg)*60.0);
    sprintf(szZod, "%2d%.3s%02d", d, szSignName[sign + 1], m);
    if (us.fSeconds) {
      s = RFract(deg)*60.0; s = RFract(s)*60.0;
      sprintf(&szZod[7], "'%02d\"", (int)s);
      if (us.fSecond1K) {
        s = RFract(s)*1000.0;
        sprintf(&szZod[10], ".%03d\"", (int)s);
      }
    }
    break;

  case dfHM:
    // However, if -sh switch in effect, format position as hours/minutes.

    d = (int)deg / 15;
    m = (int)((deg - (real)d*15.0)*4.0);
    sprintf(szZod, "%2dh,%02dm", d, m);
    if (us.fSeconds) {
      s = RFract(deg)*4.0; s = RFract(s)*60.0;
      sprintf(&szZod[7], ",%02ds", (int)s);
      if (us.fSecond1K) {
        s = RFract(s)*1000.0;
        sprintf(&szZod[10], ".%03ds", (int)s);
      }
    }
    break;

  case df360:
    // Or, if -sd in effect, format position as a simple degree.

#ifdef EXPRESS
    if (!us.fExpOff && FSzSet(us.szExpDegree)) {
      ExpSetR(iLetterZ, deg);
      deg = RParseExpression(us.szExpDegree);
      sprintf(szZod, "%15.15f", deg);
      szZod[VSeconds(7, 11, 15)] = chNull;
      break;
    }
#endif
    sprintf(szZod, VSeconds("%7.3f", "%11.7f", "%15.11f"), deg);
    break;

  default:
    // Otherwise, if -sn in effect, format position as a Nakshatra.

    deg = Mod(deg + rSmall);
    sign = (int)(deg / (rDegMax/27.0));
    d = (int)((deg - (real)sign*(rDegMax/27.0))*40.0 / (rDegMax/27.0));
    sprintf(szZod, "%2d%.4s%d", sign+1, szNakshatra[sign + 1], d/10 + 1);
    if (us.fSeconds) {
      if (!us.fSecond1K) {
        sprintf(&szZod[7], ".%d%s", d%10,
          szSignAbbrev[Mod12((int)(deg/(rDegMax/27.0/4.0))+1)]);
      } else {
        d = (int)((deg - (real)sign*(rDegMax/27.0))*40000.0 / (rDegMax/27.0));
        sprintf(&szZod[7], ".%04d%.3s", d%10000,
          szSignName[Mod12((int)(deg/(rDegMax/27.0/4.0))+1)]);
      }
    }
    break;
  }
  return szZod;
}


// This is similar to formatting a zodiac degree, but here return a string of
// a signed latitude value in degrees and minutes.

char *SzAltitude(real deg)
{
  static char szAlt[11];
  int d, m, f;
  real s;

  f = deg < 0.0;
  deg = RAbs(deg);
  if (us.nDegForm != df360) {
    if (us.fRound)
      deg = Mod(deg +
        VSeconds(rRound/60.0, rRound/3600.0, rRound/3600.0/1000.0));
    d = (int)deg;
    m = (int)(RFract(deg)*60.0);
    sprintf(szAlt, "%c%2d%c%02d'", f ? '-' : '+', d, ChDeg(), m);
    if (us.fSeconds) {
      s = RFract(deg)*60.0; s = RFract(s)*60.0;
      sprintf(&szAlt[7], "%02d\"", (int)s);
      if (us.fSecond1K) {
        s = RFract(s)*1000.0;
        sprintf(&szAlt[9], ".%03d\"", (int)s);
      }
    }
  } else {
    s = RAbs(deg);
    if (!us.fSeconds)
      sprintf(szAlt, s < 10.0 ? "%c%1.4f" : "%c%2.3f", f ? '-' : '+', s);
    else if (!us.fSecond1K)
      sprintf(szAlt, s < 10.0 ? "%c%1.7f" : "%c%2.6f", f ? '-' : '+', s);
    else
      sprintf(szAlt, s < 10.0 ? "%c%1.11f" : "%c%2.10f", f ? '-' : '+', s);
  }
  return szAlt;
}


// Here return a string simply expressing the given value as degrees and
// minutes (and sometimes seconds) in the 0 to 360 degree circle.

char *SzDegree(real deg)
{
  static char szPos[11];
  int d, m;
  real s;

  deg = RAbs(deg);
  if (us.nDegForm != df360) {
    if (us.fRound)
      deg = Mod(deg +
        VSeconds(rRound/60.0, rRound/3600.0, rRound/3600.0/1000.0));
    d = (int)deg;
    m = (int)(RFract(deg)*60.0);
    sprintf(szPos, "%3d%c%02d'", d, ChDeg(), m);
    if (us.fSeconds) {
      s = RFract(deg)*60.0; s = RFract(s)*60.0;
      sprintf(&szPos[7], "%02d\"", (int)s);
      if (us.fSecond1K) {
        s = RFract(s)*1000.0;
        sprintf(&szPos[9], ".%03d\"", (int)s);
      }
    }
  } else {
    if (!us.fSeconds)
      sprintf(szPos, "%7.3f", deg);
    else if (!us.fSecond1K)
      sprintf(szPos, "%10.6f", deg);
    else
      sprintf(szPos, "%14.10f", deg);
  }
  return szPos;
}


// Like SzDegree() except don't have extra whitespace padding around it,
// meaning different inputs are allowed to produce different length strings.

char *SzDegree2(real deg)
{
  static char szPos[11], *pch;
  int d, m;
  real s;

  deg = RAbs(deg);
  if (us.nDegForm != df360) {
    if (us.fRound)
      deg = Mod(deg +
        VSeconds(rRound/60.0, rRound/3600.0, rRound/3600.0/1000.0));
    d = (int)deg;
    m = (int)(RFract(deg)*60.0);
    sprintf(szPos, "%d%c%02d'", d, ChDeg(), m);
    if (us.fSeconds) {
      s = RFract(deg)*60.0; s = RFract(s)*60.0;
      for (pch = szPos; *pch; pch++)
        ;
      sprintf(pch, "%02d\"", (int)s);
      if (us.fSecond1K) {
        while (*pch)
          pch++;
        s = RFract(s)*1000.0;
        sprintf(pch-1, ".%03d\"", (int)s);
      }
    }
  } else {
    if (!us.fSeconds)
      sprintf(szPos, "%.3f", deg);
    else if (!us.fSecond1K)
      sprintf(szPos, "%.6f", deg);
    else
      sprintf(szPos, "%.10f", deg);
  }
  return szPos;
}


// Compose an Hours:Minutes:Seconds (HMS) time value, given a total number of
// seconds. For example, 45015 (12*60*60+30*60+15) maps to "+12:30:15"

char *SzHMS(int sec)
{
  static char szHMS[10];
  int hr, min;
  char ch;

  ch = sec >= 0 ? '+' : '-';
  sec = NAbs(sec);
  hr = sec / 3600;
  min = sec / 60 % 60;
  sec %= 60;
  // Don't display seconds or minutes:seconds if they're zero.
  if (!us.fSeconds && min == 0 && sec == 0)
    sprintf(szHMS, "%c%d", ch, hr);
  else if (!us.fSeconds || sec == 0)
    sprintf(szHMS, "%c%d:%02d", ch, hr, min);
  else
    sprintf(szHMS, "%c%d:%02d:%02d", ch, hr, min, sec);
  return szHMS;
}


// Another string formatter, here return a date string given a month, day, and
// year. Format with the day or month first based on whether the "European"
// date variable is set or not. The routine also takes a parameter to indicate
// how much the string should be abbreviated, if any.

char *SzDate(int mon, int day, int yea, int nFormat)
{
  static char szDat[20];

  if (us.fEuroDate) {
    switch (nFormat) {
    case  2: sprintf(szDat, "%2d %.3s %4d", day, szMonth[mon], yea); break;
    case  1: sprintf(szDat, "%d %s %d", day, szMonth[mon], yea);     break;
    case -1: sprintf(szDat, "%2d-%2d-%2d", day, mon, NAbs(yea)%100); break;
    default: sprintf(szDat, "%2d-%2d-%4d", day, mon, yea);           break;
    }
  } else {
    switch (nFormat) {
    case  3: sprintf(szDat, "%.3s %2d, %d", szMonth[mon], day, yea); break;
    case  2: sprintf(szDat, "%.3s %2d %4d", szMonth[mon], day, yea); break;
    case  1: sprintf(szDat, "%s %d, %d", szMonth[mon], day, yea);    break;
    case -1: sprintf(szDat, "%2d/%2d/%2d", mon, day, NAbs(yea)%100); break;
    default: sprintf(szDat, "%2d/%2d/%4d", mon, day, yea);           break;
    }
  }
  return szDat;
}


// Return a string containing the given time expressed as an hour and minute
// (and second) quantity. This is formatted in 24 hour or am/pm time based on
// whether the "European" time format flag is set or not.

char *SzTimeR(int hr, int min, int sec, real rSec)
{
  static char szTim[16];

  while (min >= 60) {
    min -= 60;
    hr++;
  }
  while (hr < 0)
    hr += 24;
  while (hr >= 24)
    hr -= 24;
  if (us.fAnsiChar == 4) {
    // Format like "01:23:45 PM", as seen in Quick*Chart files.
    sprintf(szTim, "%02d:%02d:%02d %cM",
      Mod12(hr), min, sec, hr < 12 ? 'A' : 'P');
    return szTim;
  }
  if (us.fEuroTime) {
    if (sec < 0)
      sprintf(szTim, "%2d:%02d", hr, min);
    else if (rSec < 0.0)
      sprintf(szTim, "%2d:%02d:%02d", hr, min, sec);
    else
      sprintf(szTim, "%2d:%02d:%06.3f", hr, min, rSec);
  } else {
    if (sec < 0)
      sprintf(szTim, "%2d:%02d%cm", Mod12(hr), min, hr < 12 ? 'a' : 'p');
    else if (rSec < 0.0)
      sprintf(szTim, "%2d:%02d:%02d%cm",
        Mod12(hr), min, sec, hr < 12 ? 'a' : 'p');
    else
      sprintf(szTim, "%2d:%02d:%06.3f%cm",
        Mod12(hr), min, rSec, hr < 12 ? 'a' : 'p');
  }
  return szTim;
}


// This just determines the correct hour and minute and calls the above.

char *SzTim(real tim)
{
  tim += rSmall;
  return SzTimeR(NFloor(tim), (int)(RFract(RAbs(tim))*60.0),
    us.fSeconds ? (int)(RFract(RAbs(tim))*3600.0) % 60 : -1,
    f1K ? RMod(RFract(RAbs(tim))*3600.0, 60.0) : -1.0);
}


// Return a string containing the given time zone, given as a real value
// having the hours before UTC in the integer part and minutes fractionally.

char *SzZone(real zon)
{
  static char szZon[7];

  if (zon == zonLMT)
    sprintf(szZon, "LMT");
  else if (zon == zonLAT)
    sprintf(szZon, "LAT");
  else if (!us.fSeconds && RFract(RAbs(zon)) < rSmall)
    sprintf(szZon, "%d%c", (int)RAbs(zon), zon < 0.0 ? 'E' : 'W');
  else
    sprintf(szZon, "%d:%02d%c", (int)RAbs(zon), (int)(RFract(RAbs(zon))*60.0+
      rRound/60.0), zon < 0.0 ? 'E' : 'W');
  return szZon;
}


// Return a string containing the given offset from UTC, given as a time zone
// and Daylight Time offsets. This may display Daylight Time and time zone
// separately, or display one combined value for the overall offset.

char *SzOffset(real zon, real dst, real lon)
{
  static char szOff[15], *pch;
  real off;
  int min;
  flag fLMT;

  if (!us.fOffsetOnly)
    sprintf(szOff, "%cT %s%s", ChDst(Dst), !f1K ? "Zone " : "", SzZone(Zon));
  else {
    fLMT = (zon == zonLMT || zon == zonLAT);
    if (dst == dstAuto)
      dst = (real)is.fDst;
    off = (!fLMT ? zon - dst : lon - dst*15.0);
    min = (int)(RFract(RAbs(off))*60.0);
    sprintf(szOff, "Zone %c%d%c", !fLMT ? 'h' : 'm', NAbs((int)off),
      off > 0.0 ? 'w' : 'e');
    if (min != 0) {
      for (pch = szOff; *pch; pch++)
        ;
      sprintf(pch, "%02d", min);
    }
  }
  return szOff;
}


// Nicely format the given longitude and latitude locations and return them
// in a string. Various parts of the program display a chart header, and this
// allows the similar computations to be coded only once.

char *SzLocation(real lon, real lat)
{
  static char szLoc[25];
  int i, j, i2, j2, i3, j3;
  real rT;
  char chDeg, chLon, chLat;

  if (us.fRound)
    rT = VSeconds(rRound/60.0, rRound/3600.0, rRound/3600.0/1000.0);
  else
    rT = rSmall;
  lon = RSgn(lon) * (RAbs(lon) + rT);
  lat = RSgn(lat) * (RAbs(lat) + rT);
  i = (int)(RFract(RAbs(lon))*60.0);
  j = (int)(RFract(RAbs(lat))*60.0);
  if (us.fSeconds) {
    i2 = (int)(RFract(RAbs(lon))*3600.0) % 60;
    j2 = (int)(RFract(RAbs(lat))*3600.0) % 60;
    if (us.fSecond1K) {
      i3 = (int)(RFract(RAbs(lon))*3600.0*1000.0) % 1000;
      j3 = (int)(RFract(RAbs(lat))*3600.0*1000.0) % 1000;
    }
  }
  chLon = (lon < 0.0 ? 'E' : 'W');
  chLat = (lat < 0.0 ? 'S' : 'N');
  if (us.fAnsiChar == 4) {
    // Format like "47N36,122W19", as seen in AAF files.
    if (!us.fSeconds)
      sprintf(szLoc, "%.0f%c%02d,%.0f%c%02d",
        RFloor(RAbs(lat)), chLat, j, RFloor(RAbs(lon)), chLon, i);
    else
      sprintf(szLoc, "%.0f%c%02d:%02d,%.0f%c%02d:%02d",
        RFloor(RAbs(lat)), chLat, j, j2, RFloor(RAbs(lon)), chLon, i, i2);
    return szLoc;
  }
  if (us.fAnsiChar != 3) {
    chDeg = ChDeg();
    if (us.nDegForm != df360) {
      if (!us.fSeconds)
        sprintf(szLoc, "%3.0f%c%02d%c%3.0f%c%02d%c",
          RFloor(RAbs(lon)), chDeg, i, chLon,
          RFloor(RAbs(lat)), chDeg, j, chLat);
      else if (!us.fSecond1K)
        sprintf(szLoc, "%3.0f%c%02d'%02d%c%3.0f%c%02d'%02d%c",
          RFloor(RAbs(lon)), chDeg, i, i2, chLon,
          RFloor(RAbs(lat)), chDeg, j, j2, chLat);
      else
        sprintf(szLoc, "%3.0f%c%02d'%02d.%03d%c%3.0f%c%02d'%02d.%03d%c",
          RFloor(RAbs(lon)), chDeg, i, i2, i3, chLon,
          RFloor(RAbs(lat)), chDeg, j, j2, j3, chLat);
    } else {
      if (!us.fSeconds)
        sprintf(szLoc, "%6.2f%c%6.2f%c",
          RAbs(lon), chLon, RAbs(lat), chLat);
      else
        sprintf(szLoc, "%9.5f%c%9.5f%c",
          RAbs(lon), chLon, RAbs(lat), chLat);
    }
  } else {
    if (us.nDegForm != df360) {
      if (!us.fSeconds)
        sprintf(szLoc, "%3.0f%c%02d%3.0f%c%02d",
          RFloor(RAbs(lon)), chLon, i,
          RFloor(RAbs(lat)), chLat, j);
      else if (!us.fSecond1K)
        sprintf(szLoc, "%3.0f%c%02d'%02d%3.0f%c%02d'%02d",
          RFloor(RAbs(lon)), chLon, i, i2,
          RFloor(RAbs(lat)), chLat, j, j2);
      else
        sprintf(szLoc, "%3.0f%c%02d'%02d.%03d%3.0f%c%02d'%02d.%03d",
          RFloor(RAbs(lon)), chLon, i, i2, i3,
          RFloor(RAbs(lat)), chLat, j, j2, j3);
    } else {
      if (!us.fSeconds)
        sprintf(szLoc, "%5.1f%c%5.1f%c",
          RAbs(lon), chLon, RAbs(lat), chLat);
      else
        sprintf(szLoc, "%8.4f%c%8.4f%c",
          RAbs(lon), chLon, RAbs(lat), chLat);
    }
  }
  return szLoc;
}


// Format and return a string containing an elevation above sea level,
// displayed in either meters or feet, as used with topocentric charts.

char *SzElevation(real elv)
{
  static char szElev[21];
  char *pch;

  FormatR(szElev, us.fEuroDist ? elv : elv / rFtToM, -2);
  for (pch = szElev; *pch; pch++)
    ;
  sprintf(pch, "%s", us.fEuroDist ? "m" : "ft");
  return szElev;
}


// Format and return a string containing a temperature, displayed in either
// degrees Celsius or Fahrenheit, as used with atmospheric refraction.

char *SzTemperature(real tmp)
{
  static char szTemp[21];
  char *pch;

  FormatR(szTemp, us.fEuroDist ? tmp : tmp * 9.0/5.0 + 32.0, -2);
  for (pch = szTemp; *pch; pch++)
    ;
  sprintf(pch, "%s", us.fEuroDist ? "C" : "F");
  return szTemp;
}


// Format and return a string containing a relatively short length, displayed
// in either inches or centimeters, as used with paper sizes.

char *SzLength(real len)
{
  static char szLen[21];
  char *pch;

  FormatR(szLen, !us.fEuroDist ? len : len * rInToCm, -2);
  for (pch = szLen; *pch; pch++)
    ;
  sprintf(pch, "%s", us.fEuroDist ? "cm" : "in");
  return szLen;
}


#ifdef TIME
// Compute the date and time it is right now as the program is running using
// the computer's internal clock. Do this by getting the number of seconds
// which have passed since January 1, 1970 and going from there. The time
// return value filled is expressed in the given zone parameter.

void GetTimeNow(int *mon, int *day, int *yea, real *tim, real dst, real zon)
{
#ifdef PC
  SYSTEMTIME st, lt;
  real jd;
  int dh;

  GetSystemTime(&st);
  if (dst == dstAuto) {
    // Daylight field of 24 means autodetect whether Daylight Saving Time.

    GetLocalTime(&lt);
    dh = NAbs(st.wHour - lt.wHour);
    if (dh > 12)
      dh = 24-dh;
    is.fDst = (dh == ciDefa.zon-1);
    dst = (real)is.fDst;
  }
  if (zon == zonLMT || zon == zonLAT)
    zon = ciDefa.lon / 15.0;
  jd = MdytszToJulian(st.wMonth, st.wDay, st.wYear,
    (real)(((st.wHour * 60 + st.wMinute + us.lTimeAddition) * 60 +
    st.wSecond) * 1000 + st.wMilliseconds) / (60.0 * 60.0 * 1000.0),
    0.0, -(zon-dst));
  *tim = (jd - RFloor(jd)) * 24.0;
  JulianToMdy(jd - 0.5, mon, day, yea);
#else
  time_t curtimer;
  int min, sec, i;
  real hr;
  CI ci;

  time(&curtimer);
  sec = (int)(curtimer % 60);
  curtimer = curtimer / 60 + us.lTimeAddition;
  min = (int)(curtimer % 60);
  curtimer /= 60;
  if (zon == zonLMT || zon == zonLAT)
    zon = ciDefa.lon / 15.0;
  hr = (real)(curtimer % 24) - (zon - (dst == dstAuto ? 0.0 : dst));
  curtimer /= 24;
  while (hr < 0.0) {
    curtimer--;
    hr += 24.0;
  }
  while (hr >= 24.0) {
    curtimer++;
    hr -= 24.0;
  }
  curtimer += ldTime;  // Number of days between 1/1/1970 and 1/1/4713 BC.
  JulianToMdy((real)curtimer, mon, day, yea);
  *tim = HMS(hr, min, sec);
  if (dst == dstAuto) {
    // Daylight field of 24 means autodetect whether Daylight Saving Time.

    SetCI(ci, *mon, *day, *yea, *tim, 0.0, zon, ciDefa.lon, ciDefa.lat);
    if (DisplayAtlasLookup(ciDefa.loc, 0, &i) &&
      DisplayTimezoneChanges(is.rgae[i].izn, 0, &ci)) {
      hr += ci.dst;
      while (hr < 0.0) {
        curtimer--;
        hr += 24.0;
      }
      while (hr >= 24.0) {
        curtimer++;
        hr -= 24.0;
      }
      JulianToMdy((real)curtimer, mon, day, yea);
      *tim = HMS(hr, min, sec);
    }
  }
  is.fDst = (dst > 0.0);
#endif // PC
}
#endif // TIME


// Given a string representing the complete pathname to a file, strip off all
// the path information leaving just the filename itself. This is called by
// the main program to determine the name of the Astrolog executable.

char *SzProcessProgname(char *szPath)
{
  char *pchStart, *pch, *pchEnd;

  pchStart = pch = szPath;
  while (*pch) {
#ifdef PC
    *pch = ChUncap(*pch);    // Because PC filenames are case insensitive.
#endif
    pch++;
  }
  pchEnd = pch;
  while (pch > pchStart && *pch != '.')
    pch--;
  if (pch > pchStart)
    *pch = 0;
  else
    pch = pchEnd;
  while (pch > pchStart && *pch != chDirSep)
    pch--;
  if (pch > pchStart)
    szPath = pch+1;
  return szPath;
}


// Append a set of chart information to the program's chart list.

flag FAppendCIList(CONST CI *pci)
{
  CI *pciNew = NULL;
  int cciAlloc;

  // Do nothing if exactly the same as the last chart in the list.
  if (is.cci > 0) {
    pciNew = &is.rgci[is.cci-1];
    if (FEqCI((*pci), (*pciNew)))
      return fTrue;
  }

  // Extend the size of the chart list allocation if necessary.
  if (is.cci >= is.cciAlloc) {
    cciAlloc = is.cciAlloc + 500;
    pciNew = RgAllocate(cciAlloc, CI, "chart list");
    if (pciNew == NULL)
      return fFalse;
    if (is.rgci != NULL) {
      CopyRgb((pbyte)is.rgci, (pbyte)pciNew, sizeof(CI)*is.cci);
      DeallocateP(is.rgci);
    }
    is.rgci = pciNew;
    is.cciAlloc = cciAlloc;
  }

  // Append the chart to the list, and increment the list size.
  is.rgci[is.cci] = *pci;
  is.cci++;
  return fTrue;
}



#define cShellGap 9  // Sequence A102549
CONST int rgnShellGap[] = {1, 4, 10, 23, 57, 132, 301, 701, 1750};

// Sort all charts in the chart list by the specified method.

flag FSortCIList(int nMethod)
{
  int ig, gap, i, j;
  CI ciT, *pci1, *pci2;
  real *rgr = NULL, rT;
  flag fCompare;
#ifdef EXPRESS
  CP cpSav;
#endif

  // Empty lists or length 1 lists are already sorted.
  if (is.cci <= 1)
    return fTrue;
  Assert(FBetween(nMethod, 0, 5));

  // Sorting by AstroExpression involves actually casting each chart, so
  // planet positions can be looked at by the custom sorting criterion.
  if (nMethod == 5) {
    rgr = RgAllocate(is.cci, real, "sort keys");
    if (rgr == NULL)
      return fFalse;
    for (i = 0; i < is.cci; i++) {
      rgr[i] = (real)i;
#ifdef EXPRESS
      // Set sort weight based on AstroExpression.
      if (!us.fExpOff && FSzSet(us.szExpListS)) {
        cpSav = cp0; ciT = ciCore;
        ciCore = is.rgci[i];
        CastChart(-1);
        ExpSetN(iLetterZ, i);
        rgr[i] = RParseExpression(us.szExpListS);
        ciCore = ciT;
        cp0 = cpSav;
      }
#endif
    }
  }

  // Actually sort the chart list, using shell sort.
  pci2 = &ciT;
  for (ig = cShellGap-1; ig >= 0; ig--) {
    gap = rgnShellGap[ig];
    for (i = gap; i < is.cci; i++) {
      ciT = is.rgci[i];
      if (nMethod == 5)
        rT = rgr[i];
      for (j = i; j >= gap; j -= gap) {
        pci1 = &is.rgci[j - gap];
        switch (nMethod) {
        case 0:
          fCompare = (pci1->yea > pci2->yea ||
            (pci1->yea == pci2->yea && (pci1->mon > pci2->mon ||
            (pci1->mon == pci2->mon && (pci1->day > pci2->day ||
            (pci1->day == pci2->day && pci1->tim > pci2->tim))))));
          break;
        case 1:
          fCompare = (pci1->lon < pci2->lon);
          break;
        case 2:
          fCompare = (pci1->lat < pci2->lat);
          break;
        case 3:
          fCompare = (NCompareSz(pci1->nam, pci2->nam) > 0);
          break;
        case 4:
          fCompare = (NCompareSz(pci1->loc, pci2->loc) > 0);
          break;
        case 5:
          fCompare = (rT < rgr[j - gap]);
          if (fCompare)
            rgr[j] = rgr[j - gap];
          break;
        }
        if (!fCompare)
          break;
        is.rgci[j] = is.rgci[j - gap];
      }
      is.rgci[j] = ciT;
      if (nMethod == 5)
        rgr[j] = rT;
    }
  }
  DeallocatePIf(rgr);
  return fTrue;
}


// Filter the program's chart list to those charts that meet a criteria,
// deleting all other charts from the list that don't meet it.

void FilterCIList(CONST char *szName, CONST char *szLocation)
{
  int i, j, cciNew = 0;
  CI *pci;
#ifdef EXPRESS
  CI ciSav[3];
  CP cpSav[3];

  for (i = 0; i <= 2; i++) {
    ciSav[i] = *rgpci[i];
    cpSav[i] = *rgpcp[i];
  }
  // Cast chart once ahead of time for chart slot #2 that won't be changing.
  ciCore = ciTwin;
  CastChart(0);
  cp2 = cp0;
#endif

  // Loop over each chart in chart list.
  for (i = 0; i < is.cci; i++) {
    pci = &is.rgci[i];
    // Chart must have both the name and location strings within it.
    if (*szName) {
      for (j = 0; pci->nam[j]; j++)
        if (FEqSzSubI(szName, &pci->nam[j]))
          break;
      if (!pci->nam[j])
        continue;
    }
    if (*szLocation) {
      for (j = 0; pci->loc[j]; j++)
        if (FEqSzSubI(szLocation, &pci->loc[j]))
          break;
      if (!pci->loc[j])
        continue;
    }
#ifdef EXPRESS
    // May want to skip current chart if AstroExpression says to do so.
    if (!us.fExpOff && FSzSet(us.szExpListF)) {
      ciCore = ciMain = *pci;
      CastChart(-1);
      cp1 = cp0;
      ExpSetN(iLetterZ, i);
      if (!NParseExpression(us.szExpListF))
        continue;
    }
#endif
    CopyRgb((pbyte)&is.rgci[i], (pbyte)&is.rgci[cciNew], sizeof(CI));
    cciNew++;
  }

  is.cci = cciNew;
#ifdef EXPRESS
  for (i = 0; i <= 2; i++) {
    *rgpci[i] = ciSav[i];
    *rgpcp[i] = cpSav[i];
  }
#endif
}


// Enumerate over all charts in the chart list. Implements the -Y5[2-4]
// command switch.

flag FEnumerateCIList(int nListAll)
{
#ifdef EXPRESS
  int iList, iList2, i;
  CI ciSav[4];
  CP cpSav[4];

  // Save chart data that will be edited.
  Assert(FBetween(nListAll, 1, 4));
  if (!(!us.fExpOff && FSzSet(us.szExpListY)))
    return fFalse;
  for (i = 0; i <= 2; i++) {
    ciSav[i] = *rgpci[i];
    cpSav[i] = *rgpcp[i];
  }

  // Cast chart once ahead of time for chart slot that won't be changing.
  if (nListAll <= 2) {
    i = (nListAll <= 1) * 2;
    ciCore = ciSav[i];
    CastChart(0);
    *rgpcp[i] = cp0;
    if (i == 0)
      cp1 = cp0;
  }

  // Loop over all charts in chart list.
  iList = (nListAll == 3); iList2 = 0;
  do {
    is.iciIndex1 = iList; is.iciIndex2 = iList2;
    if (nListAll == 1)
      ciCore = ciMain = is.rgci[iList];
    else if (nListAll == 2)
      ciTwin = is.rgci[iList];
    else {
      ciCore = ciMain = is.rgci[iList];
      ciTwin = is.rgci[iList2];
    }
    if (nListAll != 2) {
      CastChart(-1);
      cp1 = cp0;
    }
    if (nListAll > 1) {
      ciSav[3] = ciCore; cpSav[3] = cp0;
      ciCore = ciTwin;
      CastChart(-1);
      cp2 = cp0;
      ciCore = ciSav[3]; cp0 = cpSav[3];
    }
    // Call ~5Y switch AstroExpression so user can see current chart.
    ParseExpression(us.szExpListY);
    if (nListAll >= 3) {
      iList2++;
      if (iList2 < (nListAll >= 4 ? is.cci : iList))
        continue;
      iList2 = 0;
    }
    iList++;
  } while (iList < is.cci);

  // Restore chart data.
  is.iciIndex1 = is.iciIndex2 = -1;
  for (i = 0; i <= 2; i++) {
    *rgpci[i] = ciSav[i];
    *rgpcp[i] = cpSav[i];
  }
  return fTrue;
#else
  return fFalse;
#endif
}


/*
******************************************************************************
** Character Encoding Procedures.
******************************************************************************
*/

// Characters in 128-159 byte range of codepage Windows-1252
CONST wchar rgwch1252[32] = {
  0x20ac, 0,      0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
  0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0,      0x017d, 0,
  0,      0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
  0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0,      0x017e, 0x0178};

// Characters in 128-255 byte range of codepage IBM-437
CONST wchar rgwch437[128] = {
  0xc7,   0xfc,   0xe9,   0xe2,   0xe4,   0xe0,   0xe5,   0xe7,
  0xea,   0xeb,   0xe8,   0xef,   0xee,   0xec,   0xc4,   0xc5,
  0xc9,   0xe6,   0xc6,   0xf4,   0xf6,   0xf2,   0xfb,   0xf9,
  0xff,   0xd6,   0xdc,   0xa2,   0xa3,   0xa5,   0x20a7, 0x0192,
  0xe1,   0xed,   0xf3,   0xfa,   0xf1,   0xd1,   0xaa,   0xba,
  0xbf,   0x2310, 0xac,   0xbd,   0xbc,   0xa1,   0xab,   0xbb,
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
  0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
  0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
  0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
  0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
  0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
  0x03b1, 0xdf,   0x0393, 0x03c0, 0x03a3, 0x03c3, 0xb5,   0x03c4,
  0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
  0x2261, 0xb1,   0x2265, 0x2264, 0x2320, 0x2321, 0xf7,   0x2248,
  0xb0,   0x2219, 0xb7,   0x221a, 0x207f, 0xb2,   0x25a0, 0xa0};

// Convert a UTF8 byte sequence to Unicode, returning number of bytes in it.

int UTF8ToWch(CONST uchar *pch, wchar *pwch)
{
  int ch1, ch2, ch3;

  // 1 byte UTF8 sequence: Characters 0-127
  ch1 = pch[0];
  if (ch1 < 0x80) {
    if (pwch != NULL)
      *pwch = ch1;
    return 1;
  }

  // 2 byte UTF8 sequence: Characters 0x80 - 0x7FF
  ch2 = pch[1];
  if ((ch1 & 0xe0) == 0xc0 && (ch2 & 0xc0) == 0x80) {
    if (pwch != NULL)
      *pwch = ((ch1 & 0x1f) << 6) | (ch2 & 0x3f);
    return 2;
  }

  // 3 byte UTF8 sequence: Characters 0x800 - 0xFFFF
  ch3 = pch[2];
  if ((ch1 & 0xf0) == 0xe0 && (ch2 & 0xc0) == 0x80 && (ch3 & 0xc0) == 0x80) {
    if (pwch != NULL)
      *pwch = ((ch1 & 0x0f) << 12) | ((ch2 & 0x3f) << 6) | (ch3 & 0x3f);
    return 3;
  }

  // Treat illegal UTF8 sequence as one byte character
  if (pwch != NULL)
    *pwch = ch1;
  return 1;
}


// Convert a wide Unicode character to UTF8, returning the number of bytes.

int WchToUTF8(wchar wch, char *sz)
{
  // 1 byte UTF8 sequence: Characters 0-127
  if (wch < 0x80) {
    sprintf(sz, "%c", (uchar)wch);
    return 1;
  }

  // 2 byte UTF8 sequence: Characters 0x80 - 0x7FF
  if (wch < 0x800) {
    sprintf(sz, "%c%c", (uchar)(0xc0 | (wch >> 6)),
      (uchar)(0x80 | (wch & 0x3f)));
    return 2;
  }

  // 3 byte UTF8 sequence: Characters 0x800 - 0xFFFF
  sprintf(sz, "%c%c%c", (uchar)(0xe0 | (wch >> 12)),
    (uchar)(0x80 | ((wch >> 6) & 0x3f)), (uchar)(0x80 | (wch & 0x3f)));
  return 3;
}


// Convert a character in the Windows-1252 codepage to Unicode.

wchar WchFromChLatin(uchar ch)
{
  if (!FBetween(ch, 128, 159))
    return ch;
  return rgwch1252[ch - 128];
}


// Convert a character in the IBM codepage to Unicode.

wchar WchFromChIBM(uchar ch)
{
  if (ch < 128)
    return ch;
  return rgwch437[ch - 128];
}


// Convert a Unicode character to low-Ascii if possible, by translating to the
// equivalent letter without an accent or other diacritical mark.

uchar ChLowAscii(wchar wch)
{
  if (FBetween(wch, 0, 127))
    return (uchar)wch;
  // This translation table originally public domain code by Alois Treindl.
  switch (wch) {
  case 0x00e1: return 'a';  // Latin Small Letter A with acute
  case 0x00c1: return 'A';  // Latin Capital Letter A with acute
  case 0x00e2: return 'a';  // Latin Small Letter A with circumflex
  case 0x00c2: return 'A';  // Latin Capital Letter A with circumflex
  case 0x00e0: return 'a';  // Latin Small Letter A with grave
  case 0x00c0: return 'A';  // Latin Capital Letter A with grave
  case 0x00e4: return 'a';  // Latin Small Letter A with umlaut
  case 0x00c4: return 'A';  // Latin Capital Letter A with umlaut
  case 0x00e5: return 'a';  // Latin Small Letter A with ring above
  case 0x00c5: return 'A';  // Latin Capital Letter A with ring above
  case 0x00e3: return 'a';  // Latin Small Letter A with tilde
  case 0x00c3: return 'A';  // Latin Capital Letter A with tilde
  case 0x00e6: return 'a';  // Latin Small Letter AE
  case 0x00c6: return 'A';  // Latin Capital Letter AE
  case 0x0101: return 'a';  // Latin Small Letter A with macron
  case 0x0100: return 'A';  // Latin Capital Letter A with macron
  case 0x0103: return 'a';  // Latin Small Letter A with breve
  case 0x0102: return 'A';  // Latin Capital Letter A with breve
  case 0x0105: return 'a';  // Latin Small Letter A with ogonek
  case 0x0104: return 'A';  // Latin Capital Letter A with ogonek
  case 0x00c7: return 'C';  // Latin Capital Letter C with cedilla
  case 0x00e7: return 'c';  // Latin Small Letter C with cedilla
  case 0x0106: return 'C';  // Latin Capital Letter C with acute
  case 0x0107: return 'c';  // Latin Small Letter C with acute
  case 0x0108: return 'C';  // Latin Capital Letter C with circumflex
  case 0x0109: return 'c';  // Latin Small Letter C with circumflex
  case 0x010c: return 'C';  // Latin Capital Letter C with caron
  case 0x010d: return 'c';  // Latin Small Letter C with caron
  case 0x010e: return 'D';  // Latin Capital Letter D with caron
  case 0x010f: return 'd';  // Latin Small Letter D with caron
  case 0x0110: return 'D';  // Latin Capital Letter D with stroke
  case 0x0111: return 'd';  // Latin Small Letter D with stroke
  case 0x00e9: return 'e';  // Latin Small Letter E with acute
  case 0x00c9: return 'E';  // Latin Capital Letter E with acute
  case 0x00ea: return 'e';  // Latin Small Letter E with circumflex
  case 0x00ca: return 'E';  // Latin Capital Letter E with circumflex
  case 0x00e8: return 'e';  // Latin Small Letter E with grave
  case 0x00c8: return 'E';  // Latin Capital Letter E with grave
  case 0x00eb: return 'e';  // Latin Small Letter E with umlaut
  case 0x00cb: return 'E';  // Latin Capital Letter E with umlaut
  case 0x0113: return 'e';  // Latin Small Letter E with macron
  case 0x0112: return 'E';  // Latin Capital Letter E with macron
  case 0x0115: return 'e';  // Latin Small Letter E with breve
  case 0x0114: return 'E';  // Latin Capital Letter E with breve
  case 0x0119: return 'e';  // Latin Small Letter E with ogonek
  case 0x0118: return 'E';  // Latin Capital Letter E with ogonek
  case 0x0117: return 'e';  // Latin Small Letter E with dot above
  case 0x0116: return 'E';  // Latin Capital Letter E with dot above
  case 0x011b: return 'e';  // Latin Small Letter E with caron
  case 0x011a: return 'E';  // Latin Capital Letter E with caron
  case 0x011c: return 'G';  // Latin Capital Letter G with circumflex
  case 0x011d: return 'g';  // Latin Small Letter G with circumflex
  case 0x011e: return 'G';  // Latin Capital Letter G with breve
  case 0x011f: return 'g';  // Latin Small Letter G with breve
  case 0x0120: return 'G';  // Latin Capital Letter G with dot above
  case 0x0121: return 'g';  // Latin Small Letter G with dot above
  case 0x0122: return 'G';  // Latin Capital Letter G with cedilla
  case 0x0123: return 'g';  // Latin Small Letter G with cedilla
  case 0x0124: return 'H';  // Latin Capital Letter H with circumflex
  case 0x0125: return 'h';  // Latin Small Letter H with circumflex
  case 0x0126: return 'H';  // Latin Capital Letter H with stroke
  case 0x0127: return 'h';  // Latin Small Letter H with stroke
  case 0x00ed: return 'i';  // Latin Small Letter I with acute
  case 0x00cd: return 'I';  // Latin Capital Letter I with acute
  case 0x00ee: return 'i';  // Latin Small Letter I with circumflex
  case 0x00ce: return 'I';  // Latin Capital Letter I with circumflex
  case 0x00ec: return 'i';  // Latin Small Letter I with grave
  case 0x00cc: return 'I';  // Latin Capital Letter I with grave
  case 0x00ef: return 'i';  // Latin Small Letter I with umlaut
  case 0x00cf: return 'I';  // Latin Capital Letter I with umlaut
  case 0x0129: return 'i';  // Latin Small Letter I with tilde
  case 0x0128: return 'I';  // Latin Capital Letter I with tilde
  case 0x012b: return 'i';  // Latin Small Letter I with macron
  case 0x012a: return 'I';  // Latin Capital Letter I with macron
  case 0x012d: return 'i';  // Latin Small Letter I with breve
  case 0x012c: return 'I';  // Latin Capital Letter I with breve
  case 0x012f: return 'i';  // Latin Small Letter I with ogonek
  case 0x012e: return 'I';  // Latin Capital Letter I with ogonek
  case 0x0131: return 'i';  // Latin Small Letter dotless I
  case 0x0130: return 'I';  // Latin Capital Letter I with dot above
  case 0x0134: return 'J';  // Latin Capital Letter J with circumflex
  case 0x0135: return 'j';  // Latin Small Letter J with circumflex
  case 0x0136: return 'K';  // Latin Capital Letter K with cedilla
  case 0x0137: return 'k';  // Latin Small Letter K with cedilla
  case 0x0138: return 'k';  // Latin Small Letter Kra
  case 0x0139: return 'L';  // Latin Capital Letter L with acute
  case 0x013a: return 'l';  // Latin Small Letter L with acute
  case 0x013b: return 'L';  // Latin Capital Letter L with cedilla
  case 0x013c: return 'l';  // Latin Small Letter L with cedilla
  case 0x013d: return 'L';  // Latin Capital Letter L with caron
  case 0x013e: return 'l';  // Latin Small Letter L with caron
  case 0x013f: return 'L';  // Latin Capital Letter L with middle dot
  case 0x0140: return 'l';  // Latin Small Letter L with middle dot
  case 0x0141: return 'L';  // Latin Capital Letter L with stroke
  case 0x0142: return 'l';  // Latin Small Letter L with stroke
  case 0x00f1: return 'n';  // Latin Small Letter N with tilde
  case 0x00d1: return 'N';  // Latin Capital Letter N with tilde
  case 0x0143: return 'N';  // Latin Capital Letter N with acute
  case 0x0144: return 'n';  // Latin Small Letter N with acute
  case 0x0145: return 'N';  // Latin Capital Letter N with cedilla
  case 0x0146: return 'n';  // Latin Small Letter N with cedilla
  case 0x0147: return 'N';  // Latin Capital Letter N with caron
  case 0x0148: return 'n';  // Latin Small Letter N with caron
  case 0x0149: return 'n';  // Latin Small Letter N preceded by apostrophe
  case 0x014a: return 'N';  // Latin Capital Letter Eng
  case 0x014b: return 'n';  // Latin Small Letter Eng
  case 0x00f3: return 'o';  // Latin Small Letter O with acute
  case 0x00d3: return 'O';  // Latin Capital Letter O with acute
  case 0x00f4: return 'o';  // Latin Small Letter O with circumflex
  case 0x00d4: return 'O';  // Latin Capital Letter O with circumflex
  case 0x00f2: return 'o';  // Latin Small Letter O with grave
  case 0x00d2: return 'O';  // Latin Capital Letter O with grave
  case 0x00f6: return 'o';  // Latin Small Letter O with umlaut
  case 0x00d6: return 'O';  // Latin Capital Letter O with umlaut
  case 0x00f5: return 'o';  // Latin Small Letter O with tilde
  case 0x00d5: return 'O';  // Latin Capital Letter O with tilde
  case 0x00f8: return 'o';  // Latin Small Letter O with stroke
  case 0x00d8: return 'O';  // Latin Capital Letter O with stroke
  case 0x0151: return 'o';  // Latin Small Letter O with double acute
  case 0x0150: return 'O';  // Latin Capital Letter O with double acute
  case 0x0153: return 'o';  // Latin Small Ligature OE
  case 0x0152: return 'O';  // Latin Capital Ligature OE
  case 0x014d: return 'o';  // Latin Small Letter O with macron
  case 0x014c: return 'O';  // Latin Capital Letter O with macron
  case 0x014f: return 'o';  // Latin Small Letter O with breve
  case 0x014e: return 'O';  // Latin Capital Letter O with breve
  case 0x0154: return 'R';  // Latin Capital Letter R with acute
  case 0x0155: return 'r';  // Latin Small Letter R with acute
  case 0x0156: return 'R';  // Latin Capital Letter R with cedilla
  case 0x0157: return 'r';  // Latin Small Letter R with cedilla
  case 0x0158: return 'R';  // Latin Capital Letter R with caron
  case 0x0159: return 'r';  // Latin Small Letter R with caron
  case 0x015a: return 'S';  // Latin Capital Letter S with acute
  case 0x015b: return 's';  // Latin Small Letter S with acute
  case 0x00df: return 's';  // Latin Small Letter sharp S
  case 0x015c: return 'S';  // Latin Capital Letter S with circumflex
  case 0x015d: return 's';  // Latin Small Letter S with circumflex
  case 0x015e: return 'S';  // Latin Capital Letter S with cedilla
  case 0x015f: return 's';  // Latin Small Letter S with cedilla
  case 0x0160: return 'S';  // Latin Capital Letter S with caron
  case 0x0161: return 's';  // Latin Small Letter S with caron
  case 0x017f: return 's';  // Latin Small Letter long S
  case 0x0162: return 'T';  // Latin Capital Letter T with cedilla
  case 0x0163: return 't';  // Latin Small Letter T with cedilla
  case 0x0164: return 'T';  // Latin Capital Letter T with caron
  case 0x0165: return 't';  // Latin Small Letter T with caron
  case 0x0166: return 'T';  // Latin Capital Letter T with stroke
  case 0x0167: return 't';  // Latin Small Letter T with stroke
  case 0x00fa: return 'u';  // Latin Small Letter U with acute
  case 0x00da: return 'U';  // Latin Capital Letter U with acute
  case 0x00fb: return 'u';  // Latin Small Letter U with circumflex
  case 0x00db: return 'U';  // Latin Capital Letter U with circumflex
  case 0x00f9: return 'u';  // Latin Small Letter U with grave
  case 0x00d9: return 'U';  // Latin Capital Letter U with grave
  case 0x00fc: return 'u';  // Latin Small Letter U with umlaut
  case 0x00dc: return 'U';  // Latin Capital Letter U with umlaut
  case 0x0171: return 'u';  // Latin Small Letter U with double acute
  case 0x0170: return 'U';  // Latin Capital Letter U with double acute
  case 0x0169: return 'u';  // Latin Small Letter U with tilde
  case 0x0168: return 'U';  // Latin Capital Letter U with tilde
  case 0x016b: return 'u';  // Latin Small Letter U with macron
  case 0x016a: return 'U';  // Latin Capital Letter U with macron
  case 0x016d: return 'u';  // Latin Small Letter U with breve
  case 0x016c: return 'U';  // Latin Capital Letter U with breve
  case 0x0173: return 'u';  // Latin Small Letter U with ogonek
  case 0x0172: return 'U';  // Latin Capital Letter U with ogonek
  case 0x016f: return 'u';  // Latin Small Letter U with ring above
  case 0x016e: return 'U';  // Latin Capital Letter U with ring above
  case 0x0174: return 'W';  // Latin Capital Letter W with circumflex
  case 0x0175: return 'w';  // Latin Small Letter W with circumflex
  case 0x00ff: return 'y';  // Latin Small Letter Y with umlaut
  case 0x0178: return 'Y';  // Latin Capital Letter Y with umlaut
  case 0x0177: return 'y';  // Latin Small Letter Y with circumflex
  case 0x0176: return 'Y';  // Latin Capital Letter Y with circumflex
  case 0x0179: return 'Z';  // Latin Capital Letter Z with acute
  case 0x017a: return 'z';  // Latin Small Letter Z with acute
  case 0x017b: return 'Z';  // Latin Capital Letter Z with dot above
  case 0x017c: return 'z';  // Latin Small Letter Z with dot above
  case 0x017d: return 'Z';  // Latin Capital Letter Z with caron
  case 0x017e: return 'z';  // Latin Small Letter Z with caron
  }
  // If nothing close, only then display character as a "?".
  return '?';
}


// Convert a Unicode character to a byte in the Windows-1252 codepage.

uchar ChLatinFromWch(wchar wch)
{
  int i;

  if (FBetween(wch, 0, 255) && !FBetween(wch, 128, 159))
    return (uchar)wch;
  for (i = 0; i < 32; i++)
    if (wch == rgwch1252[i])
      return 128 + i;
  return (wch < 256 ? wch : ChLowAscii(wch));
}


// Convert a Unicode character to a byte in the IBM-437 codepage.

uchar ChIBMFromWch(wchar wch)
{
  int i;

  if (FBetween(wch, 0, 127))
    return (uchar)wch;
  for (i = 0; i < 128; i++)
    if (wch == rgwch437[i])
      return 128 + i;
  return (wch < 256 ? wch : ChLowAscii(wch));
}


// Convert a UTF8 string to a string in the Windows-1252 codepage, in place.
// The converted string may be shorter, since UTF8 has multibyte characters.

void UTF8ToLatinSz(char *sz)
{
  uchar *pchIn, *pchOut;
  wchar wch;
  int dch;

  for (pchIn = pchOut = (uchar *)sz; *pchIn; pchIn += dch, pchOut++) {
    dch = UTF8ToWch(pchIn, &wch);
    *pchOut = ChLatinFromWch(wch);
  }
  *pchOut = chNull;
}


// Convert a UTF8 string to a string in the IBM-437 codepage, in place. The
// converted string may be shorter, since UTF8 has multibyte characters.

void UTF8ToIBMSz(char *sz)
{
  uchar *pchIn, *pchOut;
  wchar wch;
  int dch;

  for (pchIn = pchOut = (uchar *)sz; *pchIn; pchIn += dch, pchOut++) {
    dch = UTF8ToWch(pchIn, &wch);
    *pchOut = ChIBMFromWch(wch);
  }
  *pchOut = chNull;
}


// Convert a UTF8 string to a different codepage, if needed.

void ConvertSzFromUTF8(char *sz)
{
  if (us.nCharset == ccIBM)
    UTF8ToIBMSz(sz);
  else if (us.nCharset < ccUTF8)
    UTF8ToLatinSz(sz);
}


// Convert a string to Windows-1252, from whatever encoding it might be in.

CONST char *ConvertSzToLatin(CONST char *sz, char *szBuf, int cchBuf)
{
  char *pch;

  if (us.nCharset <= ccNone || us.nCharset == ccLatin)
    return sz;

  // Convert string to Latin if not that format already.
  CopyRgchToSz(sz, CchSz(sz), szBuf, cchBuf);
  if (us.nCharset >= ccUTF8)
    UTF8ToLatinSz(szBuf);
  else
    for (pch = szBuf; *pch; pch++)
      *pch = ChLatinFromWch(WchFromChIBM(*pch));
  return szBuf;
}


/*
******************************************************************************
** System Procedures.
******************************************************************************
*/

// Copy a source string to a destination location, taking care of destination
// buffer reallocation and frees when needed.

flag FCloneSzCore(CONST char *szSrc, char **pszDst, flag fDestConst)
{
  char *szNew;
  int cbDst, cbNew;

  Assert(pszDst != NULL);
  if (szSrc != NULL) {
    cbNew = CchSz(szSrc)+1;
    if (*pszDst != NULL && !fDestConst) {
      // If destination buffer large enough, just use it instead.
      cbDst = CchSz(*pszDst)+1;
      if (cbDst >= cbNew) {
        CopyRgb((pbyte)szSrc, (pbyte)*pszDst, cbNew);
        return fTrue;
      }
    }
    // Allocate new buffer for string to be cloned.
    szNew = (char *)PAllocate(cbNew, "string");
    if (szNew == NULL)
      return fFalse;
    CopyRgb((pbyte)szSrc, (pbyte)szNew, cbNew);
  } else
    szNew = NULL;
  if (*pszDst != NULL && !fDestConst)
    DeallocateP(*pszDst);
  *pszDst = szNew;
  return fTrue;
}


// Given a string, return a pointer to a persistent version of it, in which
// "persistent" means its contents won't be invalidated when the stack frame
// changes. Strings such as chart names and locations need to be in their own
// space and can't just be local variables in a function reading them in.

char *SzClone(char *szSrc)
{
  char *szNew;
  int cb;

  // Make a copy of the string and use it.
  cb = CchSz(szSrc)+1;
  szNew = (char *)PAllocate(cb, "string");
  if (szNew != NULL) {
    CopyRgb((pbyte)szSrc, (pbyte)szNew, cb);
    is.cAlloc--;
    is.cAllocTotal--;
    is.cbAllocSize -= cb;
  }
  return szNew;
}


// This is Astrolog's memory allocation routine, returning a pointer given
// a size, and a string to use when printing error if the allocation fails.

pbyte PAllocate(long cb, CONST char *szType)
{
  char szT[cchSzDef];
  pbyte pb;

#ifdef DEBUG
  pb = (pbyte)PAllocateCore(cb + sizeof(dword)*3);
#else
  pb = (pbyte)PAllocateCore(cb);
#endif

  // Handle success or failure of the allocation.
  Assert(szType != NULL);
  if (pb == NULL) {
    sprintf(szT, "%s: Not enough memory for %s (%ld bytes).",
      szAppName, szType, cb);
    PrintWarning(szT);
  } else {
    is.cAlloc++;
    is.cAllocTotal++;
    is.cbAllocSize += cb;
  }

#ifdef DEBUG
  // Put sentinels at ends of allocation to check for buffer overruns.
  *(dword *)pb = dwCanary;
  *(dword *)(pb + sizeof(dword)) = cb;
  *(dword *)(pb + sizeof(dword)*2 + cb) = dwCanary;
  return pb + sizeof(dword)*2;
#else
  return pb;
#endif
}


// Free a memory buffer allocated with PAllocate().

void DeallocateP(void *pv)
{
  Assert(pv != NULL);
#ifdef DEBUG
  // Ensure buffer wasn't overrun during its existence.
  pbyte pbSys;
  dword lcb, dw;

  pbSys = (pbyte)pv - sizeof(dword)*2;
  Assert(pbSys != NULL);
  dw = *(dword *)pbSys;
  Assert(dw == dwCanary);
  lcb = *(dword *)(pbSys + sizeof(dword));
  dw = *(dword *)((pbyte)pv + lcb);
  Assert(dw == dwCanary);
  is.cbAllocSize -= lcb;
  DeallocatePCore(pbSys);
#else
  DeallocatePCore(pv);
#endif
  is.cAlloc--;
}


// Change the size of a memory allocation, containing a list of cElem items of
// cbElem size, to a list of cElemNew items.

pbyte RgReallocate(void *rgElem, int cElem, int cbElem, int cElemNew,
  CONST char *szType)
{
  pbyte rgElemNew;

  rgElemNew = PAllocate(cElemNew * cbElem, szType);
  if (rgElemNew == NULL)
    return NULL;
  ClearB(rgElemNew, cElemNew * cbElem);
  if (rgElem != NULL)
    CopyRgb((pbyte)rgElem, rgElemNew, Min(cElem, cElemNew) * cbElem);
  return rgElemNew;
}


// Ensure there are at least the given number of slots available in the
// command switch macro string array, reallocating if needed.

flag FEnsureMacro(int cszNew)
{
  char **rgszT;

  if (cszNew <= is.cszMacro)
    return fTrue;

  if (cszNew <= 48)
    cszNew = 48 + 1;
  rgszT = (char **)RgReallocate(is.rgszMacro, is.cszMacro,
    sizeof(char *), cszNew, "macro list");
  if (rgszT == NULL)
    return fFalse;
  DeallocatePIf(is.rgszMacro);
  is.rgszMacro = rgszT;
  is.cszMacro = cszNew;
  return fTrue;
}


#ifdef DEBUG
// Assert a condition. If not, display an error message.

void Assert(flag f)
{
  if (!f)
    PrintError("Debug Assert failed!\n");
}
#endif


// Exit the program, and do any cleanup necessary. Note that if there was
// a non-fatal error, and were in the -Q loop mode, then won't actually
// terminate the program, but rather drop back to the command line loop.

void Terminate(int tc)
{
  char sz[cchSzDef];

  if (us.fNoQuit)
    return;
  if (tc == tcForce) {
    is.S = stdout;
    AnsiColor(kWhiteA);
    sprintf(sz, "\n%s %s exited.\n", szAppName, szVersionCore);
    PrintSz(sz);
  }
  if (tc == tcError && us.fLoop)
    return;
  if (us.fAnsiColor) {
    sprintf(sz, "%c[0m", chEscape);    // Get out of any Ansi color mode.
    PrintSz(sz);
  }
  FinalizeProgram(tc != tcOK);
  exit(NAbs(tc));
}

/* general.cpp */
