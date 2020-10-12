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
            .type = EDITOR
        }
    };

    uiBuild(&layout);
    return platformRun(&options, argc, argv);
}