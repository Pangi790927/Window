
ifeq ($(OS),Windows_NT)
	CXX = x86_64-w64-mingw32-g++
	CXX_FLAGS = -lopengl32 -lgdi32 -lglu32 -lglew32 -o test
else
	CXX = g++-7
	CXX_FLAGS = -lGLEW -lGLU -lGL -lX11 -o test
endif

all:
	$(CXX) -std=c++11 main.cpp $(CXX_FLAGS)
	./test

clean:
	rm -rf test