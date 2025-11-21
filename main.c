#include "Tree.h"

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

	TreeDumpHTML(tree, "f.dot", "./Img", "f.html", "Test");

	TreeDestroy(tree);
	return status;
}