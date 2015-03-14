# Copyright 1999-2014 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="5"

SLOT="0"

DESCRIPTION="dio - Device I/O monitoring tool"

HOMEPAGE="https://github.com/donaldmcintosh/dio"

SRC_URI="https://github.com/donaldmcintosh/dio/archive/1.5.1.tar.gz"

LICENSE="MIT"

KEYWORDS="~x86"

DEPEND=">=sys-libs/ncurses-5.10-r2"

RDEPEND="${DEPEND}"

S="${WORKDIR}/${P}/src"

src_install() {
    dobin src/dio 
	doman src/dio.1
}