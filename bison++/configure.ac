# Process this file with autoconf to produce a configure script.
PACKAGE=bison++
AC_INIT([bison++], [2.21.5-dibbler], [alain.coetmeur@caissedesdepots.fr])
AC_CONFIG_SRCDIR([bison.cc])
AM_INIT_AUTOMAKE

# DO NOT trigger rebuild rules, unless I tell you so.
AM_MAINTAINER_MODE([disable])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([alloca.h malloc.h memory.h stddef.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
