//#include "Fcomp.h"
#include "Wolfram.h"


int Feq(const double val1, const double val2, double prec)
{
	return (fabs(val1 - val2) < fabs(prec));
}

int NodeNumEqTo(const double ref_num, const node_t *node)
{
	if(node == NULL || node->op.type != OP_NUM)
		return 0;

	return Feq(node->op.val.num, ref_num, EPS);
}
