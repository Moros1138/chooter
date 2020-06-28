# OBJS specifies which files to compile as part of the project
OBJS = src/*.c

# CC specifies which compiler we're using
CC = gcc

# INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = $(shell sdl2-config --cflags) -Iinc

# LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = $(shell sdl2-config --libs)

# COMPILER_FLAGS specifies the additional compilation options we're using
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -Wall -Wextra -pedantic -std=c99

# LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lsodium

# OBJ_NAME specifies the name of our exectuable
OBJ_NAME = chooter

# This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o bin/$(OBJ_NAME)
