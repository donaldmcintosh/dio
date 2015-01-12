# Copyright 1999-2014 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="5"

SLOT="0"

DESCRIPTION="dio - Device I/O monitoring tool"

HOMEPAGE="https://github.com/donaldmcintosh/dio"

SRC_URI="https://github.com/donaldmcintosh/dio/bin/dio-1.5.tar.gz"

LICENSE="MIT"

KEYWORDS="~x86"

DEPEND=">sys-libs/ncurses-5.10-r2"

src_install() {
    dobin dio 
	doman dio.1
}
