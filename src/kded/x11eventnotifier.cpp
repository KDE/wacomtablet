#if defined(HAVE_XCB_XINPUT)
#include "x11eventnotifier-xcb.cpp"
#else
#include "x11eventnotifier-xlib.cpp"
#endif
