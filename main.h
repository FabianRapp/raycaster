#ifndef MAIN_H
# define MAIN_H

# include <stdio.h>
# include <stdint.h>
# include <math.h>
# include "./libft/libft.h"

# define CUBE_SIZE 64
# define PLAYER_HEIGHT 32

# define FOV ((60.0 / 180.0) * M_PI)
# define Z_NEAR (WIDTH / 2 / tanf(FOV / 2))
# define ANDLE_PER_COL (FOV / WIDTH)

# define WIDTH 700
# define HEIGHT 500

typedef struct s_world
{
	uint16_t		x_size;
	uint16_t		y_size;
}	t_world;

typedef struct s_player
{
	uint16_t	x;
	uint16_t	y;
	double		direct;
}	t_player;

typedef struct s_main
{
	t_world	world;
}	t_main;


#endif
