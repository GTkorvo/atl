dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Like AC_CONFIG_HEADER, but automatically create stamp file.

AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])


# serial 44 AC_PROG_LIBTOOL
AC_DEFUN(AC_PROG_LIBTOOL,[AC_REQUIRE([_AC_PROG_LIBTOOL])])
AC_DEFUN(_AC_PROG_LIBTOOL,
[AC_REQUIRE([AC_LIBTOOL_SETUP])dnl

# Save cache, so that ltconfig can load it
AC_CACHE_SAVE

# Actually configure libtool.  ac_aux_dir is where install-sh is found.
AR="$AR" CC="$CC" CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" \
MAGIC="$MAGIC" LD="$LD" LDFLAGS="$LDFLAGS" LIBS="$LIBS" \
LN_S="$LN_S" NM="$NM" RANLIB="$RANLIB" STRIP="$STRIP" \
AS="$AS" DLLTOOL="$DLLTOOL" OBJDUMP="$OBJDUMP" \
objext="$OBJEXT" exeext="$EXEEXT" reload_flag="$reload_flag" \
deplibs_check_method="$deplibs_check_method" file_magic_cmd="$file_magic_cmd" \
${CONFIG_SHELL-/bin/sh} $ac_aux_dir/ltconfig --no-reexec \
$libtool_flags --no-verify --build="$build" $ac_aux_dir/ltmain.sh $host \
|| AC_MSG_ERROR([libtool configure failed])

# Reload cache, that may have been modified by ltconfig
AC_CACHE_LOAD

# This can be used to rebuild libtool when needed
LIBTOOL_DEPS="$ac_aux_dir/ltconfig $ac_aux_dir/ltmain.sh"

# Always use our own libtool.
LIBTOOL='$(SHELL) $(top_builddir)/libtool'
AC_SUBST(LIBTOOL)dnl

# Redirect the config.log output again, so that the ltconfig log is not
# clobbered by the next message.
exec 5>>./config.log
])

AC_DEFUN(AC_LIBTOOL_SETUP,
[AC_PREREQ(2.13)dnl
AC_REQUIRE([AC_ENABLE_SHARED])dnl
AC_REQUIRE([AC_ENABLE_STATIC])dnl
AC_REQUIRE([AC_ENABLE_FAST_INSTALL])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_LD])dnl
AC_REQUIRE([AC_PROG_LD_RELOAD_FLAG])dnl
AC_REQUIRE([AC_PROG_NM])dnl
AC_REQUIRE([AC_PROG_LN_S])dnl
AC_REQUIRE([AC_DEPLIBS_CHECK_METHOD])dnl
AC_REQUIRE([AC_OBJEXT])dnl
AC_REQUIRE([AC_EXEEXT])dnl
dnl

# Only perform the check for file, if the check method requires it
case "$deplibs_check_method" in
file_magic*)
  if test "$file_magic_cmd" = '${MAGIC}'; then
    AC_PATH_MAGIC
  fi
  ;;
esac

AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_CHECK_TOOL(STRIP, strip, :)

# Check for any special flags to pass to ltconfig.
libtool_flags="--cache-file=$cache_file"
test "$enable_shared" = no && libtool_flags="$libtool_flags --disable-shared"
test "$enable_static" = no && libtool_flags="$libtool_flags --disable-static"
test "$enable_fast_install" = no && libtool_flags="$libtool_flags --disable-fast-install"
test "$ac_cv_prog_gcc" = yes && libtool_flags="$libtool_flags --with-gcc"
test "$ac_cv_prog_gnu_ld" = yes && libtool_flags="$libtool_flags --with-gnu-ld"
ifdef([AC_PROVIDE_AC_LIBTOOL_DLOPEN],
[libtool_flags="$libtool_flags --enable-dlopen"])
ifdef([AC_PROVIDE_AC_LIBTOOL_WIN32_DLL],
[libtool_flags="$libtool_flags --enable-win32-dll"])
AC_ARG_ENABLE(libtool-lock,
  [  --disable-libtool-lock  avoid locking (might break parallel builds)])
test "x$enable_libtool_lock" = xno && libtool_flags="$libtool_flags --disable-lock"
test x"$silent" = xyes && libtool_flags="$libtool_flags --silent"

AC_ARG_WITH(pic,
  [  --with-pic              try to use only PIC/non-PIC objects [default=use both]],
     pic_mode="$withval", pic_mode=default)
test x"$pic_mode" = xyes && libtool_flags="$libtool_flags --prefer-pic"
test x"$pic_mode" = xno && libtool_flags="$libtool_flags --prefer-non-pic"

# Some flags need to be propagated to the compiler or linker for good
# libtool support.
case "$host" in
*-*-irix6*)
  # Find out which ABI we are using.
  echo '[#]line __oline__ "configure"' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case "`/usr/bin/file conftest.o`" in
    *32-bit*)
      LD="${LD-ld} -32"
      ;;
    *N32*)
      LD="${LD-ld} -n32"
      ;;
    *64-bit*)
      LD="${LD-ld} -64"
      ;;
    esac
  fi
  rm -rf conftest*
  ;;

*-*-sco3.2v5*)
  # On SCO OpenServer 5, we need -belf to get full-featured binaries.
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS -belf"
  AC_CACHE_CHECK([whether the C compiler needs -belf], lt_cv_cc_needs_belf,
    [AC_LANG_SAVE
     AC_LANG_C
     AC_TRY_LINK([],[],[lt_cv_cc_needs_belf=yes],[lt_cv_cc_needs_belf=no])
     AC_LANG_RESTORE])
  if test x"$lt_cv_cc_needs_belf" != x"yes"; then
    # this is probably gcc 2.8.0, egcs 1.0 or newer; no need for -belf
    CFLAGS="$SAVE_CFLAGS"
  fi
  ;;

ifdef([AC_PROVIDE_AC_LIBTOOL_WIN32_DLL],
[*-*-cygwin* | *-*-mingw*)
  AC_CHECK_TOOL(DLLTOOL, dlltool, false)
  AC_CHECK_TOOL(AS, as, false)
  AC_CHECK_TOOL(OBJDUMP, objdump, false)

  # recent cygwin and mingw systems supply a stub DllMain which the user
  # can override, but on older systems we have to supply one
  AC_CACHE_CHECK([if libtool should supply DllMain function], lt_cv_need_dllmain,
    [AC_TRY_LINK([],
      [extern int __attribute__((__stdcall__)) DllMain(void*, int, void*);
      DllMain (0, 0, 0);],
      [lt_cv_need_dllmain=no],[lt_cv_need_dllmain=yes])])

  case "$host/$CC" in
  *-*-cygwin*/gcc*-mno-cygwin*|*-*-mingw*)
    # old mingw systems require "-dll" to link a DLL, while more recent ones
    # require "-mdll"
    SAVE_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -mdll"
    AC_CACHE_CHECK([how to link DLLs], lt_cv_cc_dll_switch,
      [AC_TRY_LINK([], [], [lt_cv_cc_dll_switch=-mdll],[lt_cv_cc_dll_switch=-dll])])
    CFLAGS="$SAVE_CFLAGS" ;;
  *-*-cygwin*)
    # cygwin systems need to pass --dll to the linker, and not link
    # crt.o which will require a WinMain@16 definition.
    lt_cv_cc_dll_switch="-Wl,--dll -nostartfiles" ;;
  esac
  ;;
  ])
esac
])

# AC_LIBTOOL_DLOPEN - enable checks for dlopen support
AC_DEFUN(AC_LIBTOOL_DLOPEN, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])])

# AC_LIBTOOL_WIN32_DLL - declare package support for building win32 dll's
AC_DEFUN(AC_LIBTOOL_WIN32_DLL, [AC_BEFORE([$0], [AC_LIBTOOL_SETUP])])

# AC_ENABLE_SHARED - implement the --enable-shared flag
# Usage: AC_ENABLE_SHARED[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_SHARED, [dnl
define([AC_ENABLE_SHARED_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(shared,
changequote(<<, >>)dnl
<<  --enable-shared[=PKGS]  build shared libraries [default=>>AC_ENABLE_SHARED_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_shared=yes ;;
no) enable_shared=no ;;
*)
  enable_shared=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_shared=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_shared=AC_ENABLE_SHARED_DEFAULT)dnl
])

# AC_DISABLE_SHARED - set the default shared flag to --disable-shared
AC_DEFUN(AC_DISABLE_SHARED, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_SHARED(no)])

# AC_ENABLE_STATIC - implement the --enable-static flag
# Usage: AC_ENABLE_STATIC[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_STATIC, [dnl
define([AC_ENABLE_STATIC_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(static,
changequote(<<, >>)dnl
<<  --enable-static[=PKGS]  build static libraries [default=>>AC_ENABLE_STATIC_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_static=yes ;;
no) enable_static=no ;;
*)
  enable_static=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_static=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_static=AC_ENABLE_STATIC_DEFAULT)dnl
])

# AC_DISABLE_STATIC - set the default static flag to --disable-static
AC_DEFUN(AC_DISABLE_STATIC, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_STATIC(no)])


# AC_ENABLE_FAST_INSTALL - implement the --enable-fast-install flag
# Usage: AC_ENABLE_FAST_INSTALL[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_FAST_INSTALL, [dnl
define([AC_ENABLE_FAST_INSTALL_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(fast-install,
changequote(<<, >>)dnl
<<  --enable-fast-install[=PKGS]  optimize for fast installation [default=>>AC_ENABLE_FAST_INSTALL_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_fast_install=yes ;;
no) enable_fast_install=no ;;
*)
  enable_fast_install=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_fast_install=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_fast_install=AC_ENABLE_FAST_INSTALL_DEFAULT)dnl
])

# AC_DISABLE_FAST_INSTALL - set the default to --disable-fast-install
AC_DEFUN(AC_DISABLE_FAST_INSTALL, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_FAST_INSTALL(no)])

# AC_LIBTOOL_PICMODE - implement the --with-pic flag
# Usage: AC_LIBTOOL_PICMODE[(MODE)]
#   Where MODE is either `yes' or `no'.  If omitted, it defaults to
#   `both'.
AC_DEFUN(AC_LIBTOOL_PICMODE, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
pic_mode=ifelse($#,1,$1,default)])


# AC_PATH_TOOL_PREFIX - find a file program which can recognise shared library
AC_DEFUN(AC_PATH_TOOL_PREFIX,
[AC_MSG_CHECKING([for $1])
AC_CACHE_VAL(lt_cv_path_MAGIC,
[case "$MAGIC" in
  /*)
  lt_cv_path_MAGIC="$MAGIC" # Let the user override the test with a path.
  ;;
  ?:/*)
  ac_cv_path_MAGIC="$MAGIC" # Let the user override the test with a dos path.
  ;;
  *)
  ac_save_MAGIC="$MAGIC"
  IFS="${IFS=   }"; ac_save_ifs="$IFS"; IFS=":"
dnl $ac_dummy forces splitting on constant user-supplied paths.
dnl POSIX.2 word splitting is done only on the output of word expansions,
dnl not every word.  This closes a longstanding sh security hole.
  ac_dummy="ifelse([$2], , $PATH, [$2])"
  for ac_dir in $ac_dummy; do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/$1; then
      lt_cv_path_MAGIC="$ac_dir/$1"
      if test -n "$file_magic_test_file"; then
	case "$deplibs_check_method" in
	"file_magic "*)
	  file_magic_regex="`expr \"$deplibs_check_method\" : \"file_magic \(.*\)\"`"
	  MAGIC="$lt_cv_path_MAGIC"
	  if eval $file_magic_cmd \$file_magic_test_file 2> /dev/null |
	    egrep "$file_magic_regex" > /dev/null; then
	    :
	  else
	    cat <<EOF 1>&2

*** Warning: the command libtool uses to detect shared libraries,
*** $file_magic_cmd, produces output that libtool cannot recognize.
*** The result is that libtool may fail to recognize shared libraries
*** as such.  This will affect the creation of libtool libraries that
*** depend on shared libraries, but programs linked with such libtool
*** libraries will work regardless of this problem.  Nevertheless, you
*** may want to report the problem to your system manager and/or to
*** bug-libtool@gnu.org

EOF
	  fi ;;
	esac
      fi
      break
    fi
  done
  IFS="$ac_save_ifs"
  MAGIC="$ac_save_MAGIC"
  ;;
esac])
MAGIC="$lt_cv_path_MAGIC"
if test -n "$MAGIC"; then
  AC_MSG_RESULT($MAGIC)
else
  AC_MSG_RESULT(no)
fi
])


# AC_PATH_MAGIC - find a file program which can recognise a shared library
AC_DEFUN(AC_PATH_MAGIC,
[AC_REQUIRE([AC_CHECK_TOOL_PREFIX])dnl
AC_PATH_TOOL_PREFIX(${ac_tool_prefix}file, /usr/bin:$PATH)
if test -z "$lt_cv_path_MAGIC"; then
  if test -n "$ac_tool_prefix"; then
    AC_PATH_TOOL_PREFIX(file, /usr/bin:$PATH)
  else
    MAGIC=:
  fi
fi
])


# AC_PROG_LD - find the path to the GNU or non-GNU linker
AC_DEFUN(AC_PROG_LD,
[AC_ARG_WITH(gnu-ld,
[  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]],
test "$withval" = no || with_gnu_ld=yes, with_gnu_ld=no)
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
ac_prog=ld
if test "$ac_cv_prog_gcc" = yes; then
  # Check if gcc -print-prog-name=ld gives a path.
  AC_MSG_CHECKING([for ld used by GCC])
  case $host in
  *-*-mingw*)
    # gcc leaves a trailing carriage return which upsets mingw
    ac_prog=`($CC -print-prog-name=ld) 2>&5 | tr -d '\015'` ;;
  *)
    ac_prog=`($CC -print-prog-name=ld) 2>&5` ;;
  esac
  case "$ac_prog" in
    # Accept absolute paths.
changequote(,)dnl
    [\\/]* | [A-Za-z]:[\\/]*)
      re_direlt='/[^/][^/]*/\.\./'
changequote([,])dnl
      # Canonicalize the path of ld
      ac_prog=`echo $ac_prog| sed 's%\\\\%/%g'`
      while echo $ac_prog | grep "$re_direlt" > /dev/null 2>&1; do
	ac_prog=`echo $ac_prog| sed "s%$re_direlt%/%"`
      done
      test -z "$LD" && LD="$ac_prog"
      ;;
  "")
    # If it fails, then pretend we aren't using GCC.
    ac_prog=ld
    ;;
  *)
    # If it is relative, then search for the first ld in PATH.
    with_gnu_ld=unknown
    ;;
  esac
elif test "$with_gnu_ld" = yes; then
  AC_MSG_CHECKING([for GNU ld])
else
  AC_MSG_CHECKING([for non-GNU ld])
fi
AC_CACHE_VAL(ac_cv_path_LD,
[if test -z "$LD"; then
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR-:}"
  for ac_dir in $PATH; do
    test -z "$ac_dir" && ac_dir=.
    if test -f "$ac_dir/$ac_prog" || test -f "$ac_dir/$ac_prog$ac_exeext"; then
      ac_cv_path_LD="$ac_dir/$ac_prog"
      # Check to see if the program is GNU ld.  I'd rather use --version,
      # but apparently some GNU ld's only accept -v.
      # Break only if it was the GNU/non-GNU ld that we prefer.
      if "$ac_cv_path_LD" -v 2>&1 < /dev/null | egrep '(GNU|with BFD)' > /dev/null; then
	test "$with_gnu_ld" != no && break
      else
	test "$with_gnu_ld" != yes && break
      fi
    fi
  done
  IFS="$ac_save_ifs"
else
  ac_cv_path_LD="$LD" # Let the user override the test with a path.
fi])
LD="$ac_cv_path_LD"
if test -n "$LD"; then
  AC_MSG_RESULT($LD)
else
  AC_MSG_RESULT(no)
fi
test -z "$LD" && AC_MSG_ERROR([no acceptable ld found in \$PATH])
AC_PROG_LD_GNU
])

AC_DEFUN(AC_PROG_LD_GNU,
[AC_CACHE_CHECK([if the linker ($LD) is GNU ld], ac_cv_prog_gnu_ld,
[# I'd rather use --version here, but apparently some GNU ld's only accept -v.
if $LD -v 2>&1 </dev/null | egrep '(GNU|with BFD)' 1>&5; then
  ac_cv_prog_gnu_ld=yes
else
  ac_cv_prog_gnu_ld=no
fi])
with_gnu_ld=$ac_cv_prog_gnu_ld
])

# AC_PROG_LD_RELOAD_FLAG - find reload flag for linker
#   -- PORTME Some linkers may need a different reload flag.
AC_DEFUN(AC_PROG_LD_RELOAD_FLAG,
[AC_CACHE_CHECK([for $LD option to reload object files], lt_cv_ld_reload_flag,
[lt_cv_ld_reload_flag='-r'])
reload_flag=$lt_cv_ld_reload_flag
test -n "$reload_flag" && reload_flag=" $reload_flag"
])

# AC_DEPLIBS_CHECK_METHOD - how to check for library dependencies
#  -- PORTME fill in with the dynamic library characteristics
AC_DEFUN(AC_DEPLIBS_CHECK_METHOD,
[AC_CACHE_CHECK([how to recognise dependant libraries],
lt_cv_deplibs_check_method,
[lt_cv_file_magic_cmd='${MAGIC}'
lt_cv_file_magic_test_file=
lt_cv_deplibs_check_method='unknown'
# Need to set the preceding variable on all platforms that support
# interlibrary dependencies.
# 'none' -- dependencies not supported.
# `unknown' -- same as none, but documents that we really don't know.
# 'pass_all' -- all dependencies passed with no checks.
# 'test_compile' -- check by making test program.
# 'file_magic [regex]' -- check by looking for files in library path
# which responds to the $file_magic_cmd with a given egrep regex.
# If you have `file' or equivalent on your system and you're not sure
# whether `pass_all' will *always* work, you probably want this one.

case "$host_os" in
aix4*)
  lt_cv_deplibs_check_method=pass_all
  ;;

beos*)
  lt_cv_deplibs_check_method=pass_all
  ;;

bsdi4*)
  changequote(,)dnl
  lt_cv_deplibs_check_method='file_magic ELF [0-9][0-9]*-bit [ML]SB (shared object|dynamic lib)'
  changequote([, ])dnl
  lt_cv_file_magic_cmd='/usr/bin/file -L'
  lt_cv_file_magic_test_file=/shlib/libc.so
  ;;

cygwin* | mingw*)
  lt_cv_deplibs_check_method='file_magic file format pei*-i386(.*architecture: i386)?'
  lt_cv_file_magic_cmd='${OBJDUMP} -f'
  ;;

freebsd*)
  if echo __ELF__ | $CC -E - | grep __ELF__ > /dev/null; then
    case "$host_cpu" in
    i*86 )
      changequote(,)dnl
      lt_cv_deplibs_check_method=='file_magic OpenBSD/i[3-9]86 demand paged shared library'
      changequote([, ])dnl
      lt_cv_file_magic_cmd=/usr/bin/file
      lt_cv_file_magic_test_file=`echo /usr/lib/libc.so.*`
      ;;
    esac
  else
    lt_cv_deplibs_check_method=pass_all
  fi
  ;;

gnu*)
  lt_cv_deplibs_check_method=pass_all
  ;;

hpux10.20*)
  # TODO:  Does this work for hpux-11 too?
  lt_cv_deplibs_check_method='file_magic (s[0-9][0-9][0-9]|PA-RISC[0-9].[0-9]) shared library'
  lt_cv_file_magic_cmd=/usr/bin/file
  lt_cv_file_magic_test_file=/usr/lib/libc.sl
  ;;

irix5* | irix6*)
  case "$host_os" in
  irix5*)
    # this will be overridden with pass_all, but let us keep it just in case
    lt_cv_deplibs_check_method="file_magic ELF 32-bit MSB dynamic lib MIPS - version 1"
    ;;
  *)
    case "$LD" in
    *-32|*"-32 ") libmagic=32-bit;;
    *-n32|*"-n32 ") libmagic=N32;;
    *-64|*"-64 ") libmagic=64-bit;;
    *) libmagic=never-match;;
    esac
    # this will be overridden with pass_all, but let us keep it just in case
    changequote(,)dnl
    lt_cv_deplibs_check_method="file_magic ELF ${libmagic} MSB mips-[1234] dynamic lib MIPS - version 1"
    changequote([, ])dnl
    ;;
  esac
  lt_cv_file_magic_test_file=`echo /lib${libsuff}/libc.so*`
  lt_cv_deplibs_check_method=pass_all
  ;;

# This must be Linux ELF.
linux-gnu*)
  case "$host_cpu" in
  alpha* | i*86 | powerpc* | sparc* | ia64* )
    lt_cv_deplibs_check_method=pass_all ;;
  *)
    # glibc up to 2.1.1 does not perform some relocations on ARM
    changequote(,)dnl
    lt_cv_deplibs_check_method='file_magic ELF [0-9][0-9]*-bit [LM]SB (shared object|dynamic lib )' ;;
    changequote([, ])dnl
  esac
  lt_cv_file_magic_test_file=`echo /lib/libc.so* /lib/libc-*.so`
  ;;

netbsd*)
  if echo __ELF__ | $CC -E - | grep __ELF__ > /dev/null; then :
  else
    changequote(,)dnl
    lt_cv_deplibs_check_method='file_magic ELF [0-9][0-9]*-bit [LM]SB shared object'
    changequote([, ])dnl
    lt_cv_file_magic_cmd='/usr/bin/file -L'
    lt_cv_file_magic_test_file=`echo /usr/lib/libc.so*`
  fi
  ;;

osf3* | osf4* | osf5*)
  # this will be overridden with pass_all, but let us keep it just in case
  lt_cv_deplibs_check_method='file_magic COFF format alpha shared library'
  lt_cv_file_magic_test_file=/shlib/libc.so
  lt_cv_deplibs_check_method=pass_all
  ;;

sco3.2v5*)
  lt_cv_deplibs_check_method=pass_all
  ;;

solaris*)
  lt_cv_deplibs_check_method=pass_all
  lt_cv_file_magic_test_file=/lib/libc.so
  ;;

sysv4 | sysv4.2uw2* | sysv4.3* | sysv5*)
  case "$host_vendor" in
  ncr)
    lt_cv_deplibs_check_method=pass_all
    ;;
  motorola)
    changequote(,)dnl
    lt_cv_deplibs_check_method='file_magic ELF [0-9][0-9]*-bit [ML]SB (shared object|dynamic lib) M[0-9][0-9]* Version [0-9]'
    changequote([, ])dnl
    lt_cv_file_magic_test_file=`echo /usr/lib/libc.so*`
    ;;
  esac
  ;;
esac
])
file_magic_cmd=$lt_cv_file_magic_cmd
deplibs_check_method=$lt_cv_deplibs_check_method
])


# AC_PROG_NM - find the path to a BSD-compatible name lister
AC_DEFUN(AC_PROG_NM,
[AC_MSG_CHECKING([for BSD-compatible nm])
AC_CACHE_VAL(ac_cv_path_NM,
[if test -n "$NM"; then
  # Let the user override the test.
  ac_cv_path_NM="$NM"
else
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR-:}"
  for ac_dir in $PATH /usr/ccs/bin /usr/ucb /bin; do
    test -z "$ac_dir" && ac_dir=.
    tmp_nm=$ac_dir/${ac_tool_prefix}nm
    if test -f $tmp_nm || test -f $tmp_nm$ac_exeext ; then
      # Check to see if the nm accepts a BSD-compat flag.
      # Adding the `sed 1q' prevents false positives on HP-UX, which says:
      #   nm: unknown option "B" ignored
      if ($tmp_nm -B /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	ac_cv_path_NM="$tmp_nm -B"
	break
      elif ($tmp_nm -p /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	ac_cv_path_NM="$tmp_nm -p"
	break
      else
	ac_cv_path_NM=${ac_cv_path_NM="$tmp_nm"} # keep the first match, but
	continue # so that we can try to find one that supports BSD flags
      fi
    fi
  done
  IFS="$ac_save_ifs"
  test -z "$ac_cv_path_NM" && ac_cv_path_NM=nm
fi])
NM="$ac_cv_path_NM"
AC_MSG_RESULT([$NM])
])

# AC_CHECK_LIBM - check for math library
AC_DEFUN(AC_CHECK_LIBM,
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
LIBM=
case "$host" in
*-*-beos* | *-*-cygwin*)
  # These system don't have libm
  ;;
*-ncr-sysv4.3*)
  AC_CHECK_LIB(mw, _mwvalidcheckl, LIBM="-lmw")
  AC_CHECK_LIB(m, main, LIBM="$LIBM -lm")
  ;;
*)
  AC_CHECK_LIB(m, main, LIBM="-lm")
  ;;
esac
])

# AC_LIBLTDL_CONVENIENCE[(dir)] - sets LIBLTDL to the link flags for
# the libltdl convenience library and INCLTDL to the include flags for
# the libltdl header and adds --enable-ltdl-convenience to the
# configure arguments.  Note that LIBLTDL and INCLTDL are not
# AC_SUBSTed, nor is AC_CONFIG_SUBDIRS called.  If DIR is not
# provided, it is assumed to be `libltdl'.  LIBLTDL will be prefixed
# with '${top_builddir}/' and INCLTDL will be prefixed with
# '${top_srcdir}/' (note the single quotes!).  If your package is not
# flat and you're not using automake, define top_builddir and
# top_srcdir appropriately in the Makefiles.
AC_DEFUN(AC_LIBLTDL_CONVENIENCE, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  case "$enable_ltdl_convenience" in
  no) AC_MSG_ERROR([this package needs a convenience libltdl]) ;;
  "") enable_ltdl_convenience=yes
      ac_configure_args="$ac_configure_args --enable-ltdl-convenience" ;;
  esac
  LIBLTDL='${top_builddir}/'ifelse($#,1,[$1],['libltdl'])/libltdlc.la
  INCLTDL='-I${top_srcdir}/'ifelse($#,1,[$1],['libltdl'])
])

# AC_LIBLTDL_INSTALLABLE[(dir)] - sets LIBLTDL to the link flags for
# the libltdl installable library and INCLTDL to the include flags for
# the libltdl header and adds --enable-ltdl-install to the configure
# arguments.  Note that LIBLTDL and INCLTDL are not AC_SUBSTed, nor is
# AC_CONFIG_SUBDIRS called.  If DIR is not provided and an installed
# libltdl is not found, it is assumed to be `libltdl'.  LIBLTDL will
# be prefixed with '${top_builddir}/' and INCLTDL will be prefixed
# with '${top_srcdir}/' (note the single quotes!).  If your package is
# not flat and you're not using automake, define top_builddir and
# top_srcdir appropriately in the Makefiles.
# In the future, this macro may have to be called after AC_PROG_LIBTOOL.
AC_DEFUN(AC_LIBLTDL_INSTALLABLE, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  AC_CHECK_LIB(ltdl, main,
  [test x"$enable_ltdl_install" != xyes && enable_ltdl_install=no],
  [if test x"$enable_ltdl_install" = xno; then
     AC_MSG_WARN([libltdl not installed, but installation disabled])
   else
     enable_ltdl_install=yes
   fi
  ])
  if test x"$enable_ltdl_install" = x"yes"; then
    ac_configure_args="$ac_configure_args --enable-ltdl-install"
    LIBLTDL='${top_builddir}/'ifelse($#,1,[$1],['libltdl'])/libltdl.la
    INCLTDL='-I${top_srcdir}/'ifelse($#,1,[$1],['libltdl'])
  else
    ac_configure_args="$ac_configure_args --enable-ltdl-install=no"
    LIBLTDL="-lltdl"
    INCLTDL=
  fi
])

dnl old names
AC_DEFUN(AM_PROG_LIBTOOL, [indir([AC_PROG_LIBTOOL])])dnl
AC_DEFUN(AM_ENABLE_SHARED, [indir([AC_ENABLE_SHARED], $@)])dnl
AC_DEFUN(AM_ENABLE_STATIC, [indir([AC_ENABLE_STATIC], $@)])dnl
AC_DEFUN(AM_DISABLE_SHARED, [indir([AC_DISABLE_SHARED], $@)])dnl
AC_DEFUN(AM_DISABLE_STATIC, [indir([AC_DISABLE_STATIC], $@)])dnl
AC_DEFUN(AM_PROG_LD, [indir([AC_PROG_LD])])dnl
AC_DEFUN(AM_PROG_NM, [indir([AC_PROG_NM])])dnl

dnl This is just to silence aclocal about the macro not being used
ifelse([AC_DISABLE_FAST_INSTALL])dnl

dnl
dnl ac_require_hppcel_package(package, include_file, library_file)
dnl   either include file or library_file may be left off if not needed
dnl   this macro searches for the files using find_hppcel_package().  If
dnl   found, it adds directory in which the found file resides to either 
dnl   CPPFLAGS (with a -I prefix)  or LDFLAGS (with a -L prefix)
dnl
AC_DEFUN(AC_REQUIRE_HPPCEL_PACKAGE,
[
AC_REQUIRE([AC_HPPCEL_SET_ARCHIVE])
AC_ARG_WITH(installed, [  --with-installed        Don't use local copies of HPPCEL packages],with_installed_specified=1)
AC_ARG_WITH(translit($1, `/',`_'), translit([  --with-$1=DIR	Where to find $1 package], `/',`_'))
define([with_translit], translit(with_$1, `/',`_'))
if test -n "$with_translit"; then
dnl
dnl if they did a with, kill the cache variables
dnl
translit(unset ac_cv_$1_include_arg,  `/',`_')
translit(unset ac_cv_$1_link_arg,  `/',`_')
if test `echo $with_translit | sed 's/\(.\).*/\1/g'` != "/"; then
with_translit=`pwd`/$with_translit
fi
fi
ifelse([$2], , ,
dnl
dnl  if arg2 (include_file) is specified
dnl
AC_MSG_CHECKING(needed include args for $1 package)
AC_CACHE_VAL(translit(ac_cv_$1_include_arg, `/',`_'), 
[AC_FIND_HPPCEL_PACKAGE($1, $2, hppcel_tmp, $with_translit)
if test -n "$hppcel_tmp"; then
translit(ac_cv_$1_include_arg, `/',`_')=-I`echo $hppcel_tmp | sed "s/.$2//g"`
fi
])
AC_MSG_RESULT(translit($ac_cv_$1_include_arg, `/',`_'))
dnl
dnl  add the result to CPPFLAGS if it is absent
dnl
translit(if test -n "$ac_cv_$1_include_arg"; then, `/',`_')
quoted_arg=`echo translit($ac_cv_$1_include_arg, `/',`_') | sed 's/\//\\\\\\//g'`
[if test -z ""`echo $quoted_arg | grep $1`; then
CPPFLAGS=`echo $CPPFLAGS | sed "/$quoted_arg$/q;/$quoted_arg /q;s/$/ $quoted_arg/g"`
else
CPPFLAGS=`echo $CPPFLAGS | sed "/$quoted_arg$/q;/$quoted_arg /q;s/^/ $quoted_arg /g"`
fi]
fi
)
ifelse([$3], , ,
dnl
dnl  if arg3 (library_file) is specified
dnl
AC_MSG_CHECKING(needed link args for $1 package)
AC_CACHE_VAL(translit(ac_cv_$1_link_dir,  `/',`_'), 
[AC_FIND_HPPCEL_PACKAGE($1, $3, hppcel_tmp, $with_translit)
if test -n "$hppcel_tmp" -a "$hppcel_tmp" != "$3"; then
translit(ac_cv_$1_link_dir, `/',`_')=`echo $hppcel_tmp | sed "s/.$3//g"`
else
translit(ac_cv_$1_link_dir="",  `/',`_')
fi
])
AC_MSG_RESULT(translit($ac_cv_$1_link_dir, `/',`_'))
translit(ac_cv_$1_link_arg=`echo $ac_cv_$1_link_dir, `/',`_') | sed '/./ s/^/-L/1'`
dnl
dnl  add the result to LDFLAGS if it is absent
dnl
translit(if test -n "$ac_cv_$1_link_arg"; then, `/',`_')
quoted_arg=`echo translit($ac_cv_$1_link_arg, `/',`_') | sed 's/\//\\\\\\//g'`
[if test -z ""`echo $quoted_arg | grep $1`; then
LDFLAGS=`echo $LDFLAGS | sed "/$quoted_arg$/q;/$quoted_arg /q;s/$/ $quoted_arg/g"`
else
LDFLAGS=`echo $LDFLAGS | sed "/$quoted_arg$/q;/$quoted_arg /q;s/^/$quoted_arg /g"`
fi]
fi
)
])dnl
dnl
dnl ac_find_hppcel_package(package, file_to_find, variable_to_set, suggestion)
dnl    search a set of standard directories to find file_to_find.  When found,
dnl    set $variable_to_set to the path of the file.  Use package and
dnl    suggestions to help search.  Mostly this searches ~chaos (if it exists)
dnl    ~parallel (if it exists) and the usual suspects like:
dnl    /usr/{include,lib} /usr/local/{include,lib} /opt/<package>/{include,lib}
dnl    /opt/misc/{include,lib}.
dnl
AC_DEFUN(AC_FIND_HPPCEL_PACKAGE,
[
AC_REQUIRE([AC_HPPCEL_SET_ARCHIVE])
$3=""
search_list="$2"
CHAOS_HOMEDIR=`echo "echo ~chaos" | csh -sf  2>/dev/null` || CHAOS_HOMEDIR=""
PARALLEL_HOMEDIR=`echo "echo ~parallel" | csh -sf  2>/dev/null` || PARALLEL_HOMEDIR=""
if test -n "$4"; then
if test `echo $4 | cut -c1` = "~"; then
EXPANDED=`echo "echo $4" | csh -sf 2>/dev/null` || EXPANDED=""
else
EXPANDED=$4
fi

search_list="$search_list $EXPANDED/$2 $EXPANDED/include/$2 $EXPANDED/share/$2 $EXPANDED/include/$hppcel_cv_archive/$2 $EXPANDED/lib/$hppcel_cv_archive/$2 $EXPANDED/lib/$2 $EXPANDED/$1/$2 $EXPANDED/$1/$hppcel_cv_archive/$2 $EXPANDED/$1/include/$2 $EXPANDED/$1/lib/$hppcel_cv_archive/$2"
fi
if test -z "$with_installed_specified"; then
search_list="$search_list `pwd`/../$1/$2 `pwd`/../lib/$2 `pwd`/../include/$2 `pwd`/../share/$2 $HOME/$1/$2 $HOME/lib/$hppcel_cv_archive/$2 $HOME/include/$hppcel_cv_archive/$2 $HOME/include/$2 $HOME/share/$2"
fi
if test -n "$CHAOS_HOMEDIR" -a -n "$hppcel_cv_archive"; then
search_list="$search_list $CHAOS_HOMEDIR/include/$2 $CHAOS_HOMEDIR/share/$2 $CHAOS_HOMEDIR/include/$hppcel_cv_archive/$2 $CHAOS_HOMEDIR/lib/$hppcel_cv_archive/$2 $CHAOS_HOMEDIR/lib/$2"
fi
if test -n "$PARALLEL_HOMEDIR" -a -n "$hppcel_cv_archive"; then
search_list="$search_list $PARALLEL_HOMEDIR/include/$2 $PARALLEL_HOMEDIR/share/$2 $PARALLEL_HOMEDIR/include/$hppcel_cv_archive/$2 $PARALLEL_HOMEDIR/lib/$hppcel_cv_archive/$2 $PARALLEL_HOMEDIR/lib/$2"
fi
search_list="$search_list /usr/lib/$2 /usr/local/lib/$2 /usr/include/$2 /usr/share/$2 /opt/$1/lib/$2 /opt/$1/include/$2 /opt/misc/lib/$2 /opt/misc/include/$2 /opt/misc/share/$2"
AC_SEARCH($search_list)
if test -n "$tmp_search_results"; then
$3=$tmp_search_results
fi
])dnl
dnl
dnl AC_HPPCEL_SET_ARCHIVE()
dnl   set the $hppcel_cv_machine_target variable to a standard archive name
dnl
AC_DEFUN(AC_HPPCEL_SET_ARCHIVE,[
CHAOS_HOMEDIR=`echo "echo ~chaos" | csh -sf  2>/dev/null` || CHAOS_HOMEDIR=""
PARALLEL_HOMEDIR=`echo "echo ~parallel" | csh -sf  2>/dev/null` || PARALLEL_HOMEDIR=""
if test "$cross_compiling" = yes ; then
  cpu=$host_cpu
  vendor=$host_vendot
  os=$host_os
else
  cpu=
  vendor=
  os=
fi
if test -x $CHAOS_HOMEDIR/bin/hppcel_arch; then
hppcel_cv_archive=`$CHAOS_HOMEDIR/bin/hppcel_arch "$cpu" "$vendor" "$os"`
elif test -x $PARALLEL_HOMEDIR/bin/hppcel_arch; then
hppcel_cv_archive=`$PARALLEL_HOMEDIR/bin/hppcel_arch "$cpu" "$vendor" "$os"`
else
hppcel_cv_archive=""
fi
])dnl
dnl
dnl  AC_SEARCH(variable to define, options to try)
define(AC_SEARCH,
[tmp_search_results=""
for tmp_search_value in $1; do 
   if test -r $tmp_search_value; then 
	tmp_search_results=$tmp_search_value
	break
   fi 
done
])dnl
dnl
dnl
dnl AC_HPPCEL_LIB_PREFIX
dnl  this macro tries to set a reasonable default for the prefix value
dnl  call with two arguments, project name and library name
dnl
AC_DEFUN(AC_HPPCEL_LIB_PREFIX,
[if test "x$prefix" = xNONE; then
AC_REQUIRE([AC_HPPCEL_SET_ARCHIVE])
search_list=""
CHAOS_HOMEDIR=`echo "echo ~chaos" | csh -sf  2>/dev/null` || CHAOS_HOMEDIR=""
PARALLEL_HOMEDIR=`echo "echo ~parallel" | csh -sf  2>/dev/null` || PARALLEL_HOMEDIR=""
if test -n "$CHAOS_HOMEDIR"; then
search_list="$search_list $CHAOS_HOMEDIR/$hppcel_cv_archive/lib/$2 $CHAOS_HOMEDIR/lib/$2"
fi
if test -n "$PARALLEL_HOMEDIR"; then
search_list="$search_list $PARALLEL_HOMEDIR/$hppcel_cv_archive/lib/$2 $PARALLEL_HOMEDIR/lib/$2"
fi
search_list="$search_list /usr/lib/$2 /usr/local/lib/$2 /opt/$1/lib/$2 /opt/misc/lib/$2"
AC_SEARCH($search_list)
if test -n "$tmp_search_results"; then
    prefix=`echo $tmp_search_results|sed "s%$hppcel_cv_archive/lib/$2%%g;s%lib/$2%%g;s%/[^/][^/]*//*[^/][^/]*$%%"`
    exec_prefix=`echo $tmp_search_results|sed 's%lib/$2%%g;s%/[^/][^/]*//*[^/][^/]*$%%'`
fi
fi
])dnl

AC_DEFUN(AC_FPRINTF_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares fprintf)
AC_CACHE_VAL(ac_cv_fprintf_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]fprintf[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_fprintf_found=yes, ac_cv_fprintf_found=no)
])
AC_MSG_RESULT($ac_cv_fprintf_found)
if test $ac_cv_fprintf_found = yes; then
   AC_DEFINE(FPRINTF_DEFINED,1, [define if stdio.h declares fprintf])
fi
])dnl
AC_DEFUN(AC_ALARM_DEFINED,
[AC_MSG_CHECKING(whether unistd.h declares alarm)
AC_CACHE_VAL(ac_cv_alarm_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]alarm[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_alarm_found=yes, ac_cv_alarm_found=no)
])
AC_MSG_RESULT($ac_cv_alarm_found)
if test $ac_cv_alarm_found = yes; then
   AC_DEFINE(ALARM_DEFINED,1,[define if unistd.h declares alarm])
fi
])dnl
AC_DEFUN(AC_PUTENV_DEFINED,
[AC_MSG_CHECKING(whether unistd.h or stdlib.h declares putenv)
AC_CACHE_VAL(ac_cv_putenv_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]putenv[^a-zA-Z_]>>changequote([, ])dnl
, stdlib.h, ac_cv_putenv_found=yes, ac_cv_putenv_found=no)
if test $ac_cv_putenv_found != yes; then
AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]putenv[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_putenv_found=yes, ac_cv_putenv_found=no)
fi
])
AC_MSG_RESULT($ac_cv_putenv_found)
if test $ac_cv_putenv_found = yes; then
   AC_DEFINE(PUTENV_DEFINED,1,[define if unistd.h or stdlib.h declares putenv])
fi
])dnl
dnl
AC_DEFUN(AC_FSCANF_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares fscanf)
AC_CACHE_VAL(ac_cv_fscanf_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]fscanf[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_fscanf_found=yes, ac_cv_fscanf_found=no)
])
AC_MSG_RESULT($ac_cv_fscanf_found)
if test $ac_cv_fscanf_found = yes; then
   AC_DEFINE(FSCANF_DEFINED,1, [define if stdio.h declares fprintf])
fi
])dnl
AC_DEFUN(AC_PRINTF_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares printf)
AC_CACHE_VAL(ac_cv_printf_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]printf[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_printf_found=yes, ac_cv_printf_found=no)
])
AC_MSG_RESULT($ac_cv_printf_found)
if test $ac_cv_printf_found = yes; then
   AC_DEFINE(PRINTF_DEFINED,1,[define if stdio.h declares printf])
fi
])dnl
AC_DEFUN(AC_WRITE_DEFINED,
[AC_MSG_CHECKING(whether unistd.h declares write)
AC_CACHE_VAL(ac_cv_write_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]write[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_write_found=yes, ac_cv_write_found=no)
])
AC_MSG_RESULT($ac_cv_write_found)
if test $ac_cv_write_found = yes; then
   AC_DEFINE(WRITE_DEFINED,1,[define if unistd.h declares write])
fi
])dnl
AC_DEFUN(AC_WRITEV_DEFINED,
[AC_MSG_CHECKING(whether unistd.h or sys/uio.h declares writev)
AC_CACHE_VAL(ac_cv_writev_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]writev[^a-zA-Z_]>>changequote([, ])dnl
, sys/uio.h, ac_cv_writev_found=yes, ac_cv_writev_found=no)
if test $ac_cv_writev_found != yes; then
AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]writev[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_writev_found=yes, ac_cv_writev_found=no)
fi
])
AC_MSG_RESULT($ac_cv_writev_found)
if test $ac_cv_writev_found = yes; then
   AC_DEFINE(WRITEV_DEFINED,1,[define if unistd.h or sys/uio.h declares writev])
fi
])dnl
AC_DEFUN(AC_READ_DEFINED,
[AC_MSG_CHECKING(whether unistd.h declares read)
AC_CACHE_VAL(ac_cv_read_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]read[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_read_found=yes, ac_cv_read_found=no)
])
AC_MSG_RESULT($ac_cv_read_found)
if test $ac_cv_read_found = yes; then
   AC_DEFINE(READ_DEFINED,1,[define if unistd.h declares read])
fi
])dnl
AC_DEFUN(AC_READV_DEFINED,
[AC_MSG_CHECKING(whether unistd.h or sys/uio.h declares readv)
AC_CACHE_VAL(ac_cv_readv_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]readv[^a-zA-Z_]>>changequote([, ])dnl
, sys/uio.h, ac_cv_readv_found=yes, ac_cv_readv_found=no)
if test $ac_cv_readv_found != yes; then
AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]readv[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_readv_found=yes, ac_cv_readv_found=no)
fi
])
AC_MSG_RESULT($ac_cv_readv_found)
if test $ac_cv_readv_found = yes; then
   AC_DEFINE(READV_DEFINED,1,[define if unistd.h or sys/uio.h declares readv])
fi
])dnl
dnl
])dnl
AC_DEFUN(AC_SSCANF_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares sscanf)
AC_CACHE_VAL(ac_cv_sscanf_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]sscanf[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_sscanf_found=yes, ac_cv_sscanf_found=no)
])
AC_MSG_RESULT($ac_cv_sscanf_found)
if test $ac_cv_sscanf_found = yes; then
   AC_DEFINE(SSCANF_DEFINED,1,[define if stdio.h declares sscanf])
fi
])dnl
AC_DEFUN(AC_SPRINTF_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares sprintf)
AC_CACHE_VAL(ac_cv_sprintf_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]sprintf[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_sprintf_found=yes, ac_cv_sprintf_found=no)
])
AC_MSG_RESULT($ac_cv_sprintf_found)
if test $ac_cv_sprintf_found = yes; then
   AC_DEFINE(SPRINTF_DEFINED,1,[define if sprintf is declared in stdio.h])
fi
])dnl
AC_DEFUN(AC_STRDUP_DEFINED,
[AC_MSG_CHECKING(whether string.h declares strdup)
AC_CACHE_VAL(ac_cv_strdup_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]strdup[^a-zA-Z_]>>changequote([, ])dnl
, string.h, ac_cv_strdup_found=yes, ac_cv_strdup_found=no)
])
AC_MSG_RESULT($ac_cv_strdup_found)
if test $ac_cv_strdup_found = yes; then
   AC_DEFINE(STRDUP_DEFINED,1,[define if string.h declares strdup])
fi
])dnl
AC_DEFUN(AC_BZERO_DEFINED,
[AC_MSG_CHECKING(whether string.h declares bzero)
AC_CACHE_VAL(ac_cv_bzero_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]bzero[^a-zA-Z_]>>changequote([, ])dnl
, string.h, ac_cv_bzero_found=yes, ac_cv_bzero_found=no)
])
AC_MSG_RESULT($ac_cv_bzero_found)
if test $ac_cv_bzero_found = yes; then
   AC_DEFINE(BZERO_DEFINED,1,[define if string.h declares bzero])
fi
])dnl
AC_DEFUN(AC_BCOPY_DEFINED,
[AC_MSG_CHECKING(whether string.h declares bcopy)
AC_CACHE_VAL(ac_cv_bcopy_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]bcopy[^a-zA-Z_]>>changequote([, ])dnl
, string.h, ac_cv_bcopy_found=yes, ac_cv_bcopy_found=no)
])
AC_MSG_RESULT($ac_cv_bcopy_found)
if test $ac_cv_bcopy_found = yes; then
   AC_DEFINE(BCOPY_DEFINED,1,[define if string.h declares bcopy])
fi
])dnl
dnl
AC_DEFUN(AC_GETDOMAINNAME_DEFINED,
[AC_MSG_CHECKING(whether unistd.h declares getdomainname)
AC_CACHE_VAL(ac_cv_getdomainname_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]getdomainname[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_getdomainname_found=yes, ac_cv_getdomainname_found=no)
])
AC_MSG_RESULT($ac_cv_getdomainname_found)
if test $ac_cv_getdomainname_found = yes; then
   AC_DEFINE(GETDOMAINNAME_DEFINED,1,[define if unistd.h declares getdomainname])
fi
])dnl
dnl
AC_DEFUN(AC_GETHOSTNAME_DEFINED,
[AC_MSG_CHECKING(whether unistd.h declares gethostname)
AC_CACHE_VAL(ac_cv_gethostname_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]gethostname[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_gethostname_found=yes, ac_cv_gethostname_found=no)
])
AC_MSG_RESULT($ac_cv_gethostname_found)
if test $ac_cv_gethostname_found = yes; then
   AC_DEFINE(GETHOSTNAME_DEFINED,1,[define if unistd.h declares gethostname])
fi
])dnl
dnl
AC_DEFUN(AC_GETPEERNAME_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares getpeername)
AC_CACHE_VAL(ac_cv_getpeername_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]getpeername[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_getpeername_found=yes, ac_cv_getpeername_found=no)
])
AC_MSG_RESULT($ac_cv_getpeername_found)
if test $ac_cv_getpeername_found = yes; then
   AC_DEFINE(GETPEERNAME_DEFINED,1,[define if sys/socket.h declares getpeername])
fi
])dnl
AC_DEFUN(AC_GETTIMEOFDAY_DEFINED,
[AC_MSG_CHECKING(whether sys/time.h declares gettimeofday)
AC_CACHE_VAL(ac_cv_gettimeofday_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]gettimeofday[^a-zA-Z_]>>changequote([, ])dnl
, sys/time.h, ac_cv_gettimeofday_found=yes, ac_cv_gettimeofday_found=no)
])
AC_MSG_RESULT($ac_cv_gettimeofday_found)
if test $ac_cv_gettimeofday_found = yes; then
   AC_DEFINE(GETTIMEOFDAY_DEFINED,1,[define if sys/time.h declares gettimeofday])
fi
])dnl
AC_DEFUN(AC_ON_EXIT_DEFINED,
[AC_MSG_CHECKING(whether stdlib.h declares on_exit)
AC_CACHE_VAL(ac_cv_on_exit_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]on_exit[^a-zA-Z_]>>changequote([, ])dnl
, stdlib.h, ac_cv_on_exit_found=yes, ac_cv_on_exit_found=no)
])
AC_MSG_RESULT($ac_cv_on_exit_found)
if test $ac_cv_on_exit_found = yes; then
   AC_DEFINE(ON_EXIT_DEFINED,1,[define if stdlib.h declares on_exit])
fi
])
dnl
dnl
AC_DEFUN(AC_RECV_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares recv)
AC_CACHE_VAL(ac_cv_recv_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]recv[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_recv_found=yes, ac_cv_recv_found=no)
])
AC_MSG_RESULT($ac_cv_recv_found)
if test $ac_cv_recv_found = yes; then
   AC_DEFINE(RECV_DEFINED,1,[define if sys/socket.h declares recv])
fi
])dnl
dnl
AC_DEFUN(AC_RECVFROM_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares recvfrom)
AC_CACHE_VAL(ac_cv_recvfrom_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]recvfrom[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_recvfrom_found=yes, ac_cv_recvfrom_found=no)
])
AC_MSG_RESULT($ac_cv_recvfrom_found)
if test $ac_cv_recvfrom_found = yes; then
   AC_DEFINE(RECVFROM_DEFINED,1,[define if sys/socket.h declares recvfrom])
fi
])dnl
dnl
AC_DEFUN(AC_SELECT_DEFINED,
[AC_MSG_CHECKING(whether unistd.h or sys/select.h declares select)
AC_CACHE_VAL(ac_cv_select_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]select[^a-zA-Z_]>>changequote([, ])dnl
, sys/select.h, ac_cv_select_found=yes, ac_cv_select_found=no)
if test $ac_cv_select_found != yes; then
AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]select[^a-zA-Z_]>>changequote([, ])dnl
, unistd.h, ac_cv_select_found=yes, ac_cv_select_found=no)
fi
])
AC_MSG_RESULT($ac_cv_select_found)
if test $ac_cv_select_found = yes; then
   AC_DEFINE(SELECT_DEFINED,1,[define if unistd.h or sys/select.h declares select])
fi
])dnl
dnl
dnl
AC_DEFUN(AC_SEND_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares send)
AC_CACHE_VAL(ac_cv_send_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]send[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_send_found=yes, ac_cv_send_found=no)
])
AC_MSG_RESULT($ac_cv_send_found)
if test $ac_cv_send_found = yes; then
   AC_DEFINE(SEND_DEFINED,1,[define if sys/socket.h declares send])
fi
])dnl
dnl
AC_DEFUN(AC_SENDTO_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares sendto)
AC_CACHE_VAL(ac_cv_sendto_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]sendto[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_sendto_found=yes, ac_cv_sendto_found=no)
])
AC_MSG_RESULT($ac_cv_sendto_found)
if test $ac_cv_sendto_found = yes; then
   AC_DEFINE(SENDTO_DEFINED,1,[define if sys/socket.h declares sendto])
fi
])dnl
dnl
AC_DEFUN(AC_ACCEPT_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares accept)
AC_CACHE_VAL(ac_cv_accept_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]accept[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_accept_found=yes, ac_cv_accept_found=no)
])
AC_MSG_RESULT($ac_cv_accept_found)
if test $ac_cv_accept_found = yes; then
   AC_DEFINE(ACCEPT_DEFINED,1,[define if sys/socket.h declares accept])
fi
])dnl
dnl
AC_DEFUN(AC_CONNECT_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares connect)
AC_CACHE_VAL(ac_cv_connect_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]connect[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_connect_found=yes, ac_cv_connect_found=no)
])
AC_MSG_RESULT($ac_cv_connect_found)
if test $ac_cv_connect_found = yes; then
   AC_DEFINE(CONNECT_DEFINED,1,[define if sys/socket.h declares connect])
fi
])dnl
dnl
AC_DEFUN(AC_SOCKET_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares socket)
AC_CACHE_VAL(ac_cv_socket_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]socket[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_socket_found=yes, ac_cv_socket_found=no)
])
AC_MSG_RESULT($ac_cv_socket_found)
if test $ac_cv_socket_found = yes; then
   AC_DEFINE(SOCKET_DEFINED,1,[define if sys/socket.h declares socket])
fi
])dnl
dnl
AC_DEFUN(AC_SETSOCKOPT_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares setsockopt)
AC_CACHE_VAL(ac_cv_setsockopt_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]setsockopt[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_setsockopt_found=yes, ac_cv_setsockopt_found=no)
])
AC_MSG_RESULT($ac_cv_setsockopt_found)
if test $ac_cv_setsockopt_found = yes; then
   AC_DEFINE(SETSOCKOPT_DEFINED,1,[define if sys/socket.h declares setsockopt])
fi
])dnl
dnl
AC_DEFUN(AC_GETSOCKNAME_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares getsockname)
AC_CACHE_VAL(ac_cv_getsockname_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]getsockname[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_getsockname_found=yes, ac_cv_getsockname_found=no)
])
AC_MSG_RESULT($ac_cv_getsockname_found)
if test $ac_cv_getsockname_found = yes; then
   AC_DEFINE(GETSOCKNAME_DEFINED,1,[define if sys/socket.h declares getsockname])
fi
])dnl
dnl
AC_DEFUN(AC_BIND_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares bind)
AC_CACHE_VAL(ac_cv_bind_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]bind[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_bind_found=yes, ac_cv_bind_found=no)
])
AC_MSG_RESULT($ac_cv_bind_found)
if test $ac_cv_bind_found = yes; then
   AC_DEFINE(BIND_DEFINED,1,[define if sys/socket.h declares bind])
fi
])dnl
dnl
AC_DEFUN(AC_FFLUSH_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares fflush)
AC_CACHE_VAL(ac_cv_fflush_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]fflush[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_fflush_found=yes, ac_cv_fflush_found=no)
])
AC_MSG_RESULT($ac_cv_fflush_found)
if test $ac_cv_fflush_found = yes; then
   AC_DEFINE(FFLUSH_DEFINED,1,[define if stdio.h declares fflush])
fi
])dnl
AC_DEFUN(AC_SCANF_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares scanf)
AC_CACHE_VAL(ac_cv_scanf_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]scanf[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_scanf_found=yes, ac_cv_scanf_found=no)
])
AC_MSG_RESULT($ac_cv_scanf_found)
if test $ac_cv_scanf_found = yes; then
   AC_DEFINE(SCANF_DEFINED,1,[define if stdio.h declares scanf])
fi
])dnl
AC_DEFUN(AC_FCLOSE_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares fclose)
AC_CACHE_VAL(ac_cv_fclose_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]fclose[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_fclose_found=yes, ac_cv_fclose_found=no)
])
AC_MSG_RESULT($ac_cv_fclose_found)
if test $ac_cv_fclose_found = yes; then
   AC_DEFINE(FCLOSE_DEFINED,1, [define if stdio.h declares fclose])
fi
])dnl
AC_DEFUN(AC_REWIND_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares rewind)
AC_CACHE_VAL(ac_cv_rewind_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]rewind[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_rewind_found=yes, ac_cv_rewind_found=no)
])
AC_MSG_RESULT($ac_cv_rewind_found)
if test $ac_cv_rewind_found = yes; then
   AC_DEFINE(REWIND_DEFINED,1, [define if stdio.h declares rewind])
fi
])dnl
AC_DEFUN(AC_PUTS_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares puts)
AC_CACHE_VAL(ac_cv_puts_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]puts[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_puts_found=yes, ac_cv_puts_found=no)
])
AC_MSG_RESULT($ac_cv_puts_found)
if test $ac_cv_puts_found = yes; then
   AC_DEFINE(PUTS_DEFINED,1,[define if stdio.h declares puts])
fi
])dnl
AC_DEFUN(AC_FPUTS_DEFINED,
[AC_MSG_CHECKING(whether stdio.h declares fputs)
AC_CACHE_VAL(ac_cv_fputs_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]fputs[^a-zA-Z_]>>changequote([, ])dnl
, stdio.h, ac_cv_fputs_found=yes, ac_cv_fputs_found=no)
])
AC_MSG_RESULT($ac_cv_fputs_found)
if test $ac_cv_fputs_found = yes; then
   AC_DEFINE(FPUTS_DEFINED,1,[define if stdio.h declares fputs])
fi
])dnl
AC_DEFUN(AC_TIME_DEFINED,
[AC_MSG_CHECKING(whether time.h declares time)
AC_CACHE_VAL(ac_cv_time_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]time[^a-zA-Z_]>>changequote([, ])dnl
, time.h, ac_cv_time_found=yes, ac_cv_time_found=no)
])
AC_MSG_RESULT($ac_cv_time_found)
if test $ac_cv_time_found = yes; then
   AC_DEFINE(TIME_DEFINED,1,[define if time.h declares time])
fi
])dnl
AC_DEFUN(AC_SETRLIMIT_DEFINED,
[AC_MSG_CHECKING(whether sys/resource.h declares setrlimit)
AC_CACHE_VAL(ac_cv_setrlimit_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]setrlimit[^a-zA-Z_]>>changequote([, ])dnl
, sys/resource.h, ac_cv_setrlimit_found=yes, ac_cv_setrlimit_found=no)
])
AC_MSG_RESULT($ac_cv_setrlimit_found)
if test $ac_cv_setrlimit_found = yes; then
   AC_DEFINE(SETRLIMIT_DEFINED,1, [define if setrlimit is declared in sys/resource.h])
fi
])dnl
AC_DEFUN(AC_LISTEN_DEFINED,
[AC_MSG_CHECKING(whether sys/socket.h declares listen)
AC_CACHE_VAL(ac_cv_listen_found,
[AC_EGREP_HEADER(
changequote(<<, >>)dnl
<<[^a-zA-Z_]listen[^a-zA-Z_]>>changequote([, ])dnl
, sys/socket.h, ac_cv_listen_found=yes, ac_cv_listen_found=no)
])
AC_MSG_RESULT($ac_cv_listen_found)
if test $ac_cv_listen_found = yes; then
   AC_DEFINE(LISTEN_DEFINED,1, [define if listen is declared in sys/socket.h])
fi
])dnl
dnl
AC_DEFUN(AC_HAS_STRUCT_HOSTENT,
[AC_CACHE_CHECK([whether struct hostent is in netdb.h or winsock.h],
  ac_cv_struct_hostent,
[AC_TRY_COMPILE([#include <netdb.h>],
[struct hostent vec;],
  ac_cv_struct_hostent=yes, 
[AC_TRY_COMPILE([#include <winsock.h>],
[struct hostent vec;],
  ac_cv_struct_hostent=yes, ac_cv_struct_hostent=no)])
])]
if test $ac_cv_struct_hostent = yes; then
  AC_DEFINE(HAS_STRUCT_HOSTENT, 1,[define if struct hostent is declared in netdb.h or winsock.h])
fi)
])
AC_DEFUN(AC_STRUCT_FDS_BITS,
[AC_CACHE_CHECK([for fds_bits in fd_set], ac_cv_struct_fds_bits,
[AC_TRY_COMPILE([#include <sys/select.h>], [fd_set s; s.fds_bits[0];],
ac_cv_struct_fds_bits=yes, ac_cv_struct_fds_bits=no)])
if test $ac_cv_struct_fds_bits = yes; then
  AC_DEFINE(HAVE_FDS_BITS,1,[Define if fd_set has element fds_bits])
fi
])
AC_DEFUN(AC_STRUCT_IOVEC,
[AC_CACHE_CHECK([whether struct iovec is in sys/uio.h],
  ac_cv_struct_iovec,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/uio.h>],
[struct iovec *vec;],
  ac_cv_struct_iovec=yes, ac_cv_struct_iovec=no)])
if test $ac_cv_struct_iovec = no; then
  AC_DEFINE(NEED_IOVEC_DEFINE)
fi
])

