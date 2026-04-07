#!/bin/bash
set -e

echo "Building Viby Farming Simulator for WebAssembly..."

BUILD_DIR="build"
REACT_PUBLIC_DIR="react-ui/public"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring CMake with Emscripten..."
emcmake cmake .. -DBUILD_FOR_WEB=ON

echo "Building..."
emmake make

echo "Copying WASM output to React public directory..."
cp viby-farm.js "../$REACT_PUBLIC_DIR/"
cp viby-farm.wasm "../$REACT_PUBLIC_DIR/"

echo "Build complete! Files copied to $REACT_PUBLIC_DIR"
echo "Run 'cd react-ui && npm run dev' to start the dev server."