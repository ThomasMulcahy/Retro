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

        .child = &(UIElement) {
            .type = EDITOR,

            .width = TO_PERCENTAGE(100),
            .height = TO_PERCENTAGE(100),
        }
    };

    return platformRun(&options, &layout, argc, argv);
}