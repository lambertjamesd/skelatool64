
GCC_FLAGS = -Wall -Werror -g -I./assimp-5.0.1/include

LINKER_FLAGS = -L./assimp-5.0.1/lib -lassimp

SRC_FILES = main.cpp $(wildcard src/*.cpp)

OBJ_FILES =	$(patsubst %.cpp, build/%.o, $(SRC_FILES))

DEPS = $(patsubst %.cpp, build/%.d, $(SRC_FILES))

.PHONY: default
default: skeletool64

-include $(DEPS)

build/%.o: %.cpp
	@mkdir -p $(@D)
	g++ $(GCC_FLAGS) -c $< -o $@
	$(CC) $(GCINCS) $(LCINCS) -MF"$(@:.o=.d)" -MG -MM -MP -MT"$@" "$<"

skeletool64: $(OBJ_FILES)
	g++ -g -o skeletool64 $(OBJ_FILES) $(LINKER_FLAGS)

clean:
	rm $(OBJ_FILES) skeletool64

install: skeletool64
	cp skeletool64 ~/.local/bin