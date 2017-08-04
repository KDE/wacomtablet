#if defined(HAVE_XCB_XINPUT)
#include "calibrationdialog-xcb.cpp"
#else
#include "calibrationdialog-xlib.cpp"
#endif
