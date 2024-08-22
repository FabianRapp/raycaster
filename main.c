
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


void	ray_wall_intersection(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
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
			*color = SIDE_LEFT;
		else
			*color = SIDE_RIGHT;
	}
	else
	{
		*ret_dist = b;
		if (direct_y < 0)
			*color = SIDE_TOP;
		else
			*color = SIDE_BOT;
	}
}

void	draw_ray(t_main *main_data, t_texture texture, double wall_dist,
		int x, uint32_t side, int hit_x, int hit_y)
{
	int projected_size = CUBE_SIZE / wall_dist * Z_NEAR;
	int	texture_y;
	int	min_texture_y = 0;
	int	max_texture_y = texture.height - 1;
	if (projected_size >= HEIGHT)
	{
		int	overflow = projected_size - HEIGHT;
		const double PLAYER_CUBE_RATIO = PLAYER_HEIGHT / (double)CUBE_SIZE;
		min_texture_y = overflow * (1.0 - PLAYER_CUBE_RATIO);
		max_texture_y = texture.height - overflow * (PLAYER_CUBE_RATIO);
		projected_size = HEIGHT;
	}
	//currently needed to avoid assert or segault when going through walls
	if (min_texture_y > max_texture_y)
		return ;
	int	y = HEIGHT / 2 - projected_size / 2;
	double texture_x_progress;
	if (hit_y % CUBE_SIZE < hit_x % CUBE_SIZE)
		texture_x_progress = (hit_x % CUBE_SIZE) / ((double)CUBE_SIZE);
	else
		texture_x_progress = (hit_y % CUBE_SIZE) / ((double)CUBE_SIZE);
	int	texture_x = texture.width * texture_x_progress;
	MLX_ASSERT(max_texture_y >= min_texture_y, "");
	for (int i = 0; i < projected_size; i++)
	{
		double	progress = i / (double)projected_size;
		texture_y = min_texture_y + (max_texture_y - min_texture_y) * progress;
		((uint32_t *)(main_data->img->pixels))[(y + i) * WIDTH + x] = texture.buffer[texture_y * texture.width + texture_x];
	}
}

t_texture	mux_texture(t_main *main_data, int8_t sel_side)
{
	if (sel_side == SIDE_TOP)
		return (main_data->texture_top);
	else if (sel_side == SIDE_LEFT)
		return (main_data->texture_left);
	else if (sel_side == SIDE_RIGHT)
		return (main_data->texture_right);
	else if (sel_side == SIDE_BOT)
		return (main_data->texture_bot);
	else
		MLX_ASSERT(0, "invalid side for texture selector");
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
			ray_wall_intersection(ray, &dist, world, &side);
			t_texture texture = mux_texture(main_data, side);
			draw_ray(main_data, texture, dist, ray_index, side, ray.vec_x * dist + ray.x, ray.vec_y * dist + ray.y);
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
	if (keydata.key == MLX_KEY_ESCAPE)
	{
		exit(0);
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

//mlx_load_png() wrapper
//t_texture.buffer has to be freed
t_texture	load_png(t_main *main_data, char *path)
{
	mlx_texture_t	*mlx_texture;
	t_texture		texture;

	bzero(&texture, sizeof texture);
	mlx_texture = mlx_load_png(path);
	if (!mlx_texture)
		ft_error(main_data, __FILE__, __LINE__, mlx_strerror(mlx_errno));
	texture.width = mlx_texture->width;
	texture.height = mlx_texture->height;
	texture.buffer = ft_memdup(mlx_texture->pixels, sizeof(uint32_t)
			* mlx_texture->width * mlx_texture->height);
	mlx_delete_texture(mlx_texture);
	if (!texture.buffer)
		ft_error(main_data, __FILE__, __LINE__, "malloc fail");
	return (texture);
}

void	left_right_inverse_texture(t_texture *texture)
{
	uint32_t	x;
	uint32_t	y;
	uint32_t	swap_x;
	uint32_t	tmp;

	y = 0;
	while (y < texture->height)
	{
		x = 0;
		while (x < texture->width / 2)
		{
			swap_x = texture->width - 1 - x;
			tmp = texture->buffer[y * texture->width + x];
			texture->buffer[y * texture->width + x] =
				texture->buffer[y * texture->width + swap_x];
			texture->buffer[y * texture->width + swap_x] = tmp;
			x++;
		}
		y++;
	}
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
	main_data.texture_top = load_png(&main_data, "point_at_mat.png");
	main_data.texture_bot = load_png(&main_data, "point_at_mat.png");
	left_right_inverse_texture(&main_data.texture_bot);
	main_data.texture_left = load_png(&main_data, "pngs/fps_counter/2.png");
	left_right_inverse_texture(&main_data.texture_left);
	main_data.texture_right = load_png(&main_data, "pngs/fps_counter/2.png");
	mlx_loop(main_data.mlx);
	return (0);
}
