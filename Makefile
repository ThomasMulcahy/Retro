TARGET=retro
CC=clang
CFLAGS=-std=c99
DEBUG_FLAGS=-Wall -Wextra
OBJC_FLAGS=-lobjc -framework Cocoa -framework Metal -framework MetalKit -framework QuartzCore

BUILD_DIR=build
SRC_DIR=src

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJC_SOURCES=$(wildcard $(SRC_DIR)/*.m)
HEADERS=$(wildcard $(SRC_DIR)/*.h)

OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES)) 

all: $(TARGET) clean

$(TARGET): $(OBJECTS)
	@echo "Compiling executable for release mode..."
	$(CC) $(CFLAGS) $(OBJC_FLAGS) -O2 $(OBJECTS) $(OBJC_SOURCES) -o $(BUILD_DIR)/release/$@
	@echo "Compilation complete."

debug: $(OBJECTS)
	@echo "Compiling executable in debug mode..."
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OBJC_FLAGS) -O0 $(OBJECTS) $(OBJC_SOURCES) -o $(BUILD_DIR)/$(TARGET)
	@echo "Compilation complete."

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

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