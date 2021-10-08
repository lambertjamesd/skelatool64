
GCC_FLAGS = -Wall -Werror -g -I./assimp-5.0.1/include

LINKER_FLAGS = -L./assimp-5.0.1/lib -lassimp

SRC_FILES = main.cpp $(wildcard ./src/*.cpp)

OBJ_FILES =	$(SRC_FILES:.cpp=.o)

.PHONY: default
default: dlgen

%.o: %.cpp
	g++ $(GCC_FLAGS) -c $< -o $@

dlgen: $(OBJ_FILES)
	g++ -g -o dlgen $(OBJ_FILES) $(LINKER_FLAGS)

clean:
	rm $(OBJ_FILES) dlgen