#include "main.h"

void	cleanup(t_main *main_data)
{
	if (main_data->mlx)
		mlx_close_window(main_data->mlx);
}

void	ft_error(t_main *main_data, char *file, int line, const char *text)
{
	ft_fprintf(2, "Error in file \"%s\" at line %d", file, line);
	if (text)
		ft_fprintf(2, ": %s\n", text);
	else
		ft_fprintf(2, "\n");
	cleanup(main_data);
	if (errno)
		exit(errno);
	else if (mlx_errno)
		exit(mlx_errno);
	exit(1);

}
