#include "Wolfram.h"

int Taylor(const node_t *tree, const double ex_point, const char d_var, double var_val[], const size_t ex_pow, FILE* tex_file)
{
	if(tree == NULL || var_val == NULL || tex_file == NULL)
		return 1;

	var_val[d_var] = ex_point;
	double old_ex_var_val = var_val[(size_t)d_var];
	double coeff = Calc(tree, var_val);
	double fact = 1;
	frac_t frac_coeff = {};
	

	node_t *deriv = TakeDeriv(tree, d_var), *new_deriv = NULL;

	
	fprintf(tex_file, "\\begin{equation*}\nf(%c)=%lg", d_var, coeff);
	
	for (size_t i = 1; ; i++)
	{
		fprintf(stderr, "hello\n");
		/* take derivative */
		while(FoldConst(deriv) || FoldNeutral(deriv));
		
		
		/*-------------------*/
		
		coeff = Calc(deriv, var_val);
		coeff /= fact;
		fact *= (double)i + 1;
		
		if(!Feq(coeff, 0, EPS))
		{
			if(coeff > 0 || isnan(coeff) || isinf(coeff))
				fputc('+', tex_file);
			
			if(FracApprox(coeff, &frac_coeff))
			{
				if(frac_coeff.sgn == -1)
					fputc('-', tex_file);
	
				fprintf(tex_file, "\\frac{%lu}{%lu}", frac_coeff.numer, frac_coeff.denomer);
			}
			else
				fprintf(tex_file, "%lg", coeff);
	
			
			if(Feq(ex_point, 0, EPS))
				fprintf(tex_file, "\\cdot %c", d_var);
			else
				fprintf(tex_file, "\\cdot (%c-%lg)", d_var, ex_point);
		
			
			if(i > 1)
			fprintf(tex_file, "^%lu", i);
		}

		/*-------------------------------------------------------*/
		if(i + 1 <= ex_pow)
		{
			new_deriv = TakeDeriv(deriv, d_var);
			TreeDestroy(deriv);
			deriv = new_deriv;
		}
		else
		{
			TreeDestroy(deriv);
			break;
		}
	}
	fprintf(tex_file, "+o\\left((%c-%lg)^{%lu}\\right), %c\\rightarrow %lg\n\\end{equation*}\n", d_var, ex_point, ex_pow, d_var, ex_point);

	
	var_val[d_var] = old_ex_var_val;

	return 0;
}

