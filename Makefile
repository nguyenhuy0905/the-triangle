include Config.mk

vpath %/ $(BUILD_DIR) 
vpath %.o $(BUILD_DIR)/render:$(BUILD_DIR)
vpath %.h render:.
vpath %.c render:.
vpath %.a $(BUILD_DIR)
vpath %.glsl shaders

all: $(BUILD_DIR)/main

override RENDER_OBJ_FILES := $(patsubst render/%.c,$(BUILD_DIR)/render/%.o,\
	$(wildcard render/*.c))
override SHADER_FILES := $(wildcard shaders/*.glsl)
override OPTIMIZE_FLAG := $(if $(filter Debug,$(BUILD_TYPE)),$(DEBUG_BUILD_FLAG),\
	$(if $(filter Release,$(BUILD_TYPE)),$(RELEASE_BUILD_FLAG),\
	$(if $(filter RelWithDeb,$(BUILD_TYPE),$(REL_WITH_DEB_BUILD_FLAG)))))
override CC_FLAGS := -I$(BUILD_DIR)/glad-include -std=c23 -Wall -Werror\
	-Wconversion -fsanitize=address,undefined $(OPTIMIZE_FLAG)

.PHONY: nuke
nuke:
	rm -rf $(BUILD_DIR)/*

$(BUILD_DIR)/main: glad-include/ glad-gl.o main.c \
			common.h render.a \
		$(shell pkgconf --libs glfw3) 
	@echo "Run with environment ASAN_OPTIONS=detect_leak=0"
	@echo "Cuz the weak willed people can't let the kernel clean up stuff"
	@echo "On Linux, it might be fontconfig being based"
	$(CC) main.c $(shell pkgconf --libs --cflags glfw3) \
		$(CC_FLAGS) \
		$(BUILD_DIR)/glad-gl.o $(BUILD_DIR)/render.a \
		-o $(BUILD_DIR)/main

$(BUILD_DIR)/render.a: $(RENDER_OBJ_FILES)
	$(AR) rcs $@ $^

$(RENDER_OBJ_FILES): $(BUILD_DIR)/render/%.o: render/%.c glad-gl.o glad-include/ \
		common.h $(SHADER_FILES)
	@mkdir -p $(BUILD_DIR)/render
	$(CC) $< \
		-c \
		-I$(BUILD_DIR)/glad-include \
		-Ishaders \
		$(CC_FLAGS) \
		$(BUILD_DIR) \
		-o $@

$(BUILD_DIR)/glad-gl.o: glad-src/ glad-include/
	# special treatment: always -O2
	$(CC) $(BUILD_DIR)/glad-src/gl.c -c $(OPTIMIZE_FLAG) -O2 \
		-I$(BUILD_DIR)/glad-include \
		-o $(BUILD_DIR)/glad-gl.o

$(BUILD_DIR)/glad-include/ $(BUILD_DIR)/glad-src/ &:
	@mkdir -p $(BUILD_DIR)
	glad --api 'gl:core=4.6' --out-path $(BUILD_DIR) 
	@mv $(BUILD_DIR)/src $(BUILD_DIR)/glad-src
	@mv $(BUILD_DIR)/include $(BUILD_DIR)/glad-include
