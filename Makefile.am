## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies foreign

noinst_HEADERS = atom_formats.h tclHash.h unix_defs.h

libatl_la_SOURCES = atom.c attr.c tclHash.c
libatl_la_DEPENDENCIES = @KERNEL_OBJS@
libatl_la_LIBADD = @KERNEL_OBJS@
libatl_la_LDFLAGS = -no-undefined


if KERNEL
bin_PROGRAMS = 
BUILT_SOURCES = strtol.c env.c katl.c
install-exec-local: atl_module.o
		install -d /lib/modules/@KERNEL_VERSION@/misc /lib/modules/misc
		install -c atl_module.o /lib/modules/@KERNEL_VERSION@/misc
		install -c atl_module.o /lib/modules/misc
all-local: atl_module.o
MOSTLYCLEANFILES = atl_module.o
else
lib_LTLIBRARIES = libatl.la
bin_PROGRAMS = attr_test atom_test atom_server
include_HEADERS = atl.h
all-local:
install-exec-local:
endif

LIBS = @LIBS@

atl_module.o:	libatl.la
	rm -fr /tmp/atl_build
	mkdir /tmp/atl_build
	cp .libs/libatl.a /tmp/atl_build
	cd /tmp/atl_build ; ar x libatl.a
	cd /tmp/atl_build ; rm libatl.a
	cd /tmp/atl_build ; ld -r *.o -o $@
	cp /tmp/atl_build/atl_module.o .
	rm -fr /tmp/atl_build 

strtol.c:
	rm -f strtol.c
	$(LN_S) kernel/strtol.c strtol.c

env.c: katl.h
	rm -f env.c
	$(LN_S) kernel/env.c env.c

katl.c: katl.h
	rm -f katl.c
	$(LN_S) kernel/katl.c katl.c

katl.h:
	rm -f katl.h
	$(LN_S) kernel/katl.h katl.h

attr_test_SOURCES = attr_test.c
attr_test_LDADD =  libatl.la
attr_test_LDFLAGS =
attr_test_DEPENDENCIES = libatl.la

atom_test_SOURCES = atom_test.c
atom_test_LDADD =  libatl.la
atom_test_LDFLAGS =
atom_test_DEPENDENCIES = libatl.la

atom_server_SOURCES = atom_server.c
atom_server_LDADD =  libatl.la
atom_server_LDFLAGS =
atom_server_DEPENDENCIES = libatl.la

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
