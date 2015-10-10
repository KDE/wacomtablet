#if defined(HAVE_XCB_XINPUT)
#include "x11inputdevice-xcb.cpp"
#else
#include "x11inputdevice-xlib.cpp"
#endif
