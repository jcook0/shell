NAME = shell
FLAGS = -Wall -pedantic-errors

all: compile

compile: shell
	gcc -o $(NAME) $(FLAGS) $(NAME).c

clean:
	rm -f *.o $(NAME)

run: 
	./$(NAME)
