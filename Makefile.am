## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libatl.la

noinst_HEADERS = acconfig.h atom_formats.h tclHash.h unix_defs.h

libatl_la_SOURCES = atom.c atom_formats.c attr.c tclHash.c gen_info.c
libatl_la_LIBADD =  
libatl_la_LDFLAGS =
include_HEADERS = atl.h


bin_PROGRAMS = attr_test atom_test

LIBS = @LIBS@

attr_test_SOURCES = attr_test.c
attr_test_LDADD =  libatl.la
attr_test_LDFLAGS =
attr_test_DEPENDENCIES = libatl.la

atom_test_SOURCES = atom_test.c
atom_test_LDADD =  libatl.la
atom_test_LDFLAGS =
atom_test_DEPENDENCIES = libatl.la

#atom_server_SOURCES = atom_server.c
#atom_server_LDADD =  libatl.la
#atom_server_LDFLAGS =
#atom_server_DEPENDENCIES = libatl.la

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
