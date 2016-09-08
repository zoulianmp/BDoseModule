
#define MAIN

#include "libbrachy.h"


int
main(int argc, char **argv)
{
	int			seed_num;
	SEED_SPEC	*seed_list;

	read_seeds(&seed_num, &seed_list);

	//read_seeds(&num_seed_list, &seed_list);


	//seed_pdose(seed_list + type, state.exact,pt.x, pt.y, pt.z);

	return 0;

   
}

