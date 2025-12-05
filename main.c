#include "Wolfram.h"

tree_err_t Taylor(const node_t *tree, const double ex_point, const char ex_var, double var_val[], const size_t ex_pow, FILE *tex_file);

int main(void)
{
	node_t *tree = ReadInfix(stdin);
	if(tree == NULL)
		return 1;

	double var_val[(unsigned char)-1] = {};
	
	FILE *tex_file = OpenTEX("f.tex");

	Taylor(tree, 0, 'x', var_val, 7, tex_file);

	CloseTEX(tex_file);
	BuildTEX("f.tex");

	TreeDestroy(tree);
	return 0;
}

//max = 170
tree_err_t Taylor(const node_t *tree, const double ex_point, const char ex_var, double var_val[], const size_t ex_pow, FILE* tex_file)
{
	if(tree == NULL || var_val == NULL || tex_file == NULL)
		return T_NULLPTR;

	frac_t frac_coeff = {};
	tree_err_t err = T_NO_ERR;

	double old_ex_var_val = var_val[(size_t)ex_var];
	
	var_val[ex_var] = ex_point;

	double coeff = Calc(tree, var_val);

	fprintf(tex_file, "\\begin{equation*}\nf(%c)=%lg", ex_var, coeff);

	double fact = 1;
	node_t *d_tree = tree, *d_tree_new = NULL;
	
	
	for (size_t i = 1; i <= ex_pow; i++)
	{
		//printf("hello\n");
		//TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "test");
		d_tree_new = TakeDeriv(d_tree, ex_var);
		if(d_tree_new == NULL)
			return T_NODE_NULLPTR;

		if(i > 1)
			TreeDestroy(d_tree);

		d_tree = d_tree_new;
		d_tree_new = NULL;

		
		

		while(FoldConst(d_tree) || FoldNeutral(d_tree));
		//TreeDumpHTML(d_tree, "f.dot", "./Img", "f.html", "test");
		
		coeff = Calc(d_tree, var_val);
		coeff /= fact;
		fact *= (double)i + 1;
		if(Feq(coeff, 0, EPS))
			continue;
		
		if(coeff > 0 || isnan(coeff))
			fputc('+', tex_file);
		
		if(FracApprox(coeff, &frac_coeff))
		{
			if(frac_coeff.sgn == -1)
				fputc('-', tex_file);

			fprintf(tex_file, "\\frac{%lu}{%lu}", frac_coeff.numer, frac_coeff.denomer);
		}
		else
			fprintf(tex_file, "%lg", coeff);

		fprintf(tex_file, "\\cdot (%c-%lg)^%lu", ex_var, ex_point, i);
	}
	fprintf(tex_file, "+o\\left((%c-%lg)^%lu\\right), %c\\rightarrow %lg\n\\end{equation*}\n", ex_var, ex_point, ex_pow, ex_var, ex_point);
	
	if(ex_pow == 1)
	 	TreeDestroy(d_tree);
	
	var_val[ex_var] = old_ex_var_val;

	return err;
}

