//
//  CSL_iphone_testAppDelegate.m
//  CSL_iphone_test
//
//  Created by charlie on 8/20/09.
//  Copyright One More Muse 2009. All rights reserved.
//

#import "CSL_iphone_testAppDelegate.h"
#import "MainViewController.h"

@implementation CSL_iphone_testAppDelegate


@synthesize window;
@synthesize mainViewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	MainViewController *aController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
	self.mainViewController = aController;
	[aController release];
	
    mainViewController.view.frame = [UIScreen mainScreen].applicationFrame;
	[window addSubview:[mainViewController view]];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [mainViewController release];
    [window release];
    [super dealloc];
}

@end
