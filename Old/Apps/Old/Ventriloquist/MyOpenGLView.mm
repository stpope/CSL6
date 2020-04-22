#import "MyOpenGLView.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
@implementation MyOpenGLView

//- (void) mouseDown: (NSEvent *) theEvent
//{
//    [self mouseDragged: theEvent] ;
//}

//- (void) mouseDragged: (NSEvent *) theEvent
//{
//    mousePoint = [theEvent locationInWindow] ;
//    [self setNeedsDisplay: YES] ;
//}

- (id) initWithCoder: (NSCoder *) coder
{
//	SEL theSelector ;
//	NSMethodSignature* aSignature ;
//    NSInvocation* anInvocation ;
	NSOpenGLPixelFormatAttribute attrs[] = {NSOpenGLPFADoubleBuffer, nil } ;
	NSOpenGLPixelFormat* pixFmt ;
	long swapInterval ;
	
    self = [super initWithCoder: coder] ;
	
	for (int i=0; i<NUM_SOURCES; i++)
		source_positions[i] = new CVector();
	
//    mousePoint.x = [self bounds].size.width / 2.0 ;
//    mousePoint.y = 0.0 ;
//	colorIntensity = 1.0 ;

//	startTime = [NSDate date] ;
//	[startTime retain] ;

	pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: attrs] ;
	[self setPixelFormat: pixFmt] ;

	swapInterval = 1 ;
	[[self openGLContext]	setValues: &swapInterval		forParameter: NSOpenGLCPSwapInterval ] ;
	
	GLfloat position[] = {2.0, 2.0, 2.0, 0.5};
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	
//	theSelector = @selector( pulseColor );
//	aSignature = [MyOpenGLView	instanceMethodSignatureForSelector: theSelector] ;
//	anInvocation = [NSInvocation	invocationWithMethodSignature: aSignature] ;
//	[anInvocation setSelector: theSelector] ;
//	[anInvocation setTarget: self] ;

//	timer = [NSTimer scheduledTimerWithTimeInterval: 0.10 
//			invocation: anInvocation 
//			repeats: YES] ;
	   
//	[timer retain] ;
	   
    return self ;
}

- (void) dealloc

{
	for (int i=0; i<NUM_SOURCES; i++)
		delete source_positions[i];
	
//    [timer invalidate] ;
//    [timer release] ;
//    [startTime release] ;
}

//- (void) pulseColor
//{
//    colorIntensity = fabs( sin( 2.0 * M_PI * [startTime timeIntervalSinceNow] ) ) ;
	
//    [self setNeedsDisplay: YES] ;
//}


- (void) drawRect: (NSRect) bounds
{

	glClearColor( 0, 0, 0, 0 ) ;
    glClear( GL_COLOR_BUFFER_BIT ) ;
	
	GLUquadricObj *quadratic;
	quadratic=gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);  
	
	glLoadIdentity();
	glBegin(GL_LINES);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, -1.0);
	glEnd();
	
	glTranslatef(0.3f,0.0f,0.0);  
    glColor3f( 1.0f, 0.0f, 0.0f ) ;	
	gluSphere(quadratic,0.3f,32,32);
    
	glLoadIdentity();                                       // Reset The View
	gluLookAt(-1.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
//	glEnd() ;
	
    glFinish() ;
    [[self openGLContext] flushBuffer] ;
}

@end
