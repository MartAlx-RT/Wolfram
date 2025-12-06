#pragma once

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
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

typedef struct op_t
{
	op_type_t type;
	op_val_t val;
} op_t;

typedef struct node_t
{
	op_t op;

	struct node_t *parent, *left, *right;
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

typedef struct frac_t
{
	int sgn;

	size_t numer;
	size_t denomer;
} frac_t;

typedef enum mode_t
{
	MD_NOTHING_DO,
	MD_TAKE_DERIV,
	MD_TAYLOR,
} mode_t;

static const size_t MAX_REC_DEPTH = (size_t)(-1);

static const double EPS = 1e-5;

/*-----------------------------------------------*/

/* tree functions */
node_t *TreeCopy(const node_t *tree);
node_t *FindNode(node_t *tree, const op_t op);
tree_err_t TreeDestroy(node_t *tree);
void TieLeftToParent(node_t *node);
void TieRightToParent(node_t *node);
void AddLeaves(node_t *node);
node_t *NewNode(const op_t op, node_t *left, node_t *right);

/* input */
void ClearBuf(void);
long ReadFileToBuf(const char *file_path, char **buf);
char *ReadPrefix(char *curs, node_t *node);
node_t *ReadInfix(FILE *in_file);

/* dump functions */
tree_err_t TreeDumpHTML(const node_t *node, const char *dot_file_path, const char *img_dir_path, const char *html_file_path, const char *caption);
tree_err_t TreeDumpTEX(const node_t *tree, FILE *tex_file, const char *cap);
void PutsTEX(FILE *tex_file, const char *s);
FILE *OpenTEX(const char *tex_file_path);
int CloseTEX(FILE *tex_file);
int BuildTEX(const char *tex_file_name);

/* differentiator */
node_t *TakeDeriv(const node_t *node, const char d_var);

/* optimizer */
size_t FoldNeutral(node_t *tree);
size_t FoldConst(node_t *tree);

/* calculator */
double Calc(const node_t *tree, const double var_val[]);
int FracApprox(double x, frac_t *frac);

/* comparing double functions */
int Feq(const double val1, const double val2, double prec);
int NodeNumEqTo(const double ref_num, const node_t *node);

/* Taylor */
int Taylor(const node_t *tree, const double ex_point, const char ex_var, double var_val[], const size_t ex_pow, FILE *tex_file);
int Start(int argc, char *argv[]);
