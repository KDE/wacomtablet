#
# spec file for package kcm_tablet
#

# norootforbuild

Name:           kcm_tablet
Version:        1.99.7
Release:        1
License:        GPLv2+
Group:          System/GUI/KDE
Summary:        KDE Config Module for Wacom Tablets
Url:            https://projects.kde.org/projects/extragear/base/wacomtablet
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  libkde4-devel
BuildRequires:  xf86-input-wacom-devel
%define rname wacomtablet
Source:         %rname-%{version}.tar.gz
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

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_kde4_appsdir}/wacomtablet
%{_kde4_appsdir}/wacomtablet/wacomtablet.notifyrc
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
* Sun Mar 24 2013 alex@maret.de
- Version 1.99.7
  * refactored and improved backend
  * many KCM GUI changes
  * bugfixes
