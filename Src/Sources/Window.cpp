//
//  Window.cpp -- implementation of the various function window classes
//	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
//

#include "Window.h"
#include <math.h>

using namespace csl;

Window::Window() : UnitGenerator(), mGain(1) {
	this->setSize(CGestalt::blockSize());
}

// Gain is optional and defaults to 1 when not specified.

Window::Window(unsigned windowSize, float gain) : UnitGenerator(), mGain(gain) {
	this->setSize(windowSize);
}

Window::~Window() {
	mWindowBuffer.freeBuffers();
}

void Window::setGain(float gain) {
	mGain = gain;	
	fillWindow(); // Re-caluclate window using new gain value.
}

void Window::setSize(unsigned windowSize) {
	mWindowBufferPos = 0;
	mWindowSize = windowSize;
											// If previously allocated, free me first.
	mWindowBuffer.freeBuffers();
	mWindowBuffer.setSize(1, mWindowSize);	// Allocate memory to store the window.
	mWindowBuffer.allocateBuffers();
	fillWindow();							// Fill the buffer with the window data.
}

void Window::nextBuffer(Buffer &outputBuffer, unsigned outBufNum) throw (CException) {
	/// get everything into registers
	unsigned numFrames = outputBuffer.mNumFrames;
	sample* outputBufferPtr = outputBuffer.buffer(outBufNum);
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	unsigned windowBufferPos = mWindowBufferPos;
	unsigned windowBufferSize = mWindowSize;
#ifdef CSL_DEBUG
	logMsg("Window::nextBuffer");
#endif
	for (unsigned i = 0; i < numFrames; i++) {
		if (windowBufferPos > windowBufferSize)
			windowBufferPos = 0;
		*outputBufferPtr++ = windowBufferPtr[windowBufferPos++];
	}
	mWindowBufferPos = windowBufferPos;
}

void Window::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
			// create the window -- I make a Hamming window, subclasses may override
	for (unsigned i = 0; i < mWindowSize; i++ )
		*windowBufferPtr++ = (0.54 - 0.46*cos(CSL_TWOPI*i/(mWindowSize - 1) ));
}

void RectangularWindow::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	for (unsigned i = 0; i < mWindowSize; i++ )	// create the window 
		*windowBufferPtr++ = mGain;
}

void TriangularWindow::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	float accum = 0.0f;
	unsigned winHalf = mWindowSize / 2;
	float step = 1.0f / ((float) winHalf);
	for (unsigned i = 0; i < winHalf; i++ ) {	// create the rising half 
		*windowBufferPtr++ = accum;
		accum += step;
	}
	for (unsigned i = winHalf; i < mWindowSize; i++ ) {	// create the falling half 
		*windowBufferPtr++ = accum;
		accum -= step;
	}
}

void HammingWindow::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	sample increment = CSL_TWOPI/(mWindowSize - 1);
	for (unsigned i = 0; i < mWindowSize; i++ )
		*windowBufferPtr++ = (0.54 - 0.46*cos(i * increment));
}

void HannWindow::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	sample increment = CSL_TWOPI/(mWindowSize - 1);
	
	for (unsigned i = 0; i < mWindowSize; i++ )
		*windowBufferPtr++ = 0.5 * (1 - cos(i * increment)) * mGain;
}

void BlackmanWindow::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	sample increment = CSL_TWOPI/(mWindowSize - 1);
	
	for (unsigned i = 0; i < mWindowSize; i++ )
		*windowBufferPtr++ = (0.42 - 0.5 * cos(i * increment) + 0.08 * cos(2 * i * increment)) * mGain;
}

void BlackmanHarrisWindow::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	sample increment = CSL_TWOPI/(mWindowSize - 1);
	
	for (unsigned i = 0; i < mWindowSize; i++ ) 
		*windowBufferPtr++ = (0.35875 - 0.48829 * cos(i * increment) 
					+ 0.14128 * cos(2 * i * increment) 
					- 0.01168 * cos(3 * i * increment)) * mGain;
}

void WelchWindow::fillWindow() {
	sample* windowBufferPtr = mWindowBuffer.buffer(0);
	sample increment = 2.f/(mWindowSize - 1);
	sample phase = -1.f;
	
	for (unsigned i = 0; i < mWindowSize; i++ )	{ // create the window 
		*windowBufferPtr++ =  (1.f - phase * phase) * mGain;
		phase += increment;
	}
}

void Window::dump() {
	logMsg("Window of size: %d samples", mWindowSize);
}
