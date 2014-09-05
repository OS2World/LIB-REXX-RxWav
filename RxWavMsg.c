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
#include "RxWav.h"

/***********************************************************************
Sinossi delle funzioni
***********************************************************************/
VOID
Sinossi (ULONG func)
{

  switch (func)
    {
    case FUNC_LOAD_FUNCS:
      printf ("      < 0 | error\n");
      break;
    case FUNC_DROP_FUNCS:
      printf ("      < 0 | error\n");
      break;
    case FUNC_ALLOC_TRAC:
      printf ("      > samples\n");
      printf ("      < pointer\n");
      break;
    case FUNC_DROP_TRAC:
      printf ("      > pointer\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_QUERY_FILE:
      printf ("      > file name\n");
      printf ("      < 'NOWAV' | n_channels sample_rate n_bits n_samples\n");
      break;
    case FUNC_READ_STEREO:
      printf ("      > file name\n");
      printf ("      > pointer to left channel\n");
      printf ("      > pointer to right channel\n");
      printf ("     [> type of read (MIX|NOMIX)]\n");
      printf ("     [> offset]\n");
      printf ("     [> length (samples)]\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_WRITE_STEREO:
      printf ("      > file name\n");
      printf ("      > pointer to left channel\n");
      printf ("      > pointer to right channel\n");
      printf ("      > length (samples)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_READ_MONO:
      printf ("      > file name\n");
      printf ("      > pointer\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_WRITE_MONO:
      printf ("      > file name\n");
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_COPY_TRAC:
      printf ("      > pointer to source\n");
      printf ("      > pointer to destination\n");
      printf ("      > length (samples)\n");
      printf ("     [> initial amplitude]\n");
      printf ("     [> final amplitude]\n");
      printf ("     [> envelope (CONST|LIN|LOG)]\n");
      printf ("     [> type of copy (MIX|NOMIX|INV_MIX|INV_NOMIX)]\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_SIN_OSC:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > frequency in hertz\n");
      printf ("      > amplitude (from 0 to 1)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_TEST_OSC:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > amplitude (from 0 to 1)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_WHITE_NOISE:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > amplitude (from 0 to 1)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_PINK_NOISE:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > amplitude (from 0 to 1)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_MUTE:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_FILTER_BAND:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > central frequency\n");
      printf ("      > band width\n");
      printf ("      > amplitude\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_FILTER_HIGH:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > frequency band limit\n");
      printf ("      > amplitude\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_FILTER_LOW:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > frequency band limit\n");
      printf ("      > amplitude\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_REVERB:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > stem of delay (samples)\n");
      printf ("      > stem of amplitude (from 0 to 1)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_REVERT:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_ECO:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > delay (samples)\n");
      printf ("      > amplitude (from 0 to 1)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_PEEK:
      printf ("      > pointer\n");
      printf ("      < sample value | error\n");
      break;
    case FUNC_POKE:
      printf ("      > pointer\n");
      printf ("      > sample value\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_PEAK:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      < max peak value | error\n");
      break;
    case FUNC_SEEK:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > value of limit\n");
      printf ("      < offset | error\n");
      break;
    case FUNC_ANALYZE:
      printf ("      > pointer\n");
      printf ("      > band (OCTAVE|THIRDOCTAVE|CHROMATIC|LOGARITHMIC|LINEAR)\n");
      printf ("      > stem of spectrum power\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_DYN:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > dynamic (from 0.01 to 9; >1=compress, <1=expand)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_AVG:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("     [> avg value (from -1 to 1)]\n");
      printf ("      < 0 | avg_signed avg_abs\n");
      break;
    case FUNC_VOCODER:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > gain (from 0 to 1)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_CONV:
      printf ("      > pointer\n");
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      < 0 | error\n");
      break;
    case FUNC_GRAPHEQ:
      printf ("      > pointer\n");
      printf ("      > length (samples)\n");
      printf ("      > stem of band amplitude\n");
      printf ("      < 0 | error\n");
      break;
    default:
      printf ("*** description of function not available ***\n");
      break;
    }

}


/***********************************************************************
Gestione dei messaggi/errori
***********************************************************************/
VOID
SendMsg (ULONG func, ULONG errno)
{

/*
   printf("Gestione messaggi\n");
   printf("  func:%d  errno%d\n", func, errno);
 */

  printf ("RxWav: function Wav");
  switch (func)
    {
    case FUNC_LOAD_FUNCS:
      printf ("LoadFunc");
      break;
    case FUNC_DROP_FUNCS:
      printf ("DropFunc");
      break;
    case FUNC_ALLOC_TRAC:
      printf ("AllocTrac");
      break;
    case FUNC_DROP_TRAC:
      printf ("DropTrac");
      break;
    case FUNC_QUERY_FILE:
      printf ("QueryFile");
      break;
    case FUNC_READ_STEREO:
      printf ("ReadStereo");
      break;
    case FUNC_WRITE_STEREO:
      printf ("WriteStereo");
      break;
    case FUNC_READ_MONO:
      printf ("ReadMono");
      break;
    case FUNC_WRITE_MONO:
      printf ("WriteMono");
      break;
    case FUNC_COPY_TRAC:
      printf ("CopyTrac");
      break;
    case FUNC_SIN_OSC:
      printf ("SinOsc");
      break;
    case FUNC_TEST_OSC:
      printf ("TestOsc");
      break;
    case FUNC_WHITE_NOISE:
      printf ("WhiteNoiseOsc");
      break;
    case FUNC_PINK_NOISE:
      printf ("PinkNoiseOsc");
      break;
    case FUNC_MUTE:
      printf ("Mute");
      break;
    case FUNC_FILTER_BAND:
      printf ("FilterBand");
      break;
    case FUNC_FILTER_HIGH:
      printf ("FilterHigh");
      break;
    case FUNC_FILTER_LOW:
      printf ("FilterLow");
      break;
    case FUNC_REVERB:
      printf ("Reverb");
      break;
    case FUNC_REVERT:
      printf ("Revert");
      break;
    case FUNC_ECO:
      printf ("Eco");
      break;
    case FUNC_PEEK:
      printf ("Peek");
      break;
    case FUNC_POKE:
      printf ("Poke");
      break;
    case FUNC_PEAK:
      printf ("Peak");
      break;
    case FUNC_SEEK:
      printf ("Seek");
      break;
    case FUNC_ANALYZE:
      printf ("Analyze");
      break;
    case FUNC_DYN:
      printf ("Dyn");
      break;
    case FUNC_AVG:
      printf ("Avg");
      break;
    case FUNC_VOCODER:
      printf ("Vocoder");
      break;
    case FUNC_CONV:
      printf ("Conv");
      break;
    case FUNC_GRAPHEQ:
      printf ("GraphEQ");
      break;
    case FUNC_TEST:
      printf ("Test");
      break;
    default:
      printf ("\n*** name of function not available ***");
      break;
    }
  printf (".\n");

  printf ("       ");
  switch (errno)
    {
    case ERR_NUMERO_PARAMETRI:
      printf ("invalid number of parameters:\n");
      Sinossi (func);
      break;
    case ERR_VALORE_INVALIDO:
      printf ("invalid value\n");
      break;
    case ERR_FILE_NOWAV:
      printf ("no RIFF wave file\n");
      break;
    case ERR_PUNTATORE_ERRATO:
      printf ("invalid pointer\n");
      break;
    case ERR_TRACCIA_ERRATA:
      printf ("bad track\n");
      break;
    case ERR_TRACCIA_INSUFFICIENTE:
      printf ("insufficient track size\n");
      break;
    case ERR_OFFSET_ERRATO:
      printf ("invalid offset\n");
      break;
    case ERR_NUMERO_CAMPIONI:
      printf ("invalid number of samples\n");
      break;
    case ERR_FREQUENZA_CENTRALE:
      printf ("invalid central frequency\n");
      break;
    case ERR_FREQUENZA_OSCILLATORE:
      printf ("invalid frequency\n");
      break;
    case ERR_LARGHEZZA_BANDA:
      printf ("invalid band width\n");
      break;
    case ERR_AMPIEZZA:
      printf ("invalid amplitude (from -9 to 9)\n");
      break;
    case ERR_AMPIEZZA_FILTRO:
      printf ("invalid amplitude (from >0 to <1)\n");
      break;
    case ERR_AMPIEZZA_OSCILLATORE:
      printf ("invalid amplitude (from 0 to 1)\n");
      break;
    case ERR_INVILUPPO:
      printf ("invalid envelope (CONST, LIN or LOG)\n");
      break;
    case ERR_RISOLUZIONE:
      printf ("invalid resolution (8 or 16 bit)\n");
      break;
    case ERR_TIPO_COPIA:
      printf ("invalid type for copy (MIX or NOMIX)\n");
      break;
    case ERR_RITARDO_REVERB:
      printf ("invalid value for delay\n");
      break;
    case ERR_BANDA_ANALISI:
      printf ("invalid band (OCTAVE|THIRDOCTAVE|CHROMATIC|LOGARITHMIC|LINEAR)\n");
      break;
    case ERR_LETTURA_STEM:
      printf ("error accessing stem\n");
      break;
    case ERR_STEM_DIVERSI:
      printf ("different size of stems\n");
      break;
    case ERR_STEM_MASSIMO:
      printf ("elements of stem > max (256)\n");
      break;
    case ERR_REXXPOOL:
      printf ("Error in REXX variable pool interface:\n");
      break;
    case ERR_OPEN_FILE:
      printf ("error opening file\n");
      break;
    case ERR_CLOSE_FILE:
      printf ("error closing file\n");
      break;
    case ERR_READ_WAV_STRUCT:
      printf ("error reading RIFF structure\n");
      break;
    case ERR_WRITE_WAV_STRUCT:
      printf ("error writing RIFF structure\n");
      break;
    case ERR_DOSALLOCMEM:
      printf ("error in DosAllocMem:\n");
      break;
    case ERR_DOSFREEMEM:
      printf ("error in DosFreeMem:\n");
      break;
    case ERR_ALLOCMEM:
      printf ("error in allocmem:\n");
      break;
    case ERR_FREEMEM:
      printf ("error in freemem:\n");
      break;
    default:
      printf ("*** error description not available ***\n");
      break;
    }

}
