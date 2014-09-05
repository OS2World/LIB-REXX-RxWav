/*  RxWav
   Copyright (C) 1999  Giorgio Vicario

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA     */

#define INCL_REXXSAA
#include <os2emx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regexp.h>
#include <math.h>
#include <float.h>
#include "RxWav.h"

/***********************************************************************
Filtro passa banda:
***********************************************************************/


ULONG
WavFilterBand (PCSZ name, LONG argc, const RXSTRING * argv,
	       PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pFrom = NULL;
  PSHORT pTo = NULL;
  ULONG nCamp, i;
  APIRET rc;
  double d;
  float centro, larghezza, amp;
  double A, B, C;
  double out1, out2;
  long l;

  if (argc != 5)
    {
      SendMsg (FUNC_FILTER_BAND, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pFrom))
    {
      SendMsg (FUNC_FILTER_BAND, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_FILTER_BAND, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  centro = atof (argv[2].strptr);
  if ((centro < 1) || (centro > (FreqCamp / 2)))
    {
      SendMsg (FUNC_FILTER_BAND, ERR_FREQUENZA_CENTRALE);
      return INVALID_ROUTINE;
    }

  larghezza = atof (argv[3].strptr);
  if ((larghezza < 1) || (larghezza > centro))
    {
      SendMsg (FUNC_FILTER_BAND, ERR_LARGHEZZA_BANDA);
      return INVALID_ROUTINE;
    }

  amp = atof (argv[4].strptr);
  if ((amp > 1) || (amp < 0))
    {
      SendMsg (FUNC_FILTER_BAND, ERR_AMPIEZZA_FILTRO);
      return INVALID_ROUTINE;
    }

  pFrom = (PSHORT) AllineaCh (pFrom, nCamp, FUNC_FILTER_BAND);
  if (!pFrom)
    return INVALID_ROUTINE;
  pTo = pFrom;


  C = exp (-2 * M_PI * larghezza / FreqCamp);
  B = -4 * C / (1 + C) * cos (2 * M_PI * centro / FreqCamp);
  A = sqrt (((1 + C) * (1 + C) - B * B) * (1 - C) / (1 + C));
  out1 = out2 = 0.0;

  for (i = nCamp; i != 0; i--)
    {
      l = *pFrom++;
      d = (A * l - B * out1) - C * out2;
      out2 = out1;
      out1 = d;
      *pTo = d * amp + l * (1 - amp);
      *pTo++;
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}



/***********************************************************************
Filtro passa alto:
***********************************************************************/


ULONG
WavFilterHigh (PCSZ name, LONG argc, const RXSTRING * argv,
	       PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pFrom = NULL;
  PSHORT pTo = NULL;
  ULONG nCamp, i;
  APIRET rc;
  float centro, amp;
  double A, B, d, t1;
  long l1;

  if (argc != 4)
    {
      SendMsg (FUNC_FILTER_HIGH, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pFrom))
    {
      SendMsg (FUNC_FILTER_HIGH, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_FILTER_HIGH, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  centro = atof (argv[2].strptr);
  if ((centro < 1) || (centro > (FreqCamp / 2)))
    {
      SendMsg (FUNC_FILTER_HIGH, ERR_FREQUENZA_CENTRALE);
      return INVALID_ROUTINE;
    }

  amp = atof (argv[3].strptr);
  if ((amp > 1) || (amp < -1))
    {
      SendMsg (FUNC_FILTER_HIGH, ERR_AMPIEZZA_FILTRO);
      return INVALID_ROUTINE;
    }

  pFrom = (PSHORT) AllineaCh (pFrom, nCamp, FUNC_FILTER_HIGH);
  if (!pFrom)
    return INVALID_ROUTINE;
  pTo = pFrom;
  A = (M_PI * 2.0 * centro) / FreqCamp;
  B = exp (-A / FreqCamp);

  for (i = nCamp; i != 0; i--)
    {
      l1 = *pFrom++;
      d = (B * ((d - t1) + (double) l1)) / 65536.0;
      d *= 0.8;
      if (d > MAX_CAMPIONE)
	d = MAX_CAMPIONE;
      if (d < -MAX_CAMPIONE)
	d = -MAX_CAMPIONE;
      t1 = l1;
      *pTo++ = (d * 65536.0) + amp + (*pTo * (1 - amp));
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}



/***********************************************************************
Filtro passa basso:
***********************************************************************/


ULONG
WavFilterLow (PCSZ name, LONG argc, const RXSTRING * argv,
	      PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pFrom = NULL;
  PSHORT pTo = NULL;
  ULONG nCamp, i;
  APIRET rc;
  float centro, amp;
  double A, B, t1, d;
  long l1;

  if (argc != 4)
    {
      SendMsg (FUNC_FILTER_LOW, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pFrom))
    {
      SendMsg (FUNC_FILTER_LOW, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_FILTER_LOW, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  centro = atof (argv[2].strptr);
  if ((centro < 1) || (centro > (FreqCamp / 2)))
    {
      SendMsg (FUNC_FILTER_LOW, ERR_FREQUENZA_CENTRALE);
      return INVALID_ROUTINE;
    }

  amp = atof (argv[3].strptr);
  if ((amp > 1) || (amp < -1))
    {
      SendMsg (FUNC_FILTER_LOW, ERR_AMPIEZZA_FILTRO);
      return INVALID_ROUTINE;
    }

  pFrom = (PSHORT) AllineaCh (pFrom, nCamp, FUNC_FILTER_LOW);
  if (!pFrom)
    return INVALID_ROUTINE;
  pTo = pFrom;
  A = (M_PI * 2.0 * centro) / FreqCamp;
  B = exp (-A / FreqCamp);

  for (i = nCamp; i != 0; i--)
    {
      l1 = *pFrom++;
      d = A * (l1 / 65536) + B * (t1 / 65536);
      d *= 0.8;
      if (d > MAX_CAMPIONE)
	d = MAX_CAMPIONE;
      if (d < -MAX_CAMPIONE)
	d = -MAX_CAMPIONE;
      t1 = l1;
      *pTo++ = (d * 65536.0) + amp + (*pTo * (1 - amp));
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
