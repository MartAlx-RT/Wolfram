#include "Wolfram.h"

static double _Calc(const node_t *tree, const double var_val[], size_t *call_count)
{
	assert(call_count);
	
	if (tree == NULL || (*call_count)++ > MAX_REC_DEPTH)
		return NAN;
	
	switch(tree->op.type)
	{
	case OP_ARIFM:
		switch(tree->op.val.arifm)
		{
		case AR_ADD:
			return _Calc(tree->left, var_val, call_count) + _Calc(tree->right, var_val, call_count);
		case AR_DIV:
			return _Calc(tree->left, var_val, call_count) / _Calc(tree->right, var_val, call_count);
		case AR_MUL:
			return _Calc(tree->left, var_val, call_count) * _Calc(tree->right, var_val, call_count);
		case AR_POW:
			return pow(_Calc(tree->left, var_val, call_count), _Calc(tree->right, var_val, call_count));
		case AR_MIN:
			return (-1) * _Calc(tree->right, var_val, call_count);
		case AR_SUB:
			return _Calc(tree->left, var_val, call_count) - _Calc(tree->right, var_val, call_count);
		default:
			return NAN;
		}
	case OP_ELFUNC:
		switch(tree->op.val.elfunc)
		{
		case F_ACOS:
			return acos(_Calc(tree->right, var_val, call_count));
		case F_ACOSH:
			return acosh(_Calc(tree->right, var_val, call_count));
		case F_ASIN:
			return asin(_Calc(tree->right, var_val, call_count));
		case F_ASINH:
			return asinh(_Calc(tree->right, var_val, call_count));
		case F_ATAN:
			return atan(_Calc(tree->right, var_val, call_count));
		case F_ATANH:
			return atanh(_Calc(tree->right, var_val, call_count));
		case F_COS:
			return cos(_Calc(tree->right, var_val, call_count));
		case F_COSH:
			return cosh(_Calc(tree->right, var_val, call_count));
		case F_LN:
			return log(_Calc(tree->right, var_val, call_count));
		case F_SIN:
			return sin(_Calc(tree->right, var_val, call_count));
		case F_SINH:
			return sinh(_Calc(tree->right, var_val, call_count));
		case F_TAN:
			return tan(_Calc(tree->right, var_val, call_count));
		case F_TANH:
			return tanh(_Calc(tree->right, var_val, call_count));
		
		case N_ELFUNC:
		default:
			return NAN;
		}
	case OP_NUM:
		return tree->op.val.num;
	case OP_VAR:
		if(var_val)
			return var_val[(int)tree->op.val.var];
		return NAN;
	
	case N_OP:
	default:
		return NAN;
	}

	return NAN;
}

int FracApprox(double x, frac_t *frac)
{
	if(isnan(x) || isinf(x) || frac == NULL)
		return 0;
	
	if(Feq(x - trunc(x), 0, EPS))
		return 0;
	/*--------------------------*/
	
	if (x < 0)
	{
		x = -x;
		frac->sgn = -1;
	}
	else
		frac->sgn = +1;

	size_t denomer = 2;
	for (; !Feq((double)denomer * x - trunc((double)denomer * x), 0, EPS); denomer++);

	frac->denomer = denomer;
	frac->numer = (size_t)((double)denomer * x);
	return 1;
}

/* entrance to recursion */
double Calc(const node_t *tree, const double var_val[])
{
	size_t call_count = 0;
	return _Calc(tree, var_val, &call_count);
}


