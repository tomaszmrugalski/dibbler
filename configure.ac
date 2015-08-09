#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([dibbler], [1.0.1], [dibbler@klub.com.pl])
AM_INIT_AUTOMAKE(foreign)
AC_CONFIG_SRCDIR([IfaceMgr/SocketIPv6.cpp])
AC_CONFIG_HEADERS([include/dibbler-config.h])
AC_CONFIG_MACRO_DIR([m4])

# DO NOT trigger rebuild rules, unless I tell you so.
AM_MAINTAINER_MODE([disable])

# Poslib stuff
AC_LANG_CPLUSPLUS

dnl -- Check for Max OS x and define __APPLE_USE_RFC3542 if version needs
system=`uname -s`
s_version=`uname -r`
case $system in
    Darwin)
	    case $s_version in
            12*|13*|14*)
                AC_DEFINE([__APPLE_USE_RFC_3542], [1], [Are we _special_?])
                NEED_RFC_3542=yes
		# Yup, Mac OS X 10.6.x is special. 10.9 is no different, too.
		;;
	    *)
		# Let's hope that madness will go way one day.
	    ;;
        esac
    ;;
esac

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
# system=`uname -s`
  case $system in
    Linux)
      ARCH="LINUX"
      PORT_SUBDIR="Port-linux"
      PORT_CFLAGS=""
      PORT_LDFLAGS=""
      EXTRA_DIST_SUBDIRS="Port-bsd Port-winnt2k Port-sun"
      ;;
    Darwin | FreeBSD | NetBSD)
      ARCH="BSD"
      PORT_SUBDIR="Port-bsd"
      PORT_CFLAGS=""
      if test "$NEED_RFC_3542" == "yes"; then
        PORT_CFLAGS="-D__APPLE_USE_RFC_3542"
      fi
      PORT_LDFLAGS=
      EXTRA_DIST_SUBDIRS="Port-linux Port-winnt2k Port-sun"
      ;;
    OpenBSD)
      ARCH="BSD"
      PORT_SUBDIR="Port-bsd"
      PORT_CFLAGS="-DOPENBSD"
      PORT_LDFLAGS=
      EXTRA_DIST_SUBDIRS="Port-linux Port-winnt2k Port-sun"
      ;;
    SunOS)
      ARCH="SUNOS"
      PORT_SUBDIR="Port-sun"
      PORT_CFLAGS="-DSUNOS -D__EXTENSIONS__"
      PORT_LDFLAGS="-lsocket -lnsl"
      EXTRA_DIST_SUBDIRS="Port-linux Port-winnt2k Port-bsd"
      ;;
    MINGW32*)
      ARCH="WIN2K"
      PORT_LDFLAGS = "-lws2_32"
      PORT_CFLAGS  = "-std=c99 -DMINGWBUILD"
      PORT_SUBDIR  = "Port-win2k"
      EXTRA_DIST_SUBDIRS="Port-linux Port-bsd"
      ;;
    *)
      AC_MSG_ERROR("Unsupported OS: uname returned $system")
      ;;
  esac
  AC_MSG_RESULT($PORT_SUBDIR)

CFLAGS="${CFLAGS} ${PORT_CFLAGS}"
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
     OPTFLAG="-O0"
     CPPFLAGS="-g ${CPPFLAGS}"
     LINKPRINT="${LINKPRINT} debug"
  else
     OPTFLAG="-O2"
  fi

  # add -O2 or -O0 only if it isn't specified already by user
  TMP=`echo "$CPPFLAGS" | grep "\-O" -`
  if test "$CPPFLAGS" != "$TMP"; then
     CPPFLAGS="${OPTFLAG} ${CPPFLAGS}"
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

### reusing dst addr filter ####################
   AC_ARG_ENABLE(dst-addr-filter,
        [  --enable-dst-addr-check Enables server checks of dst address vs socket binding address (default: no)],
        [ case "${enableval}" in
              yes)
              MOD_SRV_DST_ADDR_CHECK=yes
              ;;
              no)
              MOD_SRV_DST_ADDR_CHECK=no
              ;;
              *)
              AC_MSG_ERROR(bad value ${enableval} for --enable-dst-addr-check)
              ;;
           esac],
        [MOD_SRV_DST_ADDR_CHECK=yes]
   )

AM_CONDITIONAL(MOD_SRV_DST_ADDR_CHECK, test $MOD_SRV_DST_ADDR_CHECK = yes)
AM_COND_IF([MOD_SRV_DST_ADDR_CHECK], [AC_DEFINE([MOD_SRV_DST_ADDR_CHECK], [1],
           [Support for server checks if incoming destination addr matches socket address])])

### support for resolvconf tool (mostly Debian, probably) ##############
   AC_ARG_ENABLE(resolvconf,
	[  --enable-resolvconf	  Enables support for resolvconf (/sbin/resolvcof) (default: no)],
	[ case "${enableval}" in
	       yes)
	       MOD_RESOLVCONF=yes
	       ;;
	       no)
	       MOD_RESOLVCONF=no
	       ;;
	       *)
	       AC_MSG_ERROR(bad value ${enablevald} for --enable-resolvconf)
	       ;;
	  esac],
	  [MOD_RESOLVCONF=no]
    )

AM_CONDITIONAL(RESOLVCONF, test $MOD_RESOLVCONF = yes)
AM_COND_IF([RESOLVCONF], [AC_DEFINE([MOD_RESOLVCONF], [1], [Support for /sbin/resolvconf enabled?])])

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

AC_ARG_ENABLE(gtest-static,
     [  --enable-gtest-static   Enables static linking for gtest (default: yes)],
     [  case "${enableval}" in
          yes)
            gtest_static=yes
            ;;
          no)
            gtest_static=no
            ;;
          *)
            AC_MSG_ERROR(bad value ${enableval} for --enable-gtest-static)
            ;;
        esac],
     [ gtest_static=yes]
   )


#echo "After gtest"

if test "$gtest_path" != "no"
then
    if test -x "${gtest_path}/scripts/gtest-config"
    then
        GTEST_CONFIG="${gtest_path}/scripts/gtest-config"
        GTEST_INCLUDES=`${GTEST_CONFIG} --cppflags`
        GTEST_LDFLAGS=`${GTEST_CONFIG} --ldflags`
        GTEST_LDADD=`${GTEST_CONFIG} --libs`

        # Linking gtest statically
        if test "$gtest_static" != "no"
        then
            GTEST_LDFLAGS="${GTEST_LDFLAGS} -static"
        fi
    else
        AC_MSG_ERROR(Google test not found: couldn't execute ${gtest_path}/scripts/gtest-config)
    fi
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

   if test x$MOD_CLNT_CONFIRM = xyes && test $ARCH = LINUX ; then
      echo "Link state detection enabled on Linux, adding -lpthreads"
   LDFLAGS="${LDFLAGS} -lpthread"
   else
      echo "Link state detection disabled or this is not Linux, NOT adding -lpthreads"
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

# Lists all port directories that are unused on this platform (e.g. BSD and winn2k on Linux)
AC_SUBST(EXTRA_DIST_SUBDIRS)
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

AC_OUTPUT(
Makefile
AddrMgr/Makefile
CfgMgr/Makefile
ClntAddrMgr/Makefile
ClntCfgMgr/Makefile
ClntIfaceMgr/Makefile
ClntMessages/Makefile
ClntOptions/Makefile
ClntTransMgr/Makefile
IfaceMgr/Makefile
Messages/Makefile
Misc/Makefile
Options/Makefile
RelCfgMgr/Makefile
RelIfaceMgr/Makefile
RelMessages/Makefile
RelOptions/Makefile
RelTransMgr/Makefile
Requestor/Makefile
SrvAddrMgr/Makefile
SrvCfgMgr/Makefile
SrvIfaceMgr/Makefile
SrvMessages/Makefile
SrvOptions/Makefile
SrvTransMgr/Makefile
poslib/Makefile
nettle/Makefile
$PORT_SUBDIR/Makefile
Port-linux/Makefile
Port-bsd/Makefile
Port-sun/Makefile
Port-win32/Makefile
Port-winnt2k/Makefile
doc/Makefile
Misc/Portable.h
doc/doxygen.cfg
doc/version.tex
AddrMgr/tests/Makefile
IfaceMgr/tests/Makefile
Options/tests/Makefile
SrvCfgMgr/tests/Makefile
CfgMgr/tests/Makefile
poslib/tests/Makefile
Misc/tests/Makefile
RelTransMgr/tests/Makefile
tests/Makefile
tests/Srv/Makefile
tests/utils/Makefile)

dnl ----------------------------------------
dnl Print out configured parameters
dnl ----------------------------------------
echo
echo "Dibbler version               : $PACKAGE_VERSION"
echo "Selected OS port              : $PORT_SUBDIR"
echo "Actual OS                     : $system $s_version"
echo "Debug                         : $debug"
echo "Electric fence                : $efence"
echo "Socket bind reuse             : $MOD_CLNT_BIND_REUSE"
echo "DNS Update (clnt/srv) disabled: $MOD_CLNT_DISABLE_DNSUPDATE/$MOD_SRV_DISABLE_DNSUPDATE"
echo "Authentication disabled       : $MOD_DISABLE_AUTH"
echo "Link-state change detection   : $MOD_CLNT_CONFIRM"
echo "/sbin/resolvconf support      : $MOD_RESOLVCONF"

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
