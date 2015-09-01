#!/bin/bash  
SRC="~/Downloads/emsdk_portable/emscripten/tag-1.34.4"
EMPP="/em++"
FLAGS_POST="-std=c++14 -O3"
CPP_FILE=$1
FLAGS_PRE="-o"
JS_FILE=${CPP_FILE%.cpp}.js
FORCE_MEMORY="-s TOTAL_MEMORY=1000000000"
CMD="$SRC$EMPP $FLAGS_PRE $JS_FILE $FLAGS_POST $CPP_FILE $FORCE_MEMORY"
eval $CMD