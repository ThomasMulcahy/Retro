TARGET=retro
CC=clang
CFLAGS=-Wall -Wextra -std=c99 -lobjc -framework Cocoa -framework Metal -framework MetalKit -framework QuartzCore

BUILD_DIR=build
SRC_DIR=src

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJC_SOURCES=$(wildcard $(SRC_DIR)/*.m)
HEADERS=$(wildcard $(SRC_DIR)/*.h)

OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES)) 

all: $(TARGET) clean

OS := $(shell uname)
$(TARGET): $(OBJECTS)
	@echo "Compiling executable..."
	$(CC) $(CFLAGS) -O0 $(OBJECTS) $(OBJC_SOURCES) -o $(BUILD_DIR)/$@
	@echo "Compilation complete."

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Setup project for compilation
setup:
	@echo "Setting up project..."
	mkdir -p $(BUILD_DIR)
	@echo "Setup complete."

run: all
	./build/retro

# Clean object files from build dir
.PHONY: clean
clean:
	@echo "Cleaning..."
	rm -f $(BUILD_DIR)/*.o
	@echo "Finished."