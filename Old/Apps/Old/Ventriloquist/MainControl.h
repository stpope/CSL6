/* MainControl */

#import <Cocoa/Cocoa.h>
#import "CVBAP.h"
#import "PME.h"
#import "CSL_Core.h"
#import "PAIO.h"
#import "SoundFile.h"
#import "ThreadedFrameStream.h"
#import "PracticalSocket.h"
#import "InOut.h"
#import "MyOpenGLView.h"

using namespace csl;


@interface MainControl : NSObject
{
	IBOutlet NSTextField *BounceSizeTB1;
	IBOutlet NSTextField *BounceSizeTB2;
	IBOutlet NSTextField *BounceSizeTB3;
	IBOutlet NSTextField *BounceSizeTB4;
	IBOutlet NSTextField *BounceSizeTB5;
	IBOutlet NSTextField *Azimuth1;
    IBOutlet NSTextField *Azimuth2;
    IBOutlet NSTextField *Azimuth3;
    IBOutlet NSTextField *Azimuth4;
    IBOutlet NSTextField *Azimuth5;
    IBOutlet NSButton *ConnectButton;
    IBOutlet NSTextField *Distance1;
    IBOutlet NSTextField *Distance2;
    IBOutlet NSTextField *Distance3;
    IBOutlet NSTextField *Distance4;
    IBOutlet NSTextField *Distance5;
    IBOutlet NSTextField *Elevation1;
    IBOutlet NSTextField *Elevation2;
    IBOutlet NSTextField *Elevation3;
    IBOutlet NSTextField *Elevation4;
    IBOutlet NSTextField *Elevation5;
    IBOutlet NSButton *LFButton;
    IBOutlet NSTextField *LFileText;
    IBOutlet NSComboBox *LiveChannel1;
    IBOutlet NSComboBox *LiveChannel2;
    IBOutlet NSComboBox *LiveChannel3;
    IBOutlet NSComboBox *LiveChannel4;
    IBOutlet NSComboBox *LiveChannel5;
    IBOutlet NSTextField *sensorIP;
    IBOutlet NSTextField *sensorPort;
    IBOutlet NSButtonCell *SoundFile1;
    IBOutlet NSButtonCell *SoundFile2;
    IBOutlet NSButtonCell *SoundFile3;
    IBOutlet NSButtonCell *SoundFile4;
    IBOutlet NSButtonCell *SoundFile5;
    IBOutlet NSButton *StartButton;
    IBOutlet NSButton *ResetButton;
    IBOutlet NSMatrix *Source1;
    IBOutlet NSMatrix *Source2;
    IBOutlet NSMatrix *Source3;
    IBOutlet NSMatrix *Source4;
    IBOutlet NSMatrix *Source5;
//    IBOutlet MyOpenGLView *GLView;
	
	NSArray *azimuthTB, *elevationTB, *distanceTB, *TBArray;
	BOOL updatePositionDisplay;
//	NSThread *positioDisplayThread;
	
#define NUM_SOURCES (5)

	CVBAP			*cvbap;
	PME				*pme;
	CSource			*cSources[NUM_SOURCES];
	PMESource		*pmeSources[NUM_SOURCES];
	InOut			*liveInputs[NUM_SOURCES];
	SoundFile		*soundFiles[NUM_SOURCES];
	ThreadedReader	*threadedReaders[NUM_SOURCES];
	
	PAIO			*io;
	
	UDPSocket		*controlSocket;
	string			*remoteIPString;
	unsigned short	remotePort;
	
	ThreadPthread *displayThread;
	
	BOOL layoutFileLoaded, sensorConnected;
	
	
}
- (IBAction)setBounceSize:(id)sender;
- (IBAction)connectToSensors:(id)sender;
- (IBAction)loadLayoutFile:(id)sender;
- (void) sfLoad:(size_t)sourceNumber;
- (IBAction)loadSF1:(id)sender;
- (IBAction)loadSF2:(id)sender;
- (IBAction)loadSF3:(id)sender;
- (IBAction)loadSF4:(id)sender;
- (IBAction)loadSF5:(id)sender;
- (IBAction)setReleaseMode1:(id)sender;
- (IBAction)setReleaseMode2:(id)sender;
- (IBAction)setReleaseMode3:(id)sender;
- (IBAction)setReleaseMode4:(id)sender;
- (IBAction)setReleaseMode5:(id)sender;
- (IBAction)start:(id)sender;
- (IBAction)reset:(id)sender;
- (IBAction)setVolume:(id)sender;

- (void)update_position_display_func:(id)nothing;

@end
