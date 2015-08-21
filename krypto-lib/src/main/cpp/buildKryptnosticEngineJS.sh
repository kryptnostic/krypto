#!/bin/bash  
#Usage: bash buildKryptnosticEngineJS.sh
SRC="~/Desktop/emscripten/tag-1.34.4"
EMPP="/em++"
FLAGS_POST="--bind -std=c++14 --memory-init-file 0 -s TOTAL_MEMORY=1000000000"
CPP_FILE="KryptnosticEngineGetter.cpp"
FLAGS_PRE="-o"
JS_FILE="../js/KryptnosticEngineClient.js"
CMD="$SRC$EMPP $FLAGS_PRE $JS_FILE $FLAGS_POST $CPP_FILE"
eval $CMD