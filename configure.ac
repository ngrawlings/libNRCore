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

AC_ARG_ENABLE([http], AS_HELP_STRING([--disable-http], [Disable feature http]))

AS_IF([test "x$enable_http" != "xno"], [
	AC_DEFINE_UNQUOTED(WITH_LIBCURL)
])

AC_OUTPUT(Makefile)
