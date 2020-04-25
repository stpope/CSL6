# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DUSE_JUCE" "-DUSE_VBAP" "-DUSE_HRTF" "-DUSE_AMBISONIC" "-DUSE_FFTREAL" "-DUSE_JSND" "-DUSE_JMIDI" "-DUSE_JTHREADS" "-DJUCE_DEBUG" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=6.0" "-DJUCE_APP_VERSION_HEX=0x60000" "-DJUCE_DISPLAY_SPLASH_SCREEN=0" "-DJUCE_USE_DARK_SPLASH_SCREEN=0" "-DJUCE_PROJUCER_VERSION=0x60000" "-DJUCE_MODULE_AVAILABLE_juce_audio_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_devices=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_formats=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_utils=1" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_cryptography=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" $(shell pkg-config --cflags alsa freetype2 libcurl webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I$(HOME)/JUCE/modules $(CPPFLAGS)  -I../../../JUCE6/modules
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_RTAS=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0"
  JUCE_TARGET_APP := CSL6Demo

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -I../../Src/Includes -I/usr/local/include -w -fpermissive $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell pkg-config --libs alsa freetype2 libcurl) -fvisibility=hidden -L/usr/local/lib -llo -lrt -ldl -lpthread -lGL $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DUSE_JUCE" "-DUSE_VBAP" "-DUSE_HRTF" "-DUSE_AMBISONIC" "-DUSE_FFTREAL" "-DUSE_JSND" "-DUSE_JMIDI" "-DUSE_JTHREADS" "-DJUCE_DEBUG" "-DJUCER_LINUX_MAKE_6D53C8B4=1" "-DJUCE_APP_VERSION=6.0" "-DJUCE_APP_VERSION_HEX=0x60000" "-DJUCE_DISPLAY_SPLASH_SCREEN=0" "-DJUCE_USE_DARK_SPLASH_SCREEN=0" "-DJUCE_PROJUCER_VERSION=0x60000" "-DJUCE_MODULE_AVAILABLE_juce_audio_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_devices=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_formats=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_processors=1" "-DJUCE_MODULE_AVAILABLE_juce_audio_utils=1" "-DJUCE_MODULE_AVAILABLE_juce_core=1" "-DJUCE_MODULE_AVAILABLE_juce_cryptography=1" "-DJUCE_MODULE_AVAILABLE_juce_data_structures=1" "-DJUCE_MODULE_AVAILABLE_juce_events=1" "-DJUCE_MODULE_AVAILABLE_juce_graphics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1" "-DJUCE_MODULE_AVAILABLE_juce_gui_extra=1" "-DJUCE_MODULE_AVAILABLE_juce_opengl=1" "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1" "-DJUCE_STRICT_REFCOUNTEDPOINTER=1" "-DJUCE_STANDALONE_APPLICATION=1" $(shell pkg-config --cflags alsa freetype2 libcurl webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I$(HOME)/JUCE/modules $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_RTAS=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0"
  JUCE_TARGET_APP := CSL6Demo

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -I../../Src/Includes -I/usr/local/include -w -fpermissive $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell pkg-config --libs alsa freetype2 libcurl) -fvisibility=hidden -L/usr/local/lib -llo -lrt -ldl -lpthread -lGL $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/CSL_Core_d8125ffe.o \
  $(JUCE_OBJDIR)/CGestalt_41f75d03.o \
  $(JUCE_OBJDIR)/Window_13c68567.o \
  $(JUCE_OBJDIR)/Envelope_628d20c7.o \
  $(JUCE_OBJDIR)/Oscillator_e6835917.o \
  $(JUCE_OBJDIR)/Noise_62b4ddcd.o \
  $(JUCE_OBJDIR)/Spectral_5b2a3c5b.o \
  $(JUCE_OBJDIR)/Granulator_58bb32bc.o \
  $(JUCE_OBJDIR)/KarplusString_c98ac35a.o \
  $(JUCE_OBJDIR)/OscillatorBL_73d01be1.o \
  $(JUCE_OBJDIR)/SHARC_f694e510.o \
  $(JUCE_OBJDIR)/SimpleSines_7dd71427.o \
  $(JUCE_OBJDIR)/WaveShaper_5df080c1.o \
  $(JUCE_OBJDIR)/BinaryOp_5ddc63ba.o \
  $(JUCE_OBJDIR)/InOut_945a38bb.o \
  $(JUCE_OBJDIR)/Mixer_3b7bd8db.o \
  $(JUCE_OBJDIR)/Filters_21c0748d.o \
  $(JUCE_OBJDIR)/FIR_baff6521.o \
  $(JUCE_OBJDIR)/Clipper_1344235f.o \
  $(JUCE_OBJDIR)/Convolver_811a5bc4.o \
  $(JUCE_OBJDIR)/Convolver2_a25132b8.o \
  $(JUCE_OBJDIR)/Freeverb_2ba66b03.o \
  $(JUCE_OBJDIR)/JUCEIO_d1380be6.o \
  $(JUCE_OBJDIR)/SoundFile_a44595a2.o \
  $(JUCE_OBJDIR)/SoundFileJ_e5df68b2.o \
  $(JUCE_OBJDIR)/FileIO_d5428b55.o \
  $(JUCE_OBJDIR)/MIDIIOJ_d641a83a.o \
  $(JUCE_OBJDIR)/OSC_support_4aa64a86.o \
  $(JUCE_OBJDIR)/SpatialAudio_47f85c7a.o \
  $(JUCE_OBJDIR)/SimplePanner_1dac0478.o \
  $(JUCE_OBJDIR)/SpatialPanner_7be89084.o \
  $(JUCE_OBJDIR)/SpatialSource_1b233a41.o \
  $(JUCE_OBJDIR)/DistanceSimulator_25a94111.o \
  $(JUCE_OBJDIR)/SpeakerLayout_8ee0330b.o \
  $(JUCE_OBJDIR)/VBAP_95cd3431.o \
  $(JUCE_OBJDIR)/Binaural_42c60511.o \
  $(JUCE_OBJDIR)/BinauralDB_ccf4778f.o \
  $(JUCE_OBJDIR)/Ambisonic_d5746c5.o \
  $(JUCE_OBJDIR)/AmbisonicPanner_dbd60fc3.o \
  $(JUCE_OBJDIR)/AmbisonicUtilities_52a3728f.o \
  $(JUCE_OBJDIR)/Instrument_6c62bf2a.o \
  $(JUCE_OBJDIR)/AdditiveInstrument_4484c416.o \
  $(JUCE_OBJDIR)/BasicFMInstrument_f98e6023.o \
  $(JUCE_OBJDIR)/SndFileInstrument_72350533.o \
  $(JUCE_OBJDIR)/Variable_9b0968a1.o \
  $(JUCE_OBJDIR)/CPoint_69fbc92.o \
  $(JUCE_OBJDIR)/BlockResizer_bcb22096.o \
  $(JUCE_OBJDIR)/RingBuffer_32c156b5.o \
  $(JUCE_OBJDIR)/ThreadUtilities_4d910765.o \
  $(JUCE_OBJDIR)/FFT_Wrapper_4da98e2d.o \
  $(JUCE_OBJDIR)/FFTReal_f8205b7.o \
  $(JUCE_OBJDIR)/fft_N_6d106468.o \
  $(JUCE_OBJDIR)/Test_Support_65e1c370.o \
  $(JUCE_OBJDIR)/Test_Oscillators_3e52e774.o \
  $(JUCE_OBJDIR)/Test_Sources_d039c2d9.o \
  $(JUCE_OBJDIR)/Test_Envelopes_c8413944.o \
  $(JUCE_OBJDIR)/Test_Effects_3849eb23.o \
  $(JUCE_OBJDIR)/Test_Panners_881f34f6.o \
  $(JUCE_OBJDIR)/Test_Audio_dd04aef7.o \
  $(JUCE_OBJDIR)/Test_Control_6dfd31fe.o \
  $(JUCE_OBJDIR)/Main_d8756245.o \
  $(JUCE_OBJDIR)/CSL_TestComponent_29c9926.o \
  $(JUCE_OBJDIR)/JCSL_Widgets_cd5f998e.o \
  $(JUCE_OBJDIR)/BinaryData_ce4232d4.o \
  $(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o \
  $(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \
  $(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(RESOURCES)
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@pkg-config --print-errors alsa freetype2 libcurl
	@echo Linking "CSL6Demo - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/CSL_Core_d8125ffe.o: ../../Src/Kernel/CSL_Core.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling CSL_Core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CGestalt_41f75d03.o: ../../Src/Kernel/CGestalt.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling CGestalt.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Window_13c68567.o: ../../Src/Sources/Window.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Window.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Envelope_628d20c7.o: ../../Src/Sources/Envelope.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Envelope.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Oscillator_e6835917.o: ../../Src/Sources/Oscillator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Oscillator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Noise_62b4ddcd.o: ../../Src/Sources/Noise.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Noise.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Spectral_5b2a3c5b.o: ../../Src/Sources/Spectral.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Spectral.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Granulator_58bb32bc.o: ../../Src/Sources/Granulator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Granulator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KarplusString_c98ac35a.o: ../../Src/Sources/KarplusString.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling KarplusString.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OscillatorBL_73d01be1.o: ../../Src/Sources/OscillatorBL.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling OscillatorBL.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SHARC_f694e510.o: ../../Src/Sources/SHARC.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SHARC.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SimpleSines_7dd71427.o: ../../Src/Sources/SimpleSines.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SimpleSines.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WaveShaper_5df080c1.o: ../../Src/Sources/WaveShaper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling WaveShaper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryOp_5ddc63ba.o: ../../Src/Processors/BinaryOp.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling BinaryOp.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InOut_945a38bb.o: ../../Src/Processors/InOut.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling InOut.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Mixer_3b7bd8db.o: ../../Src/Processors/Mixer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Mixer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Filters_21c0748d.o: ../../Src/Processors/Filters.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Filters.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FIR_baff6521.o: ../../Src/Processors/FIR.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling FIR.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Clipper_1344235f.o: ../../Src/Processors/Clipper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Clipper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Convolver_811a5bc4.o: ../../Src/Processors/Convolver.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Convolver.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Convolver2_a25132b8.o: ../../Src/Processors/Convolver2.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Convolver2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Freeverb_2ba66b03.o: ../../Src/Processors/Freeverb.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Freeverb.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/JUCEIO_d1380be6.o: ../../Src/IO/JUCEIO.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling JUCEIO.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SoundFile_a44595a2.o: ../../Src/IO/SoundFile.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SoundFile.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SoundFileJ_e5df68b2.o: ../../Src/IO/SoundFileJ.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SoundFileJ.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FileIO_d5428b55.o: ../../Src/IO/FileIO.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling FileIO.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MIDIIOJ_d641a83a.o: ../../Src/IO/MIDIIOJ.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling MIDIIOJ.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OSC_support_4aa64a86.o: ../../Src/IO/OSC_support.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling OSC_support.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpatialAudio_47f85c7a.o: ../../Src/Spatializers/SpatialAudio.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SpatialAudio.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SimplePanner_1dac0478.o: ../../Src/Spatializers/SimplePanner.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SimplePanner.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpatialPanner_7be89084.o: ../../Src/Spatializers/SpatialPanner.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SpatialPanner.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpatialSource_1b233a41.o: ../../Src/Spatializers/SpatialSource.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SpatialSource.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DistanceSimulator_25a94111.o: ../../Src/Spatializers/DistanceSimulator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling DistanceSimulator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpeakerLayout_8ee0330b.o: ../../Src/Spatializers/SpeakerLayout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SpeakerLayout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VBAP_95cd3431.o: ../../Src/Spatializers/VBAP/VBAP.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling VBAP.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Binaural_42c60511.o: ../../Src/Spatializers/Binaural/Binaural.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Binaural.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinauralDB_ccf4778f.o: ../../Src/Spatializers/Binaural/BinauralDB.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling BinauralDB.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Ambisonic_d5746c5.o: ../../Src/Spatializers/Ambisonic/Ambisonic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Ambisonic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AmbisonicPanner_dbd60fc3.o: ../../Src/Spatializers/Ambisonic/AmbisonicPanner.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling AmbisonicPanner.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AmbisonicUtilities_52a3728f.o: ../../Src/Spatializers/Ambisonic/AmbisonicUtilities.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling AmbisonicUtilities.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Instrument_6c62bf2a.o: ../../Src/Instruments/Instrument.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Instrument.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AdditiveInstrument_4484c416.o: ../../Src/Instruments/AdditiveInstrument.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling AdditiveInstrument.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BasicFMInstrument_f98e6023.o: ../../Src/Instruments/BasicFMInstrument.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling BasicFMInstrument.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SndFileInstrument_72350533.o: ../../Src/Instruments/SndFileInstrument.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling SndFileInstrument.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Variable_9b0968a1.o: ../../Src/Utilities/Variable.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Variable.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CPoint_69fbc92.o: ../../Src/Utilities/CPoint.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling CPoint.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BlockResizer_bcb22096.o: ../../Src/Utilities/BlockResizer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling BlockResizer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RingBuffer_32c156b5.o: ../../Src/Utilities/RingBuffer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling RingBuffer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ThreadUtilities_4d910765.o: ../../Src/Utilities/ThreadUtilities.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling ThreadUtilities.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FFT_Wrapper_4da98e2d.o: ../../Src/Utilities/FFT_Wrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling FFT_Wrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FFTReal_f8205b7.o: ../../Src/Utilities/FFTReal.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling FFTReal.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/fft_N_6d106468.o: ../../Src/Utilities/fft_N.c
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling fft_N.c"
	$(V_AT)$(CC) $(JUCE_CFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Support_65e1c370.o: ../../Src/Tests/Test_Support.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Support.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Oscillators_3e52e774.o: ../../Src/Tests/Test_Oscillators.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Oscillators.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Sources_d039c2d9.o: ../../Src/Tests/Test_Sources.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Sources.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Envelopes_c8413944.o: ../../Src/Tests/Test_Envelopes.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Envelopes.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Effects_3849eb23.o: ../../Src/Tests/Test_Effects.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Effects.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Panners_881f34f6.o: ../../Src/Tests/Test_Panners.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Panners.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Audio_dd04aef7.o: ../../Src/Tests/Test_Audio.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Audio.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Test_Control_6dfd31fe.o: ../../Src/Tests/Test_Control.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Test_Control.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Main_d8756245.o: ../../Src/JUCE/Main.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CSL_TestComponent_29c9926.o: ../../Src/JUCE/CSL_TestComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling CSL_TestComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/JCSL_Widgets_cd5f998e.o: ../../Src/JUCE/JCSL_Widgets.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling JCSL_Widgets.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o: ../../JuceLibraryCode/include_juce_audio_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_audio_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o: ../../JuceLibraryCode/include_juce_audio_devices.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_audio_devices.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o: ../../JuceLibraryCode/include_juce_audio_formats.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_audio_formats.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o: ../../JuceLibraryCode/include_juce_audio_processors.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_audio_processors.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o: ../../JuceLibraryCode/include_juce_audio_utils.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_audio_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_cryptography_8cb807a8.o: ../../JuceLibraryCode/include_juce_cryptography.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_cryptography.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o: ../../JuceLibraryCode/include_juce_opengl.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo; echo "Compiling include_juce_opengl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

clean:
	@echo Cleaning CSL6Demo
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping CSL6Demo
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(TARGET)

-include $(OBJECTS_APP:%.o=%.d)
