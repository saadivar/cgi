SRC =  multiplexer.cpp request.cpp config_file/location.cpp config_file/server.cpp socket.cpp response.cpp
OBJ = $(SRC:.cpp=.o)
CXX = g++
RM = rm -f
CPPFLAGS =   -std=c++98  -fsanitize=address -g3
NAME = webserve

all: $(NAME)



$(NAME): $(OBJ)
	$(CXX) $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)