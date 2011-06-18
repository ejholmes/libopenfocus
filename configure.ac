#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([LibOpenFocus], [0.1], [eric@ejholmes.net])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_MACRO_DIR([m4])
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_CPP
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Check for libusb
PKG_CHECK_MODULES([libusb], [libusb],,[
                   AC_CHECK_LIB([usb], [usb_init],,[AC_MSG_ERROR[libusb not found]])
                   AC_CHECK_HEADERS([usb.h],,[AC_MSG_ERROR[usb.h not found]])
                   ])

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
