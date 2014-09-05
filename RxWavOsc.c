/* RxWav
   Copyright (C) 1999 2000  Giorgio Vicario

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
Oscillatore sinusoidale:
Vuole il puntatore ad una traccie gia' allocate, la durata in campioni,
la frequenza in hertz e l'ampiezza da 0 a 1
***********************************************************************/
ULONG
WavSinOsc (PCSZ name, LONG argc, const RXSTRING * argv,
	   PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp, nCampAggiunti;
  double f, a, j;
  double k;
  INT i, dither;
  APIRET rc;

  if ((argc < 4) | (argc > 5))
    {
      SendMsg (FUNC_SIN_OSC, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_SIN_OSC, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_SIN_OSC, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  f = atof (argv[2].strptr);

  if ((f < 1) || (f > (FreqCamp / 2)))
    {
      SendMsg (FUNC_SIN_OSC, ERR_FREQUENZA_OSCILLATORE);
      return INVALID_ROUTINE;
    }

  a = atof (argv[3].strptr);
  if ((a < 0) || (a > 1))
    {
      SendMsg (FUNC_SIN_OSC, ERR_AMPIEZZA_OSCILLATORE);
      return INVALID_ROUTINE;
    }

  if (argc > 4)
    {
      if (!strncmp (argv[4].strptr, "D", 1))
	dither = TRUE;
      else
	dither = FALSE;
    }



  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_SIN_OSC);
  if (!pCh)
    return INVALID_ROUTINE;

  j = 0;
  a = a * MAX_CAMPIONE;
  k = ((float) M_PI * 2) / (FreqCamp / f);

  if (dither)
    for (i = nCamp; i != 0; i--)
      {
	*pCh = sin (j) * a + rand () - rand ();
	pCh++;
	j = j + k;
      }
  else
    for (i = nCamp; i != 0; i--)
      {
	*pCh = sin (j) * a;
	pCh++;
	j = j + k;
      }

  nCampAggiunti = 0;
  if (sin (j) > 0)
    {
      do
	{
	  if (dither)
	    *pCh = sin (j) * a + rand () - rand ();
	  else
	    *pCh = sin (j) * a;
	  pCh++;
	  nCampAggiunti++;
	  j = j + k;
	}
      while (sin (j) > 0);
    }
  else
    {
      do
	{
	  if (dither)
	    *pCh = sin (j) * a + rand () - rand ();
	  else
	    *pCh = sin (j) * a;
	  pCh++;
	  nCampAggiunti++;
	  j = j + k;
	}
      while (sin (j) < 0);
    }


  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Oscillatore sinusoidale di test:
***********************************************************************/
ULONG
WavTestOsc (PCSZ name, LONG argc, const RXSTRING * argv,
	    PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp, nCampAggiunti;
  double f, a, j;
  double k;
  INT i;
  APIRET rc;

  if (argc != 3)
    {
      SendMsg (FUNC_TEST_OSC, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_TEST_OSC, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_TEST_OSC, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  a = atof (argv[2].strptr);
  if ((a < 0) || (a > 1))
    {
      SendMsg (FUNC_TEST_OSC, ERR_AMPIEZZA_OSCILLATORE);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_TEST_OSC);
  if (!pCh)
    return INVALID_ROUTINE;

  j = 0;
  f = 20;
  a = a * 32000;
  k = ((float) M_PI * 2) / (FreqCamp / f);

  for (i = nCamp; i != 0; i--)
    {
      *pCh = sin (j) * a;
      pCh++;
      f = f * (float) (1 + ((float) 7 / (float) nCamp));
      if (f > 20480)
	i = 1;
      k = ((float) M_PI * 2) / (FreqCamp / f);
      j = j + k;
    }

  nCampAggiunti = 0;
  if (sin (j) > 0)
    {
      do
	{
	  *pCh = sin (j) * a;
	  pCh++;
	  nCampAggiunti++;
	  j = j + k;
	}
      while (sin (j) > 0);
    }
  else
    {
      do
	{
	  *pCh = sin (j) * a;
	  pCh++;
	  nCampAggiunti++;
	  j = j + k;
	}
      while (sin (j) < 0);
    }


  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Rumore bianco ?
***********************************************************************/
ULONG
WavWhiteNoiseOsc (PCSZ name, LONG argc, const RXSTRING * argv,
		  PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp, nCampAggiunti;
  double a, b, c;
  INT i;
  APIRET rc;

  if (argc != 3)
    {
      SendMsg (FUNC_WHITE_NOISE, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_WHITE_NOISE, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_WHITE_NOISE, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  a = atof (argv[2].strptr);
  if ((a < 0) || (a > 1))
    {
      SendMsg (FUNC_WHITE_NOISE, ERR_AMPIEZZA_OSCILLATORE);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_WHITE_NOISE);
  if (!pCh)
    return INVALID_ROUTINE;

  b = 0;
  c = 0;

  for (i = nCamp; i != 0; i--)
    {
      b = rand () * a;
      c = (c / 2) + b;
      *pCh = b;
      pCh++;
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Rumore rosa ?
***********************************************************************/
ULONG
WavPinkNoiseOsc (PCSZ name, LONG argc, const RXSTRING * argv,
		 PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp;
  double a, w, p, b0, b1, b2, b3, b4, b5, b6;
  double k0, k1, k2, k3, k4, k5, k6;
  double h0, h1, h2, h3, h4, h5, h6;
  INT i;
  APIRET rc;

  if (argc != 3)
    {
      SendMsg (FUNC_PINK_NOISE, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_PINK_NOISE, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_PINK_NOISE, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  a = atof (argv[2].strptr);
  if ((a < 0) || (a > 1))
    {
      SendMsg (FUNC_PINK_NOISE, ERR_AMPIEZZA_OSCILLATORE);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_PINK_NOISE);
  if (!pCh)
    return INVALID_ROUTINE;

  b0 = 0;
  b1 = 0;
  b2 = 0;
  b3 = 0;
  b4 = 0;
  b5 = 0;
  b6 = 0;
  k0 = 0.99886;
  k1 = 0.99332;
  k2 = 0.96900;
  k3 = 0.86650;
  k4 = 0.55000;
  k5 = -0.7616;
  k6 = 0.5362;
  h0 = 0.0555179;
  h1 = 0.0750759;
  h2 = 0.1538520;
  h3 = 0.3104856;
  h4 = 0.5329522;
  h5 = 0.0168980;
  h6 = 0.115926;
  a = a / 4;

  for (i = nCamp; i != 0; i--)
    {
      w = rand () * a;
      b0 = k0 * b0 + w * h0;
      b1 = k1 * b1 + w * h1;
      b2 = k2 * b2 + w * h2;
      b3 = k3 * b3 + w * h3;
      b4 = k4 * b4 + w * h4;
      b5 = k5 * b5 - w * h5;
      p = b0 + b1 + b2 + b3 + b4 + b5 + b6 + w * k6;
      b6 = w * h6;
      *pCh++ = p;
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Silenzia una traccia
***********************************************************************/
ULONG
WavMute (PCSZ name, LONG argc, const RXSTRING * argv,
	 PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp;
  double f, a, j;
  double k;
  INT i;
  APIRET rc;

  if (argc != 2)
    {
      SendMsg (FUNC_MUTE, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_MUTE, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_MUTE, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_MUTE);
  if (!pCh)
    return INVALID_ROUTINE;

  for (i = nCamp; i != 0; i--)
    {
      *pCh = 0;
      pCh++;
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
