#include "Tree.h"

static void PrintNodeData(node_t *node, FILE *out_file)
{
	assert(node);
	assert(out_file);

	switch(node->op_type)
	{
	case OP_ARIFM:
		fprintf(out_file, "%c", (char)node->op_val.arifm);
		break;
	
	case OP_ELFUNC:
		if(node->op_val.elfunc < N_ELFUNC)
			fprintf(out_file, "%s", ELFUNC_NAME[node->op_val.elfunc]);
		else
			fprintf(out_file, "???");
		break;

	case OP_VAR:
		fprintf(out_file, "%c", node->op_val.var);
		break;
	
	case OP_NUM:
		fprintf(out_file, "%lg", node->op_val.num);
		break;
	
	case N_OP:
	default:
		fprintf(out_file, "?");
		break;
	}
}

tree_err_t PrintTree(node_t *node, FILE *dump_file, const traversal_type_t traversal_type)
{
    static size_t call_count = 0;
    
    if(call_count++ > MAX_REC_DEPTH)
        return T_LOOP;

    if (node == NULL)
        return T_NULLPTR;
    
    if(dump_file == NULL)
        return T_FILE_NULLPTR;
    
    if(node->parent == NULL)
        return T_PARENT_NULLPTR;

    tree_err_t err = T_NO_ERR;

    putc('(', dump_file);
    
    if(traversal_type == TT_PREORDER)
		PrintNodeData(node, dump_file);

	if(node->left)
        err = PrintTree(node->left, dump_file, traversal_type);
	else
		fprintf(dump_file, "(nil)");

	if(traversal_type == TT_INORDER)
        PrintNodeData(node, dump_file);
    
    TREE_OK_OR_LEAVE(err);
    
    if(node->right)
        err = PrintTree(node->right, dump_file, traversal_type);
	else
		fprintf(dump_file, "(nil)");

    if(traversal_type == TT_POSTORDER)
        PrintNodeData(node, dump_file);

    putc(')', dump_file);

    return err;
}

tree_err_t PrintDiagraphNode(node_t *node, FILE *dot_file)
{
	static size_t call_count = 0;

	//printf("count = %lu\n", call_count);
	if (call_count++ > MAX_REC_DEPTH)
		return T_LOOP;

    if (node == NULL)
	{
		fprintf(dot_file, "NULL[shape=record, style=\"rounded, filled\", fillcolor=\"#e00808ff\"");
        return T_NODE_NULLPTR;
	}
	
    if(dot_file == NULL)
        return T_FILE_NULLPTR;
    
    if(node->parent == NULL)
	{
		fprintf(dot_file, "label%lu->NULL\n", (size_t)node);
		return T_PARENT_NULLPTR;
	}

    tree_err_t err = T_NO_ERR;

	assert(node->op_type < N_OP);
	fprintf(dot_file, "label%lu[shape=record, style=\"rounded, filled\", fillcolor=\"#a8daf0ff\", "
					  "label=\"{ node[%p] | type = %s | val = ",
			(size_t)node, node, OP_TYPE_NAME[node->op_type]);
	
	PrintNodeData(node, dot_file);

	fprintf(dot_file, " | prnt[%p] | { l[%p] | r[%p] }}\"];\n",
			node->parent, node->left, node->right);


	if(node->left)
	{
		if(node->left->parent == node)
			fprintf(dot_file, "label%lu->label%lu [color=purple, dir=both]\n", (size_t)node, (size_t)node->left);
		else
			fprintf(dot_file, "label%lu->label%lu [color=red]\n", (size_t)node, (size_t)node->left);
		
		err = PrintDiagraphNode(node->left, dot_file);
	}

	TREE_OK_OR_LEAVE(err);
	
	if(node->right)
	{
		if(node->right->parent == node)
			fprintf(dot_file, "label%lu->label%lu [color=purple, dir=both]\n", (size_t)node, (size_t)node->right);
		else
			fprintf(dot_file, "label%lu->label%lu [color=red]\n", (size_t)node, (size_t)node->right);
		
		err = PrintDiagraphNode(node->right, dot_file);
	}

	return err;
}

tree_err_t CreateDigraph(node_t *node, const char *dot_file_path)
{
    if(node == NULL)
        return T_NODE_NULLPTR;

    assert(dot_file_path);

    FILE *dot_file = fopen(dot_file_path, "w");
    if(dot_file == NULL)
    {
        printf("dot file cannot be open\n");
        return T_FILE_NULLPTR;
    }

    fprintf(dot_file, "digraph Tree\n{\n");
	tree_err_t err = PrintDiagraphNode(node, dot_file);
	
	

	putc('}', dot_file);
	fclose(dot_file);
	return err;
}

tree_err_t TreeDumpHTML(node_t *node, const char *dot_file_path, const char *img_dir_path, const char *html_file_path, const char *caption)
{
	if(node == NULL)
        return T_NODE_NULLPTR;
	
	if(dot_file_path == NULL || html_file_path == NULL || img_dir_path == NULL || caption == NULL)
		return T_FILE_NULLPTR;

	static size_t dump_call_counter = 0;

    FILE *html_file = NULL;

    if(dump_call_counter == 0)
    {
        html_file = fopen(html_file_path, "w");
        fclose(html_file);
    }

    html_file = fopen(html_file_path, "a");
    if(html_file == NULL)
	{
		printf("file(s) cannot be open\n");
	}
	
    tree_err_t err = CreateDigraph(node, dot_file_path);

    char system_msg[100] = "";
    snprintf(system_msg, 100, "dot %s -Tsvg -o %s/%lu.svg\n", dot_file_path, img_dir_path, dump_call_counter);
    //printf("sys_msg = {%s}\n", system_msg);
    if(system(system_msg))
	{
		printf("'system' error\n");
		return T_FILE_NULLPTR;
	}

	fprintf(html_file, "<pre>\n\t<h2>%s</h2>\n", caption);

    fprintf(html_file, "\t<img  src=\"%s/%lu.svg\" alt=\"%s\" width=\"1200px\"/>\n", img_dir_path,dump_call_counter, caption);

    fprintf(html_file, "\t<hr>\n</pre>\n\n");
    fclose(html_file);

    dump_call_counter++;

    return err;
}

