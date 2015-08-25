#!/bin/bash
SRC="~/Downloads/emsdk_portable/emscripten/tag-1.34.4"
EMPP="/emcc"
FLAGS="--bind -std=c++14 -O3 -s TOTAL_MEMORY=1000000000 --memory-init-file 0 -o" #include .js.mem (the memory initializer) in the generated .js
CPP_FILE=$1
JS_FILE=${CPP_FILE%.cpp}.js
CMD="$SRC$EMPP $FLAGS $JS_FILE $CPP_FILE"
eval $CMD