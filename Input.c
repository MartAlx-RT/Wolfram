#include "Wolfram.h"

void ClearBuf(void)
{
	while(getchar() != '\n');
}


long ReadFileToBuf(const char *file_path, char **buf)
{
    if(file_path == NULL || buf == NULL)
		return -1;

	FILE *file = fopen(file_path, "r");
    if(file == NULL)
    {
		printf("input file cannot be open\n");
		return -1;
	}

    struct stat file_info = {};
    if(stat(file_path, &file_info))
        return -1;
    
    long buf_size = file_info.st_size;

    *buf = (char *)calloc((size_t)buf_size + 2, sizeof(char));
	if(*buf == NULL)
	{
		printf("buffer overflow\n");
		return -1;
	}

	if(fread(*buf, sizeof(char), (size_t)buf_size, file) != (size_t)buf_size)
        return -1;

    fclose(file);

    (*buf)[buf_size] = '\0';
    (*buf)[buf_size + 1] = EOF;

    return buf_size;
}

char *ReadNode(char *curs, node_t *node)
{
	if(node == NULL || curs == NULL)
		return NULL;
	
	if(node->parent == NULL)
	{
		printf("parent nullptr, aborted (curs = %lu)\n", (size_t)curs);
		return NULL;
	}

	if(*curs == '\0' || *curs == EOF)
	{
		printf("EOF reached\n");
		return NULL;
	}
	/*----------------------------------*/

	if(strncmp(curs, "(nil)", 5) == 0)
	{
		curs += 5;

		if(node->parent->left == node)
		{
			node->parent->left = NULL;
			free(node);//use destroy
		}
		else if(node->parent->right == node)
		{
			node->parent->right = NULL;
			free(node);
		}
		else
		{
			printf("wrong tree, aborted (curs = %lu)\n", (size_t)curs);
			return NULL;
		}

		return curs;
	}
	
	if(*curs == '(')
	{
		curs++;
		AddLeaves(node);

		for (size_t i = 0; i < N_ELFUNC; i++)
		{
			if(strncmp(curs, ELFUNC_NAME[i], strlen(ELFUNC_NAME[i])) == 0)
			{
				curs += strlen(ELFUNC_NAME[i]);
				node->op.type = OP_ELFUNC;
				node->op.val.elfunc = (elfunc_t)i;
				/* ... */
			}
		}
		
		if(curs[-1] != '('); 																/* if elfunc found */
		else if (('A' <= *curs && *curs <= 'Z') || ('a' <= *curs && *curs <= 'z'))				/* case variable */
		{
			node->op.type = OP_VAR;
			node->op.val.var = *curs++;
			/* ... */
		}
		else if(('0' <= *curs && *curs <= '9') || (curs[0] == '-' && curs[1] != '('))			/* case number */
		{
			int offset = 0;
			node->op.type = OP_NUM;
			
			sscanf(curs, "%lf%n", &(node->op.val.num), &offset);
			curs += (size_t)offset;
			/* ... */
		}
		else if(*curs == '-' || *curs == '+' || *curs == '*' || *curs == '/' || *curs == '^') /* case arifmetic */
		{
			node->op.type = OP_ARIFM;
			node->op.val.arifm = (arifm_t)*curs;
			curs++;
			/* ... */
		}
		else																					/* case nothing */
		{
			printf("Wrong syntax, aborted (curs = %lu)\n", (size_t)curs);
			return NULL;
		}

		/* do this after if..elif..else */
		curs = ReadNode(curs, node->left);
		curs = ReadNode(curs, node->right);
		if(curs == NULL)
			return NULL;
		
		if(*curs != ')')
		{
			printf("missing ')', aborted (curs = %lu)\n", (size_t)curs);
			return NULL;
		}
		
		
		return curs + 1;
	}

	printf("Wrong syntaxis, aborted (curs = {%s})\n", curs);
	printf("strcmp = %d", strncmp(curs, "(nil)", 5));
	return NULL;
}


/*---------------------------------------------------------------*/

