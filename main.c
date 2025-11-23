#include "Tree.h"


// (*(x(nil)(nil))(+(arth(y(nil)(nil))(nil))(6(nil)(nil))))

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
	
	PrintTexTree(tree, tex_file);
	TreeDumpHTML(tree, "f.dot", "./Img", "f.html", "Test");

	printf("found [%p]\n", FindNode(tree, (const op_t){.type = OP_VAR, .val.var = 'x'}));

	node_t *d_tree = Deriv(tree, 'x');
	d_tree->parent = d_tree;
	PrintTexTree(d_tree, tex_file);

	size_t i = 0;
	do
	{
		i = 0;
		i += FoldConst(d_tree);
		PrintTexTree(d_tree, tex_file);
		TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");
		
		i += FoldNeutral(d_tree);
		PrintTexTree(d_tree, tex_file);
		TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");
	} while (i);

	// FoldConst(d_tree);
	// FoldNeutral(d_tree);
	
	PrintTexTree(d_tree, tex_file);

	TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "(tree)");

	CloseTex(tex_file);

	TreeDestroy(tree);
	TreeDestroy(d_tree);
	free(buf);
	return status;
}