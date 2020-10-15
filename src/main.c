#include "platform.h"
#include "ui.h"

#include <stdio.h>

int main(int argc, char *argv[]) {

    //Window settings
    WindowOpt options = {
        .title = "Retro",
        .width = 640,
        .height = 400
    };

    //UI layout
    UIElement layout = {
        .type = CONTAINER,

        .xPos = 0,
        .yPos = 0,
        .width = TO_PERCENTAGE(100),
        .height = TO_PERCENTAGE(100),

        .child = &(UIElement) {
            .type = EDITOR,

            .xPos = 0,
            .yPos = 0,
            .width = TO_PERCENTAGE(100),
            .height = TO_PERCENTAGE(100),
            .color = {45.0f / 255.0f, 48.0f / 255.0f, 58.0f / 255.0f}
        }
    };

    return platformRun(&options, &layout);
}