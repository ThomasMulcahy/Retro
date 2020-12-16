#include "platform.h"
#include "view.h"


//TODO: We initiate a view here on load, when we implement context dependant file loading
//this will need to be changed.
int main() {

    WindowOpt options = {
        .title = "Retro",
        .width = 640,
        .height = 400
    };

    return platformRun(&options);
}