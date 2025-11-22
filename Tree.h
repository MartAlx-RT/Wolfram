#pragma once

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include "Colors.h"

#define MODE_PRINT "%lg"

#define TREE_OK_OR_LEAVE(err)              \
    if (err && err != T_PARENT_NULLPTR) \
        return err


typedef enum op_type_t
{
	OP_ARIFM,
	OP_ELFUNC,
	OP_VAR,
	OP_NUM,
	
	N_OP
} op_type_t;

static const char *OP_TYPE_NAME[] =
	{
		"arifmetic",
		"elementary func",
		"variable",
		"number",
	};

typedef enum arifm_t
{
	AR_ADD = '+',
	AR_SUB = '-',
	AR_MUL = '*',
	AR_DIV = '/',
	AR_POW = '^',

} arifm_t;

typedef enum elfunc_t
{
	F_LN,

	F_SIN,
	F_COS,
	F_TAN,
	//F_COT,

	F_SINH,
	F_COSH,
	F_TANH,
	//F_COTH,

	F_ASIN,
	F_ACOS,
	F_ATAN,
	//F_ACOT,

	F_ASINH,
	F_ACOSH,
	F_ATANH,
	//F_ACOTH,
	N_ELFUNC
} elfunc_t;

/* it necessary to sort same-suffix name! */
static const char *ELFUNC_NAME[] =
	{
		"ln",
		"sin", "cos", "tg",
		"sh", "ch", "th",
		"arcsin", "arccos", "arctg",
		"arsh", "arch", "arth",
	};

typedef union op_val_t
{
	char var;
	double num; /* long?... */
	arifm_t arifm;
	elfunc_t elfunc;
} op_val_t;

typedef struct node_t
{
	op_type_t op_type;
	op_val_t op_val;

	node_t *parent, *left, *right;
} node_t;


typedef enum tree_err_t
{
    T_NODE_NULLPTR = -3,
    T_FILE_NULLPTR = -2,
    T_NULLPTR = -1,
    T_NO_ERR,
    T_LOOP,
    T_PARENT_NULLPTR,
	
} tree_err_t;

typedef enum traversal_type_t
{
    TT_PREORDER,
    TT_POSTORDER,
    TT_INORDER
} traversal_type_t;

static const size_t MAX_REC_DEPTH = 100;

/*-----------------------------------------------*/

tree_err_t TreeDumpHTML(const node_t *node, const char *dot_file_path, const char *img_dir_path, const char *html_file_path, const char *caption);
long ReadFileToBuf(const char *file_path, char **buf);
char *ReadNode(char *curs, node_t *node);
tree_err_t TreeDestroy(node_t *tree);
node_t *TreeCopy(const node_t *tree);
node_t *Deriv(const node_t *node, const char d_var);

FILE *OpenTex(const char *tex_file_path);
int CloseTex(FILE *tex_file);
tree_err_t PrintTexTree(const node_t *tree, FILE *tex_file);

