SRC =	config_file/location.cpp \
		Request/FillHeaders.cpp \
		Request/Request.cpp \
		Request/RequestErrors.cpp \
		Request/HandleUri.cpp\
	 	Request/RequestUtils.cpp \
		Request/PostElements.cpp\
	 	Request/Delete.cpp\
   		config_file/server.cpp\
		response/cgi.cpp response/main_loop.cpp response/multi.cpp response/response.cpp\
		main/main.cpp response/post_req.cpp

OBJ = $(SRC:.cpp=.o)
CXX = c++
RM = rm -f
CPPFLAGS =  -Wall -Wextra -Werror  -std=c++98
NAME = webserve

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)