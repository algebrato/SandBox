cmake_minimum_required(VERSION 3.1)
project(SandBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -s NO_DISABLE_EXCEPTION_CATCHING -s EXPORTED_FUNCTIONS=\"[_malloc, _free, _decode, _OpenDecoder]\" -s EXPORTED_RUNTIME_METHODS=\"[cwrap, ccall, addFunction]\" -s INITIAL_MEMORY=1073741824  -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=2 -s FORCE_FILESYSTEM=1 -s WASM=1 -s PTHREAD_POOL_SIZE_STRICT=2 -s ALLOW_MEMORY_GROWTH=1 -s ALLOW_TABLE_GROWTH=1 -Wemcc")
set(LINKING_FLAGS "")
set(CC emcc)
set(CXX em++)
set(AR emar)

set(FFMPEG_INCLUDES /usr/local/src/ffmpeg515/include)
set(FFMPEG_LIBS /usr/local/src/ffmpeg515/lib)

include(GNUInstallDirs)

add_subdirectory(Media)
add_subdirectory(MediaWrapper)

file (GLOB_RECURSE INCS SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )

add_executable(SandBox ${INCS} ${SRC})
target_include_directories(SandBox PRIVATE 
    "${FFMPEG_INCLUDES}"
    ${CMAKE_CURRENT_SOURCE_DIR}/MediaWrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/Media/Protocol/QADP
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(SandBox PRIVATE ${LINKING_FLAGS})

target_link_directories(SandBox PRIVATE ${FFMPEG_LIBS})
target_link_libraries(SandBox PRIVATE avcodec avformat avutil avdevice mn-media media_wrap swscale swresample)
target_link_options(SandBox BEFORE PRIVATE ${LINKING_FLAGS})
