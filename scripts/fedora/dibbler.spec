Summary: Dibbler - a portable DHCPv6
Name: dibbler
Version: 1.0.0RC1
Release: 1
License: GPL
Group: Applications/Internet
Source: %{name}-%{version}.tar.gz
URL: http://klub.com.pl/dhcpv6/
Packager: Patrick PICHON
BuildRoot: /var/tmp/%{name}-buildroot

%description
Dibbler is a portable DHCPv6 implementation. It supports stateful (i.e. IPv6 address granting and IPv6 prefix delegation) as well as stateless (i.e. option granting) autoconfiguration for IPv6. Currently Linux 2.4 or later and Windows XP or later are supported. 

%prep

%setup

%build
./configure --prefix=/usr
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install
mkdir -p $RPM_BUILD_ROOT/var/lib/dibbler
mkdir -p $RPM_BUILD_ROOT/etc/dibbler
rm -rf $RPM_BUILD_ROOT%{_docdir}/%{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc AUTHORS TODO RELNOTES LICENSE
/etc/dibbler
/var/lib/dibbler
/usr/sbin/*
%{_mandir}/*
/usr/share/*


%changelog
* Fri Aug 30 2013 Patrick Pichon <patrick@pipiche.fr>
- Initial RPM for dedibox
