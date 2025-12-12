#include "Wolfram.h"
#include "ElfuncName.h"

#define EXISTS_OR_LEAVE(node) \
	if (node == NULL)        \
		return NULL


void ClearBuf(void)
{
	while(getchar() != '\n');
}

static void SkipSpaces(char **s)
{
	assert(s);
	assert(*s);
	
	while (isblank(**s))
		(*s)++;
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

char *ReadPrefix(char *curs, node_t *node)
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
		curs = ReadPrefix(curs, node->left);
		curs = ReadPrefix(curs, node->right);
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

/*-----------------------*/
static node_t *GetG(char **s);
static node_t *GetE(char **s);
static node_t *GetT(char **s);
static node_t *GetD(char **s);
static node_t *GetP(char **s);
static node_t *GetN(char **s);
static node_t *GetF(char **s);
static node_t *GetV(char **s);
/*-----------------------*/

#include "DSLdef.h"

static node_t *GetG(char **s)
{
	assert(s);
	assert(*s);

	SkipSpaces(s);

	node_t *node = GetE(s);
	if(node == NULL)
	{
		print_err_msg("wrong expression");
		return NULL;
	}

	assert(*s);
	
	if(**s != '$')
	{
		print_err_msg("missing '$'");
		return NULL;
	}

	return node;
}

static node_t *GetE(char **s)
{
	assert(s);
	assert(*s);

	node_t *node = GetT(s);
	assert(*s);
	EXISTS_OR_LEAVE(node);

	node_t *new_node = NULL, *arg_node = NULL;

	while (**s == '+' || **s == '-')
	{
		if (**s == '+')
		{
			(*s)++;
			SkipSpaces(s);
			
			arg_node = GetT(s);
			if(arg_node == NULL)
			{
				TreeDestroy(node);
				return NULL;
			}

			new_node = ADD_(node, arg_node);
			if(new_node)
				node = new_node;
			else
			{
				TreeDestroy(node);
				return NULL;
			}
			// val += GetT(s);
		}
		else
		{
			(*s)++;
			SkipSpaces(s);
			
			arg_node = GetT(s);
			if(arg_node == NULL)
			{
				TreeDestroy(node);
				return NULL;
			}

			new_node = SUB_(node, arg_node);
			if(new_node)
				node = new_node;
			else
			{
				TreeDestroy(node);
				return NULL;
			}
		}
	}

	return node;
}

static node_t *GetT(char **s)
{
	assert(s);
	assert(*s);
	
	node_t *node = GetD(s);
	assert(*s);
	EXISTS_OR_LEAVE(node);

	node_t *new_node = NULL, *arg_node = NULL;

	while (isalnum(**s) || **s == '*' || **s == '/' || **s == '(')
	{
		if (**s == '/')
		{
			(*s)++;
			SkipSpaces(s);
			
			arg_node = GetD(s);
			if(arg_node == NULL)
			{
				TreeDestroy(node);
				return NULL;
			}

			new_node = DIV_(node, arg_node);
			if(new_node)
				node = new_node;
			else
			{
				TreeDestroy(node);
				return NULL;
			}
		}
		else
		{
			if(**s == '*')
			{
				(*s)++;
				SkipSpaces(s);
			}
			
			arg_node = GetD(s);
			if(arg_node == NULL)
			{
				TreeDestroy(node);
				return NULL;
			}

			new_node = MUL_(node, arg_node);
			if(new_node)
				node = new_node;
			else
			{
				TreeDestroy(node);
				return NULL;
			}
		}
	}

	return node;
}

static node_t *GetD(char **s)
{
	assert(s);
	assert(*s);

	node_t *node = GetP(s), *arg_node = NULL, *new_node = NULL;
	if(node == NULL)
		return NULL;
	
	assert(*s);
	
	while(**s == '^')
	{
		(*s)++;
		SkipSpaces(s);

		arg_node = GetD(s);
		if(arg_node == NULL)
		{
			TreeDestroy(node);
			return NULL;
		}

		new_node = POW_(node, arg_node);
		if(new_node)
			node = new_node;
		else
		{
			TreeDestroy(node);
			return NULL;
		}
	}
	
	return node;
}

static node_t *GetP(char **s)
{
	assert(s);
	assert(*s);

	int sgn = +1;

	if(**s == '+')
		(*s)++;
	else if(**s == '-')
	{
		sgn = -1;
		(*s)++;
	}

	node_t *node = NULL;

	if(**s == '(')
	{
		(*s)++;
		SkipSpaces(s);
		
		node = GetE(s);
		EXISTS_OR_LEAVE(node);
		assert(*s);

		if(**s != ')')
		{
			print_err_msg("missing ')':");
			print_wrong_s(*s);

			TreeDestroy(node);
			return NULL;
		}

		(*s)++;
		SkipSpaces(s);
		
		if(sgn == -1 && node)
			return MIN_(node);

		return node;
	}

	if(sgn == -1)
	{
		node = GetN(s);
		
		if(node)
			return MIN_(node);

		return NULL;
	}

	return GetN(s);
}

static node_t *GetN(char **s)
{
	assert(s);
	assert(*s);

	double val = 0;
	int off = 0;

	sscanf(*s, "%lf%n", &val, &off);
	*s += off;
	
	if(off == 0)
		return GetF(s);

	SkipSpaces(s);

	return NUM(val);
}

static node_t *GetF(char **s)
{
	assert(s);
	assert(*s);

	char *f_name = NULL;
	int off = 0;
	if(sscanf(*s, "%m[A-Za-z]%n", &f_name, &off) <= 0)
		return NULL;
	
	assert(f_name);

	for (size_t i = 0; i < N_ELFUNC; i++)
	{
		if(strncmp(f_name, ELFUNC_NAME[i], strlen(ELFUNC_NAME[i])) == 0)
		{
			free(f_name);
			
			*s += strlen(ELFUNC_NAME[i]);
			SkipSpaces(s);

			node_t *arg_node = GetP(s);
			if(arg_node)
				return NewNode((const op_t){.type = OP_ELFUNC, .val.elfunc = (elfunc_t)i}, NULL, arg_node);
			else
				return NULL;
		}
	}

	free(f_name);

	return GetV(s);
}

static node_t *GetV(char **s)
{
	assert(s);
	assert(*s);

	node_t *new_node = VAR(**s);
	assert(new_node);

	(*s)++;
	SkipSpaces(s);

	return new_node;
}

node_t *ReadInfix(FILE *in_file)
{
	if(in_file == NULL)
		return NULL;

	char *s = NULL;
	int s_len = 0;
	fscanf(in_file, "%m[^'\n']%n", &s, &s_len);
	if(s == NULL)
	{
		print_err_msg("read file failed\n");
		return NULL;
	}
	
	s = (char *)realloc(s, (2 + (size_t)s_len) * sizeof(char));
	assert(s);

	s[s_len] = '$';
	s[s_len + 1] = '\0';

	char *old_s = s;

	node_t *tree = GetG(&s);
	free(old_s);
	return tree;
}

#include "DSLundef.h"
#undef EXISTS_OR_LEAVE

