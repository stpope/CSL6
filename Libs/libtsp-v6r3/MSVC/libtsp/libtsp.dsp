# Microsoft Developer Studio Project File - Name="libtsp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtsp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtsp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtsp.mak" CFG="libtsp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtsp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x1009
# ADD RSC /l 0x1009
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libtsp.lib"
# Begin Target

# Name "libtsp - Win32 Release"
# Begin Group "AF"

# PROP Default_Filter ""
# Begin Group "header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFdecSpeaker.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFfindType.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFgetInfoRec.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFoptions.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFprAFpar.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFpreSetWPar.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdAIhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdAUhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdBLhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdEShead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdINhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdNShead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdSFhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdSPhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdSWhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdTAhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdTextAFsp.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFrdWVhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFreadHead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFsetNHread.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFsetNHwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFsetRead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFsetWrite.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFupdAIhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFupdAUhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFupdHead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFupdWVhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFwrAIhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFwrAUhead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFwriteHead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\header\AFwrWVhead.c
# End Source File
# End Group
# Begin Group "dataio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdRdAlaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdRdFx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdRdIx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdRdMulaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdRdTA.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdRdU1.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdWrAlaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdWrFx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdWrIx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdWrMulaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdWrTA.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFdWrU1.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfRdAlaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfRdFx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfRdIx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfRdMulaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfRdTA.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfRdU1.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfWrAlaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfWrFx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfWrIx.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfWrMulaw.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfWrTA.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFfWrU1.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFgetLine.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFposition.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFseek.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\dataio\AFtell.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\AF\AFclose.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFdReadData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFdWriteData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFfReadData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFfWriteData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFopenRead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFopenWrite.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFopnRead.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFopnWrite.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFreadData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFsetFileType.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFsetInfo.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFsetNHpar.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFsetSpeaker.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\AF\AFwriteData.c
# End Source File
# End Group
# Begin Group "FI"

# PROP Default_Filter ""
# Begin Group "FI-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\FI\nucleus\FIrCosF.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\nucleus\FIxKaiser.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdBiquad.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdConvol.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdConvSI.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdFiltAP.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdFiltIIR.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdKaiserLPF.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdReadFilt.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdWinHamm.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIdWinKaiser.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfBiquad.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfConvol.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfConvSI.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfDeem.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfFiltAP.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfFiltIIR.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfKaiserLPF.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfPreem.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfReadFilt.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfWinHamm.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfWinHCos.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfWinKaiser.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIfWinRCos.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIgdelFIR.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FI\FIgdelIIR.c
# End Source File
# End Group
# Begin Group "FL"

# PROP Default_Filter ""
# Begin Group "FL-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\FL\nucleus\FLdReadTF.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\nucleus\FLexist.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\nucleus\FLfileSize.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\nucleus\FLfReadTF.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\nucleus\FLjoinNames.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\nucleus\FLseekable.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\nucleus\FLterm.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\FL\FLbackup.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLbaseName.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLdate.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLdefName.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLdirName.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLdReadData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLdWriteData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLexpHome.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLextName.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLfileDate.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLfReadData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLfullName.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLfWriteData.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLgetLine.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLgetRec.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLhomeDir.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLpathList.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLpreName.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLprompt.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FL\FLreadLine.c
# End Source File
# End Group
# Begin Group "FN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\FN\FNbessI0.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FN\FNevChebP.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FN\FNgcd.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FN\FNiLog2.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\FN\FNsinc.c
# End Source File
# End Group
# Begin Group "MA"

# PROP Default_Filter ""
# Begin Group "MA-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\MA\nucleus\MAfChFactor.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\nucleus\MAfLTSolve.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\nucleus\MAfTTSolve.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\MA\MAdAllocMat.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAdFreeMat.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAdPrint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfAllocMat.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfChSolve.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfFreeMat.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfPrint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfSubMat.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfSyBilin.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfSyQuad.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfTpQuad.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAfTpSolve.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAiPrint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MA\MAlPrint.c
# End Source File
# End Group
# Begin Group "MS"

# PROP Default_Filter ""
# Begin Group "MS-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\MS\nucleus\MScoefMC.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\nucleus\MSdSlopeMC.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\nucleus\MSevalMC.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\nucleus\MSfSlopeMC.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\MS\MSdConvCof.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSdDeconvCof.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSdIntLin.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSdIntMC.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSdLocate.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSdNint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSdPolyInt.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSfConvCof.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSfGaussRand.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSfIntLin.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSfIntMC.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSfUnifRand.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSiCeil.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSiFloor.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSiPower.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSlCeil.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSlFloor.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\MS\MSratio.c
# End Source File
# End Group
# Begin Group "SP"

# PROP Default_Filter ""
# Begin Group "SP-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\SP\nucleus\SPdPreFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\nucleus\SPdTrMat2.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\nucleus\SPfPreFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\nucleus\SPfTrMat2.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\SP\SPautoc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcepXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcorBWexp.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcorFilt.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcorFmse.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcorPmse.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcorXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovar.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovCXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovFilt.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovFmse.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovLXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovMXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovPmse.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPcovXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPdCFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPdQuantL.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPdRFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPecXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPfCFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPfDCT.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPfQuantL.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPfQuantU.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPfRFFT.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPlpcISdist.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPlpcLSdist.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPlsfXpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPpcBWexp.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPpcXcep.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPpcXcor.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPpcXec.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPpcXlsf.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPpcXrc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\SP\SPrcXpc.c
# End Source File
# End Group
# Begin Group "ST"

# PROP Default_Filter ""
# Begin Group "ST-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\ST\nucleus\STdec1val.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\nucleus\STdecNval.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\nucleus\STdecPair.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\nucleus\STstrDots.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\nucleus\STstrstrNM.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\nucleus\STtrimIws.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\nucleus\STtrimNMax.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\ST\STcatMax.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STcopyMax.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STcopyNMax.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdec1double.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdec1float.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdec1int.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdec1long.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecDfrac.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecIfrac.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecIrange.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecLrange.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecNdouble.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecNfloat.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecNint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STdecNlong.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STfindToken.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STkeyMatch.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STkeyXpar.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STstrLC.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STtrim.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\ST\STunQuote.c
# End Source File
# End Group
# Begin Group "UT"

# PROP Default_Filter ""
# Begin Group "UT-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTbyteCode.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTbyteOrder.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTcheckIEEE.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTctime.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTdecOption.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTdIEEE80.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTeIEEE80.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTgetHost.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTgetUser.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\nucleus\UTswapCode.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\UT\UTdate.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTerror.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTfree.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTgetOption.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UThalt.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTmalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTrealloc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTsetProg.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTsysMsg.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTuserName.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\UT\UTwarn.c
# End Source File
# End Group
# Begin Group "VR"

# PROP Default_Filter ""
# Begin Group "VR-nucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libtsp\VR\nucleus\VRswapBytes.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdAdd.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdCopy.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdCorSym.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdDotProd.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdLinInc.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdMult.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdPrint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdRFFTMSq.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdScale.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdSet.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdShift.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdSymPart.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRdZero.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfAdd.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfAmpldB.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfCopy.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfCorSym.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfDiff.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfDiffSq.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfDotProd.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfLog10.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfMax.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfMin.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfMult.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfPow10.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfPrint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfRev.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfScale.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfSet.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfShift.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfSum.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRfZero.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRiPrint.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRiZero.c
# End Source File
# Begin Source File

SOURCE=..\..\libtsp\VR\VRlPrint.c
# End Source File
# End Group
# End Target
# End Project
