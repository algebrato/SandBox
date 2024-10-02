FROM emscripten/emsdk:3.1.63 as build

ARG FFMPEG_VERSION=4.0.4

ARG PREFIX=/opt/ffmpeg
ARG MAKEFLAGS="-j4"

# Build dependencies.
RUN apt-get update && apt-get install -y autoconf libtool build-essential llvm

# Download ffmpeg release source.
RUN cd /tmp/ && \
  wget http://ffmpeg.org/releases/ffmpeg-${FFMPEG_VERSION}.tar.gz && \
  tar zxf ffmpeg-${FFMPEG_VERSION}.tar.gz && rm ffmpeg-${FFMPEG_VERSION}.tar.gz

ARG CFLAGS="-s USE_PTHREADS=1 -O3 -I${PREFIX}/include"
ARG LDFLAGS="$CFLAGS -L${PREFIX}/lib -s INITIAL_MEMORY=33554432"

# Configure and build FFmpeg with emscripten.
# Disable all programs and only enable features we will use.
# https://github.com/FFmpeg/FFmpeg/blob/master/configure
RUN cd /tmp/ffmpeg-${FFMPEG_VERSION} && \
  emconfigure ./configure \
  --prefix=${PREFIX} \
  --target-os=none \
  --arch=x86_32 \
  --enable-cross-compile \
  --disable-debug \
  --disable-x86asm \
  --disable-inline-asm \
  --disable-stripping \
  --disable-programs \
  --disable-doc \
  --disable-all \
  --enable-avcodec \
  --enable-gray \
  --enable-avformat \
  --enable-avfilter \
  --enable-avdevice \
  --enable-avutil \
  --enable-swresample \
  --enable-swscale \
  --enable-filters \
  --enable-protocol=file \
  --enable-decoder=h264 \
  --enable-gpl \
  --extra-cflags="$CFLAGS" \
  --extra-cxxflags="$CFLAGS" \
  --extra-ldflags="$LDFLAGS" \
  --nm="llvm-nm -g" \
  --ar=emar \
  --as=llvm-as \
  --ranlib=llvm-ranlib \
  --cc=emcc \
  --cxx=em++ \
  --objcc=emcc \
  --dep-cc=emcc

RUN cd /tmp/ffmpeg-${FFMPEG_VERSION} && \
  emmake make -j4 && \
  emmake make install

RUN mkdir /build && mkdir /build/src && mkdir /build/mp4wasm

COPY ./src/SandBox.cpp /build/src/SandBox.cpp
COPY ./Makefile /build/Makefile

WORKDIR /build

