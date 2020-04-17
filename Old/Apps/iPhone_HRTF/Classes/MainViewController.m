//
//  MainViewController.m
//
//  Created by charlie on 8/20/09.
//  Copyright One More Muse 2009. All rights reserved.
//

#import "MainViewController.h"
#import "MainView.h"

@implementation MainViewController

@synthesize textField;
@synthesize label;
@synthesize string;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
				// Custom initialization
    }
    return self;
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.

- (void)viewDidLoad {
				// When the user starts typing, show the clear button in the text field.
    textField.clearButtonMode = UITextFieldViewModeWhileEditing;
				// When the view first loads, display the placeholder text that's in the
				// text field in the label.
    label.text = textField.placeholder;

	[super viewDidLoad];
						//
						// CREATE/LOAD/RUN THE CSL APP HERE
						//	
	CSL_Graph * csl = [[CSL_Graph alloc] init];
	
	[csl loadHRTF];		// load the CSL data
	
//	[csl playCSL];		// play the CSL demo fnuction

	[csl startCSL];		// start the CSL IO & Panner thread
	
						// start a player thread
	[NSThread detachNewThreadSelector: @selector(playHRTF) toTarget: csl withObject: nil];
	
						// wait a bit
	[NSThread sleepForTimeInterval: 1.5];
						// start another player thread
	[NSThread detachNewThreadSelector: @selector(playHRTF) toTarget: csl withObject: nil];
	
						// wait a bit
//	[NSThread sleepForTimeInterval: 1.5];
//						// start another player thread
//	[NSThread detachNewThreadSelector: @selector(playHRTF) toTarget: csl withObject: nil];

//						// later on - close thread
//	[csl stopCSL];
//	[csl release];
	
}

/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */

- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}


- (IBAction)showInfo {    
	
	FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	controller.delegate = self;
	
	controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
	[self presentModalViewController:controller animated:YES];
	
	[controller release];
}

- (void)updateString {	
				// Store the text of the text field in the 'string' instance variable.
	self.string = textField.text;
				// Set the text of the label to the value of the 'string' instance variable.
	label.text = self.string;
}

- (BOOL)textFieldShouldReturn:(UITextField *)theTextField {
				// When the user presses return, take focus away from the text field so that the keyboard is dismissed.
	if (theTextField == textField) {
		[textField resignFirstResponder];
				// Invoke the method that changes the greeting.
        [self updateString];
	}
	return YES;
}

/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */

- (void)didReceiveMemoryWarning {
				// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
				// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
				// Release any retained subviews of the main view.
				// e.g. self.myOutlet = nil;
}

- (void)dealloc {
	[textField release];
	[label release];
	[csl stopCSL];
	[csl release];	
	[super dealloc];
}


@end
