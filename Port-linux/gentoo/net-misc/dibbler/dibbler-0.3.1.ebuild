# Copyright 2004 Tomasz Mrugalski
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvs/dibbler/Port-linux/gentoo/net-misc/dibbler/dibbler-0.3.1.ebuild,v 1.1 2004-12-29 00:18:08 thomson Exp $

inherit eutils

DESCRIPTION="Portable DHCPv6 implementation (server and client)"
HOMEPAGE="http://klub.com.pl/dhcpv6/"
SRC_URI="http://klub.com.pl/dhcpv6/dibbler-0.3.1-src.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86"
IUSE="latex"

#USE="latex"
USE="-latex"

DEPEND=""
MAKEOPTS="client server"

S=${WORKDIR}/${PN}

src_unpack() {
	unpack dibbler-0.3.1-src.tar.gz
	cd ${S}
}

src_compile() {
	echo "USE=${USE}"
	use latex && MAKEOPTS="${MAKEOPTS} doc" 
	echo "MAKEOPTS=[${MAKEOPTS}]"
	emake || die "Compilation failed"
}

src_install() {
	dosbin dibbler-server
	dosbin dibbler-client
	doman doc/man/dibbler-server.8 doc/man/dibbler-client.8
	dodoc CHANGELOG RELNOTES 
	use latex && dodoc doc/dibbler-user.pdf doc/dibber-devel.pdf
	dodir /var/lib/dibbler
	insinto /var/lib/dibbler
	doins *.conf
}

pkg_postinst() {
	einfo "Sample dhcp6c.conf and dhcp6s.conf files are in"
	einfo "/usr/share/doc/${P}/"
}
