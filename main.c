
#include "main.h"

#define EPSILON 1e-9

typedef struct s_veci2
{
	int	x;
	int	y;
}	t_veci2;

typedef struct s_vecf2
{
	double	x;
	double	y;
}	t_vecf2;

typedef struct s_line
{
	t_veci2	p;
	t_vecf2	direct;
}	t_line;
typedef struct s_ray
{
	double	direct;//direction in rad
	t_line	l;
}	t_ray;

typedef enum e_planes
{
	X_PLANE,
	Y_PLANE,
}	t_planes;

typedef struct s_intersection
{
	t_veci2		p;
	t_planes	type;
	double		dist;//from point of given line
}	t_intersection;

bool	reset = true;
bool	pmap = true;

//double	ray_x_intersect(t_ray ray, double *ret_dist, t_world world)
//{
//	double	dist = 0;
//
//	while (!world[ray.y / CUBE_SIZE][ray.x / CUBE_SIZE]
//	{
//		ray.x += ray.vec_x;
//		dist += 1.0;
//	}
//}
//
//void	ray_wall_intersection2(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
//{
//	double	dist_x;
//	double	dist_y;
//}

t_intersection	next_plane(t_line line)
{
	t_intersection	intersection;
	t_veci2			x_p;
	t_veci2			y_p;
	t_vecf2			dists;

	if (line.direct.x < 0)
	{
		x_p.x = line.p.x / CUBE_SIZE * CUBE_SIZE - 1;
	}
	else
	{
		x_p.x = (line.p.x / CUBE_SIZE + 1) * CUBE_SIZE;
	}
	dists.x = (line.p.x - x_p.x) / line.direct.x;
	if (line.direct.y < 0)
	{
		y_p.y = line.p.y / CUBE_SIZE * CUBE_SIZE - 1;
	}
	else
	{
		y_p.y = (line.p.y / CUBE_SIZE + 1) * CUBE_SIZE;
	}
	dists.y = (y_p.y - line.p.y) / line.direct.y;
	if (dists.y > dists.x)
	{
		intersection.dist = dists.y;
		y_p.x = line.p.x + line.direct.x * dists.y;
		intersection.p = y_p;
	}
	else
	{
		intersection.dist = dists.x;
		x_p.y = line.p.y + line.direct.y * dists.x;
		intersection.p = x_p;
	}
	return (intersection);
}

void	ray_wall_intersection(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
{
}

void	draw_ray(t_main *main_data, t_texture texture, double wall_dist,
		int x, uint32_t side, int hit_x, int hit_y)
{
	if (wall_dist <= 0.001)
		return;
		//printf("%10lf\n", wall_dist);
	int projected_size = CUBE_SIZE / wall_dist * Z_NEAR;
	if (wall_dist <= 0.001)
		printf("projected+size: %d\n", projected_size);
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
	ray.l.p.x = world.player.x;
	ray.l.p.y = world.player.y;
	ray_index = 0;
	double	dist;
	uint32_t side;

	while (ray_index < WIDTH)
	{
		//if (ray.direct < 0)
		//	ray.direct = 2 * M_PI + ray.direct;
		//if (ray.direct > 2 * M_PI)
		//	ray.direct = ray.direct - 2 * M_PI;
		ray.l.direct.x = cos(ray.direct);
		ray.l.direct.y = sin(ray.direct + M_PI);
		//printf("direct: %lf; vec_x: %lf; vec_y: %lf\n", ray.direct, ray.vec_x, ray.vec_y);
		//assert(ray.direct >= 0 && ray.direct < 2 * M_PI);
		double ray_len = sqrt(ray.l.direct.x * ray.l.direct.x + ray.l.direct.y * ray.l.direct.y);
		if (ray_len < 0.98 || ray_len > 1.02)
		{
			printf("ray_len: %lf\n", ray_len);
			printf("vec_x: %lf\nvec_y: %lf\n", ray.l.direct.x, ray.l.direct.y);
			assert(0);
		}
		//ray_wall_intersection_old(ray, &dist, world, &side);
		ray_wall_intersection(ray, &dist, world, &side);
		if (dist != INFINITY)
		{
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
	printf("vec_x: %lf; vec_y: %lf\n", cos(direct), sin(direct));
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
	if (pmap)
		print_mini_map(main_data);
}

void	ft_key_hook(mlx_key_data_t keydata, void *data)
{
	t_main	*main_data;

	main_data = (t_main *)data;
	if (keydata.key == MLX_KEY_W)
	{
		main_data->world.player.x += 35 * cos(main_data->world.player.direct);
		main_data->world.player.y -= 35 * sin(main_data->world.player.direct);
	}
	if (keydata.key == MLX_KEY_S)
	{
		main_data->world.player.x -= 35 * cos(main_data->world.player.direct);
		main_data->world.player.y += 35 * sin(main_data->world.player.direct);
	}
	if (keydata.key == MLX_KEY_A)
	{
		double	side_direct = main_data->world.player.direct - M_PI_2;
		main_data->world.player.x -= 10 * cos(side_direct);
		main_data->world.player.y += 10 * sin(side_direct);
	}
	if (keydata.key == MLX_KEY_D)
	{
		double	side_direct = main_data->world.player.direct + M_PI_2;
		main_data->world.player.x -= 10 * cos(side_direct);
		main_data->world.player.y += 10 * sin(side_direct);
	}
	if (keydata.key == MLX_KEY_V && keydata.action == MLX_PRESS)
	{
		reset = true;
	}
	if (keydata.key == MLX_KEY_M && keydata.action == MLX_PRESS)
	{
		pmap = pmap ? false : true;
	}
	if (keydata.key == MLX_KEY_Q)
	{
		main_data->world.player.direct += 0.04;
	}
	if (keydata.key == MLX_KEY_E)
	{
		main_data->world.player.direct -= 0.04;
	}
	if (keydata.key == MLX_KEY_R)
	{
		main_data->world.player.direct = M_PI_4 * round(main_data->world.player.direct / M_PI_4);
	}
	if (keydata.key == MLX_KEY_T)
	{
		main_data->world.player.x = round(main_data->world.player.x / (double)CUBE_SIZE) * CUBE_SIZE;
		main_data->world.player.y = round(main_data->world.player.y / (double)CUBE_SIZE) * CUBE_SIZE;
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
	main_data.texture_top = load_png(&main_data, "Desktop.png");
	main_data.texture_bot = load_png(&main_data, "Desktop.png");
	left_right_inverse_texture(&main_data.texture_bot);
	main_data.texture_left = load_png(&main_data, "point_at_mat.png");
	left_right_inverse_texture(&main_data.texture_left);
	main_data.texture_right = load_png(&main_data, "point_at_mat.png");
	mlx_loop(main_data.mlx);
	return (0);
}
//void	ray_wall_intersection_old(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
//{
//	int	direct_x = CUBE_SIZE;
//	int	direct_y = CUBE_SIZE;
//
//	static int hit_x[WIDTH];
//	static int hit_y[WIDTH];
//	if (reset)
//	{
//		reset = false;
//		ft_bzero(hit_x, sizeof hit_x);
//		ft_bzero(hit_y, sizeof hit_y);
//	}
//
//
//	//int	direct_x = 1;
//	//int	direct_y = 1;
//
//	if (ray.vec_x < 0)
//		direct_x *= -1;
//	if (ray.vec_y < 0)
//		direct_y *= -1;
//
//	char	cell = world.map[ray.y / CUBE_SIZE][ray.x / CUBE_SIZE];
//	int		next_x = ((int)ray.x) / CUBE_SIZE * CUBE_SIZE;
//	int		next_y = ((int)ray.y) / CUBE_SIZE * CUBE_SIZE;
//	
//	if (direct_x < 0)
//	{
//		next_x = (ray.x / CUBE_SIZE) * CUBE_SIZE - 1;
//	}
//	else
//	{
//		next_x = (ray.x / CUBE_SIZE + 1) * CUBE_SIZE;
//	}
//	if (direct_y < 0)
//	{
//		next_y = (ray.y / CUBE_SIZE) * CUBE_SIZE - 1;
//	}
//	else
//	{
//		next_y = (ray.y / CUBE_SIZE + 1) * CUBE_SIZE;
//		//next_x = (((int)ray.x) / CUBE_SIZE - 1) * CUBE_SIZE;
//	}
//	//next_x = ray.x;
//	//next_y = ray.y;
//	double	a = 0;
//	double	b = 0;
//	if (fabs (ray.vec_x) < 0.0001)
//		a = INFINITY;
//	else
//		a = fabs((next_x - ray.x) / ray.vec_x);
//	if (fabs (ray.vec_y) < 0.0001)
//		b = INFINITY;
//	else
//		b = fabs((next_y - ray.y) / ray.vec_y);
//	int		index_x;
//	int		index_y;
//	int		ay;
//	int		bx;
//
//	ay = (ray.y + ray.vec_y * a);
//	bx = ray.x + ray.vec_x * b;
//
//	double when_x;
//
//
//	//if (!cell)
//	//{
//		//cell = world.map[next_y / CUBE_SIZE][next_x / CUBE_SIZE];
//		while (!cell)
//		{
//			if ((next_x > bx && direct_x < 0) || (direct_x > 0 && next_x < bx))
//			{
//				//printf("step x\n");
//				a = fabs((next_x - ray.x) / ray.vec_x);
//				index_x = next_x / CUBE_SIZE;
//				ay = (ray.y + ray.vec_y * a);
//				index_y = ay / CUBE_SIZE;
//				next_x += direct_x;
//			}
//			else
//			{
//				//printf("step y\n");
//				b = fabs((next_y - ray.y) / ray.vec_y);
//				index_y = next_y / CUBE_SIZE;
//				bx = ray.x + ray.vec_x * b;
//				index_x = bx / CUBE_SIZE;
//				next_y += direct_y;
//			}
//
//			if (index_y < 0 || index_y >= world.y_size || index_x < 0 || index_x >= world.x_size)
//			{
//				*ret_dist = INFINITY;
//				return ;
//			}
//			cell = world.map[index_y][index_x];
//			//cell = world.map[index_y][index_x];
//		}
//		//if (index_x != next_x / CUBE_SIZE || index_y != next_y / CUBE_SIZE)
//		//{
//		//	printf("miss: next_x: %d; next_y: %d\n", next_x, next_y);
//		//	printf("inexs: x: %d; y: %d\n", index_x, index_y);
//		//}
//		if (!hit_y[index_y] || !hit_x[index_x])
//		{
//			printf("hit x: %d; y: %d\n", index_x, index_y);
//			hit_y[index_y] = 1;
//			hit_x[index_x] = 1;
//		}
//	//}
//	//else
//	//{
//	//	printf("hit at player position\n");
//	//}
//
//	//a = fabs((next_x - ray.x) / ray.vec_x);
//	//b = fabs((next_y - ray.y) / ray.vec_y);
//	//last_a = fabs(last_a);
//	//last_b = fabs(last_b);
//	//if (a < b && direct_x > 0)
//	//	a = (next_x - ray.x) / ray.vec_x;
//	//if (b <= a && direct_y > 0)
//	//	b = (next_y - ray.y) / ray.vec_y;
//	//a = fabs(a);
//	//b = fabs(b);
//	if (a < b)
//	{
//		*ret_dist = a;
//		if (direct_x < 0)
//			*color = SIDE_LEFT;
//		else
//			*color = SIDE_RIGHT;
//	}
//	else
//	{
//		*ret_dist = b;
//		if (direct_y < 0)
//			*color = SIDE_TOP;
//		else
//			*color = SIDE_BOT;
//	}
//}


