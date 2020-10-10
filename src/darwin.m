#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#import "platform.h"

@interface AppDelegate : NSObject<NSApplicationDelegate> {
    
}
@end

@implementation AppDelegate {

}
@end

int platformRun(WindowOpt *winOptions, int argc, char *argv[]) {

    // Setup application.
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // Setup window
    NSRect frame = NSMakeRect(100, 100, winOptions->width, winOptions->height);
    NSWindow *window = [[NSWindow alloc]
           initWithContentRect: frame
                     styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                       backing: NSBackingStoreBuffered
                         defer: NO];
    [window setTitle:@(winOptions->title)];
    [window center];

    if (![window isMiniaturized]) {
        [window makeKeyAndOrderFront:nil];
    }

    [NSApp run];
    [pool release];
    return EXIT_SUCCESS;
}