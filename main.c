
#include "main.h"

#define EPSILON 1e-9

typedef struct s_ray
{
	double	direct;//direction in rad
	double	vec_x;//direction as 2d unit vec
	double	vec_y;//diretion as 2d unit vec
	int		x;
	int		y;
}	t_ray;

int	direct_x;
int	direct_y;
int	next_x;
int	next_y;
int	x_check;
int	y_check;

FILE	*nn;
FILE	*np;
FILE	*pn;
FILE	*pp;


void	determine_checks(void)
{
	if (direct_x < 0)
		x_check = (next_x / CUBE_SIZE );
	else
		x_check = (next_x / CUBE_SIZE ) - 1;
		//x_check = next_x / CUBE_SIZE;
	if (direct_y < 0)
		y_check = (next_y / CUBE_SIZE);
		//y_check = (next_y / CUBE_SIZE * CUBE_SIZE + CUBE_SIZE) / CUBE_SIZE;
	else
		y_check = (next_y / CUBE_SIZE);
}

void	ray_wall_intersection(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
{
	direct_x = CUBE_SIZE;
	direct_y = CUBE_SIZE;

	if (ray.vec_x < 0)
		direct_x *= -1;
	if (ray.vec_y < 0)
		direct_y *= -1;
	//int		next_x;// in cube index
	//int		next_y;// in cube index
	double a = 0;
	double b = 0;
	if (direct_x < 0)
		next_x = ((int)ray.x / CUBE_SIZE) * CUBE_SIZE;
	else
	{
		next_x = ((int)ray.x) / CUBE_SIZE * CUBE_SIZE;
		//a -= CUBE_SIZE;
	}
	if (direct_y < 0)
		next_y = ((int)ray.y) / CUBE_SIZE * CUBE_SIZE;
	else
		next_y = ((int)ray.y) / CUBE_SIZE * CUBE_SIZE;

	//int	x_check;
	//int	y_check;
	a += fabs((next_x - ray.x) / ray.vec_x);
	b += fabs((next_y - ray.y) / ray.vec_y);

	determine_checks();
	while (!world.map[y_check][x_check])
	{
		a = fabs((next_x - ray.x) / ray.vec_x);
		b = fabs((next_y - ray.y) / ray.vec_y);
		if (a < b)
		{
			next_x += direct_x;
		}
		else
		{
			next_y += direct_y;
		}
		determine_checks();
		//a = fabs((next_x - ray.x) / ray.vec_x);
		//b = fabs((next_y - ray.y) / ray.vec_y);

	}
	//if (world.map[next_y / CUBE_SIZE][next_x / CUBE_SIZE] == 1)
	//{
	//	*color = SIDE_NONE;
	//	return ;
	//}
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
	//static int nn_cnt = 0;
	//static double nn_sum = 0;
	//static int pp_cnt = 0;
	//static double pp_sum = 0;
	//static int pn_cnt = 0;
	//static double pn_sum = 0;
	//static int np_cnt = 0;
	//static double np_sum = 0;
	//double av = 0;
	//if (direct_x > 0 && direct_y > 0)
	//{
	//	pp_cnt++;
	//	pp_sum += MAX(a, b);
	//	av = pp_sum / pp_cnt;
	//	static double last_max = -1;
	//	if (last_max == MAX(a, b))
	//		return ;
	//	last_max = MAX(a, b);
	//	fprintf(pp, "-------------------------------\n");
	//	fprintf(pp, "count: %d\n", pp_cnt);
	//	fprintf(pp, "MAX: %lf\n", MAX(a, b));
	//	fprintf(pp, "a: %lf, b: %lf\n", a, b);
	//	fprintf(pp, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	//}
	//else if (direct_x > 0 && direct_y < 0)
	//{
	//	pn_cnt++;
	//	pn_sum += MAX(a, b);
	//	av = pn_sum / pn_cnt;
	//	fprintf(pn, "a: %lf, b: %lf\n", a, b);
	//	fprintf(pn, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	//}
	//else if (direct_x < 0 && direct_y > 0)
	//{
	//	np_cnt++;
	//	np_sum += MAX(a, b);
	//	av = np_sum / np_cnt;
	//	fprintf(np, "a: %lf, b: %lf\n", a, b);
	//	fprintf(np, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	//}
	//else if (direct_x < 0 && direct_y < 0)
	//{
	//	nn_cnt++;
	//	nn_sum += MAX(a, b);
	//	av = nn_sum / nn_cnt;
	//	fprintf(nn, "a: %lf, b: %lf\n", a, b);
	//	fprintf(nn, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	//}
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
	{
		//printf("min_texture_y: %d; max_texture_y: %d\n", min_texture_y, max_texture_y);
		return ;
	}
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
	
	double min_dist = 10000000000000;
	double max_dist = 0;
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
			if (side != SIDE_NONE)
			{
				min_dist = MIN(dist, min_dist);
				max_dist = MAX(dist, max_dist);
				t_texture texture = mux_texture(main_data, side);
				draw_ray(main_data, texture, dist, ray_index, side, ray.vec_x * dist + ray.x, ray.vec_y * dist + ray.y);
			}
		}
		ray.direct -= ANGLE_PER_RAY;
		ray_index++;
	}
	//printf("min dist: %lf; max dist: %lf\n", min_dist, max_dist);
}

#define CROSSAIR_THICKNESS 5

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
	for (int y = HEIGHT / 2 - CROSSAIR_THICKNESS; y <= HEIGHT / 2 + CROSSAIR_THICKNESS; y++)
	{
		for (int x = WIDTH / 2 - WIDTH / 15; x <= WIDTH / 2 + WIDTH / 15; x++)
		{
			((uint32_t *)(main_data->img->pixels))[y * main_data->img->width + x] = RED;
		}
	}
	for (int y = HEIGHT / 2 - HEIGHT / 20; y <= HEIGHT / 2 + HEIGHT / 20; y++)
	{
		for (int x = WIDTH / 2 - CROSSAIR_THICKNESS; x <= WIDTH / 2 + CROSSAIR_THICKNESS; x++)
		{
			((uint32_t *)(main_data->img->pixels))[y * main_data->img->width + x] = RED;
		}
	}
}

void	ft_key_hook(mlx_key_data_t keydata, void *data)
{
	t_main	*main_data;

	main_data = (t_main *)data;
	if (keydata.key == MLX_KEY_W)
	{
		main_data->world.player.x += 30 * cos(main_data->world.player.direct);
		main_data->world.player.y -= 30 * sin(main_data->world.player.direct);
	}
	if (keydata.key == MLX_KEY_S)
	{
		main_data->world.player.x -= 30 * cos(main_data->world.player.direct);
		main_data->world.player.y += 30 * sin(main_data->world.player.direct);
	}
	if (keydata.key == MLX_KEY_A)
	{
		double	side_direct = main_data->world.player.direct - M_PI_2;
		main_data->world.player.x -= 30 * cos(side_direct);
		main_data->world.player.y += 30 * sin(side_direct);
	}
	if (keydata.key == MLX_KEY_D)
	{
		double	side_direct = main_data->world.player.direct + M_PI_2;
		main_data->world.player.x -= 30 * cos(side_direct);
		main_data->world.player.y += 30 * sin(side_direct);
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

	nn = fopen("nn", "w");
	np = fopen("np", "w");
	pn = fopen("pn", "w");
	pp = fopen("pp", "w");
	printf("hi\n");
	init_main(&main_data, ac, av);
	parser(&main_data, ac, av);
	mlx_key_hook(main_data.mlx, ft_key_hook, &main_data);
	if (!mlx_loop_hook(main_data.mlx, ft_loop_hook, &main_data))
		ft_error(&main_data, __FILE__, __LINE__, mlx_strerror(mlx_errno));
	main_data.texture_top = load_png(&main_data, "point_at_mat.png");
	main_data.texture_bot = load_png(&main_data, "point_at_mat.png");
	left_right_inverse_texture(&main_data.texture_bot);
	main_data.texture_left = load_png(&main_data, "Desktop.png");
	left_right_inverse_texture(&main_data.texture_left);
	main_data.texture_right = load_png(&main_data, "Desktop.png");
	mlx_loop(main_data.mlx);
	return (0);
}

void	ray_wall_intersection_2(t_ray ray, double *ret_dist, t_world world, uint32_t *color)
{
	t_ray ray_cpy = ray;
	next_y = ray.y / CUBE_SIZE * CUBE_SIZE;
	if (ray.direct < M_PI)
		 next_y--;
	else
		next_y += CUBE_SIZE;
	next_x = ray.x + (ray.y - next_y) / tan(ray.direct);
	int	block_y = next_y / CUBE_SIZE;
	int	block_x = next_x / CUBE_SIZE;
	double dist_y = 0;
	//dist_y += sqrt((ray.x - next_x) * (ray.x - next_x) + (ray.y - next_y) * (ray.y - next_y));
	int y_a;
	int x_a;
	while (next_y >= 0 && next_y < world.y_size && !world.map[next_y][next_x])
	{
		if (ray.direct < M_PI)
			y_a = -CUBE_SIZE;
		else
			y_a = CUBE_SIZE;
		next_y += y_a;
		x_a = CUBE_SIZE / tan(ray.direct);
		next_x += x_a;
		block_y = next_y / CUBE_SIZE;
		block_x = next_x / CUBE_SIZE;
		dist_y += sqrt((ray.x - next_x) * (ray.x - next_x) + (ray.y - next_y) * (ray.y - next_y));
		ray.x = next_x;
		ray.y = next_y;
	}
	ray = ray_cpy;
	double dist_x = 0;
	next_x = ray.x / CUBE_SIZE * CUBE_SIZE;
	if (ray.direct < M_PI_2 || ray.direct > M_PI + M_PI_2)
		 next_x += CUBE_SIZE;
	else
		next_x--;
	next_y = ray.y + (ray.x - next_x) * tan(ray.direct);
	block_y = next_y / CUBE_SIZE;
	block_x = next_x / CUBE_SIZE;
	dist_x += sqrt((ray.x - next_x) * (ray.x - next_x) + (ray.y - next_y) * (ray.y - next_y));
	while (next_x >= 0 && next_x < world.x_size && !world.map[next_y][next_x])
	{
		if (ray.direct < M_PI_2 || ray.direct > M_PI + M_PI_2)
			next_x += CUBE_SIZE;
		else
			next_x -= CUBE_SIZE;
		block_y = next_y / CUBE_SIZE;
		block_x = next_x / CUBE_SIZE;
		dist_x += sqrt((ray.x - next_x) * (ray.x - next_x) + (ray.y - next_y) * (ray.y - next_y));
		ray.x = next_x;
		ray.y = next_y;
	}
	if (dist_x < dist_y)
	{
		*ret_dist = dist_x;
		if (ray.direct < M_PI_2 || ray.direct > M_PI + M_PI_2)
			*color = SIDE_RIGHT;
		else
			*color = SIDE_LEFT;
	}
	else
	{
		*ret_dist = dist_y;
		
		if (ray.direct < M_PI)
			*color = SIDE_TOP;
		else
			*color = SIDE_BOT;
	}
	static int nn_cnt = 0;
	static double nn_sum = 0;
	static int pp_cnt = 0;
	static double pp_sum = 0;
	static int pn_cnt = 0;
	static double pn_sum = 0;
	static int np_cnt = 0;
	static double np_sum = 0;
	double av = 0;
	double a = dist_x;
	double b = dist_y;
	if (direct_x > 0 && direct_y > 0)
	{
		pp_cnt++;
		pp_sum += MAX(a, b);
		av = pp_sum / pp_cnt;
		static double last_max = -1;
		if (last_max == MAX(a, b))
			return ;
		last_max = MAX(a, b);
		fprintf(pp, "-------------------------------\n");
		fprintf(pp, "count: %d\n", pp_cnt);
		fprintf(pp, "MAX: %lf\n", MAX(a, b));
		fprintf(pp, "a: %lf, b: %lf\n", a, b);
		fprintf(pp, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	}
	else if (direct_x > 0 && direct_y < 0)
	{
		pn_cnt++;
		pn_sum += MAX(a, b);
		av = pn_sum / pn_cnt;
		fprintf(pn, "a: %lf, b: %lf\n", a, b);
		fprintf(pn, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	}
	else if (direct_x < 0 && direct_y > 0)
	{
		np_cnt++;
		np_sum += MAX(a, b);
		av = np_sum / np_cnt;
		fprintf(np, "a: %lf, b: %lf\n", a, b);
		fprintf(np, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	}
	else if (direct_x < 0 && direct_y < 0)
	{
		nn_cnt++;
		nn_sum += MAX(a, b);
		av = nn_sum / nn_cnt;
		fprintf(nn, "a: %lf, b: %lf\n", a, b);
		fprintf(nn, "next_x: %d, next_y: %d\nav: %lf\n", next_x, next_y, av);
	}
}

