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
Riverbero:
Vuole il puntatore ad una traccia gia' allocate, la durata in campioni,
due stem con i ritardi in campioni e l'ampiezza da 0 a 1
***********************************************************************/
ULONG
WavReverb (PCSZ name, LONG argc, const RXSTRING * argv,
	   PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL, pChR = NULL, rit[MAX];
  ULONG nCamp, componenti, max_rit, k;
  float tmp = 0, liv[MAX];
  RXSTEMDATA ritardo, livello;
  INT i, j;
  APIRET rc;

  if (argc != 4)
    {
      SendMsg (FUNC_REVERB, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_REVERB, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_REVERB, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  ritardo.count = 0;
  strcpy (ritardo.varname, argv[2].strptr);
  ritardo.stemlen = argv[2].strlength;
  strupr (ritardo.varname);
  if (ritardo.varname[ritardo.stemlen - 1] != '.')
    ritardo.varname[ritardo.stemlen++] = '.';
  if (FetchStem (ritardo, &tmp))
    {
      SendMsg (FUNC_REVERB, ERR_LETTURA_STEM);
      printf ("       RexxVariablePool rc:%i\n", ritardo.shvb.shvret);
      return INVALID_ROUTINE;
    }
  else
    ritardo.count = tmp;


  livello.count = 0;
  strcpy (livello.varname, argv[3].strptr);
  livello.stemlen = argv[3].strlength;
  strupr (livello.varname);
  if (livello.varname[livello.stemlen - 1] != '.')
    livello.varname[livello.stemlen++] = '.';
  if (FetchStem (livello, &tmp))
    {
      SendMsg (FUNC_REVERB, ERR_LETTURA_STEM);
      printf ("       RexxVariablePool rc:%i\n", livello.shvb.shvret);
      return INVALID_ROUTINE;
    }
  else
    livello.count = tmp;

  if (livello.count != ritardo.count)
    {
      SendMsg (FUNC_REVERB, ERR_STEM_DIVERSI);
      return INVALID_ROUTINE;
    }

  if ((livello.count > MAX) | (ritardo.count > MAX))
    {
      SendMsg (FUNC_REVERB, ERR_STEM_MASSIMO);
      return INVALID_ROUTINE;
    }

  componenti = ritardo.count;
  max_rit = 0;
  ritardo.count = 0;
  livello.count = 0;

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_REVERB);
  if (!pCh)
    return INVALID_ROUTINE;
  pChR = pCh + nCamp;

  for (i = 0; i < componenti; i++)
    {
      ritardo.count++;
      livello.count++;
      if (FetchStem (livello, &liv[i]))
	{
	  SendMsg (FUNC_REVERB, ERR_LETTURA_STEM);
	  printf ("       RexxVariablePool rc:%i\n", livello.shvb.shvret);
	  return INVALID_ROUTINE;
	}

      if (FetchStem (ritardo, &tmp))
	{
	  SendMsg (FUNC_REVERB, ERR_LETTURA_STEM);
	  printf ("       RexxVariablePool rc:%i\n", ritardo.shvb.shvret);
	  return INVALID_ROUTINE;
	}
      if (tmp > max_rit)
	max_rit = tmp;
      rit[i] = pChR - (SHORT) tmp;
    }

  if (max_rit > nCamp)
    {
      SendMsg (FUNC_REVERB, ERR_RITARDO_REVERB);
      return INVALID_ROUTINE;
    }

  componenti--;

  for (i = (nCamp - max_rit); i != 0; i--)
    {
      for (j = componenti; j != 0; j--)
	{
	  *pChR = *pChR + *rit[j] * liv[j];
	  rit[j]--;
	}
      pChR--;
    }

  for (i = max_rit; i != 0; i--)
    {
      for (j = componenti; j != 0; j--)
	{
	  if (pCh < rit[j])
	    {
	      *pChR = *pChR + *rit[j] * liv[j];
	      rit[j]--;
	    }
	}
      pChR--;
    }


  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}




/***********************************************************************
Revert: inverte (in senso temporale) una traccia
Vuole il puntatore ad una traccia gia' allocate e la durata in campioni
***********************************************************************/
ULONG
WavRevert (PCSZ name, LONG argc, const RXSTRING * argv,
	   PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL, pChR = NULL;
  SHORT swap;
  ULONG nCamp;
  INT i, j;
  APIRET rc;

  if (argc != 2)
    {
      SendMsg (FUNC_REVERT, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_REVERT, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_REVERT, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_REVERT);
  if (!pCh)
    return INVALID_ROUTINE;
  pChR = pCh + nCamp;

  for (i = (nCamp - 1) / 2; i != 0; i--)
    {
      swap = *pChR;
      *pChR-- = *pCh;
      *pCh++ = swap;
    }


  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}



/***********************************************************************
Eco:
Vuole il puntatore ad una traccia gia' allocate, la durata in campioni,
il ritardo (in campioni) ed il livello della prima riflessione
***********************************************************************/
ULONG
WavEco (PCSZ name, LONG argc, const RXSTRING * argv,
	PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh, pChR;
  ULONG nCamp, nCampR;
  double livello;
  INT i, j;
  APIRET rc;

  if (argc != 4)
    {
      SendMsg (FUNC_ECO, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_ECO, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_ECO, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  nCampR = atol (argv[2].strptr);
  if (!nCampR)
    {
      SendMsg (FUNC_ECO, ERR_RITARDO_REVERB);
      return INVALID_ROUTINE;
    }

  livello = atof (argv[3].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_ECO, ERR_AMPIEZZA);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_ECO);
  if (!pCh)
    return INVALID_ROUTINE;
  pChR = pCh + nCampR;

  for (i = (nCamp - nCampR); i != 0; i--)
    {
      *pChR++ = *pCh++ * livello;
    }


  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
