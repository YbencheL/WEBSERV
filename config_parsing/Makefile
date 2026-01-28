CPP = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = config_parsing.cpp content_checks.cpp syntax_validation.cpp

HEADERS = ConfigPars.hpp


OBJ = $(SRC:.cpp=.o)

all: webserv

webserv: $(OBJ) $(HEADERS)
	$(CPP)  $(OBJ) -o webserv

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -f webserv

re: fclean all

.SECONDARY:

.PHONY: all fclean clean re