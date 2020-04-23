
project.name = "CSL"
project.bindir = "build"
project.libdir = "build"

project.configs = { "Debug", "Release" }

package = newpackage()
package.name = "CSL_JUCE"
package.kind = "winexe"
package.language = "c++"

package.objdir = "build/intermediate"
package.config["Debug"].objdir   = "build/intermediate/Debug"
package.config["Release"].objdir = "build/intermediate/Release"

package.config["Debug"].defines     = { "LINUX=1", "USE_JUCE", "USE_FFTREAL", "USE_HRTF", "USE_JSND" };
package.config["Debug"].buildoptions = { "-ggdb", "-O0", "-w" }

package.config["Release"].defines   = { "LINUX=1", "USE_JUCE", "USE_FFTREAL", "USE_HRTF", "USE_JSND" };
package.config["Release"].buildoptions = { "-O2", "-w" }

package.target = "CSL_JUCE"

package.includepaths = { 
    "../CSL/Includes",
    "../CSL/Spatializers/Binaural",
    "../CSL/Spatializers/Ambisonic",
    "../JUCE",
    "/home/stp/juce/"				-- edit here
}

package.libpaths = { 
    "/usr/X11R6/lib/",
    "/usr/local/lib/",
    "/home/stp/juce/bin/"			-- edit here
}

package.config["Debug"].links = { 
    "freetype", "pthread", "X11", "GL", "GLU", "Xinerama", "asound", "juce_debug"
}

package.config["Release"].links = { 
    "freetype", "pthread", "X11", "GL", "GLU", "Xinerama", "asound", "juce_debug"
}

package.linkflags = { "static-runtime" }

-- source list

package.files = { matchfiles (
    "../CSL/Kernel/*.cpp",
    "../CSL/Utilities/*.cpp",
    "../CSL/Sources/*.cpp",
    "../CSL/Processors/*.cpp",
    "../CSL/Instruments/*.cpp",
    "../CSL/Spatializers/*.cpp",
    "../CSL/Spatializers/Binaural/*",
    "../CSL/Spatializers/Ambisonic/*",
    "../CSL/IO/SoundFile.cpp",
    "../CSL/IO/SoundFileJ.cpp",
    "../CSL/IO/MIDIIOJ.cpp",
    "../CSL/Tests/*.cpp",
    "../JUCE/CSL_TestComponent.cpp",
    "../JUCE/JCSL_Widgets.cpp",
    "../JUCE/Main.cpp"
    )
}

