///
///  Window.h -- specification of the various function window classes
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///
/// Helper class for other classes that need a signal to be windowed. For example, to perform an FFT
/// usually is better to apply a window function to the signal to get beter results.
/// Usage (two ways to be used):
///		a) Instantiate a window object (for example Window myWindow(512, 1);) and then call the window()
///			function to get a pointer to the window buffer. 
///			or...
///		b) call the nextBuffer on the window, so it fills a buffer with itself, wraping around if the buffer
///			passed is larger.
///
///	The size should be specified in samples.
///	
///	TODO:
///		a) Add more window types.
///

#ifndef CSL_WINDOW_H
#define CSL_WINDOW_H

#include "CSL_Core.h"

namespace csl {

/// Window; The superclass of all other window function classes in CSL.
/// Subclasses need only to implement the fillWindow(); and the Constructors.

class Window : public UnitGenerator {
public:				// Constructors:
	Window();		///< Creates a window using the default Gestalt size and a gain of 1;
					///< Creates a window (hann) with the specified size and gain (gain is optional).
	Window(unsigned windowSize, float gain = 1); 
	~Window();		///< clean-up . . . free the allocated buffer that held the window data.

	void setSize(unsigned windowSize);	///< Set the number of samples the window spans.
	void setGain(float gain);				///< Set the gain to which the window should be normalized.
	SampleBuffer window() { return mWindowBuffer.buffer(0); }; ///< Returns a pointer to the window data.

	void nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException);
	void dump();		///< Print some info about the window.
	
protected:
	Buffer mWindowBuffer;		///< used to store the window
	unsigned mWindowBufferPos; 	///< where am I in the window buffer
	unsigned mWindowSize;		///< length in samples of the window
	float mGain;					///< gain for the window

	virtual void fillWindow();		///< subclasses override this to fill the buffer with corresponding function.
};

/// RectangularWindow:A rectangular window has all values set to the Gain value, or by default to 1.

class RectangularWindow : public Window {
public:
	RectangularWindow() : Window() { }
	RectangularWindow(unsigned windowSize) : Window(windowSize) { }
	RectangularWindow(unsigned windowSize, float gain) : Window(windowSize, gain) { }
	~RectangularWindow() { }
	
protected:
	void fillWindow();
};

/// TriangularWindow:A triangularWindow window.

class TriangularWindow : public Window {
public:
	TriangularWindow() : Window() { }
	TriangularWindow(unsigned windowSize) : Window(windowSize) { }
	TriangularWindow(unsigned windowSize, float gain) : Window(windowSize, gain) { }
	~TriangularWindow() { }
	
protected:
	void fillWindow();
};

/// HammingWindow: Belongs to the familly of cosine window functions. 

class HammingWindow : public Window {
public:

	HammingWindow() : Window() { }
	HammingWindow(unsigned windowSize) : Window(windowSize) { }
	HammingWindow(unsigned windowSize, float gain) : Window(windowSize, gain) { }
	~HammingWindow() { }

protected:
	void fillWindow();
};

/// HannWindow

class HannWindow : public Window {
public:
	HannWindow() : Window() { }
	HannWindow(unsigned windowSize) : Window(windowSize) { }
	HannWindow(unsigned windowSize, float gain) : Window(windowSize, gain) { }
	~HannWindow() { }
	
protected:
	void fillWindow();
};

/// BlackmanWindow

class BlackmanWindow : public Window {
public:
	BlackmanWindow() : Window() { }
	BlackmanWindow(unsigned windowSize) : Window(windowSize) { }
	BlackmanWindow(unsigned windowSize, float gain) : Window(windowSize, gain) { }
	~BlackmanWindow() { }
	
protected:
	void fillWindow();
};

/// BlackmanHarrisWindow

class BlackmanHarrisWindow : public Window {
public:
	BlackmanHarrisWindow() : Window() { }
	BlackmanHarrisWindow(unsigned windowSize) : Window(windowSize) { }
	BlackmanHarrisWindow(unsigned windowSize, float gain) : Window(windowSize, gain) { }
	~BlackmanHarrisWindow() { }
	
protected:
	void fillWindow();
};

/// WelchWindow: This is basically an equal-power curve.

class WelchWindow : public Window {
public:
	WelchWindow() : Window() { }
	WelchWindow(unsigned windowSize) : Window(windowSize) { }
	WelchWindow(unsigned windowSize, float gain) : Window(windowSize, gain) { }
	~WelchWindow() { }
	
protected:
	void fillWindow();
};

}  // end of namespace

#endif
