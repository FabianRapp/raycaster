#ifndef MAIN_H
# define MAIN_H

# include <stdio.h>
# include <stdint.h>
# include <math.h>
# include "./MLX42/include/MLX42/MLX42.h"
# include "./MLX42/include/MLX42/MLX42_Int.h"
# include "./libft/libft.h"

# define CUBE_SIZE 64
# define PLAYER_HEIGHT 32

# define FOV ((60.0 / 180.0) * M_PI)
# define Z_NEAR (WIDTH / 2.0 / tanf(FOV / 2))
# define ANGLE_PER_RAY (FOV / WIDTH)

# define WIDTH 1600
# define HEIGHT 800

typedef struct s_player
{
	double		x;
	double		y;
	double		direct;
}	t_player;

typedef struct s_world
{
	t_player		player;
	uint16_t		x_size;
	uint16_t		y_size;
	char			**map;
}	t_world;

typedef struct s_main
{
	t_world		world;
	mlx_t		*mlx;
	mlx_image_t	*img;
}	t_main;

// exit.c
void	cleanup(t_main *main_data);
void	ft_error(t_main *main_data, char *file, int line, const char *text);

// init/init.c
void	init_main(t_main *main_data, int ac, char *av[]);

//parser.c
void	parser(t_main *main_data, int ac, char *av[]);

#endif
