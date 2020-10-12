#ifndef UI_H
#define UI_H

/********************************************************************
 * UI rendering is handled by the platforms target rendering system.
 * 
 * For example, MacOS uses Metal for rendering, therefore the UI will
 * be rendered using Metal.
 ********************************************************************/

#define TO_PERCENTAGE(n) (n / 100)

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
    enum ElementType type;

    int width;
    int height;

    struct _UIElement *child;
} UIElement;

/*******************************
 ****** Platfrom specific ******
 *******************************/
void uiDraw(UIElement *headElement);

#endif