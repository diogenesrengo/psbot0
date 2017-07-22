AC_PREREQ([2.69])
AC_INIT([psbot0], [1.0])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])     
AC_PROG_CC 
AC_PROG_RANLIB
# imagemagick libs and headers
PKG_CHECK_MODULES([MagickWand], [MagickWand], [], [AC_MSG_ERROR(
  [sorry but you need to install imagemagick package first (lib and dev)])
])
#
# xdotool libs and headers
AC_CHECK_HEADERS([xdo.h], [xdo_not_found=yes; break;])
AS_IF([test "x$xdo_not_found" != "xyes"], [AC_MSG_ERROR(
  [sorry but you need to install xdotool package first (lib and dev)])
])
AC_SEARCH_LIBS([xdo_new], [xdo], [], [AC_MSG_ERROR(
  [sorry but you need to install xdotool package first (lib and dev)])
])
#
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

