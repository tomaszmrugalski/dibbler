Summary: Dibbler - a portable DHCPv6
Name: dibbler
Version: 0.4.1
Release: 1
URL: http://klub.com.pl/dhcpv6/dibbler
Source: dibbler-0.4.1-src.tar.gz
License: GPL
Group: System Environment/Daemons
BuildRoot: /var/tmp/%{name}-buildroot

%description
It supports both stateful (i.e. IPv6 address granting) and stateless 
(i.e. options granting) autoconfiguration modes of DHCPv6 protocol. 
There are ports available for Linux 2.4/2.6 systems as well as MS 
Windows XP,2003 (experimental support for NT4 and 2000). 
They are freely available under GNU GPL v2 or later license. 

Install Dibbler if you'd like to have IPv6 hosts automatically 
configured in your network. All infrastructure elements are
provided: server, client and relay.

%prep
%setup

%build
make client server relay doc

%install
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/sbin
%{__install} -m 755 dibbler-server $RPM_BUILD_ROOT/usr/sbin/
%{__install} -m 755 dibbler-client $RPM_BUILD_ROOT/usr/sbin/
%{__install} -m 755 dibbler-relay  $RPM_BUILD_ROOT/usr/sbin/
#mkdir -p $RPM_BUILD_ROOT/usr/share/doc/dibbler
#%{__install} -m 644 doc/dibbler-user.pdf $RPM_BUILD_ROOT/usr/share/doc/dibbler
#%{__install} -m 644 doc/dibbler-devel.pdf $RPM_BUILD_ROOT/usr/share/doc/dibbler
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man8
%{__install} -m 644 doc/man/dibbler-client.8 $RPM_BUILD_ROOT/usr/share/man/man8
%{__install} -m 644 doc/man/dibbler-server.8 $RPM_BUILD_ROOT/usr/share/man/man8
%{__install} -m 644 doc/man/dibbler-relay.8  $RPM_BUILD_ROOT/usr/share/man/man8
mkdir -p $RPM_BUILD_ROOT/etc/dibbler
%{__install} -m 644 client.conf $RPM_BUILD_ROOT/etc/dibbler
%{__install} -m 644 client-stateless.conf $RPM_BUILD_ROOT/etc/dibbler
%{__install} -m 644 server.conf $RPM_BUILD_ROOT/etc/dibbler
%{__install} -m 644 server-stateless.conf $RPM_BUILD_ROOT/etc/dibbler
%{__install} -m 644 relay.conf $RPM_BUILD_ROOT/etc/dibbler

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc CHANGELOG LICENSE RELNOTES doc/dibbler-user.pdf doc/dibbler-devel.pdf
/usr/sbin/dibbler-server
/usr/sbin/dibbler-client
/usr/sbin/dibbler-relay
/usr/share/man/man8/dibbler-server.8.gz
/usr/share/man/man8/dibbler-client.8.gz
/usr/share/man/man8/dibbler-relay.8.gz
/etc/dibbler/client.conf
/etc/dibbler/client-stateless.conf
/etc/dibbler/server.conf
/etc/dibbler/server-stateless.conf
/etc/dibbler/relay.conf
%changelog
