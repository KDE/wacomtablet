include(CheckTypeSize)

set(CMAKE_EXTRA_INCLUDE_FILES linux/uinput.h dev/evdev/uinput.h)
check_type_size("struct uinput_setup" STRUCT_UINPUT_SETUP)
set(CMAKE_EXTRA_INCLUDE_FILES)

if(HAVE_STRUCT_UINPUT_SETUP)
    add_executable(faketablet faketablet.cpp)
else()
    message(STATUS "No suitable uinput API, fake tablet is disabled")
endif()
