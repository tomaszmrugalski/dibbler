#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([dibbler], [0.8.1], [dibbler@klub.com.pl])
AM_INIT_AUTOMAKE(foreign)
AC_CONFIG_SRCDIR([IfaceMgr/SocketIPv6.cpp])
AC_CONFIG_HEADERS([include/dibbler-config.h])
AC_CONFIG_MACRO_DIR([m4])

# DO NOT trigger rebuild rules, unless I tell you so.
AM_MAINTAINER_MODE([disable])

# Poslib stuff
AC_LANG_CPLUSPLUS

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

# poslib
AC_CHECK_TOOL([STRIP],[strip])
AC_HEADER_STDC

dnl -----------------------
dnl Checks for header files
dnl -----------------------
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h limits.h malloc.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h syslog.h unistd.h wchar.h])
AC_CHECK_HEADERS(poll.h sys/poll.h winsock2.h sys/select.h)
AC_CHECK_HEADERS(slist ext/slist)

  if test x$ac_cv_header_poll_h = xyes || test x$ac_cv_header_sys_poll_h = xyes; then
      AC_DEFINE(HAVE_POLL, 1, [Defines whether the poll function is available])
  fi


AC_CHECK_DECLS([IPV6_PKTINFO],,,[
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
])
AC_CHECK_DECLS([IPV6_RECVPKTINFO],,,[
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
])

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
AC_CHECK_FUNCS([bzero gethostbyaddr gethostname gethostbyname gettimeofday inet_ntoa inet_aton memchr memmove memset select socket strcasecmp strchr strdup strerror strncasecmp strstr strtol strtoul])

CFLAGS="$CFLAGS_SAVED"
CPPFLAGS="$CPPFLAGS_SAVED"
CXXFLAGS="$CXXFLAGS_SAVED"


dnl ------------------------------------------------------------
dnl --- poslib stuff -------------------------------------------
dnl ------------------------------------------------------------

  if test $ac_cv_func_socket = no; then
    # socket is not in the default libraries.
    AC_CHECK_LIB(socket, socket,
      [ LIB_LIBS="$LIB_LIBS -lsocket" ])
  fi

  AC_MSG_CHECKING(for vsnprintf)
  AC_TRY_COMPILE([#include <stdio.h>
    #include <stdarg.h>
    ],[char buff[1]; va_list valist; vsnprintf(buff, 1, "", valist);],
    [AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_VSNPRINTF, 1, [Defines whether the vsnprintf() function is available])
    ],[AC_MSG_RESULT(no)])

  if test $ac_cv_func_inet_aton = no; then
    # inet_aton is not in the default libraries.
    AC_CHECK_LIB(resolv, inet_aton, LIB_LIBS="$LIB_LIBS -lresolv")
  fi

  if test $ac_cv_func_gethostname = no; then
    AC_CHECK_LIB(nsl, gethostname, LIB_LIBS="$LIB_LIBS -lnsl")
  fi

  dnl May end up with duplicate -lnsl -- oh well
  if test $ac_cv_func_gethostbyname = no; then
    AC_CHECK_LIB(nsl, gethostbyname, LIB_LIBS="$LIB_LIBS -lnsl")
    OLD_LIBS=$LIBS
    LIBS="$LIB_LIBS $LIBS -lws2_32"
    AC_TRY_LINK([#include <winsock2.h>], [gethostbyname("test");], LIB_LIBS="$LIB_LIBS -lws2_32")
    LIBS=$OLD_LIBS
  fi

    AC_MSG_CHECKING(for struct sockaddr_in6)
    AC_TRY_COMPILE([#include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    ],[static struct sockaddr_in6 ac_i;],
    [AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_IPV6, 1, [Defines whether IPv6 support is available])

     dnl ------------------
     dnl Check for sin6_len
     dnl ------------------

     AC_MSG_CHECKING(whether struct sockaddr_in6 has a sin6_len field)
     AC_TRY_COMPILE([#include <sys/types.h>
     #include <netinet/in.h>
     #include <sys/socket.h>
     ],[static struct sockaddr_in6 ac_i;int ac_j = sizeof(ac_i.sin6_len);],
     [AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_SIN6_LEN, 1, [Defines whether the sin6_len field should be used])],
      AC_MSG_RESULT(no))

     dnl ---------------------
     dnl Check for __ss_family
     dnl ---------------------

     AC_MSG_CHECKING(whether struct sockaddr_storage has a __ss_family field)
     AC_TRY_COMPILE([#include <sys/types.h>
     #include <netinet/in.h>
     #include <sys/socket.h>
     ],[static struct sockaddr_storage ac_i;int ac_j = sizeof(ac_i.__ss_family);],
     [AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE___SS_FAMILY, 1, [Defines wiether the __ss_family field should be used])],
      AC_MSG_RESULT(no))     
    ],
     AC_MSG_RESULT(no))

    AC_MSG_CHECKING(for struct sockaddr_storage)
    AC_TRY_COMPILE([
      #ifdef HAVE_WINSOCK2_H
      #include <winsock2.h>
      #else
      #include <sys/types.h>
      #include <netinet/in.h>
      #include <sys/socket.h>
      #endif
    ],[static struct sockaddr_storage ac_i;],
    [AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_SOCKADDR_STORAGE, 1, [Defines whether sockaddr_storage should be used])],
     AC_MSG_ERROR(For IPv6 you will need a struct sockaddr_storage!)
    )

  dnl --------------------------------
  dnl Check for sin_len in sockaddr_in
  dnl --------------------------------

  AC_MSG_CHECKING(whether struct sockaddr_in has a sin_len field)
  AC_TRY_COMPILE([#include <sys/types.h>
  #include <netinet/in.h>
  ],[static struct sockaddr_in ac_i;int ac_j = sizeof(ac_i.sin_len);],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_SIN_LEN, 1, [Defines whether the sin_len field is available])],
   AC_MSG_RESULT(no))

  dnl -------------------
  dnl Check for socklen_t
  dnl -------------------

  AC_MSG_CHECKING(for socklen_t)
    AC_EGREP_HEADER(socklen_t, sys/socket.h,
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_SOCKLEN_T, 1, [Defines whether we have the socklen_t field]),
    AC_MSG_RESULT(no))

dnl ------------------------------------------------------------
dnl --- poslib stuff -------------------------------------------
dnl ------------------------------------------------------------


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
    Darwin | FreeBSD | NetBSD)
      ARCH="BSD"
      PORT_SUBDIR="Port-bsd"
      PORT_CFLAGS=
      PORT_LDFLAGS=
      ;;
    OpenBSD)
      ARCH="BSD"
      PORT_SUBDIR="Port-bsd"
      PORT_CFLAGS=-DOPENBSD
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

CFLAGS="${CFLAGS} -std=c99 ${PORT_CFLAGS}"
LDFLAGS="${LDFLAGS} ${PORT_LDFLAGS}"
CPPFLAGS="${CPPFLAGS} -D${ARCH}"

CPPFLAGS="${CPPFLAGS} -Wall -pedantic -funsigned-char"

dnl ------------------------------------------------------------
dnl ./configure parameters
dnl ------------------------------------------------------------

  AC_ARG_ENABLE(, )
  AC_ARG_ENABLE(, Dibbler modular features:)

### debugging ##################
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

  if test x$debug = xyes; then
     CPPFLAGS="${CPPFLAGS} -O0 -g"
     LINKPRINT="${LINKPRINT} debug"
  else
     CPPFLAGS="${CPPFLAGS} -O2"
  fi

### electric-fence ####################
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

   if test x$efence = xyes; then
      LDFLAGS="${LDFLAGS} -lefence"
      LINKPRINT="${LINKPRINT} efence"
   fi

### reusing socket binding (bind SO_REUSEADDR) ####################
   AC_ARG_ENABLE(bind-reuse,
        [  --enable-bind-reuse     Enables reusing the same port/address: SO_REUSEADDR (default: yes)],
        [ case "${enableval}" in
              yes)
              MOD_CLNT_BIND_REUSE=yes
              ;;
              no)
              MOD_CLNT_BIND_REUSE=no
              ;;
              *)
              AC_MSG_ERROR(bad value ${enableval} for --enable-bind-reuse)
              ;;
           esac],
        [MOD_CLNT_BIND_REUSE=yes]
   )

   if test x$MOD_CLNT_BIND_REUSE = xyes; then
      CPPFLAGS="${CPPFLAGS} -DMOD_CLNT_BIND_REUSE"
   fi

### DNS Update ####################################
### We may add separate parameter for client and server eventually ##########
   AC_ARG_ENABLE(dns-update,
        [  --enable-dns-update     Enables DNS Update mechanism (default: yes)],
        [ case "${enableval}" in
              yes)
              MOD_CLNT_DISABLE_DNSUPDATE=no
              MOD_SRV_DISABLE_DNSUPDATE=no
              ;;
              no)
              MOD_CLNT_DISABLE_DNSUPDATE=yes
              MOD_SRV_DISABLE_DNSUPDATE=yes
              ;;
              *)
              AC_MSG_ERROR(bad value ${enableval} for --enable-dns-update)
              ;;
           esac],
        [MOD_CLNT_DISABLE_DNSUPDATE=no; MOD_SRV_DISABLE_DNSUPDATE=no]
   )

   if test x$MOD_CLNT_DISABLE_DNSUPDATE = xyes; then
      CPPFLAGS="${CPPFLAGS} -DMOD_CLNT_DISABLE_DNSUPDATE"
   fi
   if test x$MOD_SRV_DISABLE_DNSUPDATE = xyes; then
      CPPFLAGS="${CPPFLAGS} -DMOD_SRV_DISABLE_DNSUPDATE"
   fi

### Authentication ###############################
   AC_ARG_ENABLE(auth,
        [  --enable-auth           Enables authentication (default: yes)],
        [ case "${enableval}" in
              yes)
              MOD_DISABLE_AUTH=no
              ;;
              no)
              MOD_DISABLE_AUTH=yes
              ;;
              *)
              AC_MSG_ERROR(bad value ${enableval} for --enable-auth)
              ;;
           esac],
        [MOD_DISABLE_AUTH=no]
   )
   if test x$MOD_DISABLE_AUTH = xyes; then
      CPPFLAGS="${CPPFLAGS} -DMOD_DISABLE_AUTH"
   fi

### gtests #################################################
AC_ARG_WITH(gtest,
[  --with-gtest=PATH    specify a path to gtest header files and library],
   gtest_path="$withval", 
   gtest_path="no")

#echo "After gtest"

if test "$gtest_path" != "no"
then
	GTEST_CONFIG="${gtest_path}/scripts/gtest-config"
	GTEST_INCLUDES=`${GTEST_CONFIG} --cppflags`
	GTEST_LDFLAGS=`${GTEST_CONFIG} --ldflags`
	GTEST_LDADD=`${GTEST_CONFIG} --libs`
fi

AC_SUBST(GTEST_INCLUDES)
AC_SUBST(GTEST_LDFLAGS)
AC_SUBST(GTEST_LDADD)

### Link-state change detections ##########################
   AC_ARG_ENABLE(link-state,
        [  --enable-link-state     Enables link-state change detections (default: yes)],
        [ case "${enableval}" in
              yes)
              MOD_CLNT_CONFIRM=yes
              ;;
              no)
              MOD_CLNT_CONFIRM=no
              ;;
              *)
              AC_MSG_ERROR(bad value ${enableval} for --enable-link-state)
              ;;
           esac],
        [MOD_CLNT_CONFIRM=yes]
   )

   if test x$MOD_CLNT_CONFIRM = xyes; then
      CPPFLAGS="${CPPFLAGS} -DMOD_CLNT_CONFIRM"
   fi

### Remote autoconf ######################################
   AC_ARG_ENABLE(remote-autoconf,
        [ --enable-remote-autoconf Enables *experimental* remote autoconfiguration (default: no)],
        [ case "${enableval}" in
              yes)
              MOD_REMOTE_AUTOCONF=yes
              ;;
              no)
              MOD_REMOTE_AUTOCONF=no
              ;;
              *)
              AC_MSG_ERROR(bad value ${enableval} for --enable-remote-autoconf)
              ;;
           esac],
        [MOD_REMOTE_AUTOCONF=no]
   )

   if test x$MOD_REMOTE_AUTOCONF = xyes; then
      CPPFLAGS="${CPPFLAGS} -DMOD_REMOTE_AUTOCONF"
   fi

AC_SUBST(LDFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(ARCH)
AC_SUBST(PORT_SUBDIR)
AC_SUBST(PORT_LDFLAGS)
AC_SUBST(PORT_CFLAGS)
AC_SUBST(LINKPRINT)

AC_CONFIG_SUBDIRS([bison++])
#AC_CONFIG_SUBDIRS([poslib])


if test "$gtest_path" != "no"; then
  echo "GTEST enabled, generating Makefiles"
  AC_DEFINE(HAVE_GTEST, 1, [Defines if google test is available and should be used])
fi

AM_CONDITIONAL(HAVE_GTEST, [test "$gtest_path" != "no"])

dnl replace "$PORT_SUBDIR/Makefile with Port-linux/Makefile Port-bsd/Makefile"
dnl and use autoreconf to generate Makefile.in in Port directories.

AC_OUTPUT(Makefile AddrMgr/Makefile CfgMgr/Makefile ClntAddrMgr/Makefile ClntCfgMgr/Makefile ClntIfaceMgr/Makefile ClntMessages/Makefile ClntOptions/Makefile ClntTransMgr/Makefile IfaceMgr/Makefile Messages/Makefile Misc/Makefile Options/Makefile RelCfgMgr/Makefile RelIfaceMgr/Makefile RelMessages/Makefile RelOptions/Makefile RelTransMgr/Makefile Requestor/Makefile SrvAddrMgr/Makefile SrvCfgMgr/Makefile SrvIfaceMgr/Makefile SrvMessages/Makefile SrvOptions/Makefile SrvTransMgr/Makefile poslib/Makefile $PORT_SUBDIR/Makefile AddrMgr/tests/Makefile Options/tests/Makefile Port-linux/Makefile Port-bsd/Makefile Port-win32/Makefile Port-winnt2k/Makefile doc/Makefile)

dnl ----------------------------------------
dnl Print out configured parameters
dnl ----------------------------------------
echo
echo "Selected OS port              : $PORT_SUBDIR"
echo "Debug                         : $debug"
echo "Electric fence                : $efence"
echo "Socket bind reuse             : $MOD_CLNT_BIND_REUSE"
echo "DNS Update (clnt/srv) disabled: $MOD_CLNT_DISABLE_DNSUPDATE/$MOD_SRV_DISABLE_DNSUPDATE"
echo "Authentication disabled       : $MOD_DISABLE_AUTH"
echo "Link-state change detection   : $MOD_CLNT_CONFIRM"

echo
echo "Experimental features:"
echo "Remote autoconfigution        : $MOD_REMOTE_AUTOCONF"
echo 
echo "CFLAGS         : $CFLAGS"
echo "CPPFLAGS       : $CPPFLAGS"
echo "CXXFLAGS       : $CXXFLAGS"
echo "LDFLAGS        : $LDFLAGS"
echo
echo "Google test    : $gtest_path"
if test "$gtest_path" != "no"
then
echo "GTEST_INCLUDES : $GTEST_INCLUDES"
echo "GTEST_LDFLAGS  : $GTEST_LDFLAGS"
echo "GTEST_LDADD    : $GTEST_LDADD"
echo
fi
echo Type make to compile dibbler.
echo
