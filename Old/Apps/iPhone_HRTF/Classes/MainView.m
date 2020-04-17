//
//  MainView.m
//  CSL_iphone_test
//
//  Created by charlie on 8/20/09.
//  Copyright One More Muse 2009. All rights reserved.
//

#import "MainView.h"

@implementation MainView


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
					// Initialization code
    }
	
	// Set the background image for the sound stage
//	CGImageRef bgImg = [[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"stagebg" ofType:@"png"]] CGImage];
//	self.layer.contents = (id)bgImg;

    return self;
}


- (void)drawRect:(CGRect)rect {
    // Drawing code
}


- (void)dealloc {
    [super dealloc];
}


@end
