CC= cc
FLAGS_SPEED = -Wall -Wextra -fsanitize=address -g -O0
# -Ofast -march=native -flto

#
#-O3
CFLAGS= $(FLAGS_SPEED)
#-Wextra -Wall -Werror
LIBFT 	=	libft/libft.a
INCLUDES=-I./includes -I./MLX42/include/MLX42 -I./MLX42/include
MLX=MLX42/build/libmlx42.a
MLX_FLAGS_LINUX=-Iinclude -ldl -lglfw -pthread -lm
MLX_FLAGS_MAC= -framework Cocoa $(MLX) -framework OpenGL -framework IOKit -Iinclude -lglfw
#-L"/Users/$(USER)/.brew/opt/glfw/lib/"
MLX_FLAGS_LINUX =-I MLX42/include/MLX42 $(MLX) -Iinclude -ldl -lglfw -pthread -lm
OS := $(shell uname)
ifeq ($(OS), Darwin)
	MLX_FLAGS = $(MLX_FLAGS_MAC)
else
	MLX_FLAGS = $(MLX_FLAGS_LINUX)
endif


NAME=cub3D
SOURCES= \
	main.c \
	exit.c \
	parser.c \
	init/init.c

VPATH = includes:MLX42/include/MLX42/:utils:utils/fixed_point:obj_parser:matrix:menu:init

OBJ_DIR = o_files/
#OBJECTS=$(SOURCES:.c=.o)
TMP=$(notdir $(SOURCES))
OBJECTS=$(TMP:%.c=$(OBJ_DIR)%.o)
	

GREEN	=	\033[0;32m
YELLOW	=	\033[33m
CYAN	=	\033[0;36m
CLEAR	=	\033[0m

.PHONY: clone_mlx42 all clean fclean ffclean

all: mlx $(OBJECTS)
	@cd libft && make
	@$(CC) $(CFLAGS) $(OBJECTS) $(LIBFT) $(INCLUDES) -o $(NAME) $(MLX_FLAGS)
	@echo "$(GREEN)$(NAME) compiled!$(CLEAR)"

echo_sources:
	@echo $(SOURCES)
echo_includes:
	@echo $(INCLUDES)

fast: fclean
	make CFLAGS="-march=native -Ofast -DNDEBUG=1 -mavx"

prof: fclean
	make CFLAGS="-march=native -Ofast -mavx2 -DNDEBUG=1 -g -pg" CC=gcc

asm: fclean $(OBJECTS)
	@cd libft && make
	@$(CC) $(CFLAGS) -S $(OBJECTS) $(LIBFT) $(INCLUDES) -o $(NAME).asm $(MLX_FLAGS)
	@echo "$(GREEN)$(NAME).asm generated!$(CLEAR)"

bs:
	echo $(OBJECTS)

$(OBJ_DIR)%.o: %.c mlx
	@$(CC) $(CFLAGS) -c $< -o$@ $(INCLUDES)

clean: SOURCES += main.c
clean:
	@rm -f $(OBJECTS)
	@rm -rf $(FIXED_TESTS_OBJECT_DIR)
	@cd libft && make clean
	@echo "$(CYAN)object files cleaned$(CLEAR)"

fclean: clean
	@rm -f $(NAME) test
	@cd libft && make fclean
	@echo "$(CYAN)cub3D fclean$(CLEAR)"

ffclean: fclean
	@rm -rf MLX42
	@echo "$(CYAN)cub3D ffclean$(CLEAR)"

re: fclean all

rre: ffclean all

stack_usage: mlx
	$(CC) -fstack-usage -o stack_usage/stackusage $(SOURCES) $(MLX) -lglfw -lm $(LIBFT)

mlx: clone_mlx
	@if [ ! -e $(MLX) ]; then \
		cd MLX42 && cmake -DDEBUG=1 -B build && cmake --build build -j4; \
		echo "$(GREEN) lib_MLX compiled!$(CLEAR)"; \
	fi

clone_mlx:
	@if [ ! -d "MLX42" ]; then \
		git clone https://github.com/codam-coding-college/MLX42.git; \
	fi
