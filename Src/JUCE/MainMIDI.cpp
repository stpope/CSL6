//
// Stripped Juce top-level window code, based on the hello world example
//

#include "juce.h"

Component * createCSLComponent();					// extern function that creates the window

class CSLWindow  : public DocumentWindow {
public:
    CSLWindow() : DocumentWindow (T("CSL 5.0 MIDI"), 
				Colours::lightgrey, DocumentWindow::allButtons, true) {
		setContentComponent (createCSLComponent());	// create app window
		setResizable (true, false);
		setVisible (true);
		setUsingNativeTitleBar(true);
		centreWithSize (658, 354);					// top window size 8 @ 24 larger than the component
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

// START_JUCE_APPLICATION (JUCECSLApplication)

// I do this by hand so that I can keep argc/argv

extern unsigned argCnt;						// globals for argc/v
extern char **argVals;

int main (int argc, char* argv[]) {
	argCnt = argc;
	argVals = argv;
	return JUCE_NAMESPACE::JUCEApplication::main (argc, argv, new JUCECSLApplication());
}
