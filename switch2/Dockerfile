FROM devkitpro/devkita64:20260219 AS build
RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates curl cmake ninja-build libboost-dev && rm -rf /var/lib/apt/lists/*
COPY switch2/install-ounce.sh /tmp/install-ounce.sh
# The base image includes devkitPro's package indexes. If its mirror rejects
# index refreshes, use those existing indexes; package signatures stay enabled.
RUN (sh /tmp/install-ounce.sh || dkp-pacman -S --needed --noconfirm libounce ouncevk libkaihen) \
    && dkp-pacman -S --needed --noconfirm switch-sdl2 switch-physfs
WORKDIR /src
COPY . .
# Boost is header-only in this game. Isolate its headers from host libc headers.
RUN mkdir -p deps/boost-headers && cp -a /usr/include/boost deps/boost-headers/
RUN cmake -S . -B build-switch2 -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/libounce/cmake/Switch2.cmake \
    -DSWITCH2=ON -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/out/switch/blobby2 \
    && cmake --build build-switch2 --parallel 2 \
    && cmake --install build-switch2
RUN mkdir -p /out/switch/blobby2/user /out/licenses && cp COPYING AUTHORS /out/licenses/ \
    && dkp-pacman -Q > /out/build-packages.txt
FROM scratch AS export
COPY --from=build /out/ /
