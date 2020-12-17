# TODO

## Logging
Implement a suitable logger that can replace printf.

Also clean up the code base to handle errors more elegantly. Currently 
the application will fail silently withot an explanation or seg fault.

## Assess Metal
Do we need Metal for rendering? This could be useful, Windows will most likely make use
of OpenGL for rendering.


# Stretch Goals
## Split platform layer - Refine API
As the platform layer gets larger we may need to handle the platform with a more
modular system. For now the system in place will suffice, but once new features are
needed, the platform layer should be refactored.

This will also help with porting, as all neccessary functions should be defined in
platform.h. Once we have finalised the API proper implmentation of platform.h should 
allow for a simple porting procedure.