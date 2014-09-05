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
#include "RxWav.h"

static void
regfun (PCSZ name)
{
  RexxRegisterFunctionDll (name, "RxWav", name);
}

static void
deregfun (PCSZ name)
{
  RexxDeregisterFunction (name);
}


ULONG
WavLoadFuncs (PCSZ name, LONG argc, const RXSTRING * argv,
              PCSZ queuename, PRXSTRING retstr)
{
  INT entries;
  INT j;
  retstr->strlength = 0;
  if (argc != 0)
    {
      SendMsg (FUNC_LOAD_FUNCS, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  printf ("RxWav. 22 January 2003.     [beta 0.11]\n");
/*
  printf ("       http://katobleto.dyndns.org/\n");
  printf ("       mailto:rxwav@katobleto.dyndns.org\n");
 */

  entries = sizeof (RxWavFunc) / sizeof (PSZ);

  for (j = 0; j < entries; j++)
    regfun (RxWavFunc[j]);

  return 0;
}

ULONG
WavDropFuncs (PCSZ name, LONG argc, const RXSTRING * argv,
              PCSZ queuename, PRXSTRING retstr)
{
  INT entries;
  INT j;

  retstr->strlength = 0;
  if (argc != 0)
    return 1;

  entries = sizeof (RxWavFunc) / sizeof (PSZ);

  for (j = 0; j < entries; j++)
    deregfun (RxWavFunc[j]);

/*
  printf ("RxWav.  drop!\n");
 */
  return 0;
}


void
regerror (const char *s)
{
}

/***********************************************************************
Alloca una traccia di lavoro in memoria.
Richiede in input il numero di campioni a 16 bit e restituisce il
puntatore all'area nel formato "puntatore"
***********************************************************************/
ULONG
WavAllocTrac (PCSZ name, LONG argc, const RXSTRING * argv,
              PCSZ queuename, PRXSTRING retstr)
{
  LONG nCampioni;
  PVOID pCh = NULL;
  APIRET ulrc;

  if (argc != 1)
    {
      SendMsg (FUNC_ALLOC_TRAC, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!string2long (argv[0].strptr, &nCampioni) || nCampioni < 0)
    {
      SendMsg (FUNC_ALLOC_TRAC, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  nCampioni = nCampioni * 2 + 32768;
  ulrc = DosAllocMem (&pCh, nCampioni, PAG_READ | PAG_WRITE | PAG_COMMIT);
  if (ulrc)
    {
      SendMsg (FUNC_ALLOC_TRAC, ERR_DOSALLOCMEM);
      printf ("       rc:%d\n", ulrc);
      return INVALID_ROUTINE;
    }

  sprintf (retstr->strptr, "%d", ((long) pCh / 2));
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Libera dalla memoria una traccia di lavoro.
Richiede in input il puntatore all'area da rilasciare.
Funziona?
***********************************************************************/
ULONG
WavDropTrac (PCSZ name, LONG argc, const RXSTRING * argv,
             PCSZ queuename, PRXSTRING retstr)
{
  PVOID pCh = NULL;
  APIRET ulrc;

  if (argc != 1)
    {
      SendMsg (FUNC_DROP_TRAC, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_DROP_TRAC, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  pCh = (PVOID) AllineaCh (pCh, (ULONG) 2, FUNC_DROP_TRAC);
  ulrc = DosFreeMem (pCh);
  if (ulrc)
    {
      SendMsg (FUNC_DROP_TRAC, ERR_DOSFREEMEM);
      printf ("%d\n", ulrc);
      return INVALID_ROUTINE;
    }

  sprintf (retstr->strptr, "%d", ulrc);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Aquisisce le informazioni sul file wav.
Se il file e' nel formato corretto restituisce:
- numero di canali
- frequenza di campionamento
- risoluzione in bit
- numero di campioni
altrimenti "NOWAV"
***********************************************************************/
ULONG
WavQueryFile (PCSZ name, LONG argc, const RXSTRING * argv,
              PCSZ queuename, PRXSTRING retstr)
{
  FILE *StereoFile;
  UCHAR NomeFile[256];
  struct wav DatiWAV;
  APIRET rc;

  if (argc != 1)
    {
      SendMsg (FUNC_QUERY_FILE, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  strcpy (NomeFile, argv[0].strptr);

  if ((StereoFile = fopen (NomeFile, "rb")) == NULL)
    {
      SendMsg (FUNC_QUERY_FILE, ERR_OPEN_FILE);
      sprintf (retstr->strptr, "%i", ERR_OPEN_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  if (!(fread (&DatiWAV, sizeof (struct wav), 1, StereoFile)))
    {
      SendMsg (FUNC_QUERY_FILE, ERR_READ_WAV_STRUCT);
      sprintf (retstr->strptr, "%i", ERR_READ_WAV_STRUCT);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  rc = fclose (StereoFile);
  if (rc)
    {
      SendMsg (FUNC_QUERY_FILE, ERR_CLOSE_FILE);
      return INVALID_ROUTINE;
    }

  if (strncmp (DatiWAV.w_riff, "RIFF", 4) & strncmp (DatiWAV.w_wave, "WAVE", 4))
    sprintf (retstr->strptr, "%s", "NOWAV");
  else
    sprintf (retstr->strptr, "%d %d %d %d",
             DatiWAV.w_stereo,
             DatiWAV.w_freq_camp,
             DatiWAV.w_bit_camp,
             DatiWAV.w_len_data / (DatiWAV.w_bit_camp / 8) / DatiWAV.w_stereo
      );

  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}
