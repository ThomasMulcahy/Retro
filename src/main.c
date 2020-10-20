#include "platform.h"
#include "ui.h"
#include "font.h"

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
            .color = {COL_TO_FLOAT(45), COL_TO_FLOAT(48), COL_TO_FLOAT(58)}
        }
    };

    return platformRun(&options, &layout);
}