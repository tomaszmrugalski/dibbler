# $Revision: 1.1 $, $Date: 2005-02-07 20:49:18 $
Summary:	Dibbler - a portable DHCPv6
Summary(pl):	Dibbler - przeno¶ny DHCPv6
Name:		dibbler
Version:	0.3.1
Release:	0.3
License:	GPL v2
Group:		Networking/Daemons
Source0:	http://klub.com.pl/dhcpv6/%{name}-%{version}-src.tar.gz
# Source0-md5:	6bc2b0932f1000ad50624789873115d8
Source1:	http://klub.com.pl/dhcpv6/%{name}-%{version}-doc.tar.gz
# Source1-md5:	615c798ab2ca3b4203a7b0df3187c3d6
Source2:	%{name}.init
Patch0:		%{name}-Makefile.patch
URL:		http://klub.com.pl/dhcpv6/
BuildRequires:	flex
BuildRequires:	libstdc++-devel
BuildRequires:	libxml2-devel
BuildRequires:	pkgconfig
Requires(post,preun):	/sbin/chkconfig
Provides:	dhcpv6-server
Obsoletes:	dhcpv6
BuildRoot:	%{tmpdir}/%{name}-%{version}-root-%(id -u -n)

%description
Dibbler is a portable DHCPv6 implementation. It supports stateful
(i.e. IPv6 address granting) as well as stateless (i.e. option
granting) autoconfiguration for IPv6. Currently Linux 2.4/2.6 and
Windows XP and Windows 2003 ports are available. It features easy to
use install packages (Clickable Windows installer and RPM and DEB
packages for Linux) and extensive documentation (both for users as
well as developers).

%description -l pl
Dibbler to przeno¶na implementacja DHCPv6. Obs³uguje stanow± (tzn. z
nadawaniem adresów IPv6), jak i bezstanow± (tzn. z nadawaniem opcji)
autokonfiguracjê IPv6. Aktualnie dostêpne s± porty dla Linuksa 2.4/2.6
i Windows XP oraz Windows 2003. Zalety to ³atwa instalacja (klikalny
instalator pod Windows i pakiety RPM/DEB pod Linuksa) i wyczerpuj±ca
dokumentacja (zarówno dla u¿ytkowników, jak i programistów).

%package doc
Summary:	Documentation for Dibbler - a portable DHCPv6
Summary(pl):	Dokumentacja dla Dibblera - przeno¶nego DHCPv6
Group:		Documentation

%description doc
Documentation for Dibbler - a portable DHCPv6 (pdf files)

%description doc -l pl
Dokumentacja dla Dibblera - przeno¶nego DHCPv6 (pliki pdf)

%package client
Summary:	Dibbler DHCPv6 client
Summary(pl):	Dibbler - klient DHCPv6
Group:		Networking/Daemons
Provides:	dhcpv6-client

%description client
DHCPv6 protocol client.

%description client -l pl
Klient protoko³u DHCPv6.

%prep
%setup -q -n %{name}
%patch0 -p0

%build
%{__make} server client \
	ARCH=LINUX \
	CFLAGS="%{rpmcflags}" \
	CPP="%{__cpp}" \
	CXX="%{__cxx}" \
	CC="%{__cc}"

%install
rm -rf $RPM_BUILD_ROOT
install -d $RPM_BUILD_ROOT{%{_sbindir},%{_mandir}/man8} \
	$RPM_BUILD_ROOT{%{_sharedstatedir}/%{name},%{_sysconfdir}/{rc.d/init.d,dibbler}}

install dibbler-{client,server} $RPM_BUILD_ROOT%{_sbindir}
install *.conf $RPM_BUILD_ROOT%{_sharedstatedir}/%{name}
install doc/man/* $RPM_BUILD_ROOT%{_mandir}/man8
install %{SOURCE2} $RPM_BUILD_ROOT/etc/rc.d/init.d/dibbler
tar zxf %{SOURCE1} doc/dibbler-user.pdf
tar zxf %{SOURCE1} doc/dibbler-devel.pdf
ln -sf %{_sharedstatedir}/%{name}/server.conf $RPM_BUILD_ROOT%{_sysconfdir}/%{name}/server.conf
ln -sf %{_sharedstatedir}/%{name}/client.conf $RPM_BUILD_ROOT%{_sysconfdir}/%{name}/client.conf

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig
if [ "$1" = "1" ]; then
	/sbin/chkconfig --add dibbler
	if [ -f /var/lock/subsys/dibbler ]; then
        	/etc/rc.d/init.d/dibbler restart 1>&2
	else
        	echo "Run \"/etc/rc.d/init.d/dibbler start\" to start dibbler DHCP daemon."
	fi
fi


%preun
/sbin/ldconfig
if [ "$1" = "0" ];then
	if [ -f /var/lock/subsys/dibbler ]; then
		/etc/rc.d/init.d/dibbler stop >&2
	fi
	/sbin/chkconfig --del dibbler
fi

%files
%defattr(644,root,root,755)
%doc CHANGELOG LICENSE RELNOTES VERSION 
%doc server.conf server-stateless.conf doc/man/dibbler-server.8
%attr(755,root,root) %{_sbindir}/dibbler-server
%attr(754,root,root) /etc/rc.d/init.d/dibbler
%dir %{_sharedstatedir}/%{name}
%config(noreplace) %verify(not md5 mtime size) %{_sharedstatedir}/%{name}/server.conf
%dir %{_sysconfdir}/%{name}
%{_sysconfdir}/%{name}/server.conf
%{_mandir}/man8/*.8*

%files client
%defattr(644,root,root,755)
%doc CHANGELOG LICENSE RELNOTES VERSION
%doc client.conf client-stateless.conf doc/man/dibbler-client.8
%attr(755,root,root) %{_sbindir}/dibbler-client
%dir %{_sharedstatedir}/%{name}
%config(noreplace) %verify(not md5 mtime size) %{_sharedstatedir}/%{name}/client.conf
%dir %{_sysconfdir}/%{name}
%{_sysconfdir}/%{name}/client.conf
%{_mandir}/man8/*.8*

%files doc
%defattr(644,root,root,755)
%doc doc/dibbler-user.pdf doc/dibbler-devel.pdf

%define date	%(echo `LC_ALL="C" date +"%a %b %d %Y"`)
%changelog
* %{date} PLD Team <feedback@pld-linux.org>
All persons listed below can be reached at <cvs_login>@pld-linux.org

$Log: not supported by cvs2svn $
Revision 1.15  2005/01/23 15:22:59  sardzent
- works fine

Revision 1.14  2005/01/23 14:51:53  sardzent
- added doc, todo problem with chkconfig

Revision 1.13  2005/01/22 16:46:28  sardzent
- cosmetics

Revision 1.12  2005/01/22 09:57:41  sardzent
- fixed problems with running as a deamon

Revision 1.11  2005/01/19 12:12:37  sardzent
- needs to be tested
- no doc

Revision 1.10  2005/01/18 13:25:01  sardzent
- modifications in daemon start
- waiting for contact with author
- do not touch :)

Revision 1.9  2005/01/17 20:22:26  sardzent
- cosmetics

Revision 1.8  2005/01/14 09:59:03  qboosh
- persistent symlinks creation belongs to install+files, not post

Revision 1.7  2005/01/14 09:56:10  qboosh
- unified, fixed perms

Revision 1.6  2005/01/14 09:19:58  sardzent
- cosmetics
- needs do be tested

Revision 1.5  2005/01/14 01:29:05  sardzent
- separate cilent
- todo separate doc

Revision 1.4  2005/01/14 01:03:58  sardzent
- deleted comments

Revision 1.3  2005/01/14 00:59:42  sardzent
- needs testing
- what with client?

Revision 1.2  2005/01/03 14:42:16  qboosh
- pl, formatting

Revision 1.1  2005/01/01 23:28:08  radek
- new, nfy, builds (maybe even works)
