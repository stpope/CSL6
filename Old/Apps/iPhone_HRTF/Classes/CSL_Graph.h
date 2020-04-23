//
//  CSL_Graph.h -- CSL_iphone_HRTF wrapper
//
//  Created by charlie on 8/21/09.
//  Copyright 2009 One More Muse. All rights reserved.
//		Updated for HRTF Binaural Panner, 0909 STP.
//

#import <Foundation/Foundation.h>


@interface CSL_Graph : NSObject {

}

- (void) playCSL;		// play a CSL DSP demo graph

- (void) loadHRTF;		// load the HRTF data

- (void) startCSL;		// start a CSL spatializer thread

- (void) playHRTF;		// play an HRTF moving source

- (void) stopCSL;		// stop, close, release

@end
