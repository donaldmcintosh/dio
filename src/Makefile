# $Id$
# Makefile for dio : device I/O analysis tool
#
# - Makefile written by Ade Rixon, 2001-11-16

# installation directories:
PREFIX= /usr/local
BINDIR= $(PREFIX)/bin
MANDIR= $(PREFIX)/man/man1
INSTALL= /usr/ucb/install
mkinstalldirs= mkdir -p

# compiler, pick one:
#CC= cc
CC= gcc

# compiler options:
#CFLAGS= -O -g
CFLAGS= -O2 -g -Wall

# linker options:
LDFLAGS= #-s	# stripped binary
# required libraries:
LIBS= -lcurses 

all: dio

dio: dio.c dio.h 
	$(CC) $(CFLAGS) $(LDFLAGS) dio.c -o $@ $(LIBS)

install: all
	if [ ! -d "$(BINDIR)" ]; then \
		$(mkinstalldirs) $(BINDIR); \
	fi
	if [ ! -d "$(MANDIR)" ]; then \
		$(mkinstalldirs) $(MANDIR); \
	fi
	$(INSTALL) -m 0755 dio $(BINDIR)
	$(INSTALL) -m 0644 man/man1/dio.1 $(MANDIR)
