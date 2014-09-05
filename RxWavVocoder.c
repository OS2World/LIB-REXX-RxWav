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

#define FORWARD 1
#define INVERSE 0
#define K_PUNTI 4096
#define K_PUNTI1 (K_PUNTI + 1)


double P, Pinc, synt, *WAnalisi, *WSintesi, *inout, *buffer, *dativoc;
double lastphase[2][K_PUNTI1], lastamp[2][K_PUNTI1], lastfreq[2][K_PUNTI1],
  indexmat[2][K_PUNTI1];

/***********************************************************************
Vocoder:
tratto da Ceres di (oyvindha@notam.uio.no)


***********************************************************************/
ULONG
WavVocoder (PCSZ name, LONG argc, const RXSTRING * argv,
	    PCSZ queuename, PRXSTRING retstr)
{
  PSHORT pCh, pCh2, ptemp;
  APIRET rc;
  int i, frame, FDec, nCamp;
  int campioni, puntifft, Kn2, Koverlap;
  double soglia;

  if (argc != 4)
    {
      SendMsg (FUNC_VOCODER, ERR_NUMERO_PARAMETRI);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[0].strptr, "%d", &pCh))
    {
      SendMsg (FUNC_VOCODER, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  if (!sscanf (argv[1].strptr, "%d", &pCh2))
    {
      SendMsg (FUNC_VOCODER, ERR_PUNTATORE_ERRATO);
      return INVALID_ROUTINE;
    }

  nCamp = atol (argv[2].strptr);
  if (nCamp < 1)
    {
      SendMsg (FUNC_VOCODER, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }

  pCh = (PSHORT) AllineaCh (pCh, (ULONG) nCamp, FUNC_VOCODER);
  if (!pCh)
    return INVALID_ROUTINE;

  pCh2 = (PSHORT) AllineaCh (pCh2, (ULONG) nCamp, FUNC_VOCODER);
  if (!pCh2)
    return INVALID_ROUTINE;

  soglia = atof (argv[3].strptr) / 1000000;
  if ((soglia < 0) | (soglia > 1))
    {
      SendMsg (FUNC_VOCODER, ERR_VALORE_INVALIDO);
      return INVALID_ROUTINE;
    }


  fvec (inout, K_PUNTI);
  fvec (buffer, K_PUNTI);
  fvec (dativoc, K_PUNTI);
  fvec (WAnalisi, K_PUNTI);
  fvec (WSintesi, K_PUNTI);

  campioni = K_PUNTI;
  puntifft = K_PUNTI / 2;
  Kn2 = K_PUNTI / 2;
  Koverlap = 2;
  FDec = campioni / Koverlap;

  makewindows (WAnalisi, WSintesi, campioni, campioni, FDec);

  for (frame = 0; frame < (10 * nCamp / K_PUNTI); frame++)
    {
      for (i = 0; i < campioni; i++)
	inout[i] = (double) *pCh++ / (double) MAX_CAMPIONE;

      fold (inout, WAnalisi, campioni, buffer, campioni, frame * campioni);
      for (i = 0; i < campioni; i++)
	inout[i] = (double) 0;
/*
 */
      rfft (buffer, puntifft, FORWARD);
      convert (buffer, dativoc, Kn2, FDec, FreqCamp, 1);
      /*
         if(frame==100) for (i=0; i<Kn2; i++) {
         printf("freq %f, amp %f\n", dativoc[i+i+1], dativoc[i+i] * 1000);
         }

       */
      for (i = 0; i < Kn2; i++)
	{
	  if (dativoc[i + i] < soglia)
	    dativoc[i + i] = 0;
	}

      unconvert (dativoc, buffer, Kn2, FDec, FreqCamp, 1);
      rfft (buffer, puntifft, INVERSE);
      overlapadd (buffer, campioni, WSintesi, inout, campioni, frame * campioni);

      for (i = 0; i < campioni; i++)
	*pCh2++ = *pCh2 + (SHORT) (inout[i] * MAX_CAMPIONE);
      pCh = pCh - (SHORT) (K_PUNTI * 0.9);
      pCh2 = pCh2 - (SHORT) (K_PUNTI * 0.9);
    }

  free (inout);
  free (buffer);
  free (dativoc);
  free (WAnalisi);
  free (WSintesi);
  sprintf (retstr->strptr, "%f", ERR_OK);
  retstr->strlength = strlen (retstr->strptr);
  return VALID_ROUTINE;
}






/* FFT ROUTINES */

/* If forward is true, rfft replaces 2*N real data points in x with
   N complex values representing the positive frequency half of their
   Fourier spectrum, with x[1] replaced with the real part of the Nyquist
   frequency value.  If forward is false, rfft expects x to contain a
   positive frequency spectrum arranged as before, and replaces it with
   2*N real values.  N MUST be a power of 2. */

rfft (x, N, forward)
     double x[];
     int N, forward;
{
  double c1, c2, h1r, h1i, h2r, h2i, wr, wi, wpr, wpi, temp, theta;
  double xr, xi;
  int i, i1, i2, i3, i4, N2p1;

  theta = PI / N;
  wr = 1.;
  wi = 0.;
  c1 = 0.5;
  if (forward)
    {
      c2 = -0.5;
      cfft (x, N, forward);
      xr = x[0];
      xi = x[1];
    }
  else
    {
      c2 = 0.5;
      theta = -theta;
      xr = x[1];
      xi = 0.;
      x[1] = 0.;
    }
  wpr = -2. * pow (sin (0.5 * theta), 2.);
  wpi = sin (theta);
  N2p1 = (N << 1) + 1;
  for (i = 0; i <= N >> 1; i++)
    {
      i1 = i << 1;
      i2 = i1 + 1;
      i3 = N2p1 - i2;
      i4 = i3 + 1;
      if (i == 0)
	{
	  h1r = c1 * (x[i1] + xr);
	  h1i = c1 * (x[i2] - xi);
	  h2r = -c2 * (x[i2] + xi);
	  h2i = c2 * (x[i1] - xr);
	  x[i1] = h1r + wr * h2r - wi * h2i;
	  x[i2] = h1i + wr * h2i + wi * h2r;
	  xr = h1r - wr * h2r + wi * h2i;
	  xi = -h1i + wr * h2i + wi * h2r;
	}
      else
	{
	  h1r = c1 * (x[i1] + x[i3]);
	  h1i = c1 * (x[i2] - x[i4]);
	  h2r = -c2 * (x[i2] + x[i4]);
	  h2i = c2 * (x[i1] - x[i3]);
	  x[i1] = h1r + wr * h2r - wi * h2i;
	  x[i2] = h1i + wr * h2i + wi * h2r;
	  x[i3] = h1r - wr * h2r + wi * h2i;
	  x[i4] = -h1i + wr * h2i + wi * h2r;
	}
      wr = (temp = wr) * wpr - wi * wpi + wr;
      wi = wi * wpr + temp * wpi + wi;
    }
  if (forward)
    x[1] = xr;
  else
    cfft (x, N, forward);
}

/* cfft replaces double array x containing NC complex values
   (2*NC double values alternating real, imagininary, etc.)
   by its Fourier transform if forward is true, or by its
   inverse Fourier transform if forward is false, using a
   recursive Fast Fourier transform method due to Danielson
   and Lanczos.  NC MUST be a power of 2. */

cfft (x, NC, forward)
     double x[];
     int NC, forward;
{
  double wr, wi, wpr, wpi, theta, scale;
  int mmax, ND, m, i, j, delta;

  ND = NC << 1;
  bitreverse (x, ND);
  for (mmax = 2; mmax < ND; mmax = delta)
    {
      delta = mmax << 1;
      theta = TWOPI / (forward ? mmax : -mmax);
      wpr = -2. * pow (sin (0.5 * theta), 2.);
      wpi = sin (theta);
      wr = 1.;
      wi = 0.;
      for (m = 0; m < mmax; m += 2)
	{
	  double rtemp, itemp;
	  for (i = m; i < ND; i += delta)
	    {
	      j = i + mmax;
	      rtemp = wr * x[j] - wi * x[j + 1];
	      itemp = wr * x[j + 1] + wi * x[j];
	      x[j] = x[i] - rtemp;
	      x[j + 1] = x[i + 1] - itemp;
	      x[i] += rtemp;
	      x[i + 1] += itemp;
	    }
	  wr = (rtemp = wr) * wpr - wi * wpi + wr;
	  wi = wi * wpr + rtemp * wpi + wi;
	}
    }

/* scale output */

  scale = forward ? 1. / ND : 2.;
  for (i = 0; i < ND; i++)
    x[i] *= scale;
}

/* bitreverse places double array x containing N/2 complex values
   into bit-reversed order */

bitreverse (x, N)
     double x[];
     int N;
{
  double rtemp, itemp;
  int i, j, m;

  for (i = j = 0; i < N; i += 2, j += m)
    {
      if (j > i)
	{
	  rtemp = x[j];
	  itemp = x[j + 1];	/* complex exchange */
	  x[j] = x[i];
	  x[j + 1] = x[i + 1];
	  x[i] = rtemp;
	  x[i + 1] = itemp;
	}
      for (m = N >> 1; m >= 2 && j >= m; m >>= 1)
	j -= m;
    }
}


/* PHASE VOCODER INTERNALS */

/*
 * make balanced pair of analysis (A) and synthesis (S) windows;
 * window lengths are Nw, FFT length is N, synthesis interpolation
 * factor is I
 */
makewindows (A, S, Nw, N, I)
     double A[], S[];
     int Nw, N, I;
{
  int i;
  double sum;

  for (i = 0; i < Nw; i++)
    A[i] = S[i] = 0.54 - 0.46 * cos (TWOPI * i / (Nw - 1));

/*
 * when Nw > N, also apply interpolating (sinc) windows to
 * ensure that window are 0 at increments of N (the FFT length)
 * away from the center of the analysis window and of I away
 * from the center of the synthesis window
 */
  if (Nw > N)
    {
      double x;

/*
 * take care to create symmetrical windows
 */
      x = -(Nw - 1) / 2.;
      for (i = 0; i < Nw; i++, x += 1.)
	if (x != 0.)
	  {
	    A[i] *= N * sin (PI * x / N) / (PI * x);
	    S[i] *= I * sin (PI * x / I) / (PI * x);
	  }
    }
/*
 * normalize windows for unity gain across unmodified
 * analysis-synthesis procedure
 */
  for (sum = i = 0; i < Nw; i++)
    sum += A[i];

  for (i = 0; i < Nw; i++)
    {
      double afac = 2. / sum;
      double sfac = Nw > N ? 1. / afac : afac;
      A[i] *= afac;
      S[i] *= sfac;
    }

  if (Nw <= N)
    {
      for (sum = i = 0; i < Nw; i += I)
	sum += S[i] * S[i];
      for (sum = 1. / sum, i = 0; i < Nw; i++)
	S[i] *= sum;
    }
}


/*
 * multiply current input I by window W (both of length Nw);
 * using modulus arithmetic, fold and rotate windowed input
 * into output array O of (FFT) length N according to current
 * input time n
 */
fold (I, W, Nw, O, N, n)
     double I[], W[], O[];
     int Nw, N, n;
{

  int i;

  for (i = 0; i < N; i++)
    O[i] = 0.;

  while (n < 0)
    n += N;
  n %= N;
  for (i = 0; i < Nw; i++)
    {
      O[n] += I[i] * W[i];
      if (++n == N)
	n = 0;
    }
}


/* S is a spectrum in rfft format, i.e., it contains N real values
   arranged as real followed by imaginary values, except for first
   two values, which are real parts of 0 and Nyquist frequencies;
   convert first changes these into N/2+1 PAIRS of magnitude and
   phase values to be stored in output array C; the phases are then
   unwrapped and successive phase differences are used to compute
   estimates of the instantaneous frequencies for each phase vocoder
   analysis channel; decimation rate D and sampling rate R are used
   to render these frequency values directly in Hz. */

convert (S, C, N2, D, R, ch)
     double S[], C[];
     int N2, D, R, ch;
{
  static int first = 1;
  static double fundamental, factor;
  double phase, phasediff;
  int real, imag, amp, freq;
  double a, b;
  int i;

/* first pass: allocate zeroed space for previous phase
   values for each channel and compute constants */

  if (first)
    {
      first = 0;
      fundamental = (double) R / (N2 << 1);
      factor = R / (D * TWOPI);
    }

/* unravel rfft-format spectrum: note that N2+1 pairs of
   values are produced */

  for (i = 0; i <= N2; i++)
    {
      imag = freq = (real = amp = i << 1) + 1;
      a = (i == N2 ? S[1] : S[real]);
      b = (i == 0 || i == N2 ? 0. : S[imag]);

/* compute magnitude value from real and imaginary parts */

      C[amp] = hypot (a, b);

/* compute phase value from real and imaginary parts and take
   difference between this and previous value for each channel */

      if (C[amp] == 0.)
	phasediff = 0.;
      else
	{
	  phasediff = (phase = -atan2 (b, a)) - lastphase[ch][i];
	  lastphase[ch][i] = phase;

/* unwrap phase differences */

	  while (phasediff > PI)
	    phasediff -= TWOPI;
	  while (phasediff < -PI)
	    phasediff += TWOPI;
	}

/* convert each phase difference to Hz */

      C[freq] = phasediff * factor + i * fundamental;
    }
}

double
lpamp (double omega, double a0, double *coef, int M)
{
  double wpr, wpi, wr, wi, re, im, temp, lp;
  int i;

  if (a0 == 0.)
    return (0.);
  wpr = cos (omega);
  wpi = sin (omega);
  re = wr = 1.;
  im = wi = 0.;
  for (coef++, i = 1; i <= M; i++, coef++)
    {
      wr = (temp = wr) * wpr - wi * wpi;
      wi = wi * wpr + temp * wpi;
      re += *coef * wr;
      im += *coef * wi;
    }
  if (re == 0. && im == 0.)
    lp = 0.;
  else
    lp = sqrt (a0 / (re * re + im * im));
  return (lp);
}


/* oscillator bank resynthesizer for phase vocoder analyzer
   uses sum of N+1 cosinusoidal table lookup oscillators to
   compute I (interpolation factor) samples of output O
   from N+1 amplitude and frequency value-pairs in C;
   frequencies are scaled by P */

oscbank (C, N, R, I, O, ch, Nw, coef, Np, a0)
     double C[], O[], coef[], a0;
     int N, R, I, ch, Nw, Np;
{
  static int L = 8192, first = 1;
  static double *table;
  int amp, freq, n, chan;
  double Iinv;

/* first pass: allocate memory for and compute cosine table */

  if (first)
    {
      first = 0;
      fvec (table, L);

      for (n = 0; n < L; n++)
	table[n] = N * cos (TWOPI * (double) n / L);
    }

  Iinv = 1. / I;

/* for each channel, compute I samples using linear
   interpolation on the amplitude and frequency
   control values */

  for (chan = 0; chan < K_PUNTI; chan++)
    {

      double a, ainc, f, finc, address;

      freq = (amp = (chan << 1)) + 1;
      C[freq] *= Pinc;
      finc = (C[freq] - (f = lastfreq[ch][chan])) * Iinv;
      if (C[amp] < synt)
	C[amp] = 0.;
      else if (Np)
	C[amp] *= lpamp (chan * P * PI / N, a0, coef, Np) /
	  lpamp (chan * PI / N, a0, coef, Np);
      ainc = (C[amp] - (a = lastamp[ch][chan])) * Iinv;
      address = indexmat[ch][chan];

/* accumulate the I samples from each oscillator into
   output array O (initially assumed to be zero);
   f is frequency in Hz scaled by oscillator increment
   factor and pitch (Pinc); a is amplitude; */

      if (ainc != 0. || a != 0.)
	for (n = 0; n < I; n++)
	  {
	    O[n] += a * table[(int) address];

	    address += f;

	    while (address >= L)
	      address -= L;

	    while (address < 0)
	      address += L;

	    a += ainc;
	    f += finc;
	  }

/* save current values for next iteration */
      lastfreq[ch][chan] = C[freq];
      lastamp[ch][chan] = C[amp];
      indexmat[ch][chan] = address;
    }
}


/* unconvert essentially undoes what convert does, i.e., it
   turns N2+1 PAIRS of amplitude and frequency values in
   C into N2 PAIR of complex spectrum data (in rfft format)
   in output array S; sampling rate R and interpolation factor
   I are used to recompute phase values from frequencies */

unconvert (C, S, N2, I, R, ch)
     double C[], S[];
     int N2, I, R, ch;
{
  static int first = 1;
  static double fundamental, factor;
  int i, real, imag, amp, freq;
  double mag, phase;

/* first pass: allocate memory and compute constants */

  if (first)
    {
      first = 0;
      fundamental = (double) R / (N2 << 1);
      factor = TWOPI * I / R;
    }

/* subtract out frequencies associated with each channel,
   compute phases in terms of radians per I samples, and
   convert to complex form */

  for (i = 0; i <= N2; i++)
    {
      imag = freq = (real = amp = i << 1) + 1;
      if (i == N2)
	real = 1;
      mag = C[amp];
      lastphase[ch][i] += C[freq] - i * fundamental;
      phase = lastphase[ch][i] * factor;
      S[real] = mag * cos (phase);
      if (i != N2)
	S[imag] = -mag * sin (phase);
    }
}


/*
 * input I is a folded spectrum of length N; output O and
 * synthesis window W are of length Nw--overlap-add windowed,
 * unrotated, unfolded input data into output O
 */
overlapadd (I, N, W, O, Nw, n)
     double I[], W[], O[];
     int N, Nw, n;
{
  int i;
  while (n < 0)
    n += N;
  n %= N;
  for (i = 0; i < Nw; i++)
    {
      O[i] += I[n] * W[i];
      if (++n == N)
	n = 0;
    }
}
