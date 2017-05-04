/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Defined if libavcodec is available */
#define HAVE_AVCODEC 1

/* Defined if libavformat is available */
#define HAVE_AVFORMAT 1

/* Defined if libavutil is available */
#define HAVE_AVUTIL 1

/* Defined if cairo is available */
/* #undef HAVE_CAIRO */

/* Define to 1 if you have the `clock_gettime' function. */
#define HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <drm_fourcc.h> header file. */
/* #undef HAVE_DRM_FOURCC_H */

/* Define to 1 if you have the <ffmpeg/avcodec.h> header file. */
/* #undef HAVE_FFMPEG_AVCODEC_H */

/* Define to 1 if you have the <ffmpeg/avformat.h> header file. */
/* #undef HAVE_FFMPEG_AVFORMAT_H */

/* Define to 1 if you have the <ffmpeg/avutil.h> header file. */
/* #undef HAVE_FFMPEG_AVUTIL_H */

/* Define to 1 if you have the <ffmpeg/swscale.h> header file. */
/* #undef HAVE_FFMPEG_SWSCALE_H */

/* Define to 1 if you have the <GL/glu.h> header file. */
/* #undef HAVE_GL_GLU_H */

/* Define to 1 if you have the <GL/glx.h> header file. */
/* #undef HAVE_GL_GLX_H */

/* Define to 1 if you have the <GL/gl.h> header file. */
/* #undef HAVE_GL_GL_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <libavcodec/avcodec.h> header file. */
#define HAVE_LIBAVCODEC_AVCODEC_H 1

/* Define to 1 if you have the <libavcodec/vaapi.h> header file. */
#define HAVE_LIBAVCODEC_VAAPI_H 1

/* Define to 1 if you have the <libavformat/avformat.h> header file. */
#define HAVE_LIBAVFORMAT_AVFORMAT_H 1

/* Define to 1 if you have the <libavutil/pixfmt.h> header file. */
#define HAVE_LIBAVUTIL_PIXFMT_H 1

/* Define to 1 if you have the `c_r' library (-lc_r). */
/* #undef HAVE_LIBC_R */

/* Define to 1 if you have the `pthread' library (-lpthread). */
#define HAVE_LIBPTHREAD 1

/* Define to 1 if you have the `PTL' library (-lPTL). */
/* #undef HAVE_LIBPTL */

/* Define to 1 if you have the `rt' library (-lrt). */
#define HAVE_LIBRT 1

/* Define to 1 if you have the <libswscale/swscale.h> header file. */
#define HAVE_LIBSWSCALE_SWSCALE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Defined if pthreads are available */
#define HAVE_PTHREADS 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Defined if libswscale is available */
#define HAVE_SWSCALE 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* VDPAU/MPEG-4 support */
#define HAVE_VDPAU_MPEG4 0

/* Major version of the driver */
#define HWDECODE_DEMOS_MAJOR_VERSION 0

/* Micro version of the driver */
#define HWDECODE_DEMOS_MICRO_VERSION 6

/* Minor version of the driver */
#define HWDECODE_DEMOS_MINOR_VERSION 9

/* Preversion of the driver */
#define HWDECODE_DEMOS_PRE_VERSION 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "hwdecode-demos"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "gbeauchesne@splitted-desktop.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "hwdecode_demos"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "hwdecode_demos 0.9.6.pre1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "hwdecode-demos"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.9.6.pre1"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Defined if DRM is enabled */
/* #undef USE_DRM */

/* Defined if GLX is enabled */
/* #undef USE_GLX */

/* Defined if X11 is enabled */
#define USE_X11 1
//#define USE_GLX 1
//#define USE_DRM 1
#define USE_VAAPI 1
#define USE_FFMPEG 1
#define USE_VAAPI_X11 1
/* Version number of package */
#define VERSION "0.9.6.pre1"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */
