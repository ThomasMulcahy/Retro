#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#import "platform.h"

typedef struct {
    matrix_float4x4 rotationMatrix;
} Uniforms;

typedef struct {
    vector_float4 position;
    vector_float4 color;
} VertexIn;

static const VertexIn vertexData[] =
    {
        { { 0.5, -0.5, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0} },
        { {-0.5, -0.5, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0} },
        { {-0.5,  0.5, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0} },
        { { 0.5,  0.5, 0.0, 1.0}, {1.0, 1.0, 0.0, 1.0} },
        { { 0.5, -0.5, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0} },
        { {-0.5,  0.5, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0} }
    };

static matrix_float4x4 rotationMatrix2D(float radians)
{
    float cos = cosf(radians);
    float sin = sinf(radians);
    return (matrix_float4x4) {
        .columns[0] = {  cos, sin, 0, 0 },
        .columns[1] = { -sin, cos, 0, 0 },
        .columns[2] = {    0,   0, 1, 0 },
        .columns[3] = {    0,   0, 0, 1 }
    };
}


@interface AppDelegate : NSObject<NSApplicationDelegate, MTKViewDelegate> {
@public
    NSWindow* _window;
    id<MTLDevice> mtlDevice;
    id<MTLLibrary> mtlLibrary;
    id<MTLRenderPipelineState> mtlPipelineState;
    id<MTLBuffer> mtlVertexBuffer;
    id<MTLBuffer> mtlUniformBuffer;
    id<MTLCommandQueue> mtlCommandQueue;
}
@end

@implementation AppDelegate
    - (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
        return YES;
    }   

    - (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size {
        (void)view;
        (void)size;
    }

    - (void)drawInMTKView:(MTKView*)view {

        double rotationAngle = fmod(CACurrentMediaTime(), 2.0 * M_PI);
        void* uniformSrc = &(Uniforms) {
            .rotationMatrix = rotationMatrix2D(rotationAngle)
        };
        void* uniformTgt = [mtlUniformBuffer contents];
        memcpy(uniformTgt, uniformSrc, sizeof(Uniforms));

        MTLRenderPassDescriptor* passDescriptor = [view currentRenderPassDescriptor];
        id<CAMetalDrawable> drawable = [view currentDrawable];
        id<MTLCommandBuffer> commandBuffer = [mtlCommandQueue commandBuffer];
        id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:passDescriptor];

        [commandEncoder setRenderPipelineState:mtlPipelineState];
        [commandEncoder setVertexBuffer:mtlVertexBuffer offset:0 atIndex:0];
        [commandEncoder setVertexBuffer:mtlUniformBuffer offset:0 atIndex:1];
        [commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];
        [commandEncoder endEncoding];
        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
    }
@end

int platformRun(WindowOpt *winOptions, UIElement *headElement, int argc, char *argv[]) {

    // Setup application. 
    NSError* error;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    AppDelegate *appDelegate = [[AppDelegate alloc] init];

    // Setup window
    // Note: We can pass 0, 0 for the location as we set the window to centre furhter down.
    NSRect frame = NSMakeRect(0, 0, winOptions->width, winOptions->height); 
    NSWindow *window = [[NSWindow alloc]
           initWithContentRect: frame
                     styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                       backing: NSBackingStoreBuffered
                         defer: NO];

    //Setup Metal view and device
    appDelegate->mtlDevice = MTLCreateSystemDefaultDevice();
    MTKView* mtlView = [[MTKView alloc] initWithFrame:frame 
                                     device:appDelegate->mtlDevice];
    [mtlView setDelegate: appDelegate];

    [window setTitle:@(winOptions->title)];
    [window setContentView: mtlView];
    [window center];

    if (![window isMiniaturized]) {
        [window makeKeyAndOrderFront:NSApp];
    }

    /*
     * Metal setup: Library
     */
    NSString* librarySrc = [NSString stringWithContentsOfFile:@"res/shaders/library.metal" encoding:NSUTF8StringEncoding error:&error];
    if(!librarySrc) {
        [NSException raise:@"Failed to read shaders" format:@"%@", [error localizedDescription]];
    }

    appDelegate->mtlLibrary = [appDelegate->mtlDevice newLibraryWithSource:librarySrc options:nil error:&error];
    if(!appDelegate->mtlLibrary) {
        [NSException raise:@"Failed to compile shaders" format:@"%@", [error localizedDescription]];
    }

    //Metal render pipeline
    id <CAMetalDrawable> drawable = [mtlView currentDrawable];
    MTLRenderPipelineDescriptor *mtlRenderPipelineDescriptor = [MTLRenderPipelineDescriptor new];
     mtlRenderPipelineDescriptor.vertexFunction = [appDelegate->mtlLibrary newFunctionWithName:@"vertexFunction"];
    mtlRenderPipelineDescriptor.fragmentFunction = [appDelegate->mtlLibrary newFunctionWithName:@"fragmentFunction"];
    mtlRenderPipelineDescriptor.colorAttachments[0].pixelFormat = drawable.texture.pixelFormat;;

    appDelegate->mtlPipelineState = [appDelegate->mtlDevice newRenderPipelineStateWithDescriptor:mtlRenderPipelineDescriptor error:&error];
    if (!appDelegate->mtlPipelineState) {
      [NSException raise:@"Failed to create pipeline state" format:@"%@", [error localizedDescription]];
    }

    /*
     * Metal setup: Vertices
     */
    appDelegate->mtlVertexBuffer = [appDelegate->mtlDevice newBufferWithBytes:vertexData
        length:sizeof(vertexData)
        options:MTLResourceStorageModeShared];

    /*
     * Metal setup: Uniforms
     */
    appDelegate->mtlUniformBuffer = [appDelegate->mtlDevice newBufferWithLength:sizeof(Uniforms)
        options:MTLResourceCPUCacheModeWriteCombined];

    appDelegate->mtlCommandQueue = [appDelegate->mtlDevice newCommandQueue];

    [NSApp setDelegate:[[AppDelegate alloc] init]];
    [NSApp run];
    [pool release];
    return EXIT_SUCCESS;
}

void uiDraw(UIElement *headElement){}