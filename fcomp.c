#include "fcomp.h"


int fcomp(const double val, const double ref_val, const double prec)
{
	assert(prec > 0);
	
	if(isnan(ref_val))
		return -2;
	
	if(isnan(val))
		return 2;

	if (val - ref_val < -prec)
		return -1;
	
	if(val - ref_val > prec)
		return 1;

	return 0;
}