ACLOCAL_AMFLAGS = --install -I m4

lib_LIBRARIES = libbullfrog.a

libbullfrog_a_SOURCES = \
  src/general/arctan.c \
  src/general/box_clip.c \
  src/general/gbox.cpp \
  src/general/gbuffer.cpp \
  src/general/gcircle.cpp \
  src/general/gdata.c \
  src/general/gendian.c \
  src/general/gerrorlg.cpp \
  src/general/gexe_key.cpp \
  src/general/gfile.c \
  src/general/gfnuniq.c \
  src/general/ggenf.cpp \
  src/general/ggengh.cpp \
  src/general/giff.c \
  src/general/gkeybd.cpp \
  src/general/gline.cpp \
  src/general/glog.cpp \
  src/general/gmemut.c \
  src/general/gpalette.c \
  src/general/gpng.c \
  src/general/gscreen.c \
  src/general/gsprite.c \
  src/general/gspr_col.c \
  src/general/gspr_map.c \
  src/general/gspr_std.c \
  src/general/gstrut.c \
  src/general/gtext.cpp \
  src/general/gtrig.cpp \
  src/general/gutility.cpp \
  src/general/hvline.c \
  src/general/pixel.c \
  src/general/poly.c \
  src/general/rnc_1fm.c \
  src/general/rom.c \
  src/general/scr_copy.c \
  src/general/spr_scl.c \
  src/general/spr_scol.c \
  src/general/spr_smap.c \
  src/general/spr_sstd.c \
  src/general/spr_ssts.c \
  src/general/spr_ssta.c \
  src/general/sqroot.c

libbullfrog_a_SOURCES += \
  src/x86-win-sdl/sdir.c \
  src/x86-win-sdl/sdrive.c \
  src/x86-win-sdl/sfile.c \
  src/x86-win-sdl/skeybd.cpp \
  src/x86-win-sdl/smemory.c \
  src/x86-win-sdl/smouse.cpp \
  src/x86-win-sdl/spalette.cpp \
  src/x86-win-sdl/sscreen.cpp \
  src/x86-win-sdl/ssemphr.cpp \
  src/x86-win-sdl/stime.cpp \
  src/x86-win-sdl/svesa.cpp \
  src/x86-win-sdl/swindows.cpp

libbullfrog_a_CPPFLAGS = \
  -I$(top_srcdir)/include -I$(builddir)/include

include_HEADERS = $(wildcard include/*.h) $(wildcard include/*.hpp)
