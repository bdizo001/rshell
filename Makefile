all:
	g++ -Wall -Werror -pedantic -ansi src/main.cpp
	
clean:
	rm a.out
