#include "main.h"

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
	//printf("\tYELLOW(-y)\nPINK(-x)\tRED(+x)\n\tGREEN(+y)\n");
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
	printf("player(%c) pos: x: %lf; y: %lf\n", player, main_data->world.player.x / CUBE_SIZE, main_data->world.player.y / CUBE_SIZE);
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
