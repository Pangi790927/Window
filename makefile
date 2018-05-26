
ifeq ($(OS),Windows_NT)
	NAME = test.exe
	CXX = x86_64-w64-mingw32-g++
	CXX_FLAGS = -L. -lopengl32 -lgdi32 -lglu32 -o $(NAME)
	RM = del
	GLEW = glew.o
else
	NAME = test
	CXX = g++-7
	CXX_FLAGS = -lGLEW -lGLU -lGL -lX11 -o $(NAME)
	RM = rm -rf
	GLEW = 
endif

CXX_INCLUDE = -I../Misc

all: clean $(GLEW)
	$(CXX) -std=c++17 main.cpp $(GLEW) $(CXX_FLAGS) $(CXX_INCLUDE)
	./$(NAME)

ifeq ($(OS),Windows_NT)
glew.o:
	$(CXX) -c glew.c -o glew.o
endif

clean:
	$(RM) $(NAME)
