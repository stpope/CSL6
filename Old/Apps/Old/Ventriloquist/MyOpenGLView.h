/* MyOpenGLView */

#import <Cocoa/Cocoa.h>
#import "CVector.h"

using namespace point;

#define NUM_SOURCES (5)
@interface MyOpenGLView : NSOpenGLView
{
	CVector *source_positions[NUM_SOURCES];
//	NSPoint mousePoint ;
//    float colorIntensity ;
//    NSDate* startTime ;
//    NSTimer* timer ;
}

- (id) initWithCoder: (NSCoder *) coder ;
- (void) dealloc ;
//- (void) pulseColor ;
//- (void) mouseDown: (NSEvent *) theEvent ;
//- (void) mouseDragged: (NSEvent *) theEvent ;

- (void) drawRect: (NSRect) bounds ;

@end
