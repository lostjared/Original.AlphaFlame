bin_PROGRAMS = alphaflame2cross
alphaflame2cross_SOURCES = alphaflame2cross.c mxfont.c pixelbuffer.c

# set the include path found by configure
INCLUDES= $(LIBSDL_CFLAGS) $(all_includes) -I${prefix}/include/SDL_stretch

# the library search path.
alphaflame2cross_LDFLAGS = $(LIBSDL_LIBS) $(all_libraries) -lSDL_stretch
