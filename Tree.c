#include "Wolfram.h"

static node_t *NodeCopy(const node_t *node, size_t *call_count)
{
	assert(call_count);
	
	if (node == NULL || (*call_count)++ > MAX_REC_DEPTH)
		return NULL;	
	/*--------------------------------------------------*/
	
	node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
	
	new_node->op.type = node->op.type;
	new_node->op.val = node->op.val;
	
	if(node->left)
	{
		new_node->left = NodeCopy(node->left, call_count);
		assert(new_node->left);
		new_node->left->parent = new_node;
	}
	
	if(node->right)
	{
		new_node->right = NodeCopy(node->right, call_count);
		assert(new_node->right);
		new_node->right->parent = new_node;
	}
	

	return new_node;
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

static tree_err_t _TreeDestroy(node_t *tree, size_t *call_count)
{
	assert(call_count);
	
	if (tree == NULL)
		return T_NO_ERR;

    if((*call_count)++ > MAX_REC_DEPTH)
        return T_LOOP;
	/*-----------------------------*/
	
	tree_err_t err = T_NO_ERR;
    
	// if(tree->parent == NULL)
	// 	err = T_PARENT_NULLPTR;


	if(err)
		_TreeDestroy(tree->left, call_count);
	else
		err = _TreeDestroy(tree->left, call_count);
	
	if (err)
		_TreeDestroy(tree->right, call_count);
	else
		err = _TreeDestroy(tree->right, call_count);

	tree->op.type = N_OP;
	tree->op.val.var = 0;
	tree->parent = NULL;
	tree->left = tree->right = NULL;
	
	free(tree);
	return err;
}


void TieLeftToParent(node_t *node)
{
	assert(node);
	assert(node->left);
	//assert(node->right);
	//assert(node->parent);

	node->op = node->left->op;

	if(node->parent == node || node->parent == NULL)
	{
		node_t *new_left = node->left->left;
		node_t *new_right = node->left->right;
		node_t *tying = node->left;

		node->left = new_left;
		if(new_left)
			new_left->parent = node;

		free(node->right);
		node->right = new_right;
		if(new_right)
			new_right->parent = node;

		free(tying);

		return;
	}
	else if(node->parent->left == node)
	{
		node->left->parent = node->parent;
		node->parent->left = node->left;
	}
	else if(node->parent->right == node)
	{
		node->left->parent = node->parent;
		node->parent->right = node->left;
	}
	else
	{
		print_err_msg("node->parent points to unknown");
		return;
	}

	free(node->right);
	free(node);
}

void TieRightToParent(node_t *node)
{
	assert(node);
	//assert(node->left);
	assert(node->right);
	//assert(node->parent);
	
	node->op = node->right->op;

	if(node->parent == node || node->parent == NULL)
	{
		node_t *new_left = node->right->left;
		node_t *new_right = node->right->right;
		node_t *tying = node->right;

		free(node->left);
		node->left = new_left;
		if(new_left)
			new_left->parent = node;

		node->right = new_right;
		if(new_right)
			new_right->parent = node;

		free(tying);

		return;
	}
	else if(node->parent->left == node)
	{
		node->right->parent = node->parent;
		node->parent->left = node->right;
	}
	else if(node->parent->right == node)
	{
		node->right->parent = node->parent;
		node->parent->right = node->right;
	}
	else
	{
		print_err_msg("node->parent points to unknown");
		return;
	}

	free(node->left);
	free(node);
}

void AddLeaves(node_t *node)
{
	assert(node);
	
	node->left = (node_t *)calloc(1, sizeof(node_t));
	assert(node->left);
	
	node->right = (node_t *)calloc(1, sizeof(node_t));
	assert(node->right);

	node->left->parent = node->right->parent = node;
}


node_t *NewNode(const op_t op, node_t *left, node_t *right)
{
	if(op.type >= N_OP)
		return NULL;

	node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
	assert(new_node);

	new_node->op = op;

	new_node->left = left;

	if(left)
		new_node->left->parent = new_node;
	new_node->right = right;
	if(right)
		new_node->right->parent = new_node;

	return new_node;
}





/* entrance to recursion */
node_t *TreeCopy(const node_t *tree)
{
	size_t call_count = 0;
	return NodeCopy(tree, &call_count);
}

node_t *FindNode(node_t *tree, const op_t op)
{
	size_t call_count = 0;
	return _FindNode(tree, op, &call_count);
}

tree_err_t TreeDestroy(node_t *tree)
{
	size_t call_counts = 0;
	return _TreeDestroy(tree, &call_counts);
}

