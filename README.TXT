README for RxWav beta 0.11

I discovered a bug in RxWav beta 0.10. In function WavWriteStereo the left and
right channels were swapped. This is fixed now.

I also translated some parameters that were in Italian to English. I'm not a
musician and my primary language isn't English either, therefore if anybody
knows better names for the band type (especially THIRDOCTAVE, CHROMATIC), then
please let me know.

Finally, I did some code cleanup to compile without warnings.

WARNING: This code compiles fine with compiler provided with EMX, but not with
the new GCC beta 3.0.3. This maybe a problem with compilation switches not being
properly handled in the new version.

Here are the available functions and their syntax pulled from the code along
with the English parameters (> indicates input parameter, < indicates returned
value).

    WavLoadFunc
            < 0 | error
    WavDropFunc
            < 0 | error
    WavAllocTrac
            > samples
            < pointer
    WavDropTrac
            > pointer
            < 0 | error
    WavQueryFile
            > file name
            < 'NOWAV' | n_channels sample_rate n_bits n_samples
    WavReadStereo
            > file name
            > pointer to left channel
            > pointer to right channel
           [> type of read (MIX|NOMIX)]
           [> offset]
           [> length (samples)]
            < 0 | error
    WavWriteStereo
            > file name
            > pointer to left channel
            > pointer to right channel
            > length (samples)
            < 0 | error
    WavReadMono
            > file name
            > pointer
            < 0 | error
    WavWriteMono
            > file name
            > pointer
            > length (samples)
            < 0 | error
    WavCopyTrac
            > pointer to source
            > pointer to destination
            > length (samples)
           [> initial amplitude]
           [> final amplitude]
           [> envelope (CONST|LIN|LOG)]
           [> type of copy (MIX|NOMIX|INV_MIX|INV_NOMIX)]
            < 0 | error
    WavSinOsc
            > pointer
            > length (samples)
            > frequency in hertz
            > amplitude (from 0 to 1)
            < 0 | error
    WavTestOsc
            > pointer
            > length (samples)
            > amplitude (from 0 to 1)
            < 0 | error
    WavWhiteNoiseOsc
            > pointer
            > length (samples)
            > amplitude (from 0 to 1)
            < 0 | error
    WavPinkNoiseOsc
            > pointer
            > length (samples)
            > amplitude (from 0 to 1)
            < 0 | error
    WavMute
            > pointer
            > length (samples)
            < 0 | error
    WavFilterBand
            > pointer
            > length (samples)
            > central frequency
            > band width
            > amplitude
            < 0 | error
    WavFilterHigh
            > pointer
            > length (samples)
            > frequency band limit
            > amplitude
            < 0 | error
    WavFilterLow
            > pointer
            > length (samples)
            > frequency band limit
            > amplitude
            < 0 | error
    WavReverb
            > pointer
            > length (samples)
            > stem of delay (samples)
            > stem of amplitude (from 0 to 1)
            < 0 | error
    WavRevert
            > pointer
            > length (samples)
            < 0 | error
    WavEco
            > pointer
            > length (samples)
            > delay (samples)
            > amplitude (from 0 to 1)
            < 0 | error
    WavPeek
            > pointer
            < sample value | error
    WavPoke
            > pointer
            > sample value
            < 0 | error
    WavPeak
            > pointer
            > length (samples)
            < max peak value | error
    WavSeek
            > pointer
            > length (samples)
            > value of limit
            < offset | error
    WavAnalyze
            > pointer
            > band (OCTAVE|THIRDOCTAVE|CHROMATIC|LOGARITHMIC|LINEAR)
            > stem of spectrum power
            < 0 | error
    WavDyn
            > pointer
            > length (samples)
            > dynamic (from 0.01 to 9; >1=compress, <1=expand)
            < 0 | error
    WavAvg
            > pointer
            > length (samples)
           [> avg value (from -1 to 1)]
            < 0 | avg_signed avg_abs
    WavVocoder
            > pointer
            > length (samples)
            > gain (from 0 to 1)
            < 0 | error
    WavConv
            > pointer
            > pointer
            > length (samples)
            < 0 | error
    WavGraphEQ
            > pointer
            > length (samples)
            > stem of band amplitude
            < 0 | error

The original author (Giorgio Vicario) started a multiplatform project at
Sourceforge (http://sourceforge.net/projects/rxwav/) and can be reached at
katobleto@geocities.com or katobleto@yahoo.com or rxwav@sourceforge.net.

I wrote a REXX script WawUtil.CMD to do a few simple manipulations of WAV files
that utilizes RxWav library. The script is included.

If you wish to reach me you can do so at spamfilter@rafcio.com.

Rafal Szymczak

