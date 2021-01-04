#include "view.h"

static void moveCursorDown(View *view) {
    if (view->cursor->line < view->document->lineCount - 1) {
        view->cursor->line++;
    }
    else {
        view->cursor->line = view->document->lineCount - 1;
    }

    int lineLength = view->document->lineLengths[view->cursor->line];
    if (lineLength < 0)
        lineLength = 0;


    if (view->cursor->col > lineLength) {
        view->cursor->col = lineLength;
    }
}

static void moveCursorUp(View *view) {
    if (view->cursor->line > 0) {
        view->cursor->line--;
    }
    else { 
        view->cursor->line = 0;
    }

    int lineLength = view->document->lineLengths[view->cursor->line];
    if (lineLength < 0)
        lineLength = 0;

    if (view->cursor->col > lineLength) {
        view->cursor->col = lineLength;
    }
}

static void onKeyDown(View *view, char code) {
    switch (code) {
        case KEY_J_CODE:
            moveCursorDown(view);
            break;
        case KEY_K_CODE:
            moveCursorUp(view);
            break;
        case KEY_H_CODE:
            if (view->cursor->col > 0)
                view->cursor->col--;
            else
             view->cursor->col = 0;
            break;
        case KEY_L_CODE: {
            int lineLength = view->document->lineLengths[view->cursor->line];
            if (view->cursor->col < lineLength)
                view->cursor->col++;
            else
                view->cursor->col = lineLength;
            }
            break;
    }
}

void onViewEvent(View *view, ViewEvent event) {
    switch (event.eventKind) {
        case KEY_DOWN_EVENT:
            onKeyDown(view, event.code);
            break;
    }
}

void viewDestroy(View *view) {

}
