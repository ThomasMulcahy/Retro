TARGET=retro
CC=clang
CFLAGS=-0d -Wall

BUILD_DIR=build
SRC_DIR=src

SOURCES=$(wildcard $(SRC_DIR)/*.c)
WINDOWS_SOURCES=$(wildcard $(SRC_DIR)/platform/win32/*.c)
DARWIN_SOURCES=$(wildcard $(SRC_DIR)/platform/darwin/*.c)