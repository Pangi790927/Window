
ifeq ($(OS),Windows_NT)
	CXX = x86_64-w64-mingw32-g++
	CXX_FLAGS = -lopengl32 -lgdi32 -lglu32 -o test.exe
else
	CXX = g++-7
	CXX_FLAGS = -lGLU -lGL -lX11 -o test
endif

all:
	$(CXX) -std=c++11 main.cpp $(CXX_FLAGS)