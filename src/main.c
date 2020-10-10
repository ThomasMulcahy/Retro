#include "platform.h"

int main(int argc, char *argv[]) {

    WindowOpt options = {
        .title = "Retro",
        .width = 450,
        .height = 360
    };

    return platformRun(&options, argc, argv);
}