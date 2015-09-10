#Usage: buildKryptnosticEngineJS.sh
#SRC="~/Downloads/emsdk_portable/emscripten/tag-1.34.4"
SRC="~/Desktop/emscripten/tag-1.34.4"
EMPP="/em++"
FLAGS_POST="--bind -std=c++11 -O3 --memory-init-file 0 -s TOTAL_STACK=10000000 -s DEMANGLE_SUPPORT=1 -ftemplate-depth=8192"
CPP_FILE="client/KryptnosticClientGetter.cpp"
FLAGS_PRE="-o"
JS_FILE="../../../../../krypto-js/KryptnosticClient.js"
CMD="$SRC$EMPP $FLAGS_PRE $JS_FILE $FLAGS_POST $CPP_FILE"
eval $CMD
