TARGET=retro
CC=clang
CFLAGS=-Wall -std=c99

BUILD_DIR=build
SRC_DIR=src

SOURCES=$(wildcard $(SRC_DIR)/*.c)
HEADERS=$(wildcard $(SRC_DIR)/*.h)$(wildcard $(SRC_DIR)/platform/*.h)
WINDOWS_SOURCES=$(wildcard $(SRC_DIR)/platform/win32/*.c)
DARWIN_SOURCES=$(wildcard $(SRC_DIR)/platform/darwin/*.m)
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(TARGET) clean

# Build the target executable
$(TARGET): $(OBJECTS)
	@echo "Compiling executable..."
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BUILD_DIR)/$@
	@echo "Compilation complete."

# Create object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Setup project for compilation
setup:
	@echo "Setting up project..."
	mkdir -p $(BUILD_DIR)
	@echo "Setup complete."

# Clean object files from build dir
.PHONY: clean
clean:
	@echo "Cleaning..."
	rm -f $(BUILD_DIR)/*.o
	@echo "Finished."