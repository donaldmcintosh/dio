# Copyright 1999-2014 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="5"

inherit git-r3

SLOT="0"

DESCRIPTION="dio - Device I/O monitoring tool"

HOMEPAGE="https://github.com/donaldmcintosh/dio"

EGIT_REPO_URI="https://github.com/donaldmcintosh/dio.git"

LICENSE="MIT"

KEYWORDS="~x86"

DEPEND=">=sys-libs/ncurses-5.10-r2"

src_compile() {
    cd src
	make
}

src_install() {
    dobin src/dio 
	doman src/dio.1
}
