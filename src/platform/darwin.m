#import "platform.h"
#import "platform_util.h"
#import <stdio.h>
#import <stdlib.h>
#import <string.h>

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

typedef struct _VertexIn{
    vector_float2 position;
    vector_float4 color;
} VertexIn;

typedef struct _UniformsIn {
    matrix_float4x4 orthoMatrix;
} UniformsIn;

static matrix_float4x4 createOrthographicMatrix(float left, float right, float top, float bottom, float near, float far) {
    return (matrix_float4x4) {
        .columns[0] = { 2.0f / (right - left), 0.0f, 0.0f, 0.0f },
        .columns[1] = { 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f },
        .columns[2] = { 0.0f, 0.0f, -2.0f / (far - near), 0.0f },
        .columns[3] = { -((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((far + near) / (far - near)), 1.0f }
    };
}

@interface AppDelegate : NSObject<NSApplicationDelegate, MTKViewDelegate> {
@public
    float frameWidth;
    float frameHeight;

    id<MTLDevice> mtlDevice;
    id<MTLLibrary> mtlLibrary;
    id<MTLRenderPipelineState> mtlPipelineState;
    NSMutableArray<id<MTLBuffer>> *vertexBuffers;
    id<MTLBuffer> indexBuffer;
    id<MTLBuffer> uniformBuffer;
    id<MTLCommandQueue> mtlCommandQueue;
}
@end

@implementation AppDelegate
    - (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
        return YES;
    }   

    - (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size {
        frameWidth = size.width;
        frameHeight = size.height;
    }

    - (void)drawInMTKView:(MTKView*)view {

        void* uniformSrc = &(UniformsIn) {
            .orthoMatrix = createOrthographicMatrix(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f)
        };
        void* uniformTgt = [uniformBuffer contents];
        memcpy(uniformTgt, uniformSrc, sizeof(UniformsIn));

        MTLRenderPassDescriptor* passDescriptor = [view currentRenderPassDescriptor];
        id<CAMetalDrawable> drawable = [view currentDrawable];
        id<MTLCommandBuffer> commandBuffer = [mtlCommandQueue commandBuffer];
        id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:passDescriptor];
        [commandEncoder setRenderPipelineState:mtlPipelineState];
        [commandEncoder setViewport:(MTLViewport) {0.0, 0.0, frameWidth, frameHeight, -1.0, 1.0}];
        [commandEncoder setScissorRect:(MTLScissorRect) {0.0, 0.0, frameWidth, frameHeight}];
        [commandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [commandEncoder setCullMode:MTLCullModeBack];

        for (NSUInteger i = 0; i < vertexBuffers.count; i++) {
            [commandEncoder setVertexBuffer:vertexBuffers[i] offset:0 atIndex:0];
            [commandEncoder setVertexBuffer:uniformBuffer offset:0 atIndex:1];
            [commandEncoder drawIndexedPrimitives: MTLPrimitiveTypeTriangle 
                                                   indexCount: 6
                                                   indexType: MTLIndexTypeUInt32
                                                   indexBuffer: indexBuffer
                                                   indexBufferOffset: 0];
        }

        [commandEncoder endEncoding];
        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
    }
@end

/*
 * This function is ran once when building the necessary data for the UI
 * to be rendered correctly by Metal.
 */
static void buildUI(AppDelegate *appDelegate, UIElement *headElement) {

  // Setup uniform buffer
  appDelegate->uniformBuffer = [appDelegate->mtlDevice
      newBufferWithLength:sizeof(UniformsIn)
                  options:MTLResourceCPUCacheModeWriteCombined];

  // Build UI
  appDelegate->vertexBuffers = [[NSMutableArray alloc] init];

  UIElement *node = headElement;
  while (node != NULL) {

    if (node->type == EDITOR) {
      VertexIn vertexData[] = {
          {{node->xPos, node->yPos},
           {node->color[0], node->color[1], node->color[2], 1.0}},
          {{node->xPos, node->yPos + node->height},
           {node->color[0], node->color[1], node->color[2], 1.0}},
          {{node->xPos + node->width, node->yPos + node->height},
           {node->color[0], node->color[1], node->color[2], 1.0}},
          {{node->xPos + node->width, node->yPos},
           {node->color[0], node->color[1], node->color[2], 1.0}},
      };

      id<MTLBuffer> vertexBuffer = [appDelegate->mtlDevice
          newBufferWithBytes:vertexData
                      length:sizeof(vertexData)
                     options:MTLResourceOptionCPUCacheModeDefault];
      [appDelegate->vertexBuffers addObject:vertexBuffer];

      // TODO: Currently index data is always the same - Maybe move out of here?
      int indexData[] = {0, 1, 2, 3, 0, 2};

      id<MTLBuffer> indexBuffer = [appDelegate->mtlDevice
          newBufferWithBytes:indexData
                      length:sizeof(indexData)
                     options:MTLResourceOptionCPUCacheModeDefault];
      appDelegate->indexBuffer = indexBuffer;
    }
    node = node->child;
  }
}

int platformRun(WindowOpt *winOptions, UIElement *headElement) {

    // Setup application. 
    NSError* error;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    AppDelegate *appDelegate = [[AppDelegate alloc] init];

    // Setup window
    // Note: We can pass 0, 0 for the location as we set the window to center furhter down.
    NSRect frame = NSMakeRect(0, 0, winOptions->width, winOptions->height); 
    NSWindow *window = [[NSWindow alloc] initWithContentRect: frame
                                         styleMask: NSWindowStyleMaskTitled | 
                                                    NSWindowStyleMaskClosable | 
                                                    NSWindowStyleMaskMiniaturizable | 
                                                    NSWindowStyleMaskResizable
                                         backing: NSBackingStoreBuffered
                                         defer: NO];

    //Setup Metal view and device
    appDelegate->mtlDevice = MTLCreateSystemDefaultDevice();
    MTKView* mtlView = [[MTKView alloc] initWithFrame:frame 
                                        device:appDelegate->mtlDevice];
    [mtlView setDelegate: appDelegate];

    [window setTitle:@(winOptions->title)];
    [window setContentView: mtlView];

    NSAppearance *appearance = [NSAppearance appearanceNamed: NSAppearanceNameDarkAqua];
    [window setAppearance: appearance];
    [window center];

    if (![window isMiniaturized]) {
        [window makeKeyAndOrderFront:NSApp];
    }

    // Setup metal shaders
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

    //Setup font
    Font *font = fontParse("res/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf");

    //Setup UI
    buildUI(appDelegate, headElement);

    appDelegate->mtlCommandQueue = [appDelegate->mtlDevice newCommandQueue];
    appDelegate->frameWidth = mtlView.drawableSize.width;
    appDelegate->frameHeight = mtlView.drawableSize.height;

    [NSApp setDelegate:[[AppDelegate alloc] init]];
    [NSApp run];
    [pool release];

    fontDestroy(font);
    return EXIT_SUCCESS;
}

char *platformReadFileToBuffer(char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("%s%s\n", "Error reading file: ", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *result = (char *)malloc(sizeof(char) * size);
    int bytesRead = fread(result, sizeof(char), size, file);
    result[bytesRead] = '\0';

    return result;
}

char *platformReadFileBytesToBuffer(char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("%s%s\n", "Error reading file: ", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *result = (char *)malloc(sizeof(char) * size);
    int bytesRead = fread(result, sizeof(char), size, file);
    result[bytesRead] = '\0';

    return result;
}

void *mallocate(size_t size, size_t count, char *label) {
    void *result = malloc(size * count);

    if (!result) {
        if (strlen(label) > 0)
            printf("%s%s\n","Unable to allocate memory for: ", label);
        else
            printf("%s\n","Unable to allocate memory!");

        exit(EXIT_FAILURE);
    }

#if DEBUG
    printf("%s%s%s%lu\n", "Allocating memory for: ", label, " Of Size: ", size * count);
#endif

    return result;
}
