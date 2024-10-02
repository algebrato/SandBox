cmake_minimum_required(VERSION 3.1)
project(SandBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -s EXPORTED_FUNCTIONS=\"[_malloc, _free, _decode, _OpenDecoder]\" -s EXPORTED_RUNTIME_METHODS=\"[cwrap, ccall, addFunction]\" -s INITIAL_MEMORY=1073741824  -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=2 -s FORCE_FILESYSTEM=1 -s WASM=1 -s PTHREAD_POOL_SIZE_STRICT=2 -s ALLOW_MEMORY_GROWTH=1 -s ALLOW_TABLE_GROWTH=1 -s NO_DISABLE_EXCEPTION_CATCHING")
include(GNUInstallDirs)

add_subdirectory(Media)
add_subdirectory(MediaWrapper)

file (GLOB_RECURSE INCS SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )

add_executable(SandBox ${INCS} ${SRC})
target_include_directories(SandBox PRIVATE 
    "/home/algebrato/repositories/build/ffmpeg-4/include"
    ${CMAKE_CURRENT_SOURCE_DIR}/Media/Protocol/QADP
    ${CMAKE_CURRENT_SOURCE_DIR}/src


)
target_link_directories(SandBox PRIVATE "/home/algebrato/repositories/build/ffmpeg7/lib")

target_link_libraries(SandBox PRIVATE avcodec avformat avutil mn-media)
