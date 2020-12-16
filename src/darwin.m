#import "platform.h"
#import "view.h"
#import "document.h"

#import <Cocoa/Cocoa.h>

@interface AppView : NSView <NSWindowDelegate>
{
	//TODO: Temp - Delete
	NSFont *font;
	View *view;
}
@end

@implementation AppView

	- (instancetype) initWithFrame:(NSRect)frameRect {
		[super initWithFrame:frameRect];

		font = [NSFont fontWithName:@"Menlo" size:13];
		FontMetrics *metrics = (FontMetrics *) malloc(sizeof(FontMetrics));
		metrics->lineHeight = font.xHeight + font.leading;

		view = (View *) malloc(sizeof(View));
		view->fontMetrics = metrics;
		view->document = documentLoad("test/temp.c");
		return self;
	}

    //Accept window events
    - (BOOL) acceptsFirstResponder {
        return YES;
    }

    - (void) windowWillClose:(NSNotification *)notification {
        [NSApp terminate:self];
    } 

    - (void) dealloc {  
	    [super dealloc];
    }  

	//This is required, as we need to use a flipped coordinate system for text rendering.
	//NOTE: (0, 0) is now top-left of the window. All sub-views should follow this.
	- (BOOL) isFlipped {
		return YES;
	}

	- (void) drawRect:(NSRect)rectToRedraw {
		//We should use attributed strings for rendering, this could help with formatting? Below is an example
		//NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont fontWithName:@"Menlo" size:13], 
		//							  NSFontAttributeName,[NSColor whiteColor], 
		//							  NSForegroundColorAttributeName, nil];
		//NSAttributedString * currentText= [[NSAttributedString alloc] initWithString:@"Cat" attributes: attributes];
		//NSSize attrSize = [currentText size];
		//[currentText drawAtPoint:NSMakePoint(0, 0)];

		for (int i = 0; i < view->document->lineCount; i++) {
			char *line = documentGetLine(view->document, i);
			
			NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:font, 
										  NSFontAttributeName,[NSColor whiteColor], 
										  NSForegroundColorAttributeName, nil];
			NSAttributedString * currentText = [[NSAttributedString alloc] 
												initWithString:[NSString stringWithUTF8String:line] attributes: attributes];
			[currentText drawAtPoint: NSMakePoint(0, i * view->fontMetrics->lineHeight)];
		}
	}

	//Events
	- (void) windowDidResize:(NSNotification*)notification {
		
	}

	- (void) mouseMoved:(NSEvent*) event {

	}

	- (void) mouseDragged: (NSEvent*) event {

	}

	- (void) scrollWheel: (NSEvent*) event  {

	}

	- (void) mouseDown: (NSEvent*) event {

	}

	- (void) mouseUp: (NSEvent*) event {

	}

	- (void) rightMouseDown: (NSEvent*) event {

	}

	- (void) rightMouseUp: (NSEvent*) event {

	}

	- (void) otherMouseDown: (NSEvent*) event {

	}

	- (void) otherMouseUp: (NSEvent*) event {

	}

	- (void) mouseEntered: (NSEvent*)event {

	}

	- (void) mouseExited: (NSEvent*)event {

	}

	- (void) keyDown: (NSEvent*) event {
		
	}

	- (void) keyUp: (NSEvent*) event {

	}
@end

//Entry point for the Cocoa application
int platformRun(WindowOpt *winOptions) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];

    //Create Cocoa window
    NSRect frame = NSMakeRect(0, 0, winOptions->width, winOptions->height);
    NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable; 
    NSRect screenRect = [[NSScreen mainScreen] frame];
	NSRect viewRect = NSMakeRect(0, 0, winOptions->width, winOptions->height); 
	NSRect windowRect = NSMakeRect(NSMidX(screenRect) - NSMidX(viewRect),
								 NSMidY(screenRect) - NSMidY(viewRect),
								 viewRect.size.width, 
								 viewRect.size.height);

	NSWindow * window = [[NSWindow alloc] initWithContentRect:windowRect 
						styleMask:style 
						backing:NSBackingStoreBuffered 
						defer:NO]; 

	//Window controller 
	NSWindowController * windowController = [[NSWindowController alloc] initWithWindow:window]; 
	[windowController autorelease];

    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    //Create menu bar
	id menubar = [[NSMenu new] autorelease];
	id appMenuItem = [[NSMenuItem new] autorelease];
	[menubar addItem:appMenuItem];
	[NSApp setMainMenu:menubar];

	id appMenu = [[NSMenu new] autorelease];
	id quitTitle = [@"Quit " stringByAppendingString:[NSString stringWithCString:winOptions->title encoding:NSASCIIStringEncoding]];
	id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
		                                   action:@selector(terminate:) 
                                           keyEquivalent:@"q"] autorelease];
	[appMenu addItem:quitMenuItem];
	[appMenuItem setSubmenu:appMenu];

    //Create app delegate to handle system events
	AppView* view = [[[AppView alloc] initWithFrame:frame] autorelease];
	[window setAcceptsMouseMovedEvents:YES];
	[window setContentView:view];
	[window setDelegate:view];

	[window setContentView:view];
	[window makeFirstResponder:view];

	//Set app title
	[window setTitle:[NSString stringWithCString:winOptions->title encoding:NSASCIIStringEncoding]];

	//Add fullscreen button
	[window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];

	//Show window and run event loop 
	[window orderFrontRegardless]; 

    [NSApp run];
    //We reach here when the application is closed
    [pool drain];
    return(EXIT_SUCCESS);
}