#include "Wolfram.h"





static size_t _FoldConst(node_t *tree, size_t *call_count, double var_val[], const char d_var)
{
	assert(call_count);

	if(tree == NULL || var_val == NULL || (*call_count)++ > MAX_REC_DEPTH)
		return 0;

	double calc = 0;
	size_t n_fold = 0;

	switch (tree->op.type)
	{
	case OP_NUM:
		return 0;
	case OP_ARIFM:
	case OP_ELFUNC:
	case OP_VAR:
		n_fold += _FoldConst(tree->left, call_count, var_val, d_var);
		n_fold += _FoldConst(tree->right, call_count, var_val, d_var);

		calc = Calc(tree, var_val);
		if(!isnan(calc) && !isinf(calc))
		{
			n_fold++;
			tree->op.type = OP_NUM;
			tree->op.val.num = calc;
			
			TreeDestroy(tree->left);
			TreeDestroy(tree->right);
			tree->left = tree->right = NULL;
		}
		
		break;
	case N_OP:
	default:
		break;
	}
	
	return n_fold;
}

/*----------------------------------------------------------------*/

static size_t FoldSUB(node_t *tree)
{
	assert(tree);
	
	if(NodeNumEqTo(0, tree->left))
	{
		tree->op.val.arifm = AR_MIN;
		TreeDestroy(tree->left);
		tree->left = NULL;
		// tree->left->op.val.num = -1;
	}
	else if(ISMIN(tree->right))
	{
		tree->op.val.arifm = AR_ADD;
		TieRightToParent(tree->right);
	}
	else if(NodeNumEqTo(0, tree->right))
	{
		TieLeftToParent(tree);
	}
	else
		return 0;

	return 1;
}

static size_t FoldADD(node_t *tree)
{
	assert(tree);
	
	if (NodeNumEqTo(0, tree->left))
	{
		TieRightToParent(tree);
	}
	else if(NodeNumEqTo(0, tree->right))
	{
		TieLeftToParent(tree);
	}
	else if(ISMIN(tree->right))
	{
		tree->op.val.arifm = AR_SUB;
		TieRightToParent(tree->right);
	}
	else
		return 0;

	return 1;
}

static size_t FoldMULDIV(node_t *tree, const arifm_t arifm)
{
	assert(tree);
	
	if(NodeNumEqTo(0, tree->left) || 
	  (NodeNumEqTo(0, tree->right) && arifm == AR_MUL))
	{
		tree->op.type = OP_NUM;
		tree->op.val.num = 0;
		
		TreeDestroy(tree->left);
		TreeDestroy(tree->right);
		tree->left = tree->right = NULL;
	}
	else if(NodeNumEqTo(1, tree->left) && arifm == AR_MUL)
	{
		TieRightToParent(tree);
	}
	else if(NodeNumEqTo(1, tree->right))
	{
		TieLeftToParent(tree);
	}
	else if(ISMIN(tree->left))
	{
		node_t *old_left = tree->left;
		tree->left = tree->right;
		tree->right = old_left;
		
		tree->op.val.arifm = AR_MIN;
		tree->right->op.val.arifm = arifm;
		tree->right->left = tree->left;
		tree->left = NULL;
		
		node_t *temp = tree->right->right;
		tree->right->right = tree->right->left;
		tree->right->left = temp;
	}
	else if(ISMIN(tree->right))
	{
		tree->op.val.arifm = AR_MIN;
		tree->right->op.val.arifm = arifm;
		tree->right->left = tree->left;
		tree->left = NULL;
	}
	else
		return 0;

	return 1;
}

static size_t FoldPOW(node_t *tree)
{
	assert(tree);
	
	if(NodeNumEqTo(1, tree->right))
	{
		TieLeftToParent(tree);
	}
	else if(NodeNumEqTo(0, tree->right) || NodeNumEqTo(1, tree->left))
	{
		tree->op.type = OP_NUM;
		tree->op.val.num = 1;
		
		TreeDestroy(tree->left);
		TreeDestroy(tree->right);
		tree->left = tree->right = NULL;
	}
	else if(NodeNumEqTo(0, tree->left))
	{
		tree->op.type = OP_NUM;
		tree->op.val.num = 0;
		
		TreeDestroy(tree->left);
		TreeDestroy(tree->right);
		tree->left = tree->right = NULL;
	}
	else
		return 0;

	return 1;
}

static size_t FoldMIN(node_t *tree)
{
	assert(tree);
	
	if(ISMIN(tree->right))
	{
		TieRightToParent(tree->right);
		TieRightToParent(tree);
	}
	else
		return 0;

	return 1;
}

static size_t _FoldNeutral(node_t *tree, size_t *call_count)
{
	assert(call_count);
	
	if (tree == NULL || (*call_count)++ > MAX_REC_DEPTH || tree->right == NULL)
		return 0;
	
	
	/*-----------------------------------------*/
	size_t n_fold = 0;
	
	n_fold += _FoldNeutral(tree->left, call_count);
	n_fold += _FoldNeutral(tree->right, call_count);
	
	if(tree->op.type != OP_ARIFM)
		return n_fold;
	
	// if(tree->left == NULL || tree->right == NULL)
	// {
	// 	print_err_msg("operation, but not binary...");
	// 	return 0;
	// }

	switch(tree->op.val.arifm)
	{
	case AR_SUB:
		n_fold += FoldSUB(tree);
		break;
	case AR_ADD:
		n_fold += FoldADD(tree);
		break;
	case AR_MUL:
		n_fold += FoldMULDIV(tree, AR_MUL);
		break;
	case AR_DIV:
		n_fold += FoldMULDIV(tree, AR_DIV);
		break;
	case AR_POW:
		n_fold += FoldPOW(tree);
		break;
	case AR_MIN:
		n_fold += FoldMIN(tree);
		break;
	default:
		print_err_msg("tree wrong data (hint: op.val.arifm is out of range arifm_t)");
		break;
	}

	return n_fold;
}

/* entrance to recursion */
size_t FoldConst(node_t *tree, double var_val[], const char d_var)
{
	if(var_val == NULL)
		return 0;
	
	size_t call_count = 0, n_fold = 0;
	const double old_dvar = var_val[(int)d_var];
	var_val[(int)d_var] = NAN;

	n_fold =  _FoldConst(tree, &call_count, var_val, d_var);
	var_val[(int)d_var] = old_dvar;

	return n_fold;
}

size_t FoldNeutral(node_t *tree)
{
	size_t call_count = 0;
	return _FoldNeutral(tree, &call_count);
}


