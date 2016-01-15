
.SUFFIXES:	.cxx .h .o

.cxx.o :
	$(CC) $(CFLAGS) -c $<
#
#  BINARY is the name of the thing we are trying to build
#
BINARY = brachy
#
#  INSOPTS is the list of options to install
#
INSOPTS = 
#
#  MAKEFILE is the name of this Makefile - I like makefile, others
#  like Makefile.
#
MAKEFILE = Makefile
#
#  DEPEND chooses the type of dependence munging that will be
#  performed when you say 'make depend'.  See notes further
#  downstream.
#
DEPEND = depend43
#
#  RCSDIR is the absolute pathname of the directory where RCS files
#  are to be found.
#
RCSDIR = ./RCS
#
#  INSDIR is the absolute path name of the directory in which the
#  binary is to be installed
#
SHELL=/bin/sh
ARCH = `arch -k`
INSDIR = $(UP)/bin/$(ARCH)
#
#  CFLAGS are options to the c compiler
#
CFLAGS = $(HFLAGS) -I$(UP)/src/include -L$(UP)/lib/$(ARCH)

#
#  SRCS is a list of .c files for use by 'make depend'
#
SRCS =	main.cxx \
	init.cxx \
	make_windows.cxx \
	mirror_cb.cxx \
	options_cb.cxx \
	output.cxx \
	read_image.cxx \
	rotate_cb.cxx \
	sample_image.cxx\
	seed_source_select.cxx \
	set_grid_cb.cxx \
	set_grid_plane_cb.cxx \
	spread_sheet.cxx \
	window.cxx

#
#  OBJS is a list of .o (and possibly other) files on which the binary
#  depends
#
OBJS = $(SRCS:.cxx=.o)

#  LIBS is a list of libraries to be handed to ld (in -lfoo form)
#
LIBS = -lbrachy -lntab -lcom -lplanX -lplan -lplanio -lps \
	$(LGL) -l3d -lm $(LTHREAD) $(LSUN)

#
#  HDRS is a list of .h (or other) files that need to be checked out
#  to 'make depend'
#
HDRS = defines.h geometry.h xinfo.h cross.h \
	tint_extern.h event_subr_extern.h cont2grid_extern.h image_button_callback_extern.h

$(OBJS): defines.h

$(BINARY): $(OBJS)
	-rm -f $(BINARY)
	$(CC)  -o $(BINARY) $(CFLAGS) $(OBJS) $(LIBS)

$(OBJS):


install: $(BINARY)
	-rm -f $(INSDIR)/$(BINARY)
	mv $(BINARY) $(INSDIR)/$(BINARY)
	$(MAKE_MAC) $(INSDIR)/$(BINARY)

clean:
	-rm -f $(BINARY) *.o *~ core ./*,v \#colist

# DO NOT DELETE THIS LINE - make depend depends on it
 
