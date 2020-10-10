TARGET=retro
CC=clang
CFLAGS=-Wall -Wextra -std=c99

BUILD_DIR=build
SRC_DIR=src

OS := $(shell uname)
ifeq ($(OS), Darwin)
SOURCES=$(wildcard $(SRC_DIR)/*.c)
DARWIN_SOURCES=$(wildcard $(SRC_DIR)/platform/darwin/*.m)

HEADERS=$(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/platform/*.h)

OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES)) \
		$(patsubst $(SRC_DIR)platform/darwin/%.m, $(BUILD_DIR)/%.o,$(DARWIN_SOURCES))
endif 
ifeq ($(OS), Windows_NT)
SOURCES=$(wildcard $(SRC_DIR)/*.c)
		$(wildcard $(SRC_DIR)/platform/win32/*.c)

HEADERS=$(wildcard $(SRC_DIR)/*.h)$(wildcard $(SRC_DIR)/platform/*.h)

OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES)) \
		$(patsubst $(SRC_DIR)platform/win32/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
endif

all: $(TARGET) clean

OS := $(shell uname)
$(TARGET): $(OBJECTS)
	@echo "Compiling executable..."
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BUILD_DIR)/$@
	@echo "Compilation complete."

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