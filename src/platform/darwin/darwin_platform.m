#import "../platform.h"

#import "stdio.h"
#import "stdlib.h"

Window *platformCreatWindow(char *title, int width, int height) {

    Window *window = malloc(sizeof(Window));
    window->title = title;
    window->width = width;
    window->height = height;
    window->shouldClose = 0;

    return window;
}

int platformWindowShouldClose(Window *window) {
    return window->shouldClose;
}

void platformSwapBuffers(Window *window) {

}

void platformPollEvents(Window *window) {

}