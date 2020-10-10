#ifndef PLATFORM_H
#define PLATFORM_H

/*
 * Platform.h contains all of the platform specific code. If any functionality is 
 * required that interacts with the OS or related systems then is will be situated
 * in here.
 *
 * Retro used hardware accelerated rendering as this offers defined control over rendering.
 * Using Metal on MacOS and OpenGl on Windows means that speed and efficiency can be 
 * guaranteed.
 *
 * The event loop for this applicaiton is designed to be blocking as redraws are issued 
 * upon events.
 */


typedef struct _Window {
    char *title;
    int width;
    int height;

    int shouldClose;
} Window;

/**
 * Creates a new window specific for the target platform.
 *
 * MacOS uses Cocoa with a Metal rendering system.
 * Windows uses a Win32 window system and OpenGL as a rendering system.
 */
Window *platformCreatWindow(char *title, int width, int height);

/*
 * Returns true if a window close event has been fired.
 */
int platformWindowShouldClose(Window *window);

/*
 * Calling this function swaps the rendering buffers to the next buffer for drawing.
 */
void platformSwapBuffers(Window *window);

/*
 * Call this event to poll the OS events for the system. This function is blocking as
 * the core loop of thie application is designed to be event based.
 */
void platformPollEvents(Window *window);

#endif