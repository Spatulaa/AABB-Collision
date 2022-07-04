.PHONY: all

all: main

main:
	g++ main.cpp libraries/include/glad/glad.c -Ilibraries/include -Llibraries/lib -lglfw3 -lopengl32 -lgdi32 -o build