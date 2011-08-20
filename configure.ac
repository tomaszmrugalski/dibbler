#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([dibbler], [0.8.0-git], [bugs@klub.com.pl])
AM_INIT_AUTOMAKE(dibbler, 0.8.0-git)
AC_CONFIG_SRCDIR([IfaceMgr/SocketIPv6.cpp])
AC_CONFIG_HEADERS([include/dibbler-config.h])

dnl change this to [no] to have verbose build system
dnl make V=0 and make V=1 will do the trick
AM_SILENT_RULES([no])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
#AM_PROG_LEX
#AC_PROG_YACC

# Checks for libraries.

dnl -----------------------
dnl Checks for header files
dnl -----------------------
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h limits.h malloc.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h syslog.h unistd.h wchar.h])
AC_CHECK_HEADERS(poll.h sys/poll.h)

  if test x$ac_cv_header_poll_h = xyes || test x$ac_cv_header_sys_poll_h = xyes; then
      AC_DEFINE(HAVE_POLL, 1, [Defines whether the poll function is available])
  fi


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([bzero gethostbyaddr gethostbyname gettimeofday inet_ntoa memchr memmove memset select socket strcasecmp strchr strdup strerror strncasecmp strstr strtol strtoul])

CFLAGS+=" -std=c99"
CPPFLAGS+="-DLINUX"
PORT_SUBDIR="Port-linux"

AC_SUBST(LDFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(ARCH)
AC_SUBST(PORT_SUBDIR)
AC_SUBST(PORT_LDFLAGS)
AC_SUBST(PORT_CFLAGS)
AC_SUBST(LINKPRINT)

echo "#### $PORT_SUBDIR"
AC_OUTPUT(Makefile AddrMgr/Makefile CfgMgr/Makefile ClntAddrMgr/Makefile ClntCfgMgr/Makefile ClntIfaceMgr/Makefile ClntMessages/Makefile ClntOptions/Makefile ClntTransMgr/Makefile IfaceMgr/Makefile Messages/Makefile Misc/Makefile Options/Makefile RelCfgMgr/Makefile RelIfaceMgr/Makefile RelMessages/Makefile RelOptions/Makefile RelTransMgr/Makefile Requestor/Makefile SrvAddrMgr/Makefile SrvCfgMgr/Makefile SrvIfaceMgr/Makefile SrvMessages/Makefile SrvOptions/Makefile SrvTransMgr/Makefile TransMgr/Makefile $PORT_SUBDIR/Makefile)

dnl replace "$PORT_SUBDIR/Makefile with Port-linux/Makefile Port-bsd/Makefile"
dnl and use autoreconf to generate Makefile.in in Port directories.
dnl AC_OUTPUT(Port-linux/Makefile Port-bsd/Makefile)

#AC_CONFIG_SUBDIRS([bison++
#                   poslib])
#AC_CONFIG_SUBDIRS(poslib)
