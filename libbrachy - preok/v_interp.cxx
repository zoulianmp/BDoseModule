
#include <stdio.h>




int bin_search(float value, float *vector, int vec_size);



float
v_interp(
    int		mode,
    int		vec_size,
    float	xvec[],
    float	x,
    float	yvec[],
    int		*index,
    float	*fx)
{
	*index = bin_search(x, xvec, vec_size);
	if (xvec[*index + 1] == xvec[*index]) *fx = 0.0;
	else *fx = (x - xvec[*index])/(xvec[*index + 1] - xvec[*index]);
	if (mode) {
		if (*fx > 1.0)
			*fx = 1.0;
		if (*fx < 0.0)
			*fx = 0.0;
	}

	return(yvec[*index] + (yvec[*index + 1] - yvec[*index]) * *fx);
}
