#include "Wolfram.h"

static int NeedPar(const node_t *cur, const node_t *son)	/* for correct placement of parenthes */
{
	assert(cur);
	assert(son);

	switch(cur->op.type)
	{
	case OP_NUM:
		return 0;
	case OP_VAR:
		return 0;
	case OP_ELFUNC:
		return 0;
	case OP_ARIFM:
		switch(cur->op.val.arifm)
		{
		case AR_ADD:
			return 0;
		case AR_DIV:
			return 0;
		case AR_MUL:
			switch(son->op.type)
			{
			case OP_VAR:
			case OP_NUM:
			case OP_ELFUNC:
				return 0;
			case OP_ARIFM:
				switch(son->op.val.arifm)
				{
				case AR_ADD:
				case AR_SUB:
					return 1;
				case AR_MUL:
				case AR_POW:
				case AR_DIV:
				default:
					return 0;
				}
			
			case N_OP:
			default:
				break;
			}
			
			break;
		case AR_SUB:
			switch(son->op.type)
			{
			case OP_VAR:
			case OP_NUM:
			case OP_ELFUNC:
				return 0;
			case OP_ARIFM:
				switch(son->op.val.arifm)
				{
				case AR_ADD:
				case AR_SUB:
					if(son == cur->right)
						return 1;
					return 0;
				case AR_MUL:
				case AR_POW:
				case AR_DIV:
				default:
					return 0;
				}
			
			case N_OP:
			default:
				break;
			}
			
			break;
		case AR_POW:
			if(son == cur->right)
				return 0;
			
			switch(son->op.type)
			{
			case OP_NUM:
			case OP_VAR:
				return 0;
			case OP_ARIFM:
			case OP_ELFUNC:
			case N_OP:
			default:
				return 1;
			}
			break;
		
		default:
			print_err_msg("op.val.arifm is out of range arifm_t");
			break;
		}
		break;
	
	case N_OP:
	default:
		break;
	}
	
	return 1;
}


static void PrintNodeData(const node_t *node, FILE *out_file)
{
	assert(node);
	assert(out_file);

	switch(node->op.type)
	{
	case OP_ARIFM:
		fprintf(out_file, "%c", (char)node->op.val.arifm);
		break;
	
	case OP_ELFUNC:
		if(node->op.val.elfunc < N_ELFUNC)
			fprintf(out_file, "%s", ELFUNC_NAME[node->op.val.elfunc]);
		else
			fprintf(out_file, "???");
		break;

	case OP_VAR:
		fprintf(out_file, "%c", node->op.val.var);
		break;
	
	case OP_NUM:
		if(node->op.val.num < 0)
			fprintf(out_file, "%lg", node->op.val.num);
		else
			fprintf(out_file, "%lg", node->op.val.num);
		break;

	case N_OP:
	default:
		fprintf(out_file, "?");
		break;
	}
}

static void PrintNodeDataTex(const node_t *node, FILE *out_file)
{
	assert(node);
	assert(out_file);

	switch(node->op.type)
	{
	case OP_ARIFM:
		switch(node->op.val.arifm)
		{
		case AR_ADD:
			fprintf(out_file, "+");
			break;
		case AR_SUB:
			fprintf(out_file, "-");
			break;
		case AR_MUL:
			fprintf(out_file, "\\cdot ");
			break;
		case AR_DIV:
			fprintf(out_file, "\\over ");
			break;
		case AR_POW:
			fprintf(out_file, "^");
			break;
		default:
			fprintf(out_file, "???");
			break;
		}
		
		break;
	
	case OP_ELFUNC:
		if(node->op.val.elfunc < N_ELFUNC)
			fprintf(out_file, "\\text{%s}", ELFUNC_NAME[node->op.val.elfunc]);
		else
			fprintf(out_file, "???");
		break;

	case OP_VAR:
		fprintf(out_file, "%c", node->op.val.var);
		break;
	
	case OP_NUM:
		if(node->op.val.num < 0)
			fprintf(out_file, "(%lg)", node->op.val.num);
		else
			fprintf(out_file, "%lg", node->op.val.num);
		break;
	
	case N_OP:
	default:
		fprintf(out_file, "?");
		break;
	}
}


static tree_err_t _PrintDigraphNode(const node_t *node, FILE *dot_file, size_t *call_count)
{
	assert(call_count);
	if ((*call_count)++ > MAX_REC_DEPTH)
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

	assert(node->op.type < N_OP);
	fprintf(dot_file, "label%lu[shape=record, style=\"rounded, filled\", fillcolor=\"#a8daf0ff\", "
					  "label=\"{ node[%p] | type = %s | val = ",
			(size_t)node, node, OP_TYPE_NAME[node->op.type]);
	
	PrintNodeData(node, dot_file);

	fprintf(dot_file, " | prnt[%p] | { l[%p] | r[%p] }}\"];\n",
			node->parent, node->left, node->right);


	if(node->left)
	{
		if(node->left->parent == node)
			fprintf(dot_file, "label%lu->label%lu [color=purple, dir=both]\n", (size_t)node, (size_t)node->left);
		else
			fprintf(dot_file, "label%lu->label%lu [color=red]\n", (size_t)node, (size_t)node->left);
		
		err = _PrintDigraphNode(node->left, dot_file, call_count);
	}

	TREE_OK_OR_LEAVE(err);
	
	if(node->right)
	{
		if(node->right->parent == node)
			fprintf(dot_file, "label%lu->label%lu [color=purple, dir=both]\n", (size_t)node, (size_t)node->right);
		else
			fprintf(dot_file, "label%lu->label%lu [color=red]\n", (size_t)node, (size_t)node->right);
		
		err = _PrintDigraphNode(node->right, dot_file, call_count);
	}

	return err;
}

static tree_err_t PrintDigraphNode(const node_t *node, FILE *dot_file)
{
	size_t call_count = 0;
	return _PrintDigraphNode(node, dot_file, &call_count);
}

static tree_err_t CreateDigraph(const node_t *node, const char *dot_file_path)
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

    fprintf(dot_file, "digraph Wolfram\n{\n");
	tree_err_t err = PrintDigraphNode(node, dot_file);
	
	

	putc('}', dot_file);
	fclose(dot_file);
	return err;
}

static tree_err_t PrintTexNode(const node_t *node, FILE *tex_file, size_t *call_count)
{
	assert(call_count);
	
	if(node == NULL)
		return T_NODE_NULLPTR;
		
	if(tex_file == NULL)
		return T_FILE_NULLPTR;
	
	if(node->parent == NULL)
		return T_PARENT_NULLPTR;

	if((*call_count)++ > MAX_REC_DEPTH)
		return T_LOOP;
	/*----------------------------*/

	tree_err_t err = T_NO_ERR;
	
	int par_is_open = 0;
	int f_par_is_open = 0;

	fprintf(tex_file, "{");

	fprintf(tex_file, "{");
	if(node->left)
	{
		// if((node->op.type == OP_ARIFM && (node->op.val.arifm == AR_MUL) &&
		//    node->left->op.type == OP_ARIFM	&& node->left->op.val.arifm != AR_MUL && node->left->op.val.arifm != AR_DIV
		//    && node->left->op.val.arifm != AR_POW)
		//    ||
		//    (node->op.type == OP_ARIFM && (node->op.val.arifm == AR_POW) &&
		//    node->left->op.type == OP_ARIFM)
		//    ||
		//    node->left->op.type == OP_ELFUNC)
		if(NeedPar(node, node->left))
		{
			fprintf(tex_file, "\\left(");
			par_is_open = 1;
		}

		err = PrintTexNode(node->left, tex_file, call_count);
	}
	
	if(par_is_open)
	{
		fprintf(tex_file, "\\right)");
		par_is_open = 0;
	}
	fprintf(tex_file, "}");
	

	PrintNodeDataTex(node, tex_file);
	if(node->op.type == OP_ELFUNC)
	{
		fprintf(tex_file, "\\left(");
		f_par_is_open = 1;
	}

	fprintf(tex_file, "{");
	if(node->right)
	{
		// if(node->op.type == OP_ARIFM && node->op.val.arifm == AR_MUL &&
		//    node->right->op.type == OP_ARIFM	&& (node->right->op.val.arifm == AR_ADD || node->right->op.val.arifm == AR_SUB)
		//    &&
		//    node->right->op.type != OP_ELFUNC)
		if(NeedPar(node, node->right))
		{
			fprintf(tex_file, "\\left(");
			par_is_open = 1;
		}

		if(err)
			PrintTexNode(node->right, tex_file, call_count);
		else
			err = PrintTexNode(node->right, tex_file, call_count);
	}
	
	
	if(par_is_open)
	{
		fprintf(tex_file, "\\right)");
		par_is_open = 0;
	}

	fprintf(tex_file, "}");
	
	if(f_par_is_open)
	{
		fprintf(tex_file, "\\right)");
		f_par_is_open = 0;
	}

	fprintf(tex_file, "}");
	return err;
}


tree_err_t TreeDumpHTML(const node_t *node, const char *dot_file_path, const char *img_dir_path, const char *html_file_path, const char *caption)
{
	if(node == NULL)
        return T_NODE_NULLPTR;
	
	if(dot_file_path == NULL || html_file_path == NULL || img_dir_path == NULL || caption == NULL)
		return T_FILE_NULLPTR;

	static size_t call_count = 0;

    FILE *html_file = NULL;

    if(call_count == 0)
    {
        html_file = fopen(html_file_path, "w");
        fclose(html_file);
    }

    html_file = fopen(html_file_path, "a+");
	assert(html_file);

	tree_err_t err = CreateDigraph(node, dot_file_path);

    char system_msg[100] = "";
    snprintf(system_msg, 100, "dot %s -Tsvg -o %s/%lu.svg\n", dot_file_path, img_dir_path, call_count);
    //printf("sys_msg = {%s}\n", system_msg);
    if(system(system_msg))
	{
		printf("'system' error\n");
		return T_FILE_NULLPTR;
	}

	fprintf(html_file, "<pre>\n\t<h2>%s</h2>\n", caption);

    fprintf(html_file, "\t<img  src=\"%s/%lu.svg\" alt=\"%s\" width=\"1200px\"/>\n", img_dir_path, call_count, caption);

    fprintf(html_file, "\t<hr>\n</pre>\n\n");
    fclose(html_file);

    call_count++;

    return err;
}

tree_err_t PrintTexTree(const node_t *tree, FILE *tex_file, const char *cap)
{
	fprintf(tex_file, "\\begin{equation}\n%s", cap);
	
	size_t call_count = 0;
	tree_err_t err = PrintTexNode(tree, tex_file, &call_count);

	fprintf(tex_file, "\n\\end{equation}\\vspace{1cm}\n");
	return err;
}

void PrintTexText(FILE *tex_file, const char *s)
{
	if(tex_file == NULL || s == NULL)
		return;

	fprintf(tex_file, "%s", s);
}


FILE *OpenTex(const char *tex_file_path)
{
	if(tex_file_path == NULL)
		return NULL;

	FILE *tex_file = fopen(tex_file_path, "w");
	assert(tex_file);

	fprintf(tex_file,
			"\\documentclass[a4paper]{article}\n"
			"\n"
			"\\usepackage{float}\n"
			"\\usepackage{multirow}\n"
			"\\usepackage[utf8]{inputenc}\n"
			"\\usepackage[T2A]{fontenc}\n"
			"\\usepackage[english,russian]{babel}\n"
			"\\usepackage{graphicx}\n"
			"\\usepackage{amsmath}\n"
			"\\usepackage[left=2cm, top=2cm, right=2cm, bottom=2cm]{geometry}\n"
			"\\usepackage{wrapfig}\n\n\n"
			"\\begin{document}\n\n");

	return tex_file;
}

int CloseTex(FILE *tex_file)
{
	if(tex_file == NULL)
		return 1;

	fprintf(tex_file, "\n\\end{document}\n");
	
	return fclose(tex_file);
}



/*
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
*/

