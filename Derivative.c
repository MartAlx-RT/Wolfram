#include "Tree.h"

#include "DSLdef.h"

static node_t *_TreeCopy(const node_t *tree, size_t *call_count)
{
	assert(call_count);
	
	if (tree == NULL || (*call_count)++ > MAX_REC_DEPTH)
		return NULL;	
	/*--------------------------------------------------*/
	
	node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
	
	new_node->op.type = tree->op.type;
	new_node->op.val = tree->op.val;
	
	if(tree->left)
	{
		new_node->left = _TreeCopy(tree->left, call_count);
		assert(new_node->left);
		new_node->left->parent = new_node;
	}
	
	if(tree->right)
	{
		new_node->right = _TreeCopy(tree->right, call_count);
		assert(new_node->right);
		new_node->right->parent = new_node;
	}
	

	return new_node;
}

node_t *TreeCopy(const node_t *tree)
{
	size_t call_count = 0;
	return _TreeCopy(tree, &call_count);
}


static node_t *_FindNode(node_t *tree, const op_t op, size_t *call_count)
{
	assert(call_count);
	
	if (tree == NULL || (*call_count)++ > MAX_REC_DEPTH)
		return NULL;

	if(memcmp(&(tree->op), &op, sizeof(op_t)) == 0)
		return tree;

	node_t *l_search = _FindNode(tree->left, op, call_count);
	if(l_search)
		return l_search;

	return _FindNode(tree->right, op, call_count);
}

node_t *FindNode(node_t *tree, const op_t op)
{
	size_t call_count = 0;
	return _FindNode(tree, op, &call_count);
}

node_t *NewNode(const op_t op, node_t *left, node_t *right)
{
	if(op.type >= N_OP)
		return NULL;

	node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
	assert(new_node);

	// new_node->op.type = op.type;
	// new_node->op.val = op.val;
	new_node->op = op;

	new_node->left = left;

	//printf("op.type = %d, op.val = %d\n", op.type, op.val);

	if(left)
		new_node->left->parent = new_node;
	new_node->right = right;
	if(right)
		new_node->right->parent = new_node;

	return new_node;
}

node_t *Deriv(const node_t *node, const char d_var)
{
	if(node == NULL)
		return NULL;

	node_t *d_node = NULL;


	if(node->op.type == OP_NUM)
	{
		d_node = (node_t *)calloc(1, sizeof(node_t));
		
		d_node->op.type = OP_NUM;
		d_node->op.val.num = 0;
	}
	else if(node->op.type == OP_VAR)
	{
		d_node = (node_t *)calloc(1, sizeof(node_t));
		
		d_node->op.type = OP_NUM;
		d_node->op.val.num = (node->op.val.var == d_var) ? 1 : 0;
	}
	else if(node->op.type == OP_ARIFM)
	{
		if(node->left == NULL || node->right == NULL)
		{
			printf("operation, but not binary...\n");
			return NULL;
		}
		
		switch (node->op.val.arifm)
		{
		case AR_ADD:
		case AR_SUB:
			d_node = (node_t *)calloc(1, sizeof(node_t));
			
			d_node->op.type = OP_ARIFM;
			d_node->op.val = node->op.val;
			d_node->left = dL;
			d_node->right = dR;
			
			if(d_node->left)
				d_node->left->parent = d_node;
			
			if(d_node->right)
				d_node->right->parent = d_node;
			
			else
			{
				printf("what???\n");
				free(d_node);
				return NULL;
			}
			break;
		
		case AR_MUL:
			return ADD_(MUL_(dL, cR), MUL_(cL, dR));
		
		case AR_DIV:
			return DIV_(SUB_(MUL_(cR, dL), MUL_(cL, dR)), POW_(cR, NUM(2)));
		
		case AR_POW:
			if(FindNode(node->right, (const op_t){.type = OP_VAR, .val.var = d_var}))
				return MUL_(cC, ADD_(MUL_(LN_(cL), dR), MUL_(DIV_(cR, cL), dL)));

			return MUL_(MUL_(cR, POW_(cL, SUB_(cR, NUM(1)))), dL);

		default:
			printf("what???\n");
			free(d_node);
			return NULL;
		}
	}
	else if(node->op.type == OP_ELFUNC)
	{
		switch(node->op.val.elfunc)
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
			printf("what function?\n");
			return NULL;
		}
	}
	else
	{
		printf("and what operation did you mean?\n");
		return NULL;
	}

	return d_node;
}

static double _Calculate(const node_t *tree, const double var_val[], size_t *call_count)
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
			return _Calculate(tree->left, var_val, call_count) + _Calculate(tree->right, var_val, call_count);
		case AR_DIV:
			return _Calculate(tree->left, var_val, call_count) / _Calculate(tree->right, var_val, call_count);
		case AR_MUL:
			return _Calculate(tree->left, var_val, call_count) * _Calculate(tree->right, var_val, call_count);
		case AR_POW:
			return pow(_Calculate(tree->left, var_val, call_count), _Calculate(tree->right, var_val, call_count));
		case AR_SUB:
			return _Calculate(tree->left, var_val, call_count) - _Calculate(tree->right, var_val, call_count);
		default:
			return NAN;
		}
	case OP_ELFUNC:
		switch(tree->op.val.elfunc)
		{
		case F_ACOS:
			return acos(_Calculate(tree->right, var_val, call_count));
		case F_ACOSH:
			return acosh(_Calculate(tree->right, var_val, call_count));
		case F_ASIN:
			return asin(_Calculate(tree->right, var_val, call_count));
		case F_ATAN:
			return atan(_Calculate(tree->right, var_val, call_count));
		case F_ATANH:
			return atanh(_Calculate(tree->right, var_val, call_count));
		case F_COS:
			return cos(_Calculate(tree->right, var_val, call_count));
		case F_COSH:
			return cosh(_Calculate(tree->right, var_val, call_count));
		case F_LN:
			return log(_Calculate(tree->right, var_val, call_count));
		case F_SIN:
			return sin(_Calculate(tree->right, var_val, call_count));
		case F_SINH:
			return sinh(_Calculate(tree->right, var_val, call_count));
		case F_TAN:
			return tan(_Calculate(tree->right, var_val, call_count));
		case F_TANH:
			return tanh(_Calculate(tree->right, var_val, call_count));
		default:
			return NAN;
		}
	case OP_NUM:
		return tree->op.val.num;
	case OP_VAR:
		if(var_val)
			return var_val[tree->op.val.var];
		return NAN;
	default:
		return NAN;
	}

	return NAN;
}

double Calculate(const node_t *tree, const double var_val[])
{
	size_t call_count = 0;
	return _Calculate(tree, var_val, &call_count);
}

/*
void Symplify(node_t *node)
{
	if(node == NULL || node->parent == NULL)
		return;

	if(node->left == NULL && node->right == NULL)
		return;

	double l_val = Calculate(node->left, NULL);
	double r_val = Calculate(node->right, NULL);
	
	if(isnan(l_val) && isnan(r_val))
	{
		if(node->left && node->right && node->op.type == OP_ARIFM && node->left->op.val.var == node->right->op.val.var)
		{
			switch(node->op.val.arifm)
			{
			case AR_ADD:
				node->op.val.arifm = AR_MUL;
				node->left->op.type = OP_NUM;
				node->left->op.val.num = 2;
				break;
			case AR_MUL:
				node->op.val.arifm = AR_POW;
				node->right->op.type = OP_NUM;
				node->right->op.val.num = 2;
				break;
			case AR_DIV:
				node->op.type = OP_NUM;
				node->op.val.num = 1;

				TreeDestroy(node->left);
				node->left = NULL;
				TreeDestroy(node->right);
				node->right = NULL;
				break;
			case AR_SUB:
				node->op.type = OP_NUM;
				node->op.val.num = 0;
				
				TreeDestroy(node->left);
				node->left = NULL;
				TreeDestroy(node->right);
				node->right = NULL;
				break;
			case AR_POW:
			default:
				break;
			}
		}
		else
			return;
	}
	else if(isnan(l_val))
	{
		if(node->op.type != OP_ARIFM)
		{
			printf("how?\n");
			return;
		}
		
		switch(node->op.val.arifm)
		{
		case AR_ADD:
			if(fcomp(r_val, 0, EPS) == 0)
			{
				
			}
		}
	}
}
*/

static void TieLeftToParent(node_t *node)
{
	assert(node);
	assert(node->parent);

	if(node->parent->left == node)
	{
		node->left->parent = node->parent;
		node->parent->left = node->left;
	}
	else if(node->parent->right == node)
	{
		node->left->parent = node->parent;
		node->parent->right = node->left;
	}
	else if(node->parent == node)
	{
		free(node->right);
		node->right = node->left->right;
		node->left = node->left->left;
		free(node->left->parent);

		node->left->parent = node->right->parent = node;
		return;
	}
	else
	{
		printf("what a hell\n");
		return;
	}

	free(node->right);
	free(node);
}

static void TieRightToParent(node_t *node)
{
	assert(node);
	assert(node->left);
	assert(node->right);

	if(node->parent->left == node)
	{
		node->right->parent = node->parent;
		node->parent->left = node->right;
	}
	else if(node->parent->right == node)
	{
		node->right->parent = node->parent;
		node->parent->right = node->right;
	}
	else if(node->parent == node)
	{
		free(node->left);
		node->left = node->right->left;
		node->right = node->right->right;
		free(node->left->parent);

		node->left->parent = node->right->parent = node;
		return;
	}
	else
	{
		printf("what a hell\n");
		return;
	}

	free(node->left);
	free(node);
}

static size_t _FoldConst(node_t *tree, size_t *call_count)
{
	assert(call_count);

	if(tree == NULL || (*call_count)++ > MAX_REC_DEPTH)
		return 0;

	double calc = 0;
	size_t n_fold = 0;

	switch(tree->op.type)
	{
	case OP_NUM:
		return 0;
	case OP_ARIFM:
		n_fold += _FoldConst(tree->left, call_count);
		n_fold += _FoldConst(tree->right, call_count);

		calc = Calculate(tree, NULL);
		if(!isnan(calc))
		{
			n_fold++;
			tree->op.type = OP_NUM;
			tree->op.val.num = calc;
			
			TreeDestroy(tree->left);
			TreeDestroy(tree->right);
			tree->left = tree->right = NULL;
		}
		break;
	case OP_ELFUNC:
		n_fold += _FoldConst(tree->right, call_count);
		break;
	case OP_VAR:
	default:
		break;
	}

	return n_fold;
}

size_t FoldConst(node_t *tree)
{
	size_t call_count = 0;
	return _FoldConst(tree, &call_count);
}

static size_t _FoldNeutral(node_t *tree, size_t *call_count)
{
	assert(call_count);
	
	if (tree == NULL || (*call_count)++ > MAX_REC_DEPTH || tree->left == NULL || tree->right == NULL)
		return 0;
	
	
	/*-----------------------------------------*/
	size_t n_fold = 0;

	if(tree->left == NULL || tree->right == NULL)
	{
		printf("but how?\n");
		return 0;
	}
	
	n_fold += _FoldNeutral(tree->left, call_count);
	n_fold += _FoldNeutral(tree->right, call_count);
	
	if(tree->op.type != OP_ARIFM)
		return n_fold;
	
	if(tree->left == NULL || tree->right == NULL)
	{
		printf("but how?\n");
		return 0;
	}

	switch(tree->op.val.arifm)
	{
	case AR_ADD:
	case AR_SUB:
		if (tree->left->op.type == OP_NUM && fcomp(tree->left->op.val.num, 0, EPS) == 0)
		{
			TieRightToParent(tree);
			n_fold++;
		}
		else if(tree->right->op.type == OP_NUM && fcomp(tree->right->op.val.num, 0, EPS) == 0)
		{
			TieLeftToParent(tree);
			n_fold++;
		}
		break;
	case AR_MUL:
		if(tree->left->op.type == OP_NUM && fcomp(tree->left->op.val.num, 0, EPS) == 0
		||
		tree->right->op.type == OP_NUM && fcomp(tree->right->op.val.num, 0, EPS) == 0)
		{
			tree->op.type = OP_NUM;
			tree->op.val.num = 0;
			
			TreeDestroy(tree->left);
			TreeDestroy(tree->right);
			tree->left = tree->right = NULL;
			
			n_fold++;
		}
		else if(tree->left->op.type == OP_NUM && fcomp(tree->left->op.val.num, 1, EPS) == 0)
		{
			TieRightToParent(tree);
			n_fold++;
		}
		else if(tree->right->op.type == OP_NUM && fcomp(tree->right->op.val.num, 1, EPS) == 0)
		{
			TieLeftToParent(tree);
			n_fold++;
		}
		break;
	case AR_DIV:
		if(tree->left->op.type == OP_NUM && fcomp(tree->left->op.val.num, 0, EPS) == 0)
		{
			tree->op.type = OP_NUM;
			tree->op.val.num = 0;
			
			TreeDestroy(tree->left);
			TreeDestroy(tree->right);
			tree->left = tree->right = NULL;
			
			n_fold++;
		}
		else if(tree->right->op.type == OP_NUM && fcomp(tree->right->op.val.num, 1, EPS) == 0)
		{
			TieLeftToParent(tree);
			n_fold++;
		}
		break;
	case AR_POW:
		if(tree->right->op.type == OP_NUM && fcomp(tree->right->op.val.num, 1, EPS) == 0)
		{
			TieLeftToParent(tree);
			n_fold++;
		}
		else if(tree->right->op.type == OP_NUM && fcomp(tree->right->op.val.num, 0, EPS) == 0
		||
		tree->left->op.type == OP_NUM && fcomp(tree->left->op.val.num, 1, EPS) == 0)
		{
			tree->op.type = OP_NUM;
			tree->op.val.num = 1;
			
			TreeDestroy(tree->left);
			TreeDestroy(tree->right);
			tree->left = tree->right = NULL;
			
			n_fold++;
		}
		else if(tree->left->op.type == OP_NUM && fcomp(tree->left->op.val.num, 0, EPS) == 0)
		{
			tree->op.type = OP_NUM;
			tree->op.val.num = 0;
			
			TreeDestroy(tree->left);
			TreeDestroy(tree->right);
			tree->left = tree->right = NULL;
			
			n_fold++;
		}
		break;
	default:
		printf("and what's else?\n");
		break;
	}

	return n_fold;
}

size_t FoldNeutral(node_t *tree)
{
	size_t call_count = 0;
	return _FoldNeutral(tree, &call_count);
}

#include "DSLundef.h"
