#!/usr/bin/env bash
set -euo pipefail

# Simple helper to download libtorch prebuilt for Linux x86_64 (CPU)
# Usage: ./download_libtorch.sh [cpu|cuda]

TYPE=${1:-cpu}
DEST_DIR="third_party/libtorch"
mkdir -p "$DEST_DIR"

if [ "$TYPE" = "cpu" ]; then
    URL="https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-latest.zip"
else
    echo "CUDA builds require matching CUDA version; please download manually if needed." >&2
    URL="https://download.pytorch.org/libtorch/cu117/libtorch-shared-with-deps-latest.zip"
fi

echo "Downloading libtorch ($TYPE) to $DEST_DIR..."
tmpfile=$(mktemp)
curl -L "$URL" -o "$tmpfile"
unzip -o "$tmpfile" -d "$DEST_DIR"
rm "$tmpfile"
echo "libtorch extracted to $DEST_DIR"
