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
Legge un file WAV Stereo.
Vuole il nome del file e due puntatori a traccie gia' allocate e di
dimensioni sufficienti
***********************************************************************/
ULONG
WavReadStereo (PCSZ name, LONG argc, const RXSTRING * argv,
	       PCSZ queuename, PRXSTRING retstr)
{
  FILE *StereoFile;
  UCHAR NomeFile[256];
  PBYTE pChSX = NULL;
  PBYTE pChDX = NULL;
  PSHORT pChSX16 = NULL;
  PSHORT pChDX16 = NULL;
  struct wav DatiWAV;
  INT i, ch, c2;
  ULONG TipoCopia = COPIA_NOMIX;
  ULONG nSkip = 0, nCamp = 67108864;
  APIRET rc;

  if ((argc < 3) | (argc > 6))
    {
      SendMsg (FUNC_READ_STEREO, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  strcpy (NomeFile, argv[0].strptr);
  if (!sscanf (argv[1].strptr, "%d", &pChSX))
    {
      SendMsg (FUNC_READ_STEREO, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[2].strptr, "%d", &pChDX))
    {
      SendMsg (FUNC_READ_STEREO, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  if (argc > 3)
    {
      if (!strncmp (argv[3].strptr, "MIX", 3))
	TipoCopia = COPIA_MIX;
      if (!strncmp (argv[3].strptr, "NOMIX", 5))
	TipoCopia = COPIA_NOMIX;
    }

  if (argc > 4)
    {
      nSkip = atol (argv[4].strptr);
      if (!nSkip)
	{
	  SendMsg (FUNC_READ_STEREO, ERR_NUMERO_CAMPIONI);
	  return INVALID_ROUTINE;
	}

      nCamp = atol (argv[5].strptr);
      if (!nCamp)
	{
	  SendMsg (FUNC_READ_STEREO, ERR_NUMERO_CAMPIONI);
	  return INVALID_ROUTINE;
	}
    }

  if ((StereoFile = fopen (NomeFile, "rb")) == NULL)
    {
      SendMsg (FUNC_READ_STEREO, ERR_OPEN_FILE);
      sprintf (retstr->strptr, "%i", ERR_OPEN_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  if (!(fread (&DatiWAV, sizeof (struct wav), 1, StereoFile)))
    {
      SendMsg (FUNC_READ_STEREO, ERR_READ_WAV_STRUCT);
      sprintf (retstr->strptr, "%i", ERR_READ_WAV_STRUCT);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  if (strncmp (DatiWAV.w_riff, "RIFF", 4) & strncmp (DatiWAV.w_wave, "WAVE", 4))
    {
      SendMsg (FUNC_READ_STEREO, ERR_FILE_NOWAV);
      sprintf (retstr->strptr, "%i", ERR_FILE_NOWAV);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }


  if (nSkip > 0)
    fseek (StereoFile, nSkip, SEEK_SET);

  pChSX = (PBYTE) AllineaCh (pChSX, (ULONG) DatiWAV.w_len_data / 4, FUNC_READ_STEREO);
  if (!pChSX)
    return INVALID_ROUTINE;

  pChDX = (PBYTE) AllineaCh (pChDX, (ULONG) DatiWAV.w_len_data / 4, FUNC_READ_STEREO);
  if (!pChDX)
    return INVALID_ROUTINE;

  pChSX16 = (PSHORT) pChSX;
  pChDX16 = (PSHORT) pChDX;
  for (i = nCamp; ((ch = fgetc (StereoFile)) != EOF) & (i != 0); i++)
    {
      switch (DatiWAV.w_bit_camp)
	{
	case 8:
	  if (TipoCopia == COPIA_NOMIX)
	    {
	      *pChSX16++ = (BYTE) ch *128;
	      ch = fgetc (StereoFile);
	      *pChDX16++ = (BYTE) ch *128;
	    }
	  else
	    {
	      *pChSX16++ = *pChSX16 + (BYTE) ch *128;
	      ch = fgetc (StereoFile);
	      *pChDX16++ = *pChSX16 + (BYTE) ch *128;
	    }
	  break;
	case 16:
	  if (TipoCopia == COPIA_NOMIX)
	    {
	      *pChSX++ = (BYTE) ch;
	      ch = fgetc (StereoFile);
	      *pChSX++ = (BYTE) ch;
	      ch = fgetc (StereoFile);
	      *pChDX++ = (BYTE) ch;
	      ch = fgetc (StereoFile);
	      *pChDX++ = (BYTE) ch;
	    }
	  else
	    {
	      c2 = fgetc (StereoFile);
	      *pChSX16++ = (BYTE) ch + (SHORT) ((BYTE) c2 * 256) + *pChSX16;
	      ch = fgetc (StereoFile);

	      c2 = fgetc (StereoFile);
	      *pChDX16++ = (BYTE) ch + (SHORT) ((BYTE) c2 * 256) + *pChDX16;
	    }
	  break;
	default:
	  SendMsg (FUNC_READ_STEREO, ERR_RISOLUZIONE);
	  sprintf (retstr->strptr, "%i", ERR_RISOLUZIONE);
	  retstr->strlength = strlen (retstr->strptr);
	  break;
	}
    }

  rc = fclose (StereoFile);
  if (rc)
    {
      SendMsg (FUNC_READ_STEREO, ERR_CLOSE_FILE);
      sprintf (retstr->strptr, "%i", ERR_CLOSE_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Scrive un file WAV Stereo.
Vuole il nome del file e due puntatori a traccie gia' allocate e il
numero di campioni da scaricare
***********************************************************************/
ULONG
WavWriteStereo (PCSZ name, LONG argc, const RXSTRING * argv,
		PCSZ queuename, PRXSTRING retstr)
{
  FILE *StereoFile;
  UCHAR NomeFile[256];
  PBYTE pChSX = NULL;
  PBYTE pChDX = NULL;
  struct wav DatiWAV;
  ULONG nCamp, i;
  APIRET rc;

  if (argc != 4)
    {
      SendMsg (FUNC_WRITE_STEREO, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  strcpy (NomeFile, argv[0].strptr);

  if (!sscanf (argv[1].strptr, "%d", &pChSX))
    {
      SendMsg (FUNC_WRITE_STEREO, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[2].strptr, "%d", &pChDX))
    {
      SendMsg (FUNC_WRITE_STEREO, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[3].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_WRITE_STEREO, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  if ((StereoFile = fopen (NomeFile, "wb")) == NULL)
    {
      SendMsg (FUNC_WRITE_STEREO, ERR_OPEN_FILE);
      sprintf (retstr->strptr, "%i", ERR_OPEN_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  strcpy (DatiWAV.w_riff, "RIFF");
  strcpy (DatiWAV.w_wave, "WAVE");
  strcpy (DatiWAV.w_fmt, "fmt ");
  strcpy (DatiWAV.w_data, "data");
  DatiWAV.w_len_tot = (nCamp * NBit / 4) + sizeof (struct wav) - 8;
  DatiWAV.w_len_fmt = 16;
  DatiWAV.w_stereo = 2;
  DatiWAV.w_freq_camp = FreqCamp;
  DatiWAV.w_byte_sec = FreqCamp * (NBit / 8) * 2;
  DatiWAV.w_block_align = 4;
  DatiWAV.w_bit_camp = NBit;
  DatiWAV.w_len_data = (nCamp * 4) + sizeof (struct wav) - 44;
  DatiWAV.w_format_tag = 1;

  if (!(fwrite (&DatiWAV, sizeof (struct wav), 1, StereoFile)))
    {
      SendMsg (FUNC_WRITE_STEREO, ERR_WRITE_WAV_STRUCT);
      sprintf (retstr->strptr, "%i", ERR_WRITE_WAV_STRUCT);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  pChSX = (PBYTE) AllineaCh (pChSX, nCamp, FUNC_WRITE_STEREO);
  if (!pChSX)
    return INVALID_ROUTINE;

  pChDX = (PBYTE) AllineaCh (pChDX, nCamp, FUNC_WRITE_STEREO);
  if (!pChDX)
    return INVALID_ROUTINE;

  for (i = nCamp; i != 0; i--)
    {
      putc (*pChSX++, StereoFile);
      putc (*pChSX++, StereoFile);
      putc (*pChDX++, StereoFile);
      putc (*pChDX++, StereoFile);
    }

  rc = 0;
  rc = fclose (StereoFile);
/*
   if (rc)
   { SendMsg(FUNC_WRITE_STEREO, ERR_CLOSE_FILE);
   sprintf(retstr->strptr, "%i", ERR_CLOSE_FILE);
   retstr->strlength = strlen(retstr->strptr);
   return VALID_ROUTINE; }
 */

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Legge un file WAV Mono.
Vuole il nome del file e due puntatori a traccie gia' allocate e di
dimensioni sufficienti
***********************************************************************/
ULONG
WavReadMono (PCSZ name, LONG argc, const RXSTRING * argv,
	     PCSZ queuename, PRXSTRING retstr)
{
  FILE *MonoFile;
  UCHAR NomeFile[256];
  PBYTE pCh = NULL;
  PSHORT pCh16 = NULL;
  struct wav DatiWAV;
  ULONG nSkip = 0, nCamp = 67108864;
  INT i, ch;
  APIRET rc;

  if (argc != 2)
    {
      SendMsg (FUNC_READ_MONO, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  strcpy (NomeFile, argv[0].strptr);
  if (!sscanf (argv[1].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_READ_MONO, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }
/*
  if (argc > 2)
    {
      if (!strncmp (argv[2].strptr, "MIX", 3))
	TipoCopia = COPIA_MIX;
      if (!strncmp (argv[2].strptr, "NOMIX", 5))
	TipoCopia = COPIA_NOMIX;
    }

  if (argc > 3)
    {
      nSkip = atol (argv[3].strptr);
      if (!nSkip)
	{
	  SendMsg (FUNC_READ_STEREO, ERR_NUMERO_CAMPIONI);
	  return INVALID_ROUTINE;
	}

      nCamp = atol (argv[4].strptr);
      if (!nCamp)
	{
	  SendMsg (FUNC_READ_STEREO, ERR_NUMERO_CAMPIONI);
	  return INVALID_ROUTINE;
	}
    }
 */


  if ((MonoFile = fopen (NomeFile, "rb")) == NULL)
    {
      SendMsg (FUNC_READ_MONO, ERR_OPEN_FILE);
      sprintf (retstr->strptr, "%i", ERR_OPEN_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  if (!(fread (&DatiWAV, sizeof (struct wav), 1, MonoFile)))
    {
      SendMsg (FUNC_READ_MONO, ERR_READ_WAV_STRUCT);
      sprintf (retstr->strptr, "%i", ERR_READ_WAV_STRUCT);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  if (strncmp (DatiWAV.w_riff, "RIFF", 4) & strncmp (DatiWAV.w_wave, "WAVE", 4))
    {
      SendMsg (FUNC_READ_MONO, ERR_FILE_NOWAV);
      sprintf (retstr->strptr, "%i", ERR_FILE_NOWAV);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }


  if (nSkip > 0)
    fseek (MonoFile, nSkip, SEEK_SET);

  pCh = (PBYTE) AllineaCh (pCh, DatiWAV.w_len_data / 2, FUNC_READ_MONO);
  if (!pCh)
    return INVALID_ROUTINE;

  pCh16 = (PSHORT) pCh;
  for (i = nCamp; ((ch = fgetc (MonoFile)) != EOF) & (i != 0); i++)
    {
      switch (DatiWAV.w_bit_camp)
	{
	case 8:
	    *pCh16++ = (BYTE) ch *128;
	  break;

	case 16:
	    *pCh++ = (BYTE) ch;
	    ch = fgetc (MonoFile);
	    *pCh++ = (BYTE) ch;
	  break;

	default:
	  SendMsg (FUNC_READ_MONO, ERR_RISOLUZIONE);
	  sprintf (retstr->strptr, "%i", ERR_RISOLUZIONE);
	  retstr->strlength = strlen (retstr->strptr);
	  break;
	}
    }

  rc = fclose (MonoFile);
  if (rc)
    {
      SendMsg (FUNC_READ_MONO, ERR_CLOSE_FILE);
      sprintf (retstr->strptr, "%i", ERR_CLOSE_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Scrive un file WAV Mono.
Vuole il nome del file e due puntatori a traccie gia' allocate e il
numero di campioni da scaricare
***********************************************************************/
ULONG
WavWriteMono (PCSZ name, LONG argc, const RXSTRING * argv,
	      PCSZ queuename, PRXSTRING retstr)
{
  FILE *MonoFile;
  UCHAR NomeFile[256];
  PBYTE pCh = NULL;
  struct wav DatiWAV;
  ULONG nCamp, i;
  APIRET rc;

  if (argc != 3)
    {
      SendMsg (FUNC_WRITE_MONO, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  strcpy (NomeFile, argv[0].strptr);

  if (!sscanf (argv[1].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_WRITE_MONO, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[2].strptr);
  if (!nCamp)
    {
      SendMsg (FUNC_WRITE_MONO, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  if ((MonoFile = fopen (NomeFile, "wb")) == NULL)
    {
      SendMsg (FUNC_WRITE_MONO, ERR_OPEN_FILE);
      sprintf (retstr->strptr, "%i", ERR_OPEN_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  strcpy (DatiWAV.w_riff, "RIFF");
  strcpy (DatiWAV.w_wave, "WAVE");
  strcpy (DatiWAV.w_fmt, "fmt ");
  strcpy (DatiWAV.w_data, "data");
  DatiWAV.w_len_tot = (nCamp * NBit / 8) + sizeof (struct wav) - 8;
  DatiWAV.w_len_fmt = 16;
  DatiWAV.w_stereo = 1;
  DatiWAV.w_freq_camp = FreqCamp;
  DatiWAV.w_byte_sec = FreqCamp * (NBit / 8);
  DatiWAV.w_block_align = 4;
  DatiWAV.w_bit_camp = NBit;
  DatiWAV.w_len_data = (nCamp * 2) + sizeof (struct wav) - 44;
  DatiWAV.w_format_tag = 1;

  if (!(fwrite (&DatiWAV, sizeof (struct wav), 1, MonoFile)))
    {
      SendMsg (FUNC_WRITE_MONO, ERR_WRITE_WAV_STRUCT);
      sprintf (retstr->strptr, "%i", ERR_WRITE_WAV_STRUCT);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  pCh = (PBYTE) AllineaCh (pCh, nCamp, FUNC_WRITE_MONO);
  if (!pCh)
    return INVALID_ROUTINE;

  for (i = nCamp; i; i--)
    {
      putc (*pCh++, MonoFile);
      putc (*pCh++, MonoFile);
    }

  rc = fclose (MonoFile);
  if (rc)
    {
      SendMsg (FUNC_WRITE_MONO, ERR_CLOSE_FILE);
      sprintf (retstr->strptr, "%i", ERR_CLOSE_FILE);
      retstr->strlength = strlen (retstr->strptr);
      return VALID_ROUTINE;
    }

  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}


/***********************************************************************
Copia una traccia su di un'altra
Vuole i puntatori alla traccie di origine e di destinazione ed il numero
di campioni da copiare;
Opzionalmente l'ampiezza iniziale e quella finale da 0 a 1 ed il tipo di
inviluppo, lin(eare) o log(aritmico), ed il tipo di copia, mix o over
il parametro 'D'ither permette l'aggiunta di rumore di ampiezza pari a
1/2 bit meno significativo
***********************************************************************/
ULONG
WavCopyTrac (PCSZ name, LONG argc, const RXSTRING * argv,
	     PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pFrom = NULL;
  PSHORT pTo = NULL;
  ULONG nCamp, i;
  double AmpInizio = 1;
  double AmpFine = 1;
  INT TipoInviluppo = 0, dither;
  ULONG TipoCopia = COPIA_NOMIX;
  APIRET rc;

  if ((argc < 3) || (argc > 8))
    {
      SendMsg (FUNC_COPY_TRAC, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pFrom))
    {
      SendMsg (FUNC_COPY_TRAC, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[1].strptr, "%d", &pTo))
    {
      SendMsg (FUNC_COPY_TRAC, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[2].strptr);
  if (nCamp < 0)
    {
      SendMsg (FUNC_COPY_TRAC, ERR_NUMERO_CAMPIONI);
      return INVALID_ROUTINE;
    }

  if (argc > 3)
    {
      AmpInizio = atof (argv[3].strptr);
      if ((AmpInizio < -9) || (AmpInizio > 9))
	{
	  SendMsg (FUNC_COPY_TRAC, ERR_AMPIEZZA);
	  return INVALID_ROUTINE;
	}
    }

  if (argc > 4)
    {
      AmpFine = atof (argv[4].strptr);
      if ((AmpFine < -9) || (AmpFine > 9))
	{
	  SendMsg (FUNC_COPY_TRAC, ERR_AMPIEZZA);
	  return INVALID_ROUTINE;
	}
    }

  if (argc > 5)
    {
      if (!strncmp (argv[5].strptr, "CONST", 5))
	TipoInviluppo = 0;
      if (!strncmp (argv[5].strptr, "LIN", 3))
	TipoInviluppo = 1;
      if (!strncmp (argv[5].strptr, "LOG", 3))
	TipoInviluppo = 2;
    }

  if (argc > 6)
    {
      if (!strncmp (argv[6].strptr, "MIX", 3))
	TipoCopia = COPIA_MIX;
      if (!strncmp (argv[6].strptr, "NOMIX", 5))
	TipoCopia = COPIA_NOMIX;
      if (!strncmp (argv[6].strptr, "INV_MIX", 7))
	TipoCopia = COPIA_INV_MIX;
      if (!strncmp (argv[6].strptr, "INV_NOMIX", 9))
	TipoCopia = COPIA_INV_NOMIX;
    }

  if (argc > 7)
    {
      if (strncmp (argv[7].strptr, "D", 1))
	dither = TRUE;
      else
	dither = FALSE;
    }

  pFrom = (PSHORT) AllineaCh (pFrom, nCamp, FUNC_COPY_TRAC);
  if (!pFrom)
    return INVALID_ROUTINE;

  pTo = (PSHORT) AllineaCh (pTo, nCamp, FUNC_COPY_TRAC);
  if (!pTo)
    return INVALID_ROUTINE;

  if ((AmpInizio == 1) & (AmpFine == 1))
    TipoInviluppo = 0;

  switch (TipoInviluppo)
    {
    case INVILUPPO_COSTANTE:
      switch (TipoCopia)
	{
	case COPIA_NOMIX:
	  CopyTrk (pFrom, pTo, nCamp, 0);
	  break;
	case COPIA_MIX:
	  MixTrk (pFrom, pTo, nCamp, 0);
	  break;
	case COPIA_INV_NOMIX:
	  CopyTrk (pFrom, pTo, nCamp, 1);
	  break;
	case COPIA_INV_MIX:
	  MixTrk (pFrom, pTo, nCamp, 1);
	  break;
	default:
	  SendMsg (FUNC_COPY_TRAC, ERR_INVILUPPO);
	  sprintf (retstr->strptr, "%i", ERR_INVILUPPO);
	  retstr->strlength = strlen (retstr->strptr);
	  return INVALID_ROUTINE;
	  break;
	}
      break;
    case INVILUPPO_LINEARE:
      switch (TipoCopia)
	{
	case COPIA_NOMIX:
	  CopyLin (pFrom, pTo, nCamp, AmpInizio, AmpFine, 0, dither);
	  break;
	case COPIA_MIX:
	  MixLin (pFrom, pTo, nCamp, AmpInizio, AmpFine, 0, dither);
	  break;
	case COPIA_INV_NOMIX:
	  CopyLin (pFrom, pTo, nCamp, AmpInizio, AmpFine, 1, dither);
	  break;
	case COPIA_INV_MIX:
	  MixLin (pFrom, pTo, nCamp, AmpInizio, AmpFine, 1, dither);
	  break;
	default:
	  SendMsg (FUNC_COPY_TRAC, ERR_INVILUPPO);
	  sprintf (retstr->strptr, "%i", ERR_INVILUPPO);
	  retstr->strlength = strlen (retstr->strptr);
	  return INVALID_ROUTINE;
	  break;
	}
      break;
    case INVILUPPO_LOGARITMICO:
      switch (TipoCopia)
	{
	case COPIA_NOMIX:
	  CopyLog (pFrom, pTo, nCamp, AmpInizio, AmpFine, 0, dither);
	  break;
	case COPIA_MIX:
	  MixLog (pFrom, pTo, nCamp, AmpInizio, AmpFine, 0, dither);
	  break;
	case COPIA_INV_NOMIX:
	  CopyLog (pFrom, pTo, nCamp, AmpInizio, AmpFine, 1, dither);
	  break;
	case COPIA_INV_MIX:
	  MixLog (pFrom, pTo, nCamp, AmpInizio, AmpFine, 1, dither);
	  break;
	default:
	  SendMsg (FUNC_COPY_TRAC, ERR_INVILUPPO);
	  sprintf (retstr->strptr, "%i", ERR_INVILUPPO);
	  retstr->strlength = strlen (retstr->strptr);
	  return INVALID_ROUTINE;
	  break;
	}
      break;
    default:
      SendMsg (FUNC_COPY_TRAC, ERR_INVILUPPO);
      sprintf (retstr->strptr, "%i", ERR_INVILUPPO);
      retstr->strlength = strlen (retstr->strptr);
      return INVALID_ROUTINE;
      break;
    }


  sprintf (retstr->strptr, "%i", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}








/***********************************************************************
         ********* Funzioni di copia tra tracce ***********
***********************************************************************/
CopyTrk (SHORT * pFrom, SHORT * pTo, INT nCampioni, int inverti)
{
  INT i;

  if (inverti)
    {
      for (i = nCampioni; i != 0; i--)
	{
	  *pTo = -*pFrom;
	  pFrom++;
	  pTo++;
	}
    }
  else
    {
      for (i = nCampioni; i != 0; i--)
	{
	  *pTo = *pFrom;
	  pFrom++;
	  pTo++;
	}
    }
}


MixTrk (SHORT * pFrom, SHORT * pTo, INT nCampioni, int inverti)
{
  INT i;

  if (inverti)
    {
      for (i = nCampioni; i != 0; i--)
	{
	  *pTo = *pTo - *pFrom;
	  pFrom++;
	  pTo++;
	}
    }
  else
    {
      for (i = nCampioni; i != 0; i--)
	{
	  *pTo = *pTo + *pFrom;
	  pFrom++;
	  pTo++;
	}
    }
}



CopyLin (SHORT * pFrom, SHORT * pTo, INT nCampioni, double AmpInizio, double AmpFine, int inverti, int dither)
{
  INT i;
  double ki, kf, kd;
  ki = AmpInizio;
  kf = AmpFine;
  kd = (kf - ki) / nCampioni;

  if (dither)
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = -*pFrom * ki + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pFrom * ki + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
  else
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = -*pFrom * ki;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pFrom * ki;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
}


MixLin (SHORT * pFrom, SHORT * pTo, INT nCampioni, double AmpInizio, double AmpFine, int inverti, int dither)
{
  INT i;
  double ki, kf, kd;
  ki = AmpInizio;
  kf = AmpFine;
  kd = (kf - ki) / nCampioni;

  if (dither)
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo - *pFrom * ki + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo + *pFrom * ki + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
  else
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo - *pFrom * ki;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo + *pFrom * ki;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
}


CopyLog (SHORT * pFrom, SHORT * pTo, INT nCampioni, double AmpInizio, double AmpFine, int inverti, int dither)
{
  INT i;
  double ki, kf, kd, kz;
  ki = AmpInizio;
  kf = AmpFine;
  kd = (kf - ki) / nCampioni;
  kz = exp (1);

  if (dither)
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = -*pFrom * ki * (exp (ki) / kz) + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pFrom * ki * (exp (ki) / kz) + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
  else
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = -*pFrom * ki * (exp (ki) / kz);
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pFrom * ki * (exp (ki) / kz);
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
}


MixLog (SHORT * pFrom, SHORT * pTo, INT nCampioni, double AmpInizio, double AmpFine, int inverti, int dither)
{
  INT i;
  double ki, kf, kd, kz;
  ki = AmpInizio;
  kf = AmpFine;
  kd = (kf - ki) / nCampioni;
  kz = exp (1);

  if (dither)
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo - *pFrom * ki * exp (ki) / kz + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo + *pFrom * ki * exp (ki) / kz + (rand () - rand ()) / MAX_CAMPIONE;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
  else
    {
      if (inverti)
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo - *pFrom * ki * exp (ki) / kz;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
      else
	{
	  for (i = nCampioni; i != 0; i--)
	    {
	      *pTo = *pTo + *pFrom * ki * exp (ki) / kz;
	      ki = ki + kd;
	      pFrom++;
	      pTo++;
	    }
	}
    }
}
