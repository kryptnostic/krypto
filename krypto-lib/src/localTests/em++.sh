#!/bin/bash  
SRC="~/Downloads/emsdk_portable/emscripten/tag-1.34.4"
EMPP="/em++"
FLAGS_POST="-std=c++14 -O3"
CPP_FILE=$1
FLAGS_PRE="-o"
JS_FILE=${CPP_FILE%.cpp}.js
CMD="$SRC$EMPP $FLAGS_PRE $JS_FILE $FLAGS_POST $CPP_FILE"
eval $CMD