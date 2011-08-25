#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([dibbler], [0.8.0-git], [dibbler@klub.com.pl])
AM_INIT_AUTOMAKE(dibbler, 0.8.0-git)
AC_CONFIG_SRCDIR([IfaceMgr/SocketIPv6.cpp])
AC_CONFIG_HEADERS([include/dibbler-config.h])
AC_CONFIG_MACRO_DIR([m4])

dnl change this to [no] to have verbose build system
dnl make V=0 and make V=1 will do the trick
AM_SILENT_RULES([no])

CFLAGS_SAVED="$CFLAGS"
CPPFLAGS_SAVED="$CPPFLAGS"
CXXFLAGS_SAVED="$CXXFLAGS"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
LT_INIT([disable-shared])
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

CFLAGS="$CFLAGS_SAVED"
CPPFLAGS="$CPPFLAGS_SAVED"
CXXFLAGS="$CXXFLAGS_SAVED"

dnl ---------------------------------------------
dnl Detect OS (will be used to select proper port
dnl ---------------------------------------------
  AC_MSG_CHECKING(which port is appropriate for this system)
  system=`uname -s`
  case $system in
    Linux)
      ARCH="LINUX"
      PORT_SUBDIR="Port-linux"
      PORT_CFLAGS=
      PORT_LDFLAGS="-lpthread"
      ;;
    Darwin | FreeBSD)
      ARCH="BSD"
      PORT_SUBDIR="Port-bsd"
      PORT_CFLAGS=
      PORT_LDFLAGS=
      ;;
    MINGW32*)
      ARCH="WIN2K"
      PORT_LDFLAGS = "-lws2_32"
      PORT_CFLAGS  = "-DMINGWBUILD"
      PORT_SUBDIR  = "Port-win2k"
      ;;
    *)
      AC_MSG_ERROR("Unsupported OS: uname returned $system")
      ;;
  esac
  AC_MSG_RESULT($PORT_SUBDIR)

CFLAGS+=" -std=c99 ${PORT_CFLAGS}"
LDFLAGS+=" ${PORT_LDFLAGS}"
CPPFLAGS+=" -D${ARCH}"

dnl ------------------------------------------------------------
dnl ./configure parameters
dnl ------------------------------------------------------------

  AC_ARG_ENABLE(debug,
     [  --enable-debug          Turn on debugging (default: no)],
     [  case "${enableval}" in
          yes)
	    debug=yes
	    ;;
          no)
	    debug=no
	    ;;
          *)
            AC_MSG_ERROR(bad value ${enableval} for --enable-debug)
	    ;;
	esac],
     [ debug=no ]
  )

  if test x$debug == xyes; then
     CPPFLAGS+=" -O0 -g"
     LINKPRINT+=debug
  else
     CPPFLAGS+=" -O2"
  fi

  AC_ARG_ENABLE(efence,
     [  --enable-efence         Enables linking with electric-fence (default: no)],
     [  case "${enableval}" in
          yes)
            efence=yes
            ;;
          no)
            efence=no
            ;;
          *)
            AC_MSG_ERROR(bad value ${enableval} for --enable-efence)
            ;;
        esac],
     [ efence=no]
   )

   if test x$efence == xyes; then
      LDFLAGS+=-lefence
      LINKPRINT+=efence
   fi



AC_SUBST(LDFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(ARCH)
AC_SUBST(PORT_SUBDIR)
AC_SUBST(PORT_LDFLAGS)
AC_SUBST(PORT_CFLAGS)
AC_SUBST(LINKPRINT)

#AC_CONFIG_SUBDIRS([bison++])
AC_CONFIG_SUBDIRS([poslib])

AC_OUTPUT(Makefile AddrMgr/Makefile CfgMgr/Makefile ClntAddrMgr/Makefile ClntCfgMgr/Makefile ClntIfaceMgr/Makefile ClntMessages/Makefile ClntOptions/Makefile ClntTransMgr/Makefile IfaceMgr/Makefile Messages/Makefile Misc/Makefile Options/Makefile RelCfgMgr/Makefile RelIfaceMgr/Makefile RelMessages/Makefile RelOptions/Makefile RelTransMgr/Makefile Requestor/Makefile SrvAddrMgr/Makefile SrvCfgMgr/Makefile SrvIfaceMgr/Makefile SrvMessages/Makefile SrvOptions/Makefile SrvTransMgr/Makefile $PORT_SUBDIR/Makefile)

dnl replace "$PORT_SUBDIR/Makefile with Port-linux/Makefile Port-bsd/Makefile"
dnl and use autoreconf to generate Makefile.in in Port directories.
AC_OUTPUT(Port-linux/Makefile Port-bsd/Makefile)

dnl ----------------------------------------
dnl Print out configured parameters
dnl ----------------------------------------
echo
echo "Selected port  : $PORT_SUBDIR"
echo "Debug          : $debug"
echo "Electric fence : $efence"
echo 
echo "CFLAGS         : $CFLAGS"
echo "CPPFLAGS       : $CPPFLAGS"
echo "CXXFLAGS       : $CXXFLAGS"
echo "LDFLAGS        : $LDFLAGS"
echo
echo Type make to compile dibbler.
echo
