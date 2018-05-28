#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(meagle, 1.0.4, www.minigui.com)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL


AC_PREFIX_DEFAULT(/usr/local)
# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
lite_version="no"
build_ogr="yes"

AC_ARG_ENABLE(ogr,
[  --enable-ogr              Include OGR Library <default=yes>],
build_ogr=$enableval)

if test "x$build_ogr" = "xyes"; then
  AC_DEFINE(ENABLE_OGR, 1, [Define if Include OGR Library])
fi

dnl check for lite or threads version of MiniGUI
AC_CHECK_DECLS(_LITE_VERSION, lite_version="yes", foo=bar, [#include <minigui/common.h>])


if test "x$lite_version" = "xno"; then
    LIBS="$LIBS -lpthread"
fi

AC_CHECK_DECLS(_HAVE_MATH_H, LIBS="$LIBS -lm", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_PNG_FILE_SUPPORT, LIBS="$LIBS -lpng", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_JPG_FILE_SUPPORT, LIBS="$LIBS -ljpeg", foo=bar, [#include <minigui/common.h>])

# Check if include ttf support.
AC_CHECK_DECLS(_TTF_SUPPORT, LIBS="$LIBS -lttf", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_FT2_SUPPORT, LIBS="$LIBS -lfreetype", foo=bar, [#include <minigui/common.h>])

#CFLAGS="$CFLAGS -pg"
#CXXFLAGS="$CXXFLAGS -pg"

AM_CONDITIONAL(BUILD_OGR, test "x$build_ogr" = "xyes")

AC_OUTPUT(
Makefile
include/Makefile
ogr/ogrsf_frmts/Makefile
ogr/Makefile
src/Makefile
test/Makefile
test/res/Makefile
)
