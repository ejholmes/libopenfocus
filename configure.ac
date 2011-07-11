#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([LibOpenFocus], [0.1], [eric@ejholmes.net])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_CPP
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE(debug,
              [  --enable-debug          Enable debugging (disabled by default)],
              [if test x$enableval = xyes; then
                   AC_DEFINE(DEBUG, 1, [Define this to enable debug messages and assert warnings.]) debug_support=yes
               fi])

AC_PATH_PROG([LIBUSB_CONFIG], [libusb-config], [], [$PATH$PATH_SEPARATOR/usr/local/Crosspack-AVR/bin])

if test ! -z $PKG_CONFIG; then
    PKG_CHECK_MODULES([LIBUSB], [libusb],,)
else
    if test ! -z $LIBUSB_CONFIG; then
        LIBUSB_CFLAGS=`$LIBUSB_CONFIG --cflags`
        LIBUSB_LIBS=`$LIBUSB_CONFIG --libs`
        AC_SUBST([LIBUSB_CFLAGS])
        AC_SUBST([LIBUSB_LIBS])
    else
        AC_MSG_ERROR([
* No suitable version of libusb-0.1 was found.
* If you know where libusb is located, specify it
* with the LIBUSB_CFLAGS and LIBUSB_LIBS variables.
*
* If you do not have libusb installed, you can
* download it from http://www.libusb.org/])
    fi
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
