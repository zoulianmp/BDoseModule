
#define MAIN

#include "defines.h"


int
main(int argc, char **argv)
{

    init(argc, argv);
    make_windows(argc, argv);
    while (1) {
	Fl::wait();
    }
    return(0);
}

