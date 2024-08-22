#ifndef MAIN_H
# define MAIN_H

// ------------------ debug flags
//#define NO_TEXTURE
//------------------------------


# include <stdio.h>
# include <stdint.h>
# include <math.h>
# include "./MLX42/include/MLX42/MLX42.h"
# include "./MLX42/include/MLX42/MLX42_Int.h"

# include "./libft/libft.h"

#define BLACK 0xFF000000
#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define MAGENTA 0xFFFF00FF
#define YELLOW 0xFF00FFFF
#define WHITE 0xFFFFFFFF
#define CYAN 0xFF00FFFF
#define PURPLE 0xFFFF00A0
#define ORANGE 0xFF0080FF
#define TEAL 0xFF808000
#define PINK 0xFFFF80FF
#define LIME 0xFF80FF00
#define LIGHT_GREY 0xFFD3D3D3
#define DARK_GREY 0xFFA9A9A9

#define SIDE_TOP (1)
#define SIDE_LEFT (2)
#define SIDE_RIGHT (3)
#define SIDE_BOT (4)


# define CUBE_SIZE 256
# define PLAYER_HEIGHT 128

# define FOV ((60.0 / 180.0) * M_PI)
# define Z_NEAR (WIDTH / 2.0 / tanf(FOV / 2))
# define ANGLE_PER_RAY (FOV / WIDTH)

# define WIDTH 1300
# define HEIGHT 800

typedef struct s_player
{
	double		x;//in units
	double		y;//in units
	double		direct;//in rad
}	t_player;

typedef struct s_world
{
	t_player		player;
	uint16_t		x_size;//in cubes
	uint16_t		y_size;//in cubes
	char			**map;
}	t_world;

typedef struct s_texture
{
	uint32_t	*buffer;
	uint32_t	width;
	uint32_t	height;
}	t_texture;

typedef struct s_main
{
	t_world		world;
	mlx_t		*mlx;
	mlx_image_t	*img;
	t_texture	texture1;
	t_texture	texture2;
	t_texture	texture_top;
	t_texture	texture_left;
	t_texture	texture_right;
	t_texture	texture_bot;
}	t_main;

// exit.c
void	cleanup(t_main *main_data);
void	ft_error(t_main *main_data, char *file, int line, const char *text);

// init/init.c
void	init_main(t_main *main_data, int ac, char *av[]);

//parser.c
void	parser(t_main *main_data, int ac, char *av[]);

#endif
