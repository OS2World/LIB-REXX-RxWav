
.SUFFIXES: .c

RxWav.DLL:   \
  RxWavMain.OBJ \
  RxWavMsg.OBJ \
  RxWavIO.OBJ \
  RxWavOsc.OBJ \
  RxWavFilter.OBJ \
  RxWavEco.OBJ \
  RxWavPeak.OBJ \
  RxWavPeekPoke.OBJ \
  RxWavSeek.OBJ \
  RxWavUtil.OBJ \
  RxWavAnalyze.OBJ \
  RxWavDyn.OBJ \
  RxWavVocoder.OBJ \
  RxWavConv.OBJ \
  RxWavFIR.OBJ \
  RxWavTest.OBJ \
  RxWav.DEF \
  makefile
  LINK386.EXE @<<
     /A:4 /NOI +
     dll0.obj +
     RxWavMain.OBJ +
     RxWavMsg.OBJ +
     RxWavIO.OBJ +
     RxWavOsc.OBJ +
     RxWavFilter.OBJ +
     RxWavEco.OBJ +
     RxWavPeak.OBJ +
     RxWavPeekPoke.OBJ +
     RxWavSeek.OBJ +
     RxWavUtil.OBJ +
     RxWavAnalyze.OBJ +
     RxWavDyn.OBJ +
     RxWavVocoder.OBJ +
     RxWavConv.OBJ +
     RxWavFIR.OBJ +
     RxWavTest.OBJ
     RxWav.DLL

     os2 gcc c c_dllso c_alias sys end
     RxWav.DEF;
<<

{.}.c.obj:
   gcc -Zdll -Zomf -Zso -Zsys -s -O -c -o .\$*.obj .\$*.c

!include RxWav.DEP
