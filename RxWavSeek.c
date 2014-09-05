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
Seek/find
Vuole il puntatore ad una traccia e la soglia da trovare
restituisce l'offset del primo campione superiore alla soglia
***********************************************************************/
ULONG
WavSeek (PCSZ name, LONG argc, const RXSTRING * argv,
	 PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp, i;
  SHORT c;
  double max;
  APIRET rc;

  if (argc != 3)
    {
      SendMsg (FUNC_SEEK, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_SEEK, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (nCamp < 1)
    {
      SendMsg (FUNC_SEEK, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }

  max = atof (argv[2].strptr);
  if ((max < -1) | (max > 1))
    {
      SendMsg (FUNC_SEEK, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_SEEK);
  if (!pCh)
    return INVALID_ROUTINE;

  c = max * MAX_CAMPIONE;
  for (i = 0; i < nCamp; i++)
    {
      if (abs (*pCh) > c)
	i = nCamp;
      pCh++;
    }

  sprintf (retstr->strptr, "%d", ((long) pCh / 2));
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
