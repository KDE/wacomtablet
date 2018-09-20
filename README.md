KDE Wacom support
=================

This module implements a GUI for the Wacom Linux Drivers and extends it
with profile support to handle different button / pen layouts per profile.

For hardware support have a look at https://linuxwacom.github.io/

Configuration can be located in System Settings → Input devices → Tablets

Easy profile switching is supported via hotkeys or a small plasma applet in tray notification area.
In addition, a daemon running in the background helps with hotplug support.

Hardware support
----------------

All tablets can be set up as long as they are found with the wacom kernel module.

Check with

    $ xsetwacom list devices

if your device is correctly recognized first. If it dosn't show up there, we can't configure it.

Installation
------------

Wacom support in KDE is a separate component and might be not installed by default.
Package usually goes by name `wacomtablet` or `kcm-wacomtablet`.

More or less full list of distributions including the package should be avaliable at:

* https://repology.org/metapackage/kcm-wacomtablet/versions

Staring the module
------------------

Background daemon should be started automatically each time you log in.

You can enable or manually start the service via System Settings → Startup and Shutdown → Background services.

Adding missing tablet devices
-----------------------------

If your device is not recognized by this program, but you can set it up via the xsetwacom driver,
run Wacom Tablet Finder (`kde_wacom_tabletfinder`), desribe your device and click `Save`, and restart Wacom tablet service (see above).

Please send your device description (file `~/.config/tabletdblocalrc`) to the bugtracker, so it will be added in future versions.

Reporting bugs
--------------

https://bugs.kde.org/enter_bug.cgi?product=wacomtablet

Building & manual installation
==============================

This is *not* a recommended way to use this application. Installing it manually creates files untracked by your pacakge manager.
This can cause everyone problems later on. Please contact your distribution's maintainers to package it instead.

Run-time dependencies:
* X11 wacom tablet driver (xserver-xorg-input-wacom), including the xsetwacom tool, version 0.20 or higher
* libwacom. Highly recommended to have version 0.29 or higher for support of consumer-grade tablets with quirky buttons.

Build dependencies on Debian/Ubuntu:
* g++
* cmake
* extra-cmake-modules
* gettext
* libqt5x11extras5-dev
* qtdeclarative5-dev
* libkf5coreaddons-dev
* libkf5i18n-dev
* libkf5dbusaddons-dev
* libkf5globalaccel-dev
* libkf5config-dev
* libkf5xmlgui-dev
* libkf5notifications-dev
* plasma-framework-dev
* kdoctools-dev
* libxi-dev
* libwacom-dev

You can install them by running:

    $ apt install g++ cmake extra-cmake-modules gettext libqt5x11extras5-dev qtdeclarative5-dev libkf5coreaddons-dev libkf5i18n-dev libkf5dbusaddons-dev libkf5globalaccel-dev libkf5config-dev libkf5xmlgui-dev libkf5notifications-dev plasma-framework-dev kdoctools-dev libxi-dev libwacom-dev

Building from source
--------------------

    $ mkdir build
    $ cd build
    $ cmake ../ -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
    $ make

Manual installation (this *will* litter in your system):

    $ make install

Running without installing
--------------------------

This is not intended for daily use, but for people who want to test developer builds.

Running KCM component:

    $ QT_PLUGIN_PATH=src/kcmodule/ kcmshell5 wacomtablet

Running KDED component:

    $ ln -s kf5 src/
    $ kquitapp5 kded5 && QT_PLUGIN_PATH=$PWD kded5

Running tablet finder:

    $ src/tabletfinder/kde_wacom_tabletfinder

Running unit tests
==================

First, configure the build to include tests by enabling BUILD_TESTING:

    $ mkdir build
    $ cd build
    $ cmake ../ -DBUILD_TESTING=ON
    $ make

Then, run the tests with:

    $ ctest

or a single one via

    $ cd autotests/common/property
    $ ./Test.Common.Property

you can find the test results in `Testing/Temporary/LastTest.log`

Manually adding missing tablet devices
======================================

This method shouldn't really be used anymore unless you've encountered problems with kde_wacom_tabletfinder

### Step 1) Get real internal Tablet name ###

xsetwacom list dev

for my Wacom Bamboo:
Wacom Bamboo eraser ERASER
Wacom Bamboo cursor CURSOR
Wacom Bamboo pad PAD
Wacom Bamboo     STYLUS

### Step 2) Then you need the Tablet ID ###

xsetwacom get "Wacom Bamboo" tabletid

for my Wacom Bamboo:
101

### Step 3) Transform it into Hexadecimal ###

For my Wacom Bamboo: 0065

### Step 4) Add a new section into either ~/.config/tabletdblocalrc or the right datafile in /usr/share/wacomtablet/data/ ###

for the Wacom Bamboo the right file is: wacom_devicelist

Add a new entry

for my Wacom Bamboo:

[0065]
model=MTE_450        # try to google for it but not used right now
layout=bl_6          # can be empty if no button layout is available that exists (see images dir)
name=Wacom Bamboo    # name as found above
padbuttons=4         # available buttons on the pad
wheel=no
touchring=yes
touchstripl=no
touchstripr=no
hwbutton1=1          # these mag the physical button numbers to what the XServer likes to name them
hwbutton2=2          # see below for more information
hwbutton3=3
hwbutton4=8

### hwbuttonX explanantion ###

Kernels >= 2.6.38 renamed the buttons for some reason.

xsetwacom does not work with the pysical hardware button numbers 1-X
but rather with the real numbers as seen by the XServer

In case you run into trouble with the buttons do the following:
* deinstall the kded/kcm ;)
* disconnect/reconnect the tablet
* run xev in the terminal
* move the mouse over the new window
* press the hardware buttons and write down what button xev sees

and now update the wacom_device list accordingly

for the new Pen & touch tablets before the kernel 2.6.38 we got
hwbutton1=1
hwbutton2=2
hwbutton3=3
hwbutton4=8

for kernels >= 2.6.38 we get
hwbutton1=3
hwbutton2=8
hwbutton3=9
hwbutton4=1

Releasing new version
=====================

Checklist for developers:

* Bump main package version in: `CMakeLists.txt`
* Bump applet version in: `src/applet/package/metadata.desktop`
* Follow https://community.kde.org/ReleasingSoftware
