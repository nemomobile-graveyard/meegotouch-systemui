# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.25
# 

Name:       meegotouch-systemui

# >> macros
# << macros

Summary:    System UI daemon
Version:    1.5.7
Release:    2
Group:      System/Desktop
License:    LGPLv2.1
URL:        https://github.com/nemomobile/meegotouch-systemui
Source0:    %{name}-%{version}.tar.bz2
Source1:    meegotouch-systemui.service
Source100:  meegotouch-systemui.yaml
Requires:   contextkit-plugin-cellular
Requires:   contextkit-plugin-internet
Requires:   mce >= 1.12.4
BuildRequires:  pkgconfig(QtGui)
BuildRequires:  pkgconfig(QtOpenGL)
BuildRequires:  pkgconfig(contextsubscriber-1.0)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(meegotouch) >= 0.20.37
BuildRequires:  pkgconfig(meegotouch-controlpanel)
BuildRequires:  pkgconfig(qmsystem2) >= 1.1.6
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xcomposite)
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xdamage)
BuildRequires:  expat-devel
BuildRequires:  fdupes

%description
This package contains the system UI daemon.


%package devel
Summary:    Development files for system UI daemon
Group:      Development/System
Requires:   %{name} = %{version}-%{release}

%description devel
This package contains the system UI daemon development files.


%package demos
Summary:    Demo extensions for system-ui
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description demos
This package contains demo extensions for system-ui.


%package -n meegotouchcp-privatemodeapplet
Summary:    Private mode control panel applet
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}
Provides:   meegocontrolpanel-privatemodeapplet
Obsoletes:   meegocontrolpanel-privatemodeapplet

%description -n meegotouchcp-privatemodeapplet
This package contains the private mode control panel applet.


%package tests
Summary:    Unit tests for the MeeGo System UI Daemon
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description tests
This package contains the unit tests for the MeeGo System UI Daemon.



%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
unset LD_AS_NEEDED
# >> build pre
./configure
# << build pre

%qmake 

make %{?jobs:-j%jobs}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake_install
mkdir -p %{buildroot}%{_libdir}/systemd/user/
cp -a %{SOURCE1} %{buildroot}%{_libdir}/systemd/user/


# >> install post
# << install post

%fdupes  %{buildroot}/%{_datadir}

%files
%defattr(-,root,root,-)
%config %{_sysconfdir}/dbus-1/system.d/systemui.conf
%{_libdir}/systemd/user/meegotouch-systemui.service
%{_bindir}/sysuid
%{_datadir}/l10n/meegotouch/systemui.qm
%{_datadir}/themes/base/meegotouch/sysuid/*.conf
%{_datadir}/themes/base/meegotouch/sysuid/feedbacks
%{_datadir}/themes/base/meegotouch/sysuid/style/*.css
# >> files
# << files

%files devel
%defattr(-,root,root,-)
%{_includedir}/system-ui/*
%{_datadir}/qt4/mkspecs/features/system-ui.prf
# >> files devel
# << files devel

%files demos
%defattr(-,root,root,-)
%{_libdir}/meegotouch/applicationextensions/libstatusindicatormenu-profile.so
# >> files demos
# << files demos

%files -n meegotouchcp-privatemodeapplet
%defattr(-,root,root,-)
%{_libdir}/duicontrolpanel/applets/libprivatemodeapplet.so
%{_libdir}/duicontrolpanel/privatemode.desktop
# >> files meegotouchcp-privatemodeapplet
# << files meegotouchcp-privatemodeapplet

%files tests
%defattr(-,root,root,-)
/opt/tests/meegotouch-systemui-tests/*
# >> files tests
# << files tests