dist/mp4info.wasm.js:
	../../Libs/emscripten/emcc --bind \
    -O3 \
    -L/home/algebrato/repositories/build/ffmpeg7/lib \
    -I/home/algebrato/repositories/build/ffmpeg7/include/ \
    -s EXPORTED_FUNCTIONS='["_decode", "_OpenDecoder", "_malloc", "_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["cwrap", "ccall", "addFunction"]' \
    -s INITIAL_MEMORY=1073741824 \
    -s ASSERTIONS=1 \
    -s BINARYEN_ASYNC_COMPILATION=0 \
    -s STACK_OVERFLOW_CHECK=2 \
    -s FORCE_FILESYSTEM=1 \
    -s WASM=1 \
    -s PTHREAD_POOL_SIZE_STRICT=2 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ALLOW_TABLE_GROWTH=1 \
    -lavcodec \
    -lavformat \
    -lavfilter \
    -lavutil \
    -lswscale \
    -lswresample \
    -pthread \
    -lworkerfs.js \
    -o build/mp4info.js \
    src/SandBox.cpp
