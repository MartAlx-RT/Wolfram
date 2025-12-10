#include "Wolfram.h"

static int CreateCSV(FILE *csv, const node_t *tree, const char var, double var_val[], double from, double to, const size_t n_points)
{
	if(tree == NULL || csv == NULL || var_val == NULL)
		return 1;
	
	if(from > to)
	{
		double t = from;
		from = to;
		to = t;
	}
	
	if(n_points == 0)
		return 0;
	
	/*------------------------------*/

	const double old_point = var_val[(int)var];

	double calc = 0;

	const double step = (to - from) / (double)n_points;
	for (double point = from; point < to; point += step)
	{
		var_val[(int)var] = point;
		calc = Calc(tree, var_val);
		if (isnan(calc) || isinf(calc))
			continue;

		fprintf(csv, "%lg %lg", point, calc);
		if(point + step < to)
			putc('\n', csv);
	}

	var_val[(int)var] = old_point;

	return 0;
}

int DrawPlot(const node_t *tree, const char var, double var_val[], double from, double to, const size_t n_points)
{
	if(tree == NULL || var_val == NULL)
		return 1;
	
	if(from > to)
	{
		double t = from;
		from = to;
		to = t;
	}
	
	if(n_points == 0)
		return 0;
	
	/*-------------------------------*/

	FILE *csv = fopen("plot.csv", "w");
	assert(csv);
	
	if(CreateCSV(csv, tree, var, var_val, from, to, n_points))
	{
		fclose(csv);
		return 1;
	}

	fclose(csv);
	
	int status = system("gnuplot ~/gnuplot_script.plt");

	
	if(status == 0)
		system("rm plot.csv");

	return status;
}

void PlacePlotTEX(FILE *tex)
{
	if(tex == NULL)
		return;

	fprintf(tex, "\\begin{figure}[H]\n"
				 "\\begin{center}\n"
				 "\\includegraphics[width=0.5\\linewidth]{plot.pdf}\n"
				 "\\end{center}\n"
				 "\\end{figure}\n");
}
