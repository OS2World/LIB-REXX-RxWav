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
Peek
Vuole il puntatore ad un campione in una traccia gia' allocata
***********************************************************************/
ULONG
WavPeek (PCSZ name, LONG argc, const RXSTRING * argv,
	 PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  APIRET rc;

  if (argc != 1)
    {
      SendMsg (FUNC_PEEK, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_PEEK, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, (ULONG) 2, FUNC_PEEK);
  if (!pCh)
    return INVALID_ROUTINE;

  sprintf (retstr->strptr, "%i", *pCh);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Poke
Vuole il puntatore ad un campione in una traccia gia' allocata ed il
valore da assegnare
***********************************************************************/
ULONG
WavPoke (PCSZ name, LONG argc, const RXSTRING * argv,
	 PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  double camp;
  APIRET rc;

  if (argc != 2)
    {
      SendMsg (FUNC_POKE, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_POKE, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  camp = atof (argv[1].strptr);
  if ((camp < -1) | (camp > 1))
    {
      SendMsg (FUNC_POKE, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, (ULONG) 2, FUNC_POKE);
  if (!pCh)
    return INVALID_ROUTINE;

  *pCh = camp * MAX_CAMPIONE;

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
