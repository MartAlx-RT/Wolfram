//#include "Fcomp.h"
#include "Wolfram.h"


int Fcomp(const double val, const double ref_val, const double prec)
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

int NodeNumEqTo(const double ref_num, const node_t *node)
{
	if(node == NULL || node->op.type != OP_NUM)
		return 0;

	return !Fcomp(node->op.val.num, ref_num, EPS);
}
