SRC_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)

NAME = BadApple

RM = rm -f
CC = g++

LDFLAGS = -lncurses $(shell pkg-config --cflags --libs opencv4) $(shell pkg-config --cflags opencv4) -lSDL2 -lSDL2_mixer

WFLAGS = -Wall -Wextra -Werror -Wno-deprecated-enum-enum-conversion

CPPFLAGS = -std=gnu++20 -g3 -I./include -I./src

FLAGS = $(LDFLAGS) $(CPPFLAGS) $(WFLAGS)# -fsanitize=address

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(NAME) $(FLAGS)

clean:
	$(RM) $(OBJ_FILES)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
