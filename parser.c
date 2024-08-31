#include "main.h"

//fills the world with boarders for now
void	fill_edges(t_main *main_data)
{
	char	**map;
	int		y;

	map = main_data->world.map;
	ft_memset(map[0], 1, main_data->world.x_size);
	ft_memset(map[main_data->world.y_size - 1], 1, main_data->world.x_size);
	y = 1;
	while (y < main_data->world.y_size - 1)
	{
		map[y][0] = 1;
		map[y][main_data->world.x_size - 1] = 1;
		y++;
	}
}

void	print_map(t_world world)
{
	int	x;
	int	y;

	y = 0;
	while (y < world.y_size)
	{
		x = 0;
		while (x < world.x_size)
		{
			ft_printf("%c", world.map[y][x] + '0');
			x++;
		}
		ft_printf("\n");
		y++;
	}
}

void	parser(t_main *main_data, int ac, char *av[])
{
	t_world	*world;
	int		y;
	int		i;

	world = &main_data->world;
	//world->x_size = 512;
	//world->y_size = 512;
	world->x_size = 8;
	world->y_size = 10;

	world->map = ft_calloc(sizeof(char *), world->y_size + 1);
	if (!world->map)
		ft_error(main_data, __FILE__, __LINE__, "malloc fail");
	y = 0;
	while (y < world->y_size)
	{
		world->map[y] = ft_calloc(sizeof(char), world->x_size + 1);
		if (!world->map[y])
		{
			i = 0;
			while (i < y)
				free(world->map[i++]);
			free(world->map);
			ft_error(main_data, __FILE__, __LINE__, "malloc fail");
		}
		y++;
	}
	world->player.x = world->x_size / 2.2 * CUBE_SIZE;
	world->player.y = world->y_size / 3.3 * CUBE_SIZE;
	world->player.direct = M_PI_4;
	fill_edges(main_data);
	//for (int y = 1; y < world->y_size - 1; y++)
	//{
	//	for (int x = 1; x < world->x_size - 1; x++)
	//	{
	//		if (!(x % 5))
	//			world->map[y][x] = 1;
	//	}
	//}
	//world->map[4][3] = 2;
	//world->map[9][5] = 2;
	//world->map[7][7] = 2;
	//world->map[3][7] = 1;
	//world->map[12][6] = 1;
}
