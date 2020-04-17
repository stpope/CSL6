/* SpatDemoController */
/*
 *  Created by Jorge Castellanos on 7/23/06.
 *  Copyright 2006 Jorge Castellanos. All rights reserved.
 *
 */

#import <Cocoa/Cocoa.h>

#include "CSL_Includes.h"		// include all of CSL core
#include "PAIO.h"			// PortAudio IO
#include "SoundFile.h"
#include "SpatialSource.h"
#include "SpatialAudio.h"

using namespace csl;

@interface SpatDemoController : NSObject
{
	SpatSource *source;
}
- (IBAction)setAzimuth:(id)sender;

@end
