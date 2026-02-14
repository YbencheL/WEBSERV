NAME = abenzaho

CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -rf

SRC = main.cpp request.cpp client.cpp requestParse/requestParser.cpp requestParse/requestLineParser.cpp requestParse/headersParser.cpp

all : ${NAME}

${NAME} :
	${CPP} ${CPPFLAGS} ${SRC} -o ${NAME}

clean :
	${RM} ${NAME}

re : clean all

.PHONEY : clean all re