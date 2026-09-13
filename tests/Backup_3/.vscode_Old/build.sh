#!/usr/bin/env bash
# Usage: ./build.sh [Development|Shipping]
set -e
CONFIG="${1:-Development}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$ROOT/Assets"
LIB_DIR="$ROOT/Library"
INT_DIR="$ROOT/Intermediate/$CONFIG"
OUT_DIR="$ROOT/Packaging/$CONFIG"
PROJECT_NAME="Ali"

mkdir -p "$INT_DIR" "$OUT_DIR"

if [ "$CONFIG" == "Shipping" ]; then
    FLAGS="-O2 -DNDEBUG -std=c++17"
else
    FLAGS="-g -O0 -std=c++17"
fi

OBJS=()
while IFS= read -r -d '' src; do
    obj="$INT_DIR/$(basename "${src%.cpp}").o"
    echo "Compiling: $src"
    g++ $FLAGS -I"$SRC_DIR" -I"$LIB_DIR" -c "$src" -o "$obj"
    OBJS+=("$obj")
done < <(find "$SRC_DIR" -name "*.cpp" -print0)

if [ ${#OBJS[@]} -eq 0 ]; then
    echo "No .cpp files found under Assets/"
    exit 1
fi

echo "Linking: $PROJECT_NAME ($CONFIG)"
g++ $FLAGS "${OBJS[@]}" -L"$LIB_DIR" -o "$OUT_DIR/$PROJECT_NAME"
echo "Build complete: $OUT_DIR/$PROJECT_NAME"
