TARGET=retro
CC=clang
CFLAGS=-std=c99
DEBUG_FLAGS=-Wall -Wextra -g -DDEBUG=1
OBJC_FLAGS=-lobjc -framework Cocoa -framework Metal -framework MetalKit -framework QuartzCore

BUILD_DIR=build
SRC_DIR=src
SRC_PLATFORM_DIR=$(SRC_DIR)/platform
SRC_FONT_DIR=$(SRC_DIR)/font

SOURCES=$(wildcard $(SRC_DIR)/*.c) \
		$(wildcard $(SRC_PLATFORM_DIR)/*.c) \

OBJC_SOURCES=$(wildcard $(SRC_PLATFORM_DIR)/*.m)

HEADERS=$(wildcard $(SRC_DIR)/*.h) \
		$(wildcard $(SRC_PLATFORM_DIR)/*.h) \


all: $(TARGET) clean

# Target to compile for release
$(TARGET): $(SOURCES)
	@echo "Compiling executable for release mode..."
	$(CC) $(CFLAGS) $(OBJC_FLAGS) -O2 $(SOURCES) $(OBJC_SOURCES) -o $(BUILD_DIR)/release/$@
	@echo "Compilation complete."

# Target to compile in with debug options
debug: $(SOURCES)
	@echo "Compiling executable in debug mode..."
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OBJC_FLAGS) -O0 $(SOURCES) $(OBJC_SOURCES) -o $(BUILD_DIR)/$(TARGET)
	@echo "Compilation complete."

# Setup project for compilation
setup:
	@echo "Setting up project..."
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/release
	@echo "Setup complete."

run: debug
	./build/retro

# Clean object files from build dir
.PHONY: clean
clean:
	@echo "Cleaning..."
	rm -f $(BUILD_DIR)/*.o
	@echo "Finished."