# Nom de l'exécutable
NAME = exe

# Recherche automatique de tous les fichiers .cpp dans src et sous-dossiers
SRC = $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -g \
			-Ilib\glew_x64-mingw-dynamic\include \
			-Ilib\glfw3_x64-mingw-dynamic\include \
			-Ilib\glm_x64-mingw-dynamic\include

# Librairies Windows
LDFLAGS = -lglfw3dll -lglew32 -lopengl32 -lgdi32 \
			-Llib\glew_x64-mingw-dynamic\lib \
			-Llib\glfw3_x64-mingw-dynamic\lib \
			-Llib\glm_x64-mingw-dynamic\lib

# Cible par défaut
all: $(NAME)

$(NAME): $(OBJ)
	@echo Linking...
	@$(CXX) -o bin/$@ $^ $(LDFLAGS)
	@echo Build complete: $@

%.o: %.cpp
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo Cleaning object files...
	@del /Q $(subst /,\,$(OBJ)) 2>nul || exit 0

fclean: clean
	@echo Removing executable...
	@del /Q $(subst /,\,$(NAME)) 2>nul || exit 0

re: fclean all
