
.SUFFIXES:	.cxx .h .o

.cxx.o :
	$(CC) $(CFLAGS) -c $<
#
#  BINARY is the name of the thing we are trying to build
#
BINARY = brachy_dose
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

#
#  SRCS is a list of .c files for use by 'make depend'
#
SRCS =	brachy_dose.cxx \
	calc_norm_mat.cxx \
	calc_norm_vec.cxx \
	output.cxx \
	setup.cxx


#  OBJS is a list of .o (and possibly other) files on which the binary
#  depends
#
OBJS = $(SRCS:.cxx=.o)

#
#  LIBS is a list of libraries to be handed to ld (in -lfoo form)
#
LIBS = -lbrachy -lplan -lplanio -l3d -lm $(LTHREAD) $(LSUN)

#
#  HDRS is a list of .h (or other) files that need to be checked out
#  to 'make depend'
#
HDRS = defines.h geometry.h xinfo.h cross.h \
	tint_extern.h event_subr_extern.h cont2grid_extern.h image_button_callback_extern.h

$(BINARY): $(OBJS)
	-rm -f $(BINARY)
	$(CC)  -o $(BINARY) $(CFLAGS) $(OBJS) $(LIBS)

$(OBJS):


depend: $(HDRS) $(SRCS) $(DEPEND)

depend42:
	grep '^#include' ${SRCS} | grep -v '<' | \
	sed -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
	    -e 's/\.c/.o/' \
	    -e 's/\.s:/.o:/' \
	    -e 's,../[a-zA-Z]*/,,' >xtmp
	awk '$$2~/\/data\// {printf "%s ",$$2}' xtmp >xtmp1
	-if [ -s xtmp1 ]; \
	then \
		grep '^#include' `echo ``cat xtmp1``` | grep -v '<' | \
		sed -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
	    	    -e 's/_data\.c/.o/' \
	    	    -e 's,../[a-zA-Z]*/,,' >>xtmp ; \
	else \
		true; \
	fi
	-rm -f xtmp1
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep <xtmp
	-rm -f xtmp
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp ${MAKEFILE} ${MAKEFILE}.bak
	ed - ${MAKEFILE} < eddep
	rm eddep makedep

depend43: 
	${CC} -M ${CFLAGS} ${SRCS} | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
	        else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	END { print rec } ' > makedep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp ${MAKEFILE} ${MAKEFILE}.bak
	ex - ${MAKEFILE} < eddep
	rm eddep makedep

undepend: 
	echo ' ' > makedep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp ${MAKEFILE} ${MAKEFILE}.bak
	ex - ${MAKEFILE} < eddep
	rm eddep makedep

install: $(BINARY)
	-rm -f $(INSDIR)/$(BINARY)
	mv $(BINARY) $(INSDIR)/$(BINARY)

clean:
	-rm -f $(BINARY) *.o *~ core ./*,v \#colist

# DO NOT DELETE THIS LINE - make depend depends on it
 
