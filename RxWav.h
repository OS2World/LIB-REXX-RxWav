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

#define  VALID_ROUTINE     0
#define  INVALID_ROUTINE  40

#define MAX_DIGITS         9
#define MAX              256

#define PI 3.141592654
#define TWOPI 6.2831853

#define BUILDRXSTRING(t, s) { \
  strcpy((t)->strptr,(s));\
  (t)->strlength = strlen((s)); \
}

#define fvec(name, size)\
if ((name=(double *)calloc(size, sizeof(double)))==NULL) {\
   SendMsg(0, ERR_ALLOCMEM);\
   return INVALID_ROUTINE; }

#define FreqCamp       44100
#define MAX_CAMPIONE   32767
#define NBit              16

/* Formato file WAV */

struct wav
  {
    char w_riff[4];
    int w_len_tot;
    char w_wave[4];
    char w_fmt[4];
    int w_len_fmt;
    short int w_format_tag;
    short int w_stereo;
    int w_freq_camp;
    int w_byte_sec;
    short int w_block_align;
    short int w_bit_camp;
    char w_data[4];
    int w_len_data;
  };


typedef struct RxStemData
  {
    SHVBLOCK shvb;
    CHAR varname[MAX];
    CHAR ibuf[MAX];
    ULONG stemlen;
    ULONG count;

  }
RXSTEMDATA;





/* errori procedurali */
#define ERR_OK                         0

#define ERR_NUMERO_PARAMETRI           1
#define ERR_VALORE_INVALIDO            2
#define ERR_FILE_NOWAV                 3
#define ERR_PUNTATORE_ERRATO           4
#define ERR_TRACCIA_ERRATA             5
#define ERR_TRACCIA_INSUFFICIENTE      6
#define ERR_OFFSET_ERRATO              7
#define ERR_NUMERO_CAMPIONI            8
#define ERR_FREQUENZA_CENTRALE         9
#define ERR_FREQUENZA_OSCILLATORE     10
#define ERR_LARGHEZZA_BANDA           11
#define ERR_AMPIEZZA                  12
#define ERR_AMPIEZZA_FILTRO           13
#define ERR_AMPIEZZA_OSCILLATORE      14
#define ERR_INVILUPPO                 15
#define ERR_RISOLUZIONE               16
#define ERR_TIPO_COPIA                17
#define ERR_RITARDO_REVERB            18
#define ERR_BANDA_ANALISI             19
#define ERR_LETTURA_STEM              20
#define ERR_STEM_DIVERSI              21
#define ERR_STEM_MASSIMO              22

#define ERR_REXXPOOL                 100
#define ERR_OPEN_FILE                101
#define ERR_CLOSE_FILE               102
#define ERR_READ_WAV_STRUCT          103
#define ERR_WRITE_WAV_STRUCT         104
#define ERR_DOSALLOCMEM              105
#define ERR_DOSFREEMEM               106
#define ERR_ALLOCMEM                 107
#define ERR_FREEMEM                  108


/* errori in bseerr.h */
#define  NO_ERROR                      0
#define  ERROR_INVALID_ADDRESS       487


/* funzioni definite                   */

static PSZ RxWavFunc[] =
{
  "WavLoadFuncs",
  "WavDropFuncs",
  "WavAllocTrac",
  "WavDropTrac",
  "WavQueryFile",
  "WavReadStereo",
  "WavWriteStereo",
  "WavReadMono",
  "WavWriteMono",
  "WavCopyTrac",
  "WavSinOsc",
  "WavWhiteNoiseOsc",
  "WavPinkNoiseOsc",
  "WavMute",
  "WavTestOsc",
  "WavFilterBand",
  "WavFilterHigh",
  "WavFilterLow",
  "WavReverb",
  "WavRevert",
  "WavEco",
  "WavPeek",
  "WavPoke",
  "WavPeak",
  "WavSeek",
  "WavAnalyze",
  "WavDyn",
  "WavAvg",
  "WavVocoder",
  "WavConv",
  "WavGraphEQ",
  "WavTest",
};



#define FUNC_LOAD_FUNCS                1
#define FUNC_DROP_FUNCS                2
#define FUNC_ALLOC_TRAC                3
#define FUNC_DROP_TRAC                 4
#define FUNC_QUERY_FILE                5
#define FUNC_READ_STEREO               6
#define FUNC_WRITE_STEREO              7
#define FUNC_READ_MONO                 8
#define FUNC_WRITE_MONO                9
#define FUNC_COPY_TRAC                10
#define FUNC_SIN_OSC                  11
#define FUNC_TEST_OSC                 12
#define FUNC_WHITE_NOISE              13
#define FUNC_PINK_NOISE               14
#define FUNC_MUTE                     15
#define FUNC_FILTER_BAND              16
#define FUNC_FILTER_HIGH              17
#define FUNC_FILTER_LOW               18
#define FUNC_REVERB                   19
#define FUNC_REVERT                   20
#define FUNC_ECO                      21
#define FUNC_PEEK                     22
#define FUNC_POKE                     23
#define FUNC_PEAK                     24
#define FUNC_SEEK                     25
#define FUNC_ANALYZE                  26
#define FUNC_DYN                      27
#define FUNC_AVG                      28
#define FUNC_VOCODER                  29
#define FUNC_CONV                     30
#define FUNC_GRAPHEQ                  31
#define FUNC_TEST                    999


/* Modi di copia e inviluppi */
#define COPIA_NOMIX                    0
#define COPIA_MIX                      1
#define COPIA_INV_MIX                  2
#define COPIA_INV_NOMIX                3

#define INVILUPPO_COSTANTE             0
#define INVILUPPO_LINEARE              1
#define INVILUPPO_LOGARITMICO          2

#define BANDA_LINEARE                  0
#define BANDA_OTTAVE                   1
#define BANDA_TERZE                    2
#define BANDA_CROMATICA                3
#define BANDA_LOGARITMICA              4

#define PUNTI_FFT_LINEARE          65536
#define PUNTI_FFT_OTTAVE            8192
#define PUNTI_FFT_TERZE            16384
#define PUNTI_FFT_CROMATICA        32768
#define PUNTI_FFT_LOGARITMICA      65536
