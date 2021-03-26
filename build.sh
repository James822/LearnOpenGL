#!/bin/bash

### Build file for LearnOpenGL



ROOT_DIR="/run/media/james/extra_space/EXTRA_STORAGE/Projects/learnopenGL/LearnOpenGL"

CFLAGS="-std=c++17 -Wall"
LDFLAGS="`pkg-config --static --libs glfw3`"
LCFLAGS="`pkg-config --cflags glfw3`"
##

THESE_FLAGS="$LCFLAGS $LDFLAGS $CFLAGS"
OUTPUT="$ROOT_DIR/builds/linux-x64/main"
INCLUDES_FLAG="-isystem $ROOT_DIR/includes"
g++ $THESE_FLAGS main.cpp glad.c -o $OUTPUT $INCLUDES_FLAG
