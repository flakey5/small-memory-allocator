CC = g++
CCFLAGS = -Wall -Wextra -Wpedantic -pedantic-errors -Wno-unused-parameter -std=c++2a -g -O2 -fmax-errors=1 -D_GLIBCXX_DEBUG -fsanitize=undefined -fsanitize=address
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=%.o)
INC = $(wildcard *.hpp)

a.out: $(OBJ)
	$(CC) $(CCFLAGS) $^

%.o: %.cpp %.hpp
	$(CC) $(CCFLAGS) -c $<

%.o: %.cpp %.hpp

clean:
	rm -f a.out *.o
