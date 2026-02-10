CPP = c++
COMPILE_FLAG = -std=c++98
CPP_FLAGS = -Wall -Wextra -Werror #-fsanitize=address

NAME = webserv
SRC = ./main.cpp ./socket_init/init_socket_engine.cpp ./socket_init/init_client_side.cpp \
		./socket_init/init_server_side.cpp ./multiplexeur/multiplexeur.cpp \
		./config_parsing/extracting_values.cpp ./config_parsing/content_final_checks.cpp \
		./config_parsing/get_values.cpp ./config_parsing/location_block.cpp ./config_parsing/server_block.cpp \
		./config_parsing/syntax_validation.cpp ./multiplexeur/response_handler.cpp ./multiplexeur/request.cpp \
		./multiplexeur/response.cpp 

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(COMPILE_FLAG) $(POLY) $(CPP_FLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp
	$(CPP) $(COMPILE_FLAG) $(POLY) $(CPP_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
