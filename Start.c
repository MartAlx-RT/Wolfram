#include "Wolfram.h"

static int Deriv(FILE* tex_file, const node_t *tree, const size_t deriv_ord, const char d_var)
{
	assert(tree);
	assert(tex_file);

	node_t *new_deriv = NULL, *deriv = TakeDeriv(tree, d_var);

	for (size_t i = 0; ; i++)
	{
		while(FoldConst(deriv) || FoldNeutral(deriv));
		
		if(TreeDumpTEX(deriv, tex_file, ""))
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
	char d_var = 'x';
	double ex_point = 0;

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
		else if(strcmp(argv[i], "-p") == 0)
		{
			while(++i < argc && isalpha(argv[i][0]))
			{
				if(argv[i][1] != '=')
					continue;

				sscanf(argv[i] + 2, "%lg", &(var_val[argv[i][0]]));
			}

			i--;
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
					colorize("-p <x=5 A=7 ...>\t", _BOLD_ _CYAN_) colorize("parameters value\n", _BOLD_ _MAGENTA_)
					colorize("-v<v>\t\t\t", _BOLD_ _CYAN_) colorize("diff variable v\n", _BOLD_ _MAGENTA_));
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
			goto exit;
		}
	}

	node_t *tree = ReadInfix(in_file);
	if(tree == NULL)
	{
		print_err_msg("wrong input");
		goto exit;
	}

	tex_file = OpenTEX(tex_file_name);
	if(tex_file == NULL)
		goto exit;
	/*------------------------------------------------*/

	fprintf(tex_file, "Вы ввели\\vspace{0.5cm}\n");
	char f_str[6]=""; /* 'f(x)=' */
	snprintf(f_str, 6, "f(%c)=", d_var);
	TreeDumpTEX(tree, tex_file, f_str);

	switch(mode)
	{
	case MD_TAKE_DERIV:
		if(Deriv(tex_file, tree, deriv_ord, d_var))
			goto exit;
		break;
	case MD_TAYLOR:
		if(Taylor(tree, ex_point, d_var, var_val, taylor_ord, tex_file))
			goto exit;
		break;
	case MD_NOTHING_DO:
	default:
		break;
	}

	/*------------------------------------------------*/

	CloseTEX(tex_file);
	if(in_file)
		fclose(in_file);
	free(tex_file_name);
	free(in_file_name);
	TreeDestroy(tree);

	return 0;

exit:
	CloseTEX(tex_file);
	if(in_file)
		fclose(in_file);
	free(tex_file_name);
	free(in_file_name);
	TreeDestroy(tree); //TODO 

	return 1;
	
}
