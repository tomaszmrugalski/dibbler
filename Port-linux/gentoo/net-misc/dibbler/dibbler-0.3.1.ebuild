# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvs/dibbler/Port-linux/gentoo/net-misc/dibbler/dibbler-0.3.1.ebuild,v 1.4 2004-12-29 23:29:56 thomson Exp $

inherit eutils

DESCRIPTION="Portable DHCPv6 implementation (server and client)"
HOMEPAGE="http://klub.com.pl/dhcpv6/"
SRC_URI="http://klub.com.pl/dhcpv6/dibbler-0.3.1-src.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86"
IUSE="latex"

# thomson: latex flag is used to generate User's Guide and Developer's Guide - a nice PDF files.
# LaTeX system is required to generate them.

# thomson 2004-12-29 FIXME: repoman complains about invalid IUSE. latex and tex seem not to be working.
# Is there any other USE tag related to the latex? If not, dibbler-user.pdf should be probably
# ripped from http://klub.com.pl/dhcpv6/dibbler-0.3.1-doc.tar.gz 

DEPEND=""
MAKEOPTS="client server"

S=${WORKDIR}/${PN}

src_unpack() {
	unpack ${A}
	epatch ${FILESDIR}/${P}-security.patch
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
	dodir /etc/dibbler
	dosym /var/lib/dibbler/client.conf /etc/dibbler/client.conf
	dosym /var/lib/dibbler/server.conf /etc/dibbler/server.conf
}

pkg_postinst() {
	einfo "Make sure that you modify client.conf and/or server.conf to suit your needs."
	einfo "They are stored in /var/lib/dibbler along with some examples."
}
