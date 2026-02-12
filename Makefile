vpath %/ build
vpath %.o build

all: build/main

.PHONY: nuke
nuke:
	rm -r build/*

build/main: glad-include/ glad-gl.o main.c \
			triangle-fragment.glsl triangle-vertex.glsl \
		$(shell pkgconf --libs glfw3)
	@echo "Run with environment ASAN_OPTIONS=detect_leak=0"
	@echo "Cuz the weak willed people can't let the kernel clean up stuff"
	@echo "On Linux, it might be fontconfig being based"
	@echo
	$(CC) main.c $(shell pkgconf --libs --cflags glfw3) \
		-Ibuild/glad-include \
		-std=c23 \
		-Wall -Werror \
		-fsanitize=address \
		-ggdb \
		build/glad-gl.o \
		-o build/main

build/glad-gl.o: glad-src/ glad-include/
	$(CC) build/glad-src/gl.c -c -O2 -ggdb \
		-Ibuild/glad-include \
		-o build/glad-gl.o

build/glad-include/ build/glad-src/ &:
	@mkdir -p build
	@rm -rf build/glad-include build/glad-src
	glad --api 'gl:core=4.6' --out-path build
	@mv build/src build/glad-src
	@mv build/include build/glad-include

