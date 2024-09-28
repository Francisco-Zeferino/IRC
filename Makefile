NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./includes

SRCS = $(shell find src -name '*.cpp')

OBJS_DIR = objs
OBJS = $(patsubst %.cpp,$(OBJS_DIR)/%.o,$(notdir $(SRCS)))


all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

vpath %.cpp $(sort $(dir $(SRCS)))

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

run:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes \
	./ircserv 8080 123

.PHONY: all clean fclean re