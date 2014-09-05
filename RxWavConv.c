/*  RxWav
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
Convoluzione:
Vuole due puntatore e la durata della convoluzione
***********************************************************************/
ULONG
WavConv (PCSZ name, LONG argc, const RXSTRING * argv,
	 PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh1, pCh2;
  ULONG nCamp;
  double conv;
  INT i, j;
  APIRET rc;

  if (argc != 3)
    {
      SendMsg (FUNC_CONV, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh1))
    {
      SendMsg (FUNC_CONV, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[1].strptr, "%d", &pCh2))
    {
      SendMsg (FUNC_CONV, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[2].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_CONV, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  pCh1 = (PSHORT) AllineaCh (pCh1, nCamp, FUNC_CONV);
  if (!pCh1)
    return INVALID_ROUTINE;

  pCh2 = (PSHORT) AllineaCh (pCh2, nCamp, FUNC_CONV);
  if (!pCh2)
    return INVALID_ROUTINE;

  for (i = nCamp; i != 0; i--)
    {
      conv = (double) *pCh1 * (double) *pCh2++;
      *pCh1++ = (SHORT) (conv / MAX_CAMPIONE);
    }


  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
