#include "Wolfram.h"

static int Deriv(FILE* tex_file, const node_t *tree, const size_t deriv_ord, double var_val[], const char d_var)
{
	assert(tree);
	assert(tex_file);
	assert(var_val);

	node_t *new_deriv = NULL, *deriv = TakeDeriv(tree, d_var);

	char tex_cap[100] = "";
	snprintf(tex_cap, 100, "\\frac{\\partial f}{\\partial %c}=", d_var);

	for (size_t i = 0; ; i++)
	{
		while(FoldConst(deriv, var_val, d_var) || FoldNeutral(deriv));

		if(TreeDumpTEX(deriv, tex_file, tex_cap))
		{
			print_err_msg("dump errors occured");
			return 1;
		}

		if(i + 1 < deriv_ord)
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
		
		snprintf(tex_cap, 100, "\\frac{\\partial^{%lu} f}{{\\partial %c}^{%lu}}=", i + 2, d_var, i + 2);
	}

	return 0;
}


int Start(int argc, char *argv[])
{
	char *tex_file_name = NULL;
	char *in_file_name = NULL;
	FILE *in_file = NULL, *tex_file = NULL;
	size_t taylor_ord = 0;
	size_t deriv_ord = 1;
	mode_t mode = MD_NOTHING_DO;
	
	double var_val[(unsigned char)-1] = {};
	for (size_t i = 0; i < sizeof(var_val) / sizeof(double); i++)
		var_val[i] = NAN;

	char d_var = 'x';
	double ex_point = 0;
	node_t *tree = NULL;
	int need_plot = 0;
	double plot_from = 0, plot_to = 0;
	size_t n_points = 10; /* default */
	int status = 0;

	for (int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-o") == 0 && ++i < argc)
		{
			free(tex_file_name);
			tex_file_name = strdup(argv[i]);
		}
		else if(strcmp(argv[i], "-i") == 0 && ++i < argc)
		{
			free(in_file_name);
			in_file_name = strdup(argv[i]);
		}
		else if(strncmp(argv[i], "-t", 2) == 0)
		{
			sscanf(argv[i] + 2, "%lup%lg", &taylor_ord, &ex_point);
			mode = MD_TAYLOR;
		}
		else if(strncmp(argv[i], "-d", 2) == 0)
		{
			sscanf(argv[i] + 2, "%lu", &deriv_ord);
			mode = MD_TAKE_DERIV;
		}
		else if(strcmp(argv[i], "--par") == 0)
		{
			while(++i < argc && isalpha(argv[i][0]))
			{
				if(argv[i][1] != '=')
					continue;

				sscanf(argv[i] + 2, "%lg", &(var_val[(int)argv[i][0]]));
			}

			i--;
		}
		else if(strncmp(argv[i], "--plot=", 7) == 0)
		{
			if(sscanf(argv[i], "--plot=%lf-%lf,%lu", &plot_from, &plot_to, &n_points) >= 2)
				need_plot = 1;
		}
		else if(strncmp(argv[i], "-v", 2) == 0)
		{
			if(isalpha(argv[i][2]))
				d_var = argv[i][2];
		}
		else
		{
			printf( colorize("usage: ", _YELLOW_) colorize("wolfram\n", _BOLD_ _GREEN_)
					colorize("-o <filename>\t\t", _BOLD_ _CYAN_) colorize("output file name (default: out.tex)\n", _BOLD_ _MAGENTA_)
					colorize("-i <filename>\t\t", _BOLD_ _CYAN_) colorize("input file name (default: stdin stream)\n", _BOLD_ _MAGENTA_)
					colorize("-t<ord>p<pnt>\t\t", _BOLD_ _CYAN_) colorize("taylor expansion to order ord at pnt point\n", _BOLD_ _MAGENTA_)
					colorize("-d<ord>\t\t\t", _BOLD_ _CYAN_) colorize("ord -order derivative\n", _BOLD_ _MAGENTA_)
					colorize("--par <x=5 A=7 ...>\t", _BOLD_ _CYAN_) colorize("parameters value\n", _BOLD_ _MAGENTA_)
					colorize("-v<v>\t\t\t", _BOLD_ _CYAN_) colorize("diff variable v\n", _BOLD_ _MAGENTA_)
					colorize("--plot=i1-i2,n\t\t", _BOLD_ _CYAN_) colorize("draw plot in interval [i1, i2] with n points\n", _BOLD_ _MAGENTA_));
			status = 1;
			goto exit;
		}
	}
	
	if(tex_file_name == NULL)
		tex_file_name = strdup("out.tex");

	tex_file = fopen(tex_file_name, "w");
	
	if(in_file_name == NULL)
		in_file = stdin;
	else
	{
		in_file = fopen(in_file_name, "r");
		if(in_file == NULL)
		{
			print_err_msg("input cannot be open");
			status = 1;
			goto exit;
		}
	}

	tree = ReadInfix(in_file);
	if(tree == NULL)
	{
		print_err_msg("wrong input");
		status = 1;
		goto exit;
	}

	TreeDumpHTML(tree, "f.dot", "./Img", "f.html", "check");

	tex_file = OpenTEX(tex_file_name);
	if(tex_file == NULL)
	{
		status = 1;
		goto exit;
	}
	/*------------------------------------------------*/

	fprintf(tex_file, "Вы ввели\\vspace{0.5cm}\n");
	char f_str[6]=""; /* 'f(x)=' */
	snprintf(f_str, 6, "f(%c)=", d_var);
	TreeDumpTEX(tree, tex_file, f_str);
	
	if(need_plot)
	{
		fprintf(tex_file, "График функции:\\vspace{0.5cm}\n");
		if (DrawPlot(tree, d_var, var_val, plot_from, plot_to, n_points))
		{
			status = 1;
			goto exit;
		}

		PlacePlotTEX(tex_file);
	}

	switch(mode)
	{
	case MD_TAKE_DERIV:
		if(Deriv(tex_file, tree, deriv_ord, var_val, d_var))
		{
			status = 1;
			goto exit;
		}
		break;
	case MD_TAYLOR:
		if(Taylor(tree, ex_point, d_var, var_val, taylor_ord, tex_file))
		{
			status = 1;
			goto exit;
		}
		break;
	case MD_NOTHING_DO:
	default:
		break;
	}

	/*------------------------------------------------*/

exit:
	CloseTEX(tex_file);
	if (BuildTEX(tex_file_name))
		status = 1;
	if (in_file)
		fclose(in_file);
	free(tex_file_name);
	free(in_file_name);
	TreeDestroy(tree);

	return status;	
}
