#include "platform/platform.h"
#include "ui.h"

int main() {

    WindowOpt options = {
        .title = "Retro",
        .width = 640,
        .height = 400
    };

    UIElement root = {
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

    return platformRun(&options, &root);
}