
.SUFFIXES: .cxx .h .o

.cxx.o :
	$(CC) $(CFLAGS) -c $<

ARCH = `arch -k`
CFLAGS = $(HFLAGS) -I$(UP)/src/include -L$(UP)/lib/$(ARCH)

PLANLIBS = -lbrachy -lplan -lplanio -l3d $(LSUN)

all: make_ls_dat make_seed_dat

make_ls_dat: make_ls_dat.o
	$(CC) $(CFLAGS) -o make_ls_dat make_ls_dat.o $(PLANLIBS) -lm

make_ls_dat.o: make_ls_dat.cxx
	$(CC) $(CFLAGS) -c make_ls_dat.cxx

make_seed_dat: make_seed_dat.o
	$(CC) $(CFLAGS) -o make_seed_dat make_seed_dat.o $(PLANLIBS) -lm

make_seed_dat.o: make_seed_dat.cxx
	$(CC) $(CFLAGS) -c make_seed_dat.cxx

install: make_ls_dat make_seed_dat
	make_seed_dat
	make_ls_dat
	rm -f *.o make_ls_dat make_seed_dat

clean:
	rm -f *.o make_ls_dat make_seed_dat

