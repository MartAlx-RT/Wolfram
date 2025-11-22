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

/*
static node_t *_FindNode(node_t *tree, tree_elem_t elem, int (*comp)(const tree_elem_t, const tree_elem_t), size_t *call_count)
{
	assert(call_count);
	
	if (tree == NULL || (*call_count)++ > MAX_REC_DEPTH || comp == NULL)
		return NULL;

	if(comp(tree->data, elem) == 0)
		return tree;

	node_t *l_search = _FindNode(tree->left, elem, comp, call_count);
	if(l_search)
		return l_search;

	return _FindNode(tree->right, elem, comp, call_count);
}
*/

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
			return DIV_(SUB_(MUL_(cR, dL), MUL_(cL, dR)), POW_(cR, NewNode((const op_t){.type = OP_NUM, .val.num = 2}, NULL, NULL)));
		
		case AR_POW:
			break;

		default:
			printf("what???\n");
			free(d_node);
			return NULL;
		}
	}
	
	return d_node;
}

#include "DSLundef.h"
