#include "Wolfram.h"


int main(void)
{
	/*	
	int status = 0;
	node_t *tree = (node_t *)calloc(1, sizeof(node_t));
	assert(tree);
	tree->parent = tree;

	char *buf = NULL;
	ReadFileToBuf("f.txt", &buf);
	assert(buf);

	printf("buf = %lu\n", (size_t)buf);
	
	if(ReadPrefix(buf, tree) == NULL)
		status = 1;

	FILE *tex_file = OpenTEX("f.tex");

	PutsTEX(tex_file, "В качестве примера письменной работы по математическому анализу рассмотрим\n");

	TreeDumpTEX(tree, tex_file, "f=");
	TreeDumpHTML(tree, "f.dot", "./Img", "f.html", "Test");

	printf("found [%p]\n", FindNode(tree, (const op_t){.type = OP_VAR, .val.var = 'x'}));

	node_t *d1_tree = tree;
	node_t *d_tree = tree;

	d_tree->parent = d_tree;

	PutsTEX(tex_file, "Очевидно, что\n");

	TreeDumpTEX(d_tree, tex_file, "\\frac{\\partial f}{\\partial x}=");
	//getchar();

	size_t i = 0;
	do
	{
		i = 0;
		i += FoldConst(d_tree);
		//TreeDumpTEX(d_tree, tex_file);
		TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");
		
		i += FoldNeutral(d_tree);
		if(i)
			TreeDumpTEX(d_tree, tex_file, "\\frac{\\partial f}{\\partial x}=");
		TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");
	} while (i);

	// FoldConst(d_tree);
	// FoldNeutral(d_tree);
	
	//TreeDumpTEX(d_tree, tex_file);

	TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");

	CloseTEX(tex_file);

	TreeDestroy(tree);
	//TreeDestroy(d_tree);
	//TreeDestroy(d1_tree);

	free(buf);
	return status;
	*/

	node_t *tree = ReadInfix(stdin);
	assert(tree);

	tree->parent = tree;

	FILE *tex_file = OpenTEX("f.tex");
	assert(tex_file);

	TreeDumpHTML(tree, "f.dot", "./Img", "f.html", "test");

	PutsTEX(tex_file, "вы ввели\n");
	TreeDumpTEX(tree, tex_file, "");
	
	PutsTEX(tex_file, "первая производная\n");
	node_t *d_tree = TakeDeriv(tree, 'x');
	assert(d_tree);
	d_tree->parent = d_tree;

	TreeDumpTEX(d_tree, tex_file, "\\frac{\\partial}{\\partial x} = ");
	TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "D(before folding)");

	while(FoldConst(d_tree) || FoldNeutral(d_tree));
	PutsTEX(tex_file, "после упрощения\n");
	TreeDumpTEX(d_tree, tex_file, "\\frac{\\partial}{\\partial x} = ");
	TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "D()");

	CloseTEX(tex_file);
	system("pdflatex f.tex -o f.pdf > /dev/null\n");

	TreeDestroy(tree);
	TreeDestroy(d_tree);
	return 0;
}