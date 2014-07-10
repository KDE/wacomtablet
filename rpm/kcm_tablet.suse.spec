#
# spec file for package kcm_tablet
#

# norootforbuild

Name:           kcm_tablet
Version:        2.0.2
Release:        1
License:        GPLv2+
Group:          System/GUI/KDE
Summary:        KDE Config Module for Wacom Tablets
Url:            https://projects.kde.org/projects/extragear/base/wacomtablet
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Requires:       kdelibs4
Requires:       kdebase4-runtime
Requires:       xf86-input-wacom
BuildRequires:  libkde4-devel
BuildRequires:  xf86-input-wacom-devel
%define rname wacomtablet
Source:         %rname-%{version}.tar.xz
%kde4_runtime_requires

%description
This module implements a GUI for the Wacom Linux Drivers and extends it
with profile support to handle different button / pen layouts per profile.

For hardware support have a look at http://www.linuxwacom.sourceforge.net

Author(s):
  Joerg Ehrichs <joerg.ehrichs@gmx.de>
  Alexander Maret-Huskinson <alex@maret.de>

%prep
%setup -q -n %rname-%{version}

%build
  %cmake_kde4 -d build
  %make_jobs

%install
  %kde4_makeinstall -C build
  %kde_post_install
  %find_lang wacomtablet

%clean
rm -rf $RPM_BUILD_ROOT

%files -f wacomtablet.lang
%defattr(-,root,root)
%{_kde4_appsdir}/wacomtablet
%{_kde4_appsdir}/wacomtablet/wacomtablet.notifyrc
%{_kde4_htmldir}/en/kcontrol
%{_kde4_htmldir}/en/kcontrol/wacomtablet
%{_kde4_htmldir}/en/kcontrol/wacomtablet/common
%{_kde4_htmldir}/en/kcontrol/wacomtablet/index.cache.bz2
%{_kde4_htmldir}/en/kcontrol/wacomtablet/index.docbook
%{_kde4_htmldir}/en/kcontrol/wacomtablet/presscurve.png
%{_kde4_htmldir}/en/kcontrol/wacomtablet/tabletarea.png
%{_kde4_modulesdir}/kcm_wacomtablet.so
%{_kde4_modulesdir}/kded_wacomtablet.so
%{_kde4_modulesdir}/plasma_applet_wacomtabletsettings.so
%{_kde4_servicesdir}/kcm_wacomtablet.desktop
%{_kde4_servicesdir}/kded/wacomtablet.desktop
%{_kde4_servicesdir}/plasma-applet-wacomtabletsettings.desktop
/usr/share/dbus-1/interfaces/org.kde.Wacom*xml
%doc AUTHORS COPYING README

%changelog
* Thu Jul 10 2014 joerg.ehrichs@gmx.de
- Version 2.0.2
  * Fix compile error on older Qt versions
  * Add Raw Sample and Suppress to ui

* Wed Jun 25 2014 joerg.ehrichs@gmx.de
- Version 2.0.1
  * More translations
  * Add Wacom Intuos Pro S / PTH-451/S
  * Add WacomIntuos Pen Small / CTL-480S tablet
  * Add definition of Wacom Intuos PT S / CTH-680S-RUPL
  * Add Wacom Intuos PT M / CTH-680S-ENES
  * Add support for Wacom Intuos Pro M / PTH-651
  * cmake: remove extra kio linking
  * adjusted control point and draw below curve area colors to be less websafe looking
  * Added Wacom Cintiq 13HD
  * Added Samsung Series 7 Slate

* Wed May 08 2013 alex@maret.de
- Version 2.0
  * No changes to release candidate 2.

* Sun May 05 2013 alex@maret.de
- Version 1.99.9 (2.0 Release Candidate 2)
  * Fixed bug where the default profile would not be created properly.
  * Fixed bug which prevented special keys from being used as express button shortcuts.

* Sun Mar 30 2013 alex@maret.de
- Version 1.99.8 (2.0 Release Candidate 1)
  * Fixed bug where the plasma applet would not rotate the stylus controls.
  * Fixed bug which displayed the tablet in an inverted rotation if auto-rotate was selected.
  * Fixed bug which prevented the screen mappings from being applied correctly when the tablet was rotated.
  * Fixed bug which would not allow to set a screen mapping on the touch device.
  * Fixed bug where invalid area mappings would make the stylus stick in the upper left corner.
  * Changed way how tablets are reset to their full tablet mapping to fix problems with Intuos 5 tablets.
  * Added some more tablets to the tablet database.
  * Updated button hardware mappings for the Intuos 5 Touch.
  * Disabled Waltop devices in the database as they are in conflict with some Wacom devices.

* Sun Mar 24 2013 alex@maret.de
- Version 1.99.7
  * refactored and improved backend
  * many KCM GUI changes
  * bugfixes
