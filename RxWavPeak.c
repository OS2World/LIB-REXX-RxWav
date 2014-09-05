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
Peak
Vuole il puntatore ad un campione in una traccia gia' allocata e la
durata della traccia in campioni;
restituisce il valore di picco massimo
***********************************************************************/
ULONG
WavPeak (PCSZ name, LONG argc, const RXSTRING * argv,
	 PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp, i;
  SHORT c;
  double max;
  APIRET rc;

  if (argc != 2)
    {
      SendMsg (FUNC_PEAK, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_PEAK, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (nCamp < 1)
    {
      SendMsg (FUNC_PEAK, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp + 2, FUNC_PEAK);
  if (!pCh)
    return INVALID_ROUTINE;

  c = 0;
  max = 0;
  for (i = nCamp; i != 0; i--)
    {
      if (abs (*pCh) > c)
	c = abs (*pCh);
      pCh++;
    }

  max = (double) c / MAX_CAMPIONE;

  sprintf (retstr->strptr, "%f", max);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Avg
Vuole il puntatore ad un campione in una traccia gia' allocata e la
durata della traccia in campioni;
restituisce il valore medio segnato e quello assoluto
***********************************************************************/
ULONG
WavAvg (PCSZ name, LONG argc, const RXSTRING * argv,
	PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh, pTemp;
  SHORT d;
  ULONG nCamp, i;
  double avg, avg2, scostamento;
  APIRET rc;

  if ((argc < 2) | (argc > 3))
    {
      SendMsg (FUNC_AVG, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_AVG, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (nCamp < 1)
    {
      SendMsg (FUNC_AVG, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_AVG);
  if (!pCh)
    return INVALID_ROUTINE;

  pTemp = pCh;

  avg = 0;
  for (i = nCamp; i != 0; i--)
    {
      avg = avg + *pTemp;
      avg2 = avg2 + abs (*pTemp);
      pTemp++;
    }

  avg = (avg / (double) nCamp) / (double) MAX_CAMPIONE;
  avg2 = (avg2 / (double) nCamp) / (double) MAX_CAMPIONE;

  if (argc == 3)
    {
      scostamento = atof (argv[2].strptr);
      d = (scostamento - avg) * MAX_CAMPIONE;
      for (i = nCamp; i != 0; i--)
	{
	  *pCh = *pCh + d;
	  pCh++;
	}
    }


  sprintf (retstr->strptr, "%f %f", avg, avg2);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
