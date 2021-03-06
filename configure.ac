AC_INIT(myconfig, version-0.1)
echo "                             Testing for a C compiler"
AC_PROG_CC
echo "                             Testing for a C++ compiler"
AC_PROG_CXX

AC_LANG(C++)

AC_CHECK_HEADERS(openssl/rsa.h)
AC_CHECK_HEADERS(openssl/pem.h)
AC_CHECK_HEADERS(openssl/err.h)
AC_CHECK_HEADERS(openssl/x509.h)
AC_CHECK_HEADERS(openssl/x509v3.h)

AC_CHECK_HEADERS(event2/event_struct.h)
AC_CHECK_HEADERS(event2/event.h)
AC_CHECK_HEADERS(event2/event_compat.h)
AC_CHECK_HEADERS(event2/buffer.h)

AC_CHECK_LIB(crypto, EVP_PKEY_get1_RSA)
AC_CHECK_LIB(event, event_new)

AC_SUBST(CC, g++)

AC_ARG_ENABLE([debugging], AS_HELP_STRING([—-enable-debugging], [Enable Debug support]))
AC_ARG_ENABLE([http], AS_HELP_STRING([--disable-http], [Disable feature http]))
AC_ARG_ENABLE([64bit], AS_HELP_STRING([--enable-64bit], [Compile for x86_64 architecture]))
AC_ARG_ENABLE([threading], AS_HELP_STRING([--disable-threading], [Disable threading]))
AC_ARG_ENABLE([sockets], AS_HELP_STRING([--disable-sockets], [Disable sockets]))
AC_ARG_ENABLE([debug], AS_HELP_STRING([--disable-debug], [Disable debugging and Logging]))
AC_ARG_ENABLE([encryption], AS_HELP_STRING([--disable-encryption], [Disable encryption support]))
AC_ARG_ENABLE([io], AS_HELP_STRING([--disable-io], [Disable io support]))

DEBUG_FLAGS=
AS_IF([test "x$enable_debugging" == "xyes"], [
	[DEBUG_FLAGS="-Wall -g3 -gdwarf-2 -D DEBUG"]
])
AC_SUBST(DEBUG_FLAGS)

AS_IF([test "x$enable_http" != "xno"], [
	AC_DEFINE_UNQUOTED(WITH_LIBCURL)
])

ARCH=
AS_IF([test "x$enable_64bit" == "xyes"], [
	[ARCH="-m64"]
])
AC_SUBST(ARCH)

EXCLUDE=
AS_IF([test "x$enable_threading" == "xno"], [
	AC_DEFINE_UNQUOTED(THREADING_DISABLED)
	[EXCLUDE=" -o -name threading "]
])
AS_IF([test "x$enable_sockets" == "xno"], [
	AC_DEFINE_UNQUOTED(SOCKETS_DISABLED)
	[EXCLUDE="$EXCLUDE -o -name socket "]
])
AS_IF([test "x$enable_debug" == "xno"], [
	AC_DEFINE_UNQUOTED(DEBUG_DISABLED)
	[EXCLUDE="$EXCLUDE -o -name debug "]
])
AS_IF([test "x$enable_encryption" == "xno"], [
	AC_DEFINE_UNQUOTED(ENCRYPTION_DISABLED)
	[EXCLUDE="$EXCLUDE -o -name encryption "]
])
AS_IF([test "x$enable_io" == "xno"], [
	AC_DEFINE_UNQUOTED(IO_DISABLED)
	[EXCLUDE="$EXCLUDE -o -name io "]
])
AC_SUBST(EXCLUDE)

TOOLPATH=
AC_ARG_WITH(
    [toolpath],
    AS_HELP_STRING(
	[--with-toolpath=PATH],
	[set the path to makefile tools]
    ),
    [TOOLPATH="$withval"]
)
AC_SUBST(TOOLPATH)

AC_CONFIG_HEADERS(config.h)

AC_OUTPUT(Makefile)
