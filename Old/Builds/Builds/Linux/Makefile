# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(TARGET_ARCH),)
  TARGET_ARCH := -march=native
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG" -D "_DEBUG=1" -D "USE_JUCE" -D "USE_VBAP" -D "USE_HRTF" -D "USE_AMBISONIC" -D "USE_FFTREAL" -D "USE_JSND" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I /usr/local/include -I ../../../CSL/Includes
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) -w
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -ldl -lfreetype -lpthread -lrt -lX11 -lGL -lGLU -lXinerama -lasound -lXext 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "DEBUG" -D "_DEBUG=1" -D "USE_JUCE" -D "USE_VBAP" -D "USE_HRTF" -D "USE_AMBISONIC" -D "USE_FFTREAL" -D "USE_JSND" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I /usr/local/include -I ../../../CSL/Includes
  TARGET := CSL_JUCE
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "USE_JUCE" -D "USE_VBAP" -D "USE_HRTF" -D "USE_AMBISONIC" -D "USE_FFTREAL" -D "USE_JSND" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I /usr/local/include -I ../../../CSL/Includes
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -O3
  CXXFLAGS += $(CFLAGS) -w
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -ldl -lfreetype -lpthread -lrt -lX11 -lGL -lGLU -lXinerama -lasound -lXext 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "NDEBUG=1" -D "USE_JUCE" -D "USE_VBAP" -D "USE_HRTF" -D "USE_AMBISONIC" -D "USE_FFTREAL" -D "USE_JSND" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I /usr/local/include -I ../../../CSL/Includes
  TARGET := CSL_JUCE
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

OBJECTS := \
  $(OBJDIR)/CSL_Core_233c1c07.o \
  $(OBJDIR)/CGestalt_8d21190c.o \
  $(OBJDIR)/FFT_Wrapper_c5ae5f76.o \
  $(OBJDIR)/FFTReal_372ce380.o \
  $(OBJDIR)/CPoint_20ad81a9.o \
  $(OBJDIR)/Variable_68ba43f8.o \
  $(OBJDIR)/RingBuffer_57a8b84c.o \
  $(OBJDIR)/ThreadUtilities_a8ca0c2e.o \
  $(OBJDIR)/BlockResizer_4547786d.o \
  $(OBJDIR)/Window_cbe0a43e.o \
  $(OBJDIR)/Envelope_7c9ae5de.o \
  $(OBJDIR)/Noise_d4000816.o \
  $(OBJDIR)/SimpleSines_6641a3b0.o \
  $(OBJDIR)/Oscillator_b434346e.o \
  $(OBJDIR)/Spectral_75380172.o \
  $(OBJDIR)/SHARC_67e00f59.o \
  $(OBJDIR)/KarplusString_418f94a3.o \
  $(OBJDIR)/Granulator_266c0e13.o \
  $(OBJDIR)/Lorenz_9933be90.o \
  $(OBJDIR)/OscillatorBL_98b77d78.o \
  $(OBJDIR)/WaveShaper_2ba15c18.o \
  $(OBJDIR)/BinaryOp_4646f343.o \
  $(OBJDIR)/Mixer_55899df2.o \
  $(OBJDIR)/Filters_ef714fe4.o \
  $(OBJDIR)/FIR_731983f8.o \
  $(OBJDIR)/Freeverb_1410fa8c.o \
  $(OBJDIR)/InOut_ae67fdd2.o \
  $(OBJDIR)/Clipper_e0f4feb6.o \
  $(OBJDIR)/SpatialSource_270ace98.o \
  $(OBJDIR)/SpatialAudio_a3316143.o \
  $(OBJDIR)/SpatialPanner_87d024db.o \
  $(OBJDIR)/SimplePanner_78e50941.o \
  $(OBJDIR)/DistanceSimulator_1070d1e8.o \
  $(OBJDIR)/SpeakerLayout_9ac7c762.o \
  $(OBJDIR)/PME_315b3ea5.o \
  $(OBJDIR)/PracticalSocket_47011285.o \
  $(OBJDIR)/VBAP_1e628c08.o \
  $(OBJDIR)/Ambisonic_647dfddc.o \
  $(OBJDIR)/AmbisonicPanner_1386d99a.o \
  $(OBJDIR)/AmbisonicUtilities_18b3c558.o \
  $(OBJDIR)/BinauralDB_241b2ea6.o \
  $(OBJDIR)/Binaural_2d8d95e8.o \
  $(OBJDIR)/Instrument_f4f81701.o \
  $(OBJDIR)/BasicFMInstrument_53b5d86c.o \
  $(OBJDIR)/SndFileInstrument_cc5c7d7c.o \
  $(OBJDIR)/AdditiveInstrument_2f4c54ed.o \
  $(OBJDIR)/FileIO_c148421e.o \
  $(OBJDIR)/SoundFile_d1377e39.o \
  $(OBJDIR)/SoundFileJ_572a92fb.o \
  $(OBJDIR)/OSC_support_2c0695d.o \
  $(OBJDIR)/Test_Support_33929ec7.o \
  $(OBJDIR)/Test_Oscillators_c6e83f4b.o \
  $(OBJDIR)/Test_Sources_9dea9e30.o \
  $(OBJDIR)/Test_Envelopes_ed289adb.o \
  $(OBJDIR)/Test_Effects_5fac67a.o \
  $(OBJDIR)/Test_Panners_55d0104d.o \
  $(OBJDIR)/Test_Control_3bae0d55.o \
  $(OBJDIR)/Test_Audio_f712740e.o \
  $(OBJDIR)/CSL_TestComponent_5e484c6a.o \
  $(OBJDIR)/JCSL_Widgets_edb9ebca.o \
  $(OBJDIR)/Main_bcf3b081.o \
  $(OBJDIR)/BinaryData_ce4232d4.o \
  $(OBJDIR)/juce_audio_basics_2442e4ea.o \
  $(OBJDIR)/juce_audio_devices_a4c8a728.o \
  $(OBJDIR)/juce_audio_formats_d349f0c8.o \
  $(OBJDIR)/juce_audio_processors_44a134a2.o \
  $(OBJDIR)/juce_audio_utils_f63b12e8.o \
  $(OBJDIR)/juce_core_aff681cc.o \
  $(OBJDIR)/juce_data_structures_bdd6d488.o \
  $(OBJDIR)/juce_events_79b2840.o \
  $(OBJDIR)/juce_graphics_c8f1e7a4.o \
  $(OBJDIR)/juce_gui_basics_a630dd20.o \
  $(OBJDIR)/juce_gui_extra_7767d6a8.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking CSL_JUCE
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning CSL_JUCE
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

$(OBJDIR)/CSL_Core_233c1c07.o: ../../../CSL/Kernel/CSL_Core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CSL_Core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CGestalt_8d21190c.o: ../../../CSL/Kernel/CGestalt.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CGestalt.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FFT_Wrapper_c5ae5f76.o: ../../../CSL/Utilities/FFT_Wrapper.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FFT_Wrapper.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FFTReal_372ce380.o: ../../../CSL/Utilities/FFTReal.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FFTReal.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CPoint_20ad81a9.o: ../../../CSL/Utilities/CPoint.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CPoint.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Variable_68ba43f8.o: ../../../CSL/Utilities/Variable.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Variable.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RingBuffer_57a8b84c.o: ../../../CSL/Utilities/RingBuffer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RingBuffer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ThreadUtilities_a8ca0c2e.o: ../../../CSL/Utilities/ThreadUtilities.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ThreadUtilities.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BlockResizer_4547786d.o: ../../../CSL/Utilities/BlockResizer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BlockResizer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Window_cbe0a43e.o: ../../../CSL/Sources/Window.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Window.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Envelope_7c9ae5de.o: ../../../CSL/Sources/Envelope.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Envelope.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Noise_d4000816.o: ../../../CSL/Sources/Noise.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Noise.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SimpleSines_6641a3b0.o: ../../../CSL/Sources/SimpleSines.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SimpleSines.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Oscillator_b434346e.o: ../../../CSL/Sources/Oscillator.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Oscillator.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Spectral_75380172.o: ../../../CSL/Sources/Spectral.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Spectral.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SHARC_67e00f59.o: ../../../CSL/Sources/SHARC.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SHARC.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/KarplusString_418f94a3.o: ../../../CSL/Sources/KarplusString.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling KarplusString.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Granulator_266c0e13.o: ../../../CSL/Sources/Granulator.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Granulator.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Lorenz_9933be90.o: ../../../CSL/Sources/Lorenz.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Lorenz.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OscillatorBL_98b77d78.o: ../../../CSL/Sources/OscillatorBL.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OscillatorBL.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/WaveShaper_2ba15c18.o: ../../../CSL/Sources/WaveShaper.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling WaveShaper.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryOp_4646f343.o: ../../../CSL/Processors/BinaryOp.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryOp.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Mixer_55899df2.o: ../../../CSL/Processors/Mixer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Mixer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Filters_ef714fe4.o: ../../../CSL/Processors/Filters.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Filters.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FIR_731983f8.o: ../../../CSL/Processors/FIR.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FIR.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Freeverb_1410fa8c.o: ../../../CSL/Processors/Freeverb.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Freeverb.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/InOut_ae67fdd2.o: ../../../CSL/Processors/InOut.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling InOut.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Clipper_e0f4feb6.o: ../../../CSL/Processors/Clipper.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Clipper.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SpatialSource_270ace98.o: ../../../CSL/Spatializers/SpatialSource.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SpatialSource.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SpatialAudio_a3316143.o: ../../../CSL/Spatializers/SpatialAudio.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SpatialAudio.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SpatialPanner_87d024db.o: ../../../CSL/Spatializers/SpatialPanner.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SpatialPanner.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SimplePanner_78e50941.o: ../../../CSL/Spatializers/SimplePanner.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SimplePanner.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DistanceSimulator_1070d1e8.o: ../../../CSL/Spatializers/DistanceSimulator.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DistanceSimulator.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SpeakerLayout_9ac7c762.o: ../../../CSL/Spatializers/SpeakerLayout.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SpeakerLayout.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PME_315b3ea5.o: ../../../CSL/Spatializers/VBAP/PME.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PME.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PracticalSocket_47011285.o: ../../../CSL/Spatializers/VBAP/PracticalSocket.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PracticalSocket.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/VBAP_1e628c08.o: ../../../CSL/Spatializers/VBAP/VBAP.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling VBAP.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Ambisonic_647dfddc.o: ../../../CSL/Spatializers/Ambisonic/Ambisonic.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Ambisonic.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AmbisonicPanner_1386d99a.o: ../../../CSL/Spatializers/Ambisonic/AmbisonicPanner.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AmbisonicPanner.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AmbisonicUtilities_18b3c558.o: ../../../CSL/Spatializers/Ambisonic/AmbisonicUtilities.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AmbisonicUtilities.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinauralDB_241b2ea6.o: ../../../CSL/Spatializers/Binaural/BinauralDB.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinauralDB.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Binaural_2d8d95e8.o: ../../../CSL/Spatializers/Binaural/Binaural.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Binaural.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Instrument_f4f81701.o: ../../../CSL/Instruments/Instrument.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Instrument.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BasicFMInstrument_53b5d86c.o: ../../../CSL/Instruments/BasicFMInstrument.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BasicFMInstrument.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SndFileInstrument_cc5c7d7c.o: ../../../CSL/Instruments/SndFileInstrument.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SndFileInstrument.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AdditiveInstrument_2f4c54ed.o: ../../../CSL/Instruments/AdditiveInstrument.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AdditiveInstrument.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FileIO_c148421e.o: ../../../CSL/IO/FileIO.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FileIO.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SoundFile_d1377e39.o: ../../../CSL/IO/SoundFile.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SoundFile.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SoundFileJ_572a92fb.o: ../../../CSL/IO/SoundFileJ.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SoundFileJ.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSC_support_2c0695d.o: ../../../CSL/IO/OSC_support.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSC_support.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Support_33929ec7.o: ../../../CSL/Tests/Test_Support.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Support.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Oscillators_c6e83f4b.o: ../../../CSL/Tests/Test_Oscillators.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Oscillators.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Sources_9dea9e30.o: ../../../CSL/Tests/Test_Sources.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Sources.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Envelopes_ed289adb.o: ../../../CSL/Tests/Test_Envelopes.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Envelopes.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Effects_5fac67a.o: ../../../CSL/Tests/Test_Effects.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Effects.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Panners_55d0104d.o: ../../../CSL/Tests/Test_Panners.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Panners.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Control_3bae0d55.o: ../../../CSL/Tests/Test_Control.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Control.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Test_Audio_f712740e.o: ../../../CSL/Tests/Test_Audio.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Test_Audio.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CSL_TestComponent_5e484c6a.o: ../../../JUCE/CSL_TestComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CSL_TestComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JCSL_Widgets_edb9ebca.o: ../../../JUCE/JCSL_Widgets.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JCSL_Widgets.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Main_bcf3b081.o: ../../../JUCE/Main.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Main.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_basics_2442e4ea.o: ../../JuceLibraryCode/modules/juce_audio_basics/juce_audio_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_devices_a4c8a728.o: ../../JuceLibraryCode/modules/juce_audio_devices/juce_audio_devices.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_devices.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_formats_d349f0c8.o: ../../JuceLibraryCode/modules/juce_audio_formats/juce_audio_formats.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_formats.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_processors_44a134a2.o: ../../JuceLibraryCode/modules/juce_audio_processors/juce_audio_processors.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_processors.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_utils_f63b12e8.o: ../../JuceLibraryCode/modules/juce_audio_utils/juce_audio_utils.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_utils.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_aff681cc.o: ../../JuceLibraryCode/modules/juce_core/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_bdd6d488.o: ../../JuceLibraryCode/modules/juce_data_structures/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_79b2840.o: ../../JuceLibraryCode/modules/juce_events/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_c8f1e7a4.o: ../../JuceLibraryCode/modules/juce_graphics/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_a630dd20.o: ../../JuceLibraryCode/modules/juce_gui_basics/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_extra_7767d6a8.o: ../../JuceLibraryCode/modules/juce_gui_extra/juce_gui_extra.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_extra.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
