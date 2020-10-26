# TODO

## Font
Clean up font parsing. We do not need to parse the whole font 
in order to correctly render the font.

This can be established once font rendering is implemented and tested.
Currently it is in it's infancy.

## Logging
Implement a suitable logger that can replace printf.

Also clean up the code base to handle errors more elegantly. Currently 
the application will fail silently withot an explanation or seg fault.