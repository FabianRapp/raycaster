#include "init.h"

void	init_main(t_main *main_data, int ac, char *av[])
{
	ft_bzero(main_data, sizeof(t_main));
	main_data->mlx = mlx_init(WIDTH, HEIGHT, "raycaster", true);
	if (!main_data->mlx)
		ft_error(main_data, __FILE__, __LINE__, mlx_strerror(mlx_errno));
	main_data->img = mlx_new_image(main_data->mlx, WIDTH, HEIGHT);
	if (!main_data->img)
		ft_error(main_data, __FILE__, __LINE__, mlx_strerror(mlx_errno));
	if (mlx_image_to_window(main_data->mlx, main_data->img, 0, 0) == -1)
		ft_error(main_data, __FILE__, __LINE__, mlx_strerror(mlx_errno));
}
