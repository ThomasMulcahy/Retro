#include "platform/platform.h"

#include <stdio.h>

int main() {

    Window *window = platformCreatWindow("Retro", 480, 340);

    while(!platformWindowShouldClose(window)) {

        platformPollEvents(window);
        platformSwapBuffers(window);

        //TODO: Remove
        break;
    }

    return 0;
}