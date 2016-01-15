SHELL=/bin/sh
BUILD_OPTS = UP=$(UP) HFLAGS="$(HFLAGS)"

SUBDIR = libbrachy phys_dat brachy brachy_dose

all:
	-for i in $(SUBDIR); do \
		(echo making $$i...; \
		cd $$i; \
		make $(GJB) $(BUILD_OPTS));\
	done

clean:
	-for i in $(SUBDIR); do \
		(echo cleaning $$i...; \
		cd $$i; \
		make clean);\
	done

install:
	-for i in $(SUBDIR); do \
		(echo installing $$i...; \
		cd $$i; \
		make $(GJB) install $(BUILD_OPTS));\
	done

