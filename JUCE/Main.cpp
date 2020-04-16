//
// Stripped Juce top-level window code, based on the hello world example
//

#include "JuceHeader.h"

juce::Component * createCSLComponent();					// extern function that creates the window

class CSLWindow  : public juce::DocumentWindow {
public:
    CSLWindow() : juce::DocumentWindow ("CSL 5.2 Demos", 
				juce::Colours::lightgrey, DocumentWindow::allButtons, true) {
		setContentOwned(createCSLComponent(), true); // create app window
		setResizable (true, false);
		setVisible (true);
		setUsingNativeTitleBar(true);
		centreWithSize (608, 424);					// top window size 8 @ 24 larger than the component
    }

    ~CSLWindow() { }

    void closeButtonPressed() {
        juce::JUCEApplication::quit();
    }
};

// This is the application object that is started up when Juce starts.

class JUCECSLApplication : public juce::JUCEApplication {
    CSLWindow * mCSLWindow;
	
public:
    JUCECSLApplication() : mCSLWindow (0) { }

    ~JUCECSLApplication() { }

    void initialise (const juce::String& commandLine) {
        mCSLWindow = new CSLWindow();
    }

    void shutdown() {
        if (mCSLWindow != 0)
            delete mCSLWindow;
    }

    const juce::String getApplicationName() {
        return "JUCE/CSL";
    }

    const juce::String getApplicationVersion() {
        return "5.2";
    }

    bool moreThanOneInstanceAllowed() {
        return true;
    }
	
    void anotherInstanceStarted (const juce::String& commandLine) { }
};

// This macro creates the application's main() function..

START_JUCE_APPLICATION (JUCECSLApplication)

// I do this by hand so that I can keep argc/argv

//extern unsigned argCnt;						// globals for argc/v
//extern const char **argVals;

//int main (int argc, const char* argv[]) {
//	argCnt = argc;
//	argVals = argv;
//	return JUCE_NAMESPACE::JUCEApplication::main (argc, argv /*, new JUCECSLApplication() */);
//}
