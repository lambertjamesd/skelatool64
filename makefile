
GCC_FLAGS = -Wall -Werror -g -I./assimp/include -I./yaml-cpp/include

LINKER_FLAGS = -L./assimp/bin -L./yaml-cpp -lassimp -lyaml-cpp

SRC_FILES = main.cpp $(wildcard src/*.cpp)

OBJ_FILES =	$(patsubst %.cpp, build/%.o, $(SRC_FILES))

DEPS = $(patsubst %.cpp, build/%.d, $(SRC_FILES))

.PHONY: default
default: skeletool64

-include $(DEPS)

build/%.o: %.cpp
	@mkdir -p $(@D)
	g++ $(GCC_FLAGS) -c $< -o $@
	$(CC) $(GCC_FLAGS) -MM $^ -MF "$(@:.o=.d)" -MT"$@"

skeletool64: $(OBJ_FILES)
	g++ -g -o skeletool64 $(OBJ_FILES) $(LINKER_FLAGS)

clean:
	rm -r build/

init:
	

install: skeletool64
	cp skeletool64 ~/.local/bin