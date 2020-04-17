//
// Stripped Juce top-level window code, based on the hello world example
//

#include "juce.h"

Component * createCSLComponent();					// extern function that creates the window

// CSL Window is the document object that holds onto the component

class CSLWindow  : public DocumentWindow {
public:
    CSLWindow() : DocumentWindow (T("CSL Player 1.0"), Colours::lightgrey, DocumentWindow::allButtons, true) {
        setContentComponent (createCSLComponent());	// create the actual component here
		setResizable (true, false);
        setVisible (true);
        centreWithSize (608, 632);					// top window size 8 @ 36 larger than the component
		setUsingNativeTitleBar(true);
    }

    ~CSLWindow() { }

    void closeButtonPressed() {
        JUCEApplication::quit();
    }
};

// This is the application object that is started up when Juce starts.

class JUCECSLApplication : public JUCEApplication {
    CSLWindow * mCSLWindow;
	
public:
    JUCECSLApplication() : mCSLWindow (0) { }

    ~JUCECSLApplication() { }

				// This method creates the document whi8ch creates the component
    void initialise (const String& commandLine) {
        mCSLWindow = new CSLWindow();
    }

    void shutdown() {
        if (mCSLWindow != 0)
            delete mCSLWindow;
    }

    const String getApplicationName() {
        return T("JUCE/CSL");
    }

    const String getApplicationVersion() {
        return T("1.0");
    }

    bool moreThanOneInstanceAllowed() {
        return true;
    }
	
    void anotherInstanceStarted (const String& commandLine) { }
};

// This macro creates the application's main() function..

START_JUCE_APPLICATION (JUCECSLApplication)
