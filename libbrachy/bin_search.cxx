#include <stdio.h>

/*
  WORTH_IT is the threshold above which you get binary search and below
  which we do linear search.
*/
#define WORTH_IT (100)


int
bin_search(float value, float *vector, int vec_size)
{
    if (value < vector[1])
	return(0);
    else if (value >= vector[vec_size - 2])
	return(vec_size - 2);
    
    if (vec_size >= WORTH_IT)
    {
	register int low, high, middle;
	
	low = 1;
	high = vec_size - 3;
	
	while (low != high)
	{
	    middle = (high + low + 1) / 2;
	    
	    if (value < vector[middle])
		high = middle - 1;
	    else if (value > vector[middle])
		low = middle;
	    else
		return(middle);
	}
	return(low);
    }
    else
    {
	static int last_time = 0,
                   last_vec_size = 0;
	register int index;

/*
  Let's take a chance that the tables involved will be used in some
  orderly fashion - in particular in ascending order.  We do the
  following:

  1. Test to make sure that our previous choice is inside the bounds
     of the current vector.  It would be nice to be able to check that
     it is the *same* vector as last time but that would be too expensive.

  2. Try the index we found last time on for size.  If we are moving
     along the vector in an orderly fashion, we stand a good chance of
     finding the index in vec_size / 2 tries by my reckoning.

  3. If the index we found on the last call is no good, do a search
     from the start of the vector.  This is a big lose if the vector
     is used in any order other than increasing - it is a couple of
     comparisons worse than a regular linear search.
*/

	if (last_vec_size == vec_size)
	{
	    if (value >= vector[last_time])
		index = last_time + 1;
	    else
		index = 1;
	}
	else
	{
	    last_vec_size = vec_size;
	    index = 1;
	}

	for (; index < vec_size - 1; index++)
	{
	    if (value < vector[index])
		break;
	}

#ifdef EBUG
	fprintf (stderr, "%f is between %f and %f\n", value, vector[index-1], vector[index]);
#endif

	return (last_time = (index - 1));
    }
}

