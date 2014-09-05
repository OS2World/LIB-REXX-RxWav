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
Dyn (compressore/espansore di dinamica)
Vuole il puntatore ad un campione in una traccia gia' allocata, il
numero di campioni da elaborare, e il fattore di espansione/compressione
***********************************************************************/
ULONG
WavDyn (PCSZ name, LONG argc, const RXSTRING * argv,
        PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCampioni, i, j;
  float dinamica;
  APIRET rc;
  double kz, t, d, max;

  if (argc != 3)
    {
      SendMsg (FUNC_DYN, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_DYN, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCampioni = atol (argv[1].strptr);
  if (nCampioni < 1)
    {
      SendMsg (FUNC_DYN, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCampioni, FUNC_DYN);
  if (!pCh)
    return INVALID_ROUTINE;

  dinamica = atof (argv[2].strptr);
  if ((dinamica < 0.01) | (dinamica > 9))
    {
      SendMsg (FUNC_DYN, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }

  kz = exp (1) * dinamica;
  max = MAX_CAMPIONE;

  for (i = nCampioni; i != 0; i--)
    {
      if (*pCh < 0)
        {
          for (j = 0; (*pCh < 0) & (j < i); j++)
            d = d + (double) *pCh++;
          d = (d / (double) j) / -max;
          t = pow (kz, log (d)) / d;
          pCh = pCh - j;
          i = i - j;
          for (; j != 0; j--)
            *pCh++ = (USHORT) (((double) *pCh * t));
        }
      else if (*pCh > 0)
        {
          for (j = 0; (*pCh > 0) & (j < i); j++)
            d = d + (double) *pCh++;
          d = (d / (double) j) / max;
          t = pow (kz, log (d)) / d;
          pCh = pCh - j;
          i = i - j;
          for (; j != 0; j--)
            *pCh++ = (USHORT) (((double) *pCh * t));
        }
      pCh++;
    }


  sprintf (retstr->strptr, "%i", *pCh);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
