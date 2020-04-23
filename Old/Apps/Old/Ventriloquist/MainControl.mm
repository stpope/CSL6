#import "MainControl.h"

@implementation MainControl

- (IBAction)setBounceSize:(id)sender
{
	int value = [sender floatValue] * 100;
	pmeSources[ [sender tag] ]->set_bounce_distance( [sender floatValue] );
	NSArray *array = [NSArray arrayWithObjects: BounceSizeTB1, BounceSizeTB2, BounceSizeTB3, BounceSizeTB4, BounceSizeTB5, nil];
	[ [array objectAtIndex:[sender tag] ] setIntValue:value];
}

- (IBAction)connectToSensors:(id)sender
{
	*remoteIPString = [ [sensorIP stringValue] cString];
	remotePort = [ [sensorPort stringValue] intValue];
	
	string receivedAddr;
	unsigned short receivedPort;
	
	char sendMessage[5] = "init";
	char recvMessage[5];

	[ConnectButton setTitle:@"Connecting..."];
	[ConnectButton setEnabled:NO];
	
	controlSocket->sendTo(sendMessage, 5, *remoteIPString, remotePort);
	controlSocket->recvFrom(recvMessage, 5, receivedAddr, receivedPort);
	if (0 != strcmp(recvMessage, "succ")){
		[ConnectButton setTitle:@"Try Again"];
		[ConnectButton setEnabled:YES];
	}
	else{
		[ConnectButton setTitle:@"Connected"];
		pme->set_remote_addr_and_port( *remoteIPString, remotePort );
		sensorConnected = true;
		if (sensorConnected && layoutFileLoaded)
			[StartButton setEnabled:YES];
	}
}
	
- (IBAction)setVolume:(id)sender
{
	cSources[ [sender tag] ]->set_gain( [sender floatValue] );
}


- (IBAction)loadLayoutFile:(id)sender
{
	int result;
    NSArray *fileTypes = [NSArray arrayWithObject:@"dat"];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	
    [oPanel setAllowsMultipleSelection:NO];
    result = [oPanel runModalForDirectory:NSHomeDirectory() file:nil types:fileTypes];
    if (result == NSOKButton) {
        NSArray *filesToOpen = [oPanel filenames];
		
        NSString *aFile = [filesToOpen objectAtIndex:0];
		
		
		[LFileText setStringValue:[aFile lastPathComponent]];
		cvbap->read_speaker_layout_from_file( [aFile cString] );
		cvbap->find_triples();
		layoutFileLoaded = true;
		if (sensorConnected && layoutFileLoaded)
			[StartButton setEnabled:YES];
	}		
}

- (void)sfLoad:(size_t)sourceNumber
{
	NSArray *sfTextFields = [NSArray arrayWithObjects:SoundFile1, SoundFile2, SoundFile3, SoundFile4, SoundFile5, nil];
	int result;
    NSArray *fileTypes = [NSArray arrayWithObjects:@"aiff", @"aif", @"wav", nil];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	
    [oPanel setAllowsMultipleSelection:NO];
    result = [oPanel runModalForDirectory:NSHomeDirectory() file:nil types:fileTypes];
    if (result == NSOKButton) {
        NSArray *filesToOpen = [oPanel filenames];
		
        NSString *aFile = [filesToOpen objectAtIndex:0];
		
		
		soundFiles[sourceNumber]->close();
		soundFiles[sourceNumber]->set_path( [aFile cString] );
		soundFiles[sourceNumber]->open_for_read();
		soundFiles[sourceNumber]->set_is_looping(true);
		
		if (soundFiles[sourceNumber]->channels() != 1){
			NSAlert *alert = [[NSAlert alloc] init];
			[alert addButtonWithTitle:@"OK"];
			[alert setMessageText:@"Soundfile error!"];
			[alert setInformativeText:@"Soundfile not monophonic."];
			[alert setAlertStyle:NSWarningAlertStyle];		
			[alert runModal];
			[alert release];
			soundFiles[sourceNumber]->close();
			[ [sfTextFields objectAtIndex:sourceNumber] setTitle:@"Load soundfile"];
		}
		else
			[ [sfTextFields objectAtIndex:sourceNumber] setTitle:[aFile lastPathComponent] ];

//		threadedReaders[sourceNumber]->set_input( *(soundFiles[sourceNumber]) );
		
//		cSources[sourceNumber]->set_graph( *(threadedReaders[sourceNumber]) );

    }
}
	
- (IBAction)loadSF1:(id)sender
{
	[self sfLoad:0];
}

- (IBAction)loadSF2:(id)sender
{
	[self sfLoad:1];
}


- (IBAction)loadSF3:(id)sender
{
	[self sfLoad:2];
}


- (IBAction)loadSF4:(id)sender
{
	[self sfLoad:3];
}


- (IBAction)loadSF5:(id)sender
{
	[self sfLoad:4];
}

- (void)setReleaseMode:(size_t)sourceNumber :(int)tagNum
{
	MovementType mt;
	switch (tagNum){
		case 0:
			mt = kBounce;
			break;
		case 1:
			mt = kDraw;
			break;
		case 2:
			mt = kOrbit;
			break;
	}
			
	pmeSources[sourceNumber]->set_next_move_type( mt );
}

- (IBAction)setReleaseMode1:(id)sender
{
	[self setReleaseMode:0 :[ [sender selectedCell] tag] ];
}

- (IBAction)setReleaseMode2:(id)sender
{
	[self setReleaseMode:1 :[ [sender selectedCell] tag] ];
}


- (IBAction)setReleaseMode3:(id)sender
{
	[self setReleaseMode:2 :[ [sender selectedCell] tag] ];
}


- (IBAction)setReleaseMode4:(id)sender
{
	[self setReleaseMode:3 :[ [sender selectedCell] tag] ];
}


- (IBAction)setReleaseMode5:(id)sender
{
	[self setReleaseMode:4 :[ [sender selectedCell] tag] ];
}


- (IBAction)start:(id)sender
{
	string receivedAddr;
	unsigned short receivedPort;

	NSArray *sourceBoxes;
	sourceBoxes =  [NSArray arrayWithObjects:Source1, Source2, Source3, Source4, Source5, nil];
	NSArray *inputChan;
	inputChan =  [NSArray arrayWithObjects:LiveChannel1, LiveChannel2, LiveChannel3, LiveChannel4, LiveChannel5, nil];
				
	azimuthTB = [NSArray arrayWithObjects:Azimuth1, Azimuth2, Azimuth3, Azimuth4, Azimuth5, nil];
	elevationTB = [NSArray arrayWithObjects:Elevation1, Elevation2, Elevation3, Elevation4, Elevation5, nil];
	distanceTB = [NSArray arrayWithObjects:Distance1, Distance2, Distance3, Distance4, Distance5, nil];
	TBArray = [NSArray arrayWithObjects:azimuthTB, elevationTB, distanceTB,nil];
				
	
	switch ([StartButton state]){
		char message[5];
		case NSOffState:				// stop button pushed
			updatePositionDisplay = NO;		//stop display thread
			usleep(100000);					// wait for other thread to exit
			
			io->stop();
			io->close();
			delete io;
			
			pme->stop_management_thread();
			pme->remove_all_sources();
			cvbap->remove_all_sources();
			
			strcpy(message, "stop");	
			controlSocket->sendTo(message, 5, *remoteIPString, remotePort);
			controlSocket->recvFrom(message, 5, receivedAddr, receivedPort);
			
			for (int i=0;i<5;i++){
				[[sourceBoxes objectAtIndex:i] setEnabled:YES];
				[[inputChan objectAtIndex:i] setEnabled:YES];
				[[azimuthTB objectAtIndex:i] setEnabled:YES];
				[[elevationTB objectAtIndex:i] setEnabled:YES];
				[[distanceTB objectAtIndex:i] setEnabled:YES];
			}
			[LFButton setEnabled:YES];
			[ResetButton setEnabled:YES];
			
			
			
		break;

		case NSOnState:					// start button pushed
			
			for (int i=0;i<5;i++){
				[[sourceBoxes objectAtIndex:i] setEnabled:NO];
				[[inputChan objectAtIndex:i] setEnabled:NO];
				[[azimuthTB objectAtIndex:i] setEnabled:NO];
				[[elevationTB objectAtIndex:i] setEnabled:NO];
				[[distanceTB objectAtIndex:i] setEnabled:NO];
			}
			[LFButton setEnabled:NO];
			[ResetButton setEnabled:NO];

			int highestInputNumber = 0;
			float azi, ele, dist;
			for (int i=0; i<[sourceBoxes count]; i++){
				unsigned short chanelNumber;
				
				switch ([ [ [sourceBoxes objectAtIndex:i] selectedCell] tag] ){
					case 0:
						cSources[i]->set_graph( *(threadedReaders[i]) );
						cvbap->add_source(cSources[i]);
						pme->add_pme_source(*pmeSources[i]);
						break;
					case 1:
						chanelNumber = [ [inputChan objectAtIndex:i] intValue];
						liveInputs[i]->setInChan( ( chanelNumber ) - 1 );
						cSources[i]->set_graph( *(liveInputs[i]) );
						
						
						cvbap->add_source(cSources[i]);
						pme->add_pme_source(*pmeSources[i]);
						highestInputNumber = ( chanelNumber > highestInputNumber)? chanelNumber : highestInputNumber;
						break;
					case 2:
						break;
				}
				
				azi = [ [azimuthTB objectAtIndex:i] floatValue] * CSL_TWOPI / 360.0;
				ele = [ [elevationTB objectAtIndex:i] floatValue] * CSL_TWOPI / 360.0;
				dist = [ [distanceTB objectAtIndex:i] floatValue] / 100.0;
				
				cSources[i]->set_position('s', azi, ele, dist);
				
			}
			
			if (highestInputNumber==0){
				io = new PAIO(paNoDevice, 
							  Pa_GetDefaultOutputDeviceID(), 
							  highestInputNumber, 
							  cvbap->channels() );
				}
			else{
				io = new PAIO(Pa_GetDefaultInputDeviceID(), 
						  Pa_GetDefaultOutputDeviceID(), 
						  highestInputNumber, 
						  cvbap->channels() );
			}
			
			updatePositionDisplay = YES;
			
			io->set_root(*cvbap);
			io->open();
			io->start();

			strcpy(message, "capt");	
			controlSocket->sendTo(message, 5, *remoteIPString, remotePort);
			controlSocket->recvFrom(message, 5, receivedAddr, receivedPort);

			pme->start_management_thread();
			[NSThread detachNewThreadSelector: @selector(update_position_display_func:) toTarget: self withObject: TBArray];

			
			
			break;
	}
}

- (IBAction)reset:(id)sender
{
	string receivedAddr;
	unsigned short receivedPort;
	*remoteIPString = [ [sensorIP stringValue] cString];
	remotePort = [ [sensorPort stringValue] intValue];
	char message[5] = "term";
	controlSocket->sendTo(message, 5, *remoteIPString, remotePort);
	controlSocket->recvFrom(message, 5, receivedAddr, receivedPort);
	[ConnectButton setTitle:@"Connect"];
	[ConnectButton setEnabled:YES];
}

- (void)update_position_display_func:(id)nothing{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	while(updatePositionDisplay){
		for (int i=0;i<NUM_SOURCES; i++){
			int azi = (int)(cSources[i]->get_azi() / CSL_TWOPI * 360.0f);
			int ele = (int)(cSources[i]->get_ele() / CSL_TWOPI * 360.0f);
			int dist = (int)(cSources[i]->get_radius() * 100.f);
			
//			int count = [azimuthTB count];
//			[ [azimuthTB objectAtIndex:i] setIntValue: azi];
//			[ [elevationTB objectAtIndex:i] setIntValue: ele];
//			[ [distanceTB objectAtIndex:i] setIntValue: dist];
			
			[ [[TBArray objectAtIndex:0] objectAtIndex:i] setIntValue: azi];
			[ [[TBArray objectAtIndex:1] objectAtIndex:i] setIntValue: ele];
			[ [[TBArray objectAtIndex:2] objectAtIndex:i] setIntValue: dist];
			
		}
		usleep(50000);	// update 20 times per second
	}
    [pool release];
	
}


-(id)init
{
	if (self = [super init])
	{
		cvbap = new CVBAP();
//		pme = new PME("128.111.221.19", 54321);
		pme = new PME();
		
		controlSocket = new UDPSocket();
		remoteIPString = new string;
		displayThread = new ThreadPthread();
		
		for (size_t i=0;i<NUM_SOURCES;i++){
			cSources[i] = new CSource(0.0, 0.0, 0.2);
			pmeSources[i] = new PMESource( *cSources[i] );
			pmeSources[i]->set_next_move_type( kDraw );
			liveInputs[i] = new InOut();
			liveInputs[i]->set_num_channels(1);
			soundFiles[i] = new SoundFile();
			threadedReaders[i] = new ThreadedReader(1);
			threadedReaders[i]->set_input( *(soundFiles[i]) );

		}
		
//		azimuthTB = [NSArray arrayWithObjects:Azimuth1, Azimuth2, Azimuth3, Azimuth4, Azimuth5, nil];
//		elevationTB = [NSArray arrayWithObjects:Elevation1, Elevation2, Elevation3, Elevation4, Elevation5, nil];
//		distanceTB = [NSArray arrayWithObjects:Distance1, Distance2, Distance3, Distance4, Distance5, nil];
//		TBArray = [NSArray arrayWithObjects:azimuthTB, elevationTB, distanceTB,nil];
//		
//		[azimuthTB retain];
//		[elevationTB retain];
//		[distanceTB retain];
//		[TBArray retain];
		

		layoutFileLoaded = sensorConnected = updatePositionDisplay = false;
				
	}
	return self;
}

-(void)dealloc
{
	delete cvbap;
	delete pme;
	for (size_t i=0; i<5; i++){
		delete cSources[i];
		delete pmeSources[i];
		delete liveInputs[i];
		delete soundFiles[i];
		delete threadedReaders[i];
	}
	
//	char message[5] = "term";
//	controlSocket->sendTo(message, 5, remoteIPString, remotePort);
	
	
	delete controlSocket;
	delete io;
	
	[super dealloc];
}


@end
