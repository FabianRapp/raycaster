
#include "main.h"

typedef struct s_ray
{
	double	direct;
	double	direct_x;
	double	direct_y;
	double	x;
	double	y;
}	t_ray;

uint16_t	ray_dist(t_world world, double ray_direct)
{
	int	x;
	int	y;

	double	ray_angle;

	x = world.player.x;
	y = world.player.y;
	while (!world.map[y][x])
	{

	}
	return (0);
}

//naive, slow and prob wrong
void	ray_wall_intersection(t_ray ray, int *ret_x, int *ret_y, int *ret_dist, t_world world)
{
	*ret_dist = 0;

	while (!world.map[(int)ray.y][(int)ray.x])
	{
		ray.x += ray.direct_x;
		ray.y += ray.direct_y;
		*ret_dist += 1;
		if (ray.x < 0 || ray.x >= WIDTH)
		{
			printf("x: %lf\n", ray.x);
			exit(1);
		}
		if (ray.y < 0 || ray.y >= HEIGHT)
		{
			printf("y: %lf\n", ray.y);
			exit(1);
		}
		//printf("iter %d\n", iter++);
		//printf("x: %d\ny: %d\n", ray.x, ray.y);
	}
	*ret_x = (int)ray.x;
	*ret_y = (int)ray.y;
}

void	draw_ray(t_main *main_data, uint16_t wall_dist, int x)
{
	int projected_size = 1 / (double)wall_dist * Z_NEAR;

	if (projected_size >= HEIGHT)
	{
		return ;
		printf("HEIGT - projected_size: %d\n", HEIGHT - projected_size);
		projected_size = HEIGHT;
	}
	printf("try\n");
	int	y = HEIGHT / 2 - projected_size / 2;
	for (int i = 0; i < projected_size; i++)
	{
		((uint32_t *)(main_data->img->pixels))[(y + i) * WIDTH + x] = 0;
	}
}

void	project(t_main *main_data)
{
	uint16_t			ray_index;
	const t_world		world = main_data->world;
	t_ray				ray;

	ray.direct = world.player.direct - (FOV / 2);
	ray.x = world.player.x;
	ray.y = world.player.y;
	ray_index = 0;
	int		hit_x;
	int		hit_y;
	int		dist;

	while (ray_index < WIDTH)
	{

		if (ray.direct < 0)
			ray.direct = 2 * M_PI + ray.direct;
		if (ray.direct > 2 * M_PI)
			ray.direct = ray.direct - 2 * M_PI;
		ray.direct_x = cos(ray.direct);
		ray.direct_y = sin(ray.direct);
		assert(ray.direct >= 0 && ray.direct < 2 * M_PI);
		double ray_len = sqrt(ray.direct_x * ray.direct_x + ray.direct_y * ray.direct_y);
		if (ray_len < 0.98 || ray_len > 1.02)
		{
			printf("ray_len: %lf\n", ray_len);
			printf("direct_x: %lf\ndirect_y: %lf\n", ray.direct_x, ray.direct_y);
			assert(0);
		}
		else
		{
			ray_wall_intersection(ray, &hit_x, &hit_y, &dist, world);
			//ft_printf("found_wall %d after %d\n", ray_index, dist);
			draw_ray(main_data, dist, ray_index);
		}
		ray.direct += ANGLE_PER_RAY;
		ray_index++;
	}
}

void	ft_loop_hook(void *data)
{
	t_main	*main_data;

	main_data = (t_main *)data;
	ft_memset(main_data->img->pixels, 0xFF, WIDTH * HEIGHT * sizeof(uint32_t));
	project(main_data);
	main_data->world.player.direct += 0.01;
	if (main_data->world.player.direct < 0)
		main_data->world.player.direct = 2 * M_PI + main_data->world.player.direct;
	if (main_data->world.player.direct > 2 * M_PI)
		main_data->world.player.direct = main_data->world.player.direct - 2 * M_PI;
}

int	main(int ac, char *av[])
{
	t_main	main_data;

	printf("hi\n");
	init_main(&main_data, ac, av);
	parser(&main_data, ac, av);
	if (!mlx_loop_hook(main_data.mlx, ft_loop_hook, &main_data))
		ft_error(&main_data, __FILE__, __LINE__, mlx_strerror(mlx_errno));
	mlx_loop(main_data.mlx);
	return (0);
}
