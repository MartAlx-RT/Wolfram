#include "Wolfram.h"
#include "DSLdef.h"


node_t *TakeDeriv(const node_t *tree, const char d_var)
{
	if(tree == NULL)
		return NULL;

	node_t *d_node = NULL;


	switch(tree->op.type)
	{
	case OP_NUM:
		return NUM(0);

	case OP_VAR:
		return NUM((tree->op.val.var == d_var) ? 1 : 0);

	case OP_ARIFM:
		if(tree->left == NULL || tree->right == NULL)
		{
			print_err_msg("operation, but not binary...");
			return NULL;
		}
		
		switch (tree->op.val.arifm)
		{
		case AR_ADD:
			return ADD_(dL, dR);
		case AR_SUB:
			return SUB_(dL, dR);

		case AR_MUL:
			return ADD_(MUL_(dL, cR), MUL_(cL, dR));
		
		case AR_DIV:
			return DIV_(SUB_(MUL_(cR, dL), MUL_(cL, dR)), POW_(cR, NUM(2)));
		
		case AR_POW:
			if(FindNode(tree->right, (const op_t){.type = OP_VAR, .val.var = d_var}))
				return MUL_(cC, ADD_(MUL_(LN_(cL), dR), MUL_(DIV_(cR, cL), dL)));

			return MUL_(MUL_(cR, POW_(cL, SUB_(cR, NUM(1)))), dL);

		default:
			print_err_msg("tree wrong data (hint: op.val.arifm is out of range arifm_t)");
			free(d_node);
			return NULL;
		}
		
		break;
	case OP_ELFUNC:
		switch(tree->op.val.elfunc)
		{
		case F_LN:
			return MUL_(DIV_(NUM(1), cR), dR);
		case F_SIN:
			return MUL_(COS_(cR), dR);
		case F_COS:
			return MUL_(NUM(-1), MUL_(SIN_(cR), dR));
		case F_TAN:
			return MUL_(DIV_(NUM(1), POW_(COS_(cR), NUM(2))), dR);
		case F_SINH:
			return MUL_(COSH_(cR), dR);
		case F_COSH:
			return MUL_(SINH_(cR), dR);
		case F_TANH:
			return MUL_(DIV_(NUM(1), POW_(COSH_(cR), NUM(2))), dR);
		case F_ASIN:
			return MUL_(DIV_(NUM(1), POW_(SUB_(NUM(1), POW_(cR, NUM(2))), NUM(0.5))), dR);
		case F_ACOS:
			return MUL_(NUM(-1), MUL_(DIV_(NUM(1), POW_(SUB_(NUM(1), POW_(cR, NUM(2))), NUM(0.5))), dR));
		case F_ATAN:
			return MUL_(DIV_(NUM(1), (ADD_(NUM(1), POW_(cR, NUM(2))))), dR);
		case F_ASINH:
			return MUL_(DIV_(NUM(1), POW_(ADD_(POW_(cR, NUM(2)), NUM(1)), NUM(0.5))), dR);
		case F_ACOSH:
			return MUL_(DIV_(NUM(1), POW_(SUB_(POW_(cR, NUM(2)), NUM(1)), NUM(0.5))), dR);
		case F_ATANH:
			return MUL_(DIV_(NUM(1), (SUB_(NUM(1), POW_(cR, NUM(2))))), dR);
		case N_ELFUNC:
		default:
			print_err_msg("tree wrong data (hint: op.val.elfunc is out of range elfunc_t)");
			return NULL;
		}
		
		break;
	
	case N_OP:
	default:
		print_err_msg("tree wrong data (hint: op.type is out of range op_type_t)");
		return NULL;
		
		break;
	}

	return d_node;
}


#include "DSLundef.h"
