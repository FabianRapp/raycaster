
#include "main.h"

void	project(t_main *main_data)
{
}

void	init_main(t_main *main_data)
{
	ft_bzero(main_data, sizeof(t_main));
	main_data->world.x_size = 128;
	main_data->world.y_size = 128;
}

int	main(void)
{
	t_main	main_data;

	init_main(&main_data);
	printf("hi\n");
	return (0);
}
