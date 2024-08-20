
#include "main.h"

#define EPSILON 1e-9

typedef struct s_ray
{
	double	direct;//direction in rad
	double	vec_x;//direction as 2d unit vec
	double	vec_y;//diretion as 2d unit vec
	double		x;
	double		y;
}	t_ray;

void	ray_wall_intersection_new(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
{
	//int	direct_x = CUBE_SIZE;
	//int	direct_y = CUBE_SIZE;
	int	direct_x = 1;
	int	direct_y = 1;

	if (ray.vec_x < 0)
		direct_x *= -1;
	if (ray.vec_y < 0)
		direct_y *= -1;
	double	next_x_dist;//in world units
	double	next_y_dist;//in world units
	int		next_x;// in cube index
	int		next_y;// in cube index
	if (direct_x < 0)
	{
		next_x = ((int)ray.x) / CUBE_SIZE;
	}
	else
	{
		next_x = ((int)ray.x) / CUBE_SIZE ;
	}
	if (direct_y < 0)
	{
		next_y = ((int)ray.y) / CUBE_SIZE;
	}
	else
	{
		next_y = ((int)ray.y) / CUBE_SIZE ;
	}
	double	a;
	double	b;

	//printf("a: %lf; b: %lf\n", a, b);
	while (!world.map[next_y][next_x])
	{
		a = (next_x * CUBE_SIZE - ray.x) / ray.vec_x;
		b = (next_y * CUBE_SIZE - ray.y) / ray.vec_y;
		if (a < b)
		{
			next_x += direct_x;
		}
		else
		{
			next_y += direct_y;
		}
		//a = (next_x * CUBE_SIZE - ray.x) / ray.vec_x;
		//b = (next_y * CUBE_SIZE - ray.y) / ray.vec_y;
	}
	if (a < b)
	{
		*ret_dist = a;
		if (direct_x < 0)
			*color = PINK;
		else
			*color = RED;
	}
	else
	{
		*ret_dist = b;
		if (direct_y < 0)
			*color = YELLOW;
		else
			*color = GREEN;
	}
}

void	ray_wall_intersection_old(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
{
	*ret_dist = 0;

	int	direct_x = CUBE_SIZE;
	int	direct_y = CUBE_SIZE;
	if (ray.vec_x < 0)
		direct_x *= -1;
	if (ray.vec_y < 0)
		direct_y *= -1;

	uint32_t	side = 0;
	//while (((int)ray.x) % CUBE_SIZE || ((int)ray.y) % CUBE_SIZE)
	//{
	//	ray.x += ray.vec_x;
	//	ray.y += ray.vec_y;
	//	*ret_dist += 1;
	//}

	double	next_x = (ray.x + direct_x);
	double	next_y = (ray.y + direct_y);
	double a = (next_x - ray.x ) / ray.vec_x;
	double b = (next_y - ray.y ) / ray.vec_y;
	t_ray	start_ray = ray;
	/*
		YELLO(-y)
PINK(-x)		RED(+x)
		GREEN(+y)
	*/
	//double	dist_x = 0;
	//double	dist_y = 0;
	//t_ray	base_ray = ray;
	//while (!world.map[(int)(ray.y / CUBE_SIZE)][(int)(ray.x / CUBE_SIZE) ])
	//{
	//	*ret_dist += 1;
	//	ray.x += ray.vec_x;
	//	ray.y += ray.vec_y;
	//	side = RED;
	//}
	//ray = base_ray;
	while (!world.map[(int)(ray.y / CUBE_SIZE)][(int)(ray.x / CUBE_SIZE)])
	{
		a = (next_x - ray.x ) / ray.vec_x;
		b = (next_y - ray.y ) / ray.vec_y;
		
		if (a < b * (1.0 + EPSILON))
		{
			//ray.x += ray.vec_x * a;
			ray.x = next_x;
			ray.y += ray.vec_y * a;
			*ret_dist += a;
			side = RED;
		}
		else if (b < a * (1.0 + EPSILON))
		{
			ray.x += ray.vec_x * b;
			ray.y = next_y;
			//ray.y += ray.vec_y * b;
			*ret_dist += b;
			side = GREEN;
		}
		else
		{
			ray.x += ray.vec_x * b;
			ray.y = next_y;
			//ray.y += ray.vec_y * b;
			*ret_dist += b;
			side = WHITE;
		}
		next_x = (ray.x + direct_x);
		next_y = (ray.y + direct_y);
	}
	double dist_x = (start_ray.x - ray.x);
	double dist_y = (start_ray.y - ray.y);
	double check_dist = sqrt(dist_x * dist_x + dist_y * dist_y);
	assert(fabs(check_dist - *ret_dist) < 0.001);
	if (side == RED && direct_x < 0)
		side = PINK;
	if (side == GREEN && direct_y < 0)
		side = YELLOW;
	*color = side;
}

void	draw_ray(t_main *main_data, double wall_dist, int x, uint32_t side)
{
	int projected_size = CUBE_SIZE / wall_dist * Z_NEAR;

	if (projected_size >= HEIGHT)
	{
		projected_size = HEIGHT;
	}
	int	y = HEIGHT / 2 - projected_size / 2;
	for (int i = 0; i < projected_size; i++)
	{
		((uint32_t *)(main_data->img->pixels))[(y + i) * WIDTH + x] = side;
	}
}

void	project(t_main *main_data)
{
	uint16_t			ray_index;
	const t_world		world = main_data->world;
	t_ray				ray;

	ray.direct = world.player.direct + (FOV / 2);
	//if (ray.direct < 0)
	//	ray.direct = 2 * M_PI + ray.direct;
	//if (ray.direct > 2 * M_PI)
	//	ray.direct = ray.direct - 2 * M_PI;
	ray.x = world.player.x;
	ray.y = world.player.y;
	ray_index = 0;
	double	dist;
	uint32_t side;

	while (ray_index < WIDTH)
	{
		//if (ray.direct < 0)
		//	ray.direct = 2 * M_PI + ray.direct;
		//if (ray.direct > 2 * M_PI)
		//	ray.direct = ray.direct - 2 * M_PI;
		ray.vec_x = cos(ray.direct);
		ray.vec_y = -1 * sin(ray.direct);
		//printf("direct: %lf; vec_x: %lf; vec_y: %lf\n", ray.direct, ray.vec_x, ray.vec_y);
		//assert(ray.direct >= 0 && ray.direct < 2 * M_PI);
		double ray_len = sqrt(ray.vec_x * ray.vec_x + ray.vec_y * ray.vec_y);
		if (ray_len < 0.98 || ray_len > 1.02)
		{
			printf("ray_len: %lf\n", ray_len);
			printf("vec_x: %lf\nvec_y: %lf\n", ray.vec_x, ray.vec_y);
			assert(0);
		}
		else
		{
			//ray_wall_intersection_old(ray, &dist, world, &side);
			ray_wall_intersection_new(ray, &dist, world, &side);
			draw_ray(main_data, dist, ray_index, side);
		}
		ray.direct -= ANGLE_PER_RAY;
		ray_index++;
	}
}

// for debugging to check image vs expected output
void	print_mini_map(t_main *main_data)
{
	char	**map_cpy;

	map_cpy = dyn_arr_init(sizeof(char *), 10000);
	if (!map_cpy)
		ft_error(main_data, __FILE__, __LINE__, "malloc error\n");
	for (int y = 0; y < main_data->world.y_size; y++)
	{
		char *dup = ft_memdup(main_data->world.map[y], main_data->world.x_size);
		dyn_arr_add_save((void **)&map_cpy, &dup, y);
	}
	char	player = 0;
	double	direct = main_data->world.player.direct;

	char icons[] = ">/^\\<(v)>";
	const double M_PI_8 = M_PI_4 / 2;
	double cur = M_PI_8;
	double last = -M_PI_8;
	for (int i = 0; i < 9; i++)
	{
		if (direct >= last && direct <= cur)
			player = icons[i];
		last = cur;
		cur += M_PI_4;
	}
	printf("angle: %lf\n", direct / (M_PI * 2) * 360);
	printf("\tYELLOW(-y)\nPINK(-x)\tRED(+x)\n\tGREEN(+y)\n");
	switch (player)
	{
		case ('^'): printf("YELLOW\n"); break;
		case ('<'): printf("PINK\n"); break;
		case ('>'): printf("RED\n"); break;
		case ('v'): printf("GREEN\n"); break;
		case ('/'): printf("YELLOW/RED\n"); break;
		case ('\\'): printf("PINK/YELLOW\n"); break;
		case ('('): printf("GREEN/PINK\n"); break;
		case (')'): printf("RED/GREEN\n"); break;
		default: printf("direct: %lf / pi\n", direct / M_PI); exit(1);
	}

	map_cpy[(int)main_data->world.player.y / CUBE_SIZE][(int)main_data->world.player.x / CUBE_SIZE] = player;
	printf("player pos: x: %lf; y: %lf\n", main_data->world.player.x / CUBE_SIZE, main_data->world.player.y / CUBE_SIZE);
	printf("   ");
	for (int x = 0; x < main_data->world.y_size; x++)
	{
		printf("%-2d|", x);
	}
	printf("\n");
	for (int y = 0; y < main_data->world.y_size; y++)
	{
		printf("%-2d|", y);
		for (int x = 0; x < main_data->world.x_size; x++)
		{
			if (map_cpy[y][x] > 5)
				printf("%-2c|", map_cpy[y][x]);
			else
				printf("%-2d|", map_cpy[y][x]);
		}
		printf("\n");
		free((map_cpy)[y]);
	}
	dyn_arr_free((void **)&map_cpy);
}

void	ft_loop_hook(void *data)
{
	t_main	*main_data;

	main_data = (t_main *)data;
	ft_memset(main_data->img->pixels, BLACK, WIDTH * HEIGHT * sizeof(uint32_t));
	if (main_data->world.player.direct < 0)
		main_data->world.player.direct = 2 * M_PI + main_data->world.player.direct;
	if (main_data->world.player.direct > 2 * M_PI)
		main_data->world.player.direct = main_data->world.player.direct - 2 * M_PI;
	project(main_data);
	//main_data->world.player.direct += 0.005;
	//if (main_data->world.player.direct < 0)
	//	main_data->world.player.direct = 2 * M_PI + main_data->world.player.direct;
	//if (main_data->world.player.direct > 2 * M_PI)
	//	main_data->world.player.direct = main_data->world.player.direct - 2 * M_PI;
	print_mini_map(main_data);
}

void	ft_key_hook(mlx_key_data_t keydata, void *data)
{
	t_main	*main_data;

	main_data = (t_main *)data;
	if (keydata.key == MLX_KEY_W)
	{
		main_data->world.player.x += 3 * cos(main_data->world.player.direct);
		main_data->world.player.y -= 3 * sin(main_data->world.player.direct);
	}
	if (keydata.key == MLX_KEY_S)
	{
		main_data->world.player.x -= 3 * cos(main_data->world.player.direct);
		main_data->world.player.y += 3 * sin(main_data->world.player.direct);
	}
	if (keydata.key == MLX_KEY_A)
	{
		double	side_direct = main_data->world.player.direct - M_PI_2;
		main_data->world.player.x -= 3 * cos(side_direct);
		main_data->world.player.y += 3 * sin(side_direct);
	}
	if (keydata.key == MLX_KEY_D)
	{
		double	side_direct = main_data->world.player.direct + M_PI_2;
		main_data->world.player.x -= 3 * cos(side_direct);
		main_data->world.player.y += 3 * sin(side_direct);
	}
	if (keydata.key == MLX_KEY_Q)
	{
		main_data->world.player.direct += 0.01;
	}
	if (keydata.key == MLX_KEY_E)
	{
		main_data->world.player.direct -= 0.01;
	}
	if (main_data->world.player.x < CUBE_SIZE)
		main_data->world.player.x = CUBE_SIZE;
	if (main_data->world.player.y < CUBE_SIZE)
		main_data->world.player.y = CUBE_SIZE;
	if (main_data->world.player.x >= (main_data->world.x_size - 1) * (CUBE_SIZE))
		main_data->world.player.x = (main_data->world.x_size - 1) * (CUBE_SIZE) - 1;
	if (main_data->world.player.y >= (main_data->world.y_size - 1) * (CUBE_SIZE))
		main_data->world.player.y = (main_data->world.y_size - 1) * (CUBE_SIZE) - 1;
}


void mlx_key_hook(mlx_t* mlx, mlx_keyfunc func, void* param);
int	main(int ac, char *av[])
{
	t_main	main_data;

	printf("hi\n");
	init_main(&main_data, ac, av);
	parser(&main_data, ac, av);
	mlx_key_hook(main_data.mlx, ft_key_hook, &main_data);
	if (!mlx_loop_hook(main_data.mlx, ft_loop_hook, &main_data))
		ft_error(&main_data, __FILE__, __LINE__, mlx_strerror(mlx_errno));
	mlx_loop(main_data.mlx);
	return (0);
}
