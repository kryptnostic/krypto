#!/bin/bash  
SRC="~/Downloads/emsdk_portable/emscripten/tag-1.34.4"
EMPP="/emcc"
FLAGS="--bind -std=c++14 -O3 -o" #rm -O3 if not able to include js.mem to work for debugging purpose
CPP_FILE=$1
JS_FILE=${CPP_FILE%.cpp}.js
CMD="$SRC$EMPP $FLAGS $JS_FILE $CPP_FILE"
eval $CMD