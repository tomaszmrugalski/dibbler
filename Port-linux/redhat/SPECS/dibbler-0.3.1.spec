Summary: Dibbler - a portable DHCPv6
Name: dibbler
Version: 0.3.1
Release: 1
URL: http://klub.com.pl/dhcpv6/
Source: dibbler-0.3.1-src.tar.gz
License: GPL
Group: System Environment/Daemons
BuildRoot: /var/tmp/%{name}-buildroot

%description
It supports both stateful (i.e. IPv6 address granting) and stateless 
(i.e. options granting) autoconfiguration modes of DHCPv6 protocol. 
There are ports available for Linux 2.4/2.6 systems as well as MS 
Windows XP and 2003. They are freely available under GNU GPL v2 
or later license.

Install Dibbler if you'd like to have IPv6 hosts automatically 
configured in your network.

%prep
%setup -n dibbler

%build
make client server doc

%install
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/sbin
%{__install} -m 755 dibbler-server $RPM_BUILD_ROOT/usr/sbin/
%{__install} -m 755 dibbler-client $RPM_BUILD_ROOT/usr/sbin/
#mkdir -p $RPM_BUILD_ROOT/usr/share/doc/dibbler
#%{__install} -m 644 doc/dibbler-user.pdf $RPM_BUILD_ROOT/usr/share/doc/dibbler
#%{__install} -m 644 doc/dibbler-devel.pdf $RPM_BUILD_ROOT/usr/share/doc/dibbler
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man8
%{__install} -m 644 doc/man/dibbler-client.8 $RPM_BUILD_ROOT/usr/share/man/man8
%{__install} -m 644 doc/man/dibbler-server.8 $RPM_BUILD_ROOT/usr/share/man/man8
mkdir -p $RPM_BUILD_ROOT/var/lib/dibbler
%{__install} -m 644 client.conf $RPM_BUILD_ROOT/var/lib/dibbler
%{__install} -m 644 client-stateless.conf $RPM_BUILD_ROOT/var/lib/dibbler
%{__install} -m 644 server.conf $RPM_BUILD_ROOT/var/lib/dibbler
%{__install} -m 644 server-stateless.conf $RPM_BUILD_ROOT/var/lib/dibbler

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc CHANGELOG LICENSE RELNOTES doc/dibbler-user.pdf doc/dibbler-devel.pdf
/usr/sbin/dibbler-server
/usr/sbin/dibbler-client
/usr/share/man/man8/dibbler-server.8.gz
/usr/share/man/man8/dibbler-client.8.gz
#/usr/share/doc/dibbler/dibbler-user.pdf
#/usr/share/doc/dibbler/dibbler-devel.pdf
/var/lib/dibbler/client.conf
/var/lib/dibbler/client-stateless.conf
/var/lib/dibbler/server.conf
/var/lib/dibbler/server-stateless.conf

%changelog
