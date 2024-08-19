
#include "main.h"

typedef struct s_ray
{
	double	direct;
	double	vec_x;
	double	vec_y;
	double	x;
	double	y;
}	t_ray;

//naive, slow and prob wrong
void	ray_wall_intersection(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
{
	*ret_dist = 0;
	int	direct_x = 1;
	int	direct_y = 1;
	if (ray.vec_x < 0)
		direct_x = -1;
	if (ray.vec_y < 0)
		direct_y = -1;
	static int	ac = 0;
	static int  bc = 0;
	uint32_t	side;
	ray.x *= CUBE_SIZE;
	ray.y *= CUBE_SIZE;
	int	next_x;
	int	next_y;
	while (!world.map[(int)(ray.y / CUBE_SIZE)][(int)(ray.x / CUBE_SIZE)])
	{
		// ray.x + a * ray.vec_x = ray.x + direct_x
		// ray.x + a * ray.vec_x = next_x
		// a = (next_x - ray.x ) / ray.vec_x
		next_x = (int)(ray.x + direct_x);
		next_y = (int)(ray.y + direct_y);
		//double a = (ray.x + direct_x - ray.x) / ray.vec_x;
		double a = (next_x - ray.x ) / ray.vec_x;
		double b = (next_y - ray.y ) / ray.vec_y;
		//double b = (ray.y + direct_y - ray.y) / ray.vec_y;
		if (a < b)
		{
			ac++;
			ray.x += ray.vec_x * a;
			ray.y += ray.vec_y * a;
			*ret_dist += a;
			side = RED;
		}
		else
		{
			bc++;
			ray.x += ray.vec_x * b;
			ray.y += ray.vec_y * b;
			*ret_dist += b;
			side = GREEN;
		}
	}
	if (side == RED && direct_x < 0)
		side = PINK;
	if (side == GREEN && direct_y < 0)
		side = YELLOW;
	*color = side;
	//printf("dist: %lf\n", *ret_dist);
	//printf("ac: %d; bc: %d; (ac / bc: %lf)\n", ac, bc, (double)ac / bc);
}

void	draw_ray(t_main *main_data, double wall_dist, int x, uint32_t side)
{
	//int projected_size = 1 / wall_dist * Z_NEAR;
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
			ray_wall_intersection(ray, &dist, world, &side);
			//ft_printf("found_wall %d after %d\n", ray_index, dist);
			draw_ray(main_data, dist, ray_index, side);
		}
		ray.direct -= ANGLE_PER_RAY;
		ray_index++;
	}
}

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

	char icons[] = ">/^\\<(v))";
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
	if (player == '^')
		printf("YELLOW\n");
	if (player == '<')
		printf("PINK\n");
	if (player == 'v')
		printf("GREEN\n");
	if (player == '>')
		printf("RED\n");
	if (player == '/')
		printf("YELLOW/RED\n");
	if (player == '\\')
		printf("PINK/YELLOW\n");
	if (player == '(')
		printf("GREEN/PINK\n");
	if (player == ')')
		printf("RED/GREEN\n");
	if (!player)
	{
		printf("direct: %lf / pi\n", direct / M_PI);
		exit(1);
	}
	//if (direct <= M_PI - M_PI_4 && direct > M_PI_4)
	//	player = '^';
	//else if (direct <= M_PI_4 || direct > M_PI * 2 - M_PI_4)
	//	player = '>';
	//else if (direct <= M_PI + M_PI_4 && direct > M_PI - M_PI_4)
	//	player = '<';
	//else //if (diect <= 2 * M_PI - M_PI_4 && direct > M_PI + M_PI_4)
	//	 player = 'v';
	map_cpy[(int)main_data->world.player.y][(int)main_data->world.player.x] = player;
	for (int y = 0; y < main_data->world.y_size; y++)
	{
		for (int x = 0; x < main_data->world.x_size; x++)
		{
			if (map_cpy[y][x] > 5)
				printf("%c", map_cpy[y][x]);
			else
				printf("%d", map_cpy[y][x]);
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
	project(main_data);
	main_data->world.player.direct += 0.005;
	if (main_data->world.player.direct < 0)
		main_data->world.player.direct = 2 * M_PI + main_data->world.player.direct;
	if (main_data->world.player.direct > 2 * M_PI)
		main_data->world.player.direct = main_data->world.player.direct - 2 * M_PI;
	print_mini_map(main_data);
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
