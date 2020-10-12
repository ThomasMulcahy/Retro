#include "platform.h"
#include "ui.h"

int main(int argc, char *argv[]) {

    //Window settings
    WindowOpt options = {
        .title = "Retro",
        .width = 450,
        .height = 360
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

    uiBuild(&layout);
    return platformRun(&options, argc, argv);
}