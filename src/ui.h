#ifndef UI_H
#define UI_H

/********************************************************************
 * UI rendering is handled by the platforms target rendering system.
 * 
 * For example, MacOS uses Metal for rendering, therefore the UI will
 * be rendered using Metal.
 *
 * TODO: Abstract into reusable components
 ********************************************************************/

#define TO_PERCENTAGE(n) ((n < 0) ? 0 : (n > 100) ? 100 : (n / 100))

 /*
  * ElementType determines how the UIElement should be handled. There
  * will be corresponding functions for each type of element, these
  * will be handled in there own corresponding .c file, possibly paired 
  * with a .h file.
  */
enum ElementType {
    CONTAINER,

    EDITOR
};

typedef struct _UIElement {
    // The type of the element. Used for specified drawing.
    enum ElementType type;

    /* All elements contain a width and a height.
     * This is a percentage(%) in relation to the parent container, currently
     * there is only one possible tree and container.
     *
     * TODO: Add functionality for mulitple UI trees and containers.
     */
    int width;
    int height;

    // The child element, currently only used by the container and only one child permitted.
    struct _UIElement *child;
} UIElement;

void uiEditorDraw(UIElement element);

/*******************************
 ****** Platfrom specific ******
 *******************************/
void uiDraw(UIElement *headElement);

#endif