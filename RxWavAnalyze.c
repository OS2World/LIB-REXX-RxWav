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

int *BitReversed;

typedef double t_fft;

/***********************************************************************
Inizializzazione e terminazione FFT (allocazione memoria...)
***********************************************************************/
int *BitReversed;
t_fft *SinTable;
int punti = 0;

void
InitFFT (int fftlen)
{
  int i;
  int temp;
  int mask;

  punti = fftlen / 2;

  if ((SinTable = (t_fft *) malloc (2 * punti * sizeof (t_fft))) == NULL)
    {
      puts ("Error allocating memory for Sine table.");
      exit (1);
    }

  if ((BitReversed = (int *) malloc (punti * sizeof (int))) == NULL)
    {
      puts ("Error allocating memory for BitReversed.");
      exit (1);
    }

  for (i = 0; i < punti; i++)
    {
      temp = 0;
      for (mask = punti / 2; mask > 0; mask >>= 1)
	temp = (temp >> 1) + (i & mask ? punti : 0);
      BitReversed[i] = temp;
    }

  for (i = 0; i < punti; i++)
    {
      SinTable[BitReversed[i]] = -sin (2 * M_PI * i / (2 * punti));
      SinTable[BitReversed[i] + 1] = -cos (2 * M_PI * i / (2 * punti));
    }
}

void
EndFFT ()
{
  free (BitReversed);
  free (SinTable);
  punti = 0;
}


/***********************************************************************
Esecuzione FFT reale
***********************************************************************/
t_fft *A, *B;
t_fft *sptr;
t_fft *endptr1, *endptr2;
int *br1, *br2;
t_fft HRpiu, HRmeno, HIpiu, HImeno;

void
RealFFT (t_fft * buffer)
{
  int ButterfliesPerGroup = punti / 2;

  endptr1 = buffer + punti * 2;


  while (ButterfliesPerGroup > 0)
    {
      A = buffer;
      B = buffer + ButterfliesPerGroup * 2;
      sptr = SinTable;

      while (A < endptr1)
	{
	  register t_fft sin = *sptr;
	  register t_fft cos = *(sptr + 1);
	  endptr2 = B;
	  while (A < endptr2)
	    {
	      t_fft v1 = *B * cos + *(B + 1) * sin;
	      t_fft v2 = *B * sin - *(B + 1) * cos;
	      *B = (*A + v1) * 0.5;
	      *(A++) = *(B++) - v1;
	      *B = (*A - v2) * 0.5;
	      *(A++) = *(B++) + v2;
	    }
	  A = B;
	  B += ButterfliesPerGroup * 2;
	  sptr += 2;
	}
      ButterfliesPerGroup >>= 1;
    }
  br1 = BitReversed + 1;
  br2 = BitReversed + punti - 1;

  while (br1 <= br2)
    {
      register t_fft temp1;
      register t_fft temp2;
      t_fft sin = SinTable[*br1];
      t_fft cos = SinTable[*br1 + 1];
      A = buffer + *br1;
      B = buffer + *br2;
      HRpiu = (HRmeno = *A - *B) + (*B * 2);
      HIpiu = (HImeno = *(A + 1) - *(B + 1)) + (*(B + 1) * 2);
      temp1 = (sin * HRmeno - cos * HIpiu);
      temp2 = (cos * HRmeno + sin * HIpiu);
      *B = (*A = (HRpiu + temp1) * 0.5) - temp1;
      *(B + 1) = (*(A + 1) = (HImeno + temp2) * 0.5) - HImeno;

      br1++;
      br2--;
    }
  buffer[0] += buffer[1];
  buffer[1] = 0;
}


/***********************************************************************
Analyze
Vuole il puntatore ad una traccia ed un parametro ('OTTAVE', 'TERZI', 'LINEARE')
indicante il tipo di analisi (ottava, terzi di ottava o lineare) e
restituisce uno stem con i valori richiesti
***********************************************************************/
ULONG
WavAnalyze (PCSZ name, LONG argc, const RXSTRING * argv,
	    PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh = NULL;
  ULONG nCamp, TipoOut, i, j, cband;
  INT PuntiFFT;
  SHORT c;
  t_fft *data, *window, *output, re, im, outbuf, elementi;
  double banda, k;
  APIRET rc;
  RXSTEMDATA spettro;

  if (argc != 3)
    {
      SendMsg (FUNC_ANALYZE, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_ANALYZE, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  TipoOut = BANDA_OTTAVE;
  if (!strncmp (argv[1].strptr, "OCT", 3))
    TipoOut = BANDA_OTTAVE;
  if (!strncmp (argv[1].strptr, "THI", 3))
    TipoOut = BANDA_TERZE;
  if (!strncmp (argv[1].strptr, "CHR", 3))
    TipoOut = BANDA_CROMATICA;
  if (!strncmp (argv[1].strptr, "LOG", 3))
    TipoOut = BANDA_LOGARITMICA;
  if (!strncmp (argv[1].strptr, "LIN", 3))
    TipoOut = BANDA_LINEARE;

  spettro.count = 0;
  strcpy (spettro.varname, argv[2].strptr);
  spettro.stemlen = argv[2].strlength;
  strupr (spettro.varname);

  if (spettro.varname[spettro.stemlen - 1] != '.')
    spettro.varname[spettro.stemlen++] = '.';

  switch (TipoOut)
    {
    case BANDA_LINEARE:
      PuntiFFT = PUNTI_FFT_LINEARE;
      banda = 1;
      break;
    case BANDA_OTTAVE:
      PuntiFFT = PUNTI_FFT_OTTAVE;
      banda = 20;
      break;
    case BANDA_TERZE:
      PuntiFFT = PUNTI_FFT_TERZE;
      banda = 20;
      break;
    case BANDA_CROMATICA:
      PuntiFFT = PUNTI_FFT_CROMATICA;
      banda = 20;
      break;
    case BANDA_LOGARITMICA:
      PuntiFFT = PUNTI_FFT_LOGARITMICA;
      banda = 20;
      break;
    default:
      SendMsg (FUNC_ANALYZE, ERR_BANDA_ANALISI);
      sprintf (retstr->strptr, "%i", ERR_BANDA_ANALISI);
      retstr->strlength = strlen (retstr->strptr);
      return INVALID_ROUTINE;
      break;
    }

  pCh = (PSHORT) AllineaCh (pCh, (ULONG) PuntiFFT, FUNC_ANALYZE);
  if (!pCh)
    return INVALID_ROUTINE;

  InitFFT (PuntiFFT);

  data = (t_fft *) malloc (PuntiFFT * sizeof (t_fft));
  window = (t_fft *) malloc (PuntiFFT * sizeof (t_fft));
  output = (t_fft *) malloc (PuntiFFT * sizeof (t_fft));
  if ((window == NULL) || (data == NULL) || (output == NULL))
    {
      SendMsg (FUNC_ANALYZE, ERR_ALLOCMEM);
      return INVALID_ROUTINE;
    }

/* windowing ???
   for(i=0;i<PuntiFFT;i++)
   window[i]=0.42-0.5*cos(2*M_PI*i/PuntiFFT)+0.08*cos(4*M_PI*i/PuntiFFT);

   for(i=0;i<PuntiFFT;i++)
   data[i]=(t_fft)(*pCh++) * window[i];
 */
  for (i = 0; i < PuntiFFT; i++)
    data[i] = (t_fft) (*pCh++);

  RealFFT (data);

  k = (double) FreqCamp / (double) PuntiFFT;
  cband = 1;

  for (i = 0; i < (PuntiFFT / 2); i++)
    {
      re = data[BitReversed[i]];
      im = data[BitReversed[i] + 1];
      output[i] = sqrt (re * re + im * im);

      /*
         if(output[i]>1) printf("freq %f pow---> %f\n", i*k, output[i]);
       */

      switch (TipoOut)
	{
	case BANDA_LINEARE:
	  spettro.count++;
	  if (impostaOut (spettro, pow (output[i], 2), i))
	    {
	      sprintf (retstr->strptr, "%i", ERR_REXXPOOL);
	      retstr->strlength = strlen (retstr->strptr);
	      return INVALID_ROUTINE;
	    }
	  break;
	case BANDA_OTTAVE:
	  if ((i * k) < banda)
	    {
	      cband++;
	      outbuf = outbuf + output[i];
	    }
	  else
	    {
	      spettro.count++;
	      if (impostaOut (spettro, (pow (outbuf, 2) / cband), banda))
		{
		  sprintf (retstr->strptr, "%i", ERR_REXXPOOL);
		  retstr->strlength = strlen (retstr->strptr);
		  return INVALID_ROUTINE;
		}
	      outbuf = output[i];
	      cband = 1;
	      banda = banda * 2;
	    }
	  break;
	case BANDA_TERZE:
	  if ((i * k) < banda)
	    {
	      cband++;
	      outbuf = outbuf + output[i];
	    }
	  else
	    {
	      spettro.count++;
	      if (impostaOut (spettro, (pow (outbuf, 2) / cband), banda))
		{
		  sprintf (retstr->strptr, "%i", ERR_REXXPOOL);
		  retstr->strlength = strlen (retstr->strptr);
		  return INVALID_ROUTINE;
		}
	      outbuf = output[i];
	      cband = 1;
	      banda = banda * pow ((double) 2, (double) 1 / (double) 3);
	    }
	  break;
	case BANDA_CROMATICA:
	  if ((i * k) < banda)
	    {
	      cband++;
	      outbuf = outbuf + output[i];
	    }
	  else
	    {
	      spettro.count++;
	      if (impostaOut (spettro, (pow (outbuf, 2) / cband), banda))
		{
		  sprintf (retstr->strptr, "%i", ERR_REXXPOOL);
		  retstr->strlength = strlen (retstr->strptr);
		  return INVALID_ROUTINE;
		}
	      outbuf = output[i];
	      cband = 1;
	      banda = banda * pow ((double) 2, (double) 1 / (double) 12);
	    }
	  break;
	case BANDA_LOGARITMICA:
	  if ((i * k) < banda)
	    {
	      cband++;
	      outbuf = outbuf + output[i];
	    }
	  else
	    {
	      spettro.count++;
	      if (impostaOut (spettro, (pow (outbuf, 2) / cband), banda))
		{
		  sprintf (retstr->strptr, "%i", ERR_REXXPOOL);
		  retstr->strlength = strlen (retstr->strptr);
		  return INVALID_ROUTINE;
		}
	      outbuf = output[i];
	      cband = 1;
	      banda = banda * pow ((double) 2, (double) 1 / (double) 32);
	    }
	  break;
	default:
	  SendMsg (FUNC_ANALYZE, ERR_BANDA_ANALISI);
	  sprintf (retstr->strptr, "%i", ERR_BANDA_ANALISI);
	  retstr->strlength = strlen (retstr->strptr);
	  return INVALID_ROUTINE;
	  break;
	}
    }

  elementi = spettro.count;
  spettro.count = 0;

  if (impostaOut (spettro, elementi, (double) 0))
    {
      sprintf (retstr->strptr, "%i", ERR_REXXPOOL);
      retstr->strlength = strlen (retstr->strptr);
      return INVALID_ROUTINE;
    }

  EndFFT ();
  free (data);
  free (window);
  free (output);

  sprintf (retstr->strptr, "%f", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}



RXSTEMDATA spettro;

int
impostaOut (RXSTEMDATA spettro, t_fft valore, t_fft banda)
{
  /*
     if (valore < 1)
     valore = 0;
   */
  sprintf (spettro.varname + spettro.stemlen, "%d", spettro.count);
  if (banda > 0)
    sprintf (spettro.ibuf, "%f", sqrt (valore));
  else
    sprintf (spettro.ibuf, "%f", valore);

  spettro.shvb.shvnext = NULL;
  spettro.shvb.shvname.strptr = spettro.varname;
  spettro.shvb.shvname.strlength = strlen (spettro.varname);
  spettro.shvb.shvvalue.strptr = spettro.ibuf;
  spettro.shvb.shvvalue.strlength = strlen (spettro.ibuf);
  spettro.shvb.shvcode = RXSHV_SET;
  spettro.shvb.shvret = 0;
  if (RexxVariablePool (&spettro.shvb) == RXSHV_BADN)
    {
      SendMsg (FUNC_ANALYZE, ERR_REXXPOOL);
      printf ("       RexxVariablePool rc:%i\n", spettro.shvb.shvret);
      return spettro.shvb.shvret;
    }
  else
    return 0;
}
