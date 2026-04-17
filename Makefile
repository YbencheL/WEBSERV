CPP = c++
COMPILE_FLAG = -std=c++98
CPP_FLAGS = -Wall -Wextra -Werror #-fsanitize=address

NAME = webserv
SRC = ./main.cpp ./socket_init/init_socket_engine.cpp ./socket_init/init_client_side.cpp \
		./socket_init/init_server_side.cpp ./multiplexer/multiplexer.cpp \
		./multiplexer/server_event.cpp ./multiplexer/client_event.cpp \
		./multiplexer/handle_epollin.cpp ./multiplexer/handle_epollout.cpp \
		./multiplexer/handle_pipe_read.cpp ./multiplexer/handle_pipe_write.cpp \
		./config_parsing/src/extracting_values.cpp ./config_parsing/src/content_final_checks.cpp ./config_parsing/src/helper_functions.cpp \
        ./config_parsing/src/location_block.cpp ./config_parsing/src/server_block.cpp ./config_parsing/src/syntax_validation.cpp ./config_parsing/src/location_block_handlers.cpp \
        ./config_parsing/src/server_block_handlers.cpp ./config_parsing/src/server_block_lookup.cpp ./response/response_builder.cpp ./response/response.cpp \
		./response/default_page_gen.cpp ./utils/helper_utils.cpp ./response/path_validation.cpp ./methodes/handle_post.cpp \
		./methodes/handle_delete.cpp ./methodes/handle_get.cpp ./request/request.cpp client.cpp ./request/requestParse/requestParser.cpp \
		./request/requestParse/requestLineParser.cpp ./request/requestParse/headersParser.cpp ./request/requestParse/parseBody.cpp \
		./cgi/cgi.cpp

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
