.SUFFIXES:	.cxx .h .o

.cxx.o :
	$(CC) $(CFLAGS) -c $<

#
#  BINARY is the name of the thing we are trying to build
#
BINARY = libbrachy.a
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
#  INSDIR is the absolute path name of the directory in which the
#  binary is to be installed
#
ARCH = `arch -k`
INSDIR = $(UP)/lib/$(ARCH)
#
#  CFLAGS are options to the c compiler
#
CFLAGS = $(HFLAGS) -I$(UP)/src/include
#
#  SRCS is a list of .c files for use by 'make depend'
#
SRCS =  Sievert.cxx \
	read_sources.cxx \
	read_seeds.cxx \
	seed_pdose.cxx \
	source_pdose.cxx

#  OBJS is a list of .o (and possibly other) files on which the binary
#  depends
#
OBJS =  $(SRCS:.cxx=.o)

$(BINARY): $(OBJS)
	-rm -f $(BINARY)
	ar rvc $(BINARY) $(OBJS)
	-ar ts $(BINARY)

$(OBJS):

install: $(BINARY)
	-rm -f $(INSDIR)/$(BINARY) 
	mv $(BINARY) $(INSDIR)
	cp libbrachy.h $(UP)/src/include/

clean:
	-rm -f $(BINARY) *.o *~ core ./*,v \#colist

# DO NOT DELETE THIS LINE - make depend depends on it
 
