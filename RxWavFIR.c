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
FIR: filtro a risposta finita all'impulso
Vuole il puntatore ad una traccia gia' allocate, la durata in campioni,
uno stem con i valori di ampiezza per ciascuna banda
***********************************************************************/
ULONG
WavGraphEQ (PCSZ name, LONG argc, const RXSTRING * argv,
	    PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp, nbandeF;
  float tmp;
  double ampBanda[MAX], *coef;
  RXSTEMDATA bandeFreq;
  double sum, fh, fl;
  INT i, j, nCoeff;
  APIRET rc;

  if (argc != 3)
    {
      SendMsg (FUNC_GRAPHEQ, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_GRAPHEQ, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[1].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_GRAPHEQ, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  bandeFreq.count = 0;
  strcpy (bandeFreq.varname, argv[2].strptr);
  bandeFreq.stemlen = argv[2].strlength;
  strupr (bandeFreq.varname);
  if (bandeFreq.varname[bandeFreq.stemlen - 1] != '.')
    bandeFreq.varname[bandeFreq.stemlen++] = '.';
  if (FetchStem (bandeFreq, &tmp))
    {
      SendMsg (FUNC_GRAPHEQ, ERR_LETTURA_STEM);
      printf ("       RexxVariablePool rc:%i\n", bandeFreq.shvb.shvret);
      return INVALID_ROUTINE;
    }
  else
    bandeFreq.count = tmp;

  if (bandeFreq.count > MAX)
    {
      SendMsg (FUNC_GRAPHEQ, ERR_STEM_MASSIMO);
      return INVALID_ROUTINE;
    }

  nbandeF = bandeFreq.count;
  bandeFreq.count = 0;

  pCh = (PSHORT) AllineaCh (pCh, nCamp, FUNC_GRAPHEQ);
  if (!pCh)
    return INVALID_ROUTINE;

  for (i = 0; i < nbandeF; i++)
    {
      bandeFreq.count++;
      if (FetchStem (bandeFreq, &tmp))
	{
	  SendMsg (FUNC_GRAPHEQ, ERR_LETTURA_STEM);
	  printf ("       RexxVariablePool rc:%i\n", bandeFreq.shvb.shvret);
	  return INVALID_ROUTINE;
	}
      ampBanda[i + 1] = (double) tmp;
    }

  nCoeff = 32 + nbandeF * 32;
  fvec (coef, nCoeff);
  fl = (double) (20480 / ((double) FreqCamp / 2));
  fh = 1;
  for (i = 0; i < nCoeff; i++)
    coef[i] = 0;

  for (j = nbandeF; j != 0; j--)
    {
      fl = fl / pow ((double) 2048, (double) (1 / (double) nbandeF));
      FIRCoef (coef, (int) nCoeff, fl, fh, ampBanda[j]);
      fh = fl;
    }

  pCh = pCh + nCamp;
  for (i = nCamp - nCoeff; i != 0; i--)
    {
      sum = 0.0;
      for (j = nCoeff; j != 0; j--)
	{
	  sum += coef[j] * *pCh--;
	}
      pCh = pCh + nCoeff;
      *pCh-- = (SHORT) sum;
    }

  for (i = nCoeff; i != 0; i--)
    {
      sum = 0.0;
      for (j = nCoeff; j != 0; j--)
	{
	  sum += coef[j] * *pCh--;
	}
      pCh = pCh + nCoeff--;
      *pCh-- = (SHORT) sum;
    }



  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}




int
FIRCoef (double *cf, int Len, double fl, double fh, double amp)
{
  int i;
  double *Coef;

  double Sum, TmpFloat;
  int CoefNum, HalfLen, Cnt;

  fvec (Coef, 8192);

  CoefNum = Len;
  if (Len % 2 == 0)
    {
      CoefNum++;
    }
  HalfLen = (CoefNum - 1) / 2;


  Coef[HalfLen] = fh - fl;
  for (Cnt = 1; Cnt <= HalfLen; Cnt++)
    {
      TmpFloat = M_PI * Cnt;
      Coef[HalfLen + Cnt] = 2.0 * sin ((fh - fl) / 2.0 * TmpFloat) *
	cos ((fh + fl) / 2.0 * TmpFloat) / TmpFloat;
      Coef[HalfLen - Cnt] = Coef[HalfLen + Cnt];
    }

  TmpFloat = 2.0 * M_PI / (CoefNum - 1.0);
  Sum = 0.0;
  for (Cnt = 0; Cnt < CoefNum; Cnt++)
    {
      Coef[Cnt] *= (0.54 - 0.46 * cos (TmpFloat * Cnt));
      Sum += Coef[Cnt];
    }

  for (Cnt = 0; Cnt < CoefNum; Cnt++)
    cf[Cnt] = cf[Cnt] + Coef[Cnt] * amp;

  free (Coef);

  return 0;
}
