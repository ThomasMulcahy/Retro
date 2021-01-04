#import "platform.h"
#import "view.h"
#import "document.h"

#import <Cocoa/Cocoa.h>

/*
 * TODO: This temporarily holds a reference to the View, this should be moved into
 * it's own 'TextView' view in the future.
 * 
 * TODO: We may need multiple views for rendering i.e. status bars, this should be the parent view.
 */
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

		//TODO: Make this configurable
		font = [NSFont fontWithName:@"Menlo" size:13];
		FontMetrics *metrics = (FontMetrics *) malloc(sizeof(FontMetrics));
		metrics->lineHeight = font.ascender - font.descender + font.leading;
		metrics->advancement = font.maximumAdvancement.width;

		Cursor *cursor = (Cursor *) malloc(sizeof(Cursor));
		cursor->line = 0;
		cursor->col = 0;

		view = (View *) malloc(sizeof(View));
		view->fontMetrics = metrics;
		view->cursor = cursor;
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

		//Render text
		for (int i = 0; i < view->document->lineCount; i++) {
			char *line = documentGetLine(view->document, i);
			
			//NOTE: We may need to handle this for cursor position when the line has no characters?
			// if (strcmp(line, "\n") == 0 || line == NULL || strlen(line) <= 0)
			// 	continue;

			NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:font, 
										  NSFontAttributeName,[NSColor whiteColor], 
										  NSForegroundColorAttributeName, nil];
			NSString *str  = [NSString stringWithCString:line encoding:NSUTF8StringEncoding];
			[str drawAtPoint: NSMakePoint(0, i * view->fontMetrics->lineHeight) withAttributes: attributes];
		}

		//Render cursor
		NSRect cursorRect = NSMakeRect(view->cursor->col * view->fontMetrics->advancement, 
									   view->cursor->line * view->fontMetrics->lineHeight, 
									   view->fontMetrics->advancement, 
									   view->fontMetrics->lineHeight);
		[[NSColor whiteColor] setFill];
		NSRectFill(cursorRect);
	}

	//Events
	- (void) windowDidResize:(NSNotification*) notification {
		
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
		NSString *chars = event.charactersIgnoringModifiers;
    	unichar aChar = [chars characterAtIndex: 0];

		ViewEvent ve = {
			.eventKind = KEY_DOWN_EVENT,
			.code = aChar
		};

		onViewEvent(view, ve);
		[self setNeedsDisplay:YES];
	}

	- (void) keyUp: (NSEvent*) event {

	}
@end

//Entry point for the Cocoa application.
int platformRun(WindowOpt *winOptions) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];

    //Create Cocoa window.
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

    //Create menu bar - we require this as from Snow Leopard this is not given to us.
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

	[[NSApp mainWindow] makeKeyWindow];
    [NSApp run];
    //We reach here when the application is closed
    [pool drain];
    return(EXIT_SUCCESS);
}