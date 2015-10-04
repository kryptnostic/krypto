# usage: buildKryptnosticEngineJS.sh
FLAGS_PRE="-o"
FLAGS_POST="--bind -std=c++11 -O3 --memory-init-file 0 -s TOTAL_STACK=10000000 -s DEMANGLE_SUPPORT=1 -ftemplate-depth=8192"
EMSCRIPTEN="$EMSCRIPTEN_HOME/emscripten/tag-1.34.11/em++"
CPP_FILE="client/KryptnosticClientGetter.cpp"
JS_FILE="$KRYPTO_JS_HOME/KryptnosticClient.js"
CMD="$EMSCRIPTEN $FLAGS_PRE $JS_FILE $FLAGS_POST $CPP_FILE"
eval $CMD
