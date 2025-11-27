#include "Wolfram.h"


// (/(+(sh(nil)(+(^(x(nil)(nil))(3(nil)(nil)))(1(nil)(nil))))(ch(nil)(+(^(x(nil)(nil))(4(nil)(nil)))(2(nil)(nil)))))(th(nil)(+(^(x(nil)(nil))(5(nil)(nil)))(3(nil)(nil)))))
// (*(x(nil)(nil))(y(nil)(nil)))
int main(void)
{
	
	int status = 0;
	node_t *tree = (node_t *)calloc(1, sizeof(node_t));
	assert(tree);
	tree->parent = tree;

	char *buf = NULL;
	ReadFileToBuf("f.txt", &buf);
	assert(buf);

	printf("buf = %lu\n", (size_t)buf);
	
	if(ReadNode(buf, tree) == NULL)
		status = 1;

	FILE *tex_file = OpenTex("f.tex");

	PrintTexText(tex_file, "В качестве примера письменной работы по математическому анализу рассмотрим\n");

	PrintTexTree(tree, tex_file, "f=");
	TreeDumpHTML(tree, "f.dot", "./Img", "f.html", "Test");

	printf("found [%p]\n", FindNode(tree, (const op_t){.type = OP_VAR, .val.var = 'x'}));

	node_t *d1_tree = tree;
	node_t *d_tree = tree;

	d_tree->parent = d_tree;

	PrintTexText(tex_file, "Очевидно, что\n");

	PrintTexTree(d_tree, tex_file, "\\frac{\\partial f}{\\partial x}=");
	//getchar();

	size_t i = 0;
	do
	{
		i = 0;
		i += FoldConst(d_tree);
		//PrintTexTree(d_tree, tex_file);
		TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");
		
		i += FoldNeutral(d_tree);
		if(i)
			PrintTexTree(d_tree, tex_file, "\\frac{\\partial f}{\\partial x}=");
		TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");
	} while (i);

	// FoldConst(d_tree);
	// FoldNeutral(d_tree);
	
	//PrintTexTree(d_tree, tex_file);

	TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");

	CloseTex(tex_file);

	TreeDestroy(tree);
	//TreeDestroy(d_tree);
	//TreeDestroy(d1_tree);

	free(buf);
	return status;
	
	
}