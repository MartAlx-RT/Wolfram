#pragma once
#include "Tree.h"

#define ADD_(expr1, expr2)	NewNode(OP_ARIFM, (const op_val_t){.arifm = AR_ADD}, expr1, expr2)
#define SUB_(expr1, expr2)	NewNode(OP_ARIFM, (const op_val_t){.arifm = AR_SUB}, expr1, expr2)
#define MUL_(expr1, expr2)	NewNode(OP_ARIFM, (const op_val_t){.arifm = AR_MUL}, expr1, expr2)
#define DIV_(expr1, expr2)	NewNode(OP_ARIFM, (const op_val_t){.arifm = AR_DIV}, expr1, expr2)
#define POW_(expr1, expr2)	NewNode(OP_ARIFM, (const op_val_t){.arifm = AR_POW}, expr1, expr2)


#define dL		Deriv(node->left, d_var)
#define dR		Deriv(node->right, d_var)

#define cL		TreeCopy(node->left)
#define cR		TreeCopy(node->right)

