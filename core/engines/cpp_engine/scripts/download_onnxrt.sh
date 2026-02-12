#!/usr/bin/env bash
set -euo pipefail

# Simple helper to download ONNX Runtime C API (Linux x64) to third_party
DEST_DIR="third_party/onnxruntime"
mkdir -p "$DEST_DIR"

echo "Downloading ONNX Runtime wheel / package is platform-specific."
echo "This script attempts to download prebuilt linux-x64 'onnxruntime' package from GitHub releases."

RELEASE_URL="https://github.com/microsoft/onnxruntime/releases/latest/download/onnxruntime-linux-x64-*.tgz"
echo "Please download the correct package manually if automatic download fails."
echo "Attempting to fetch..."

tmpfile=$(mktemp)
set +e
curl -L "$RELEASE_URL" -o "$tmpfile"
rc=$?
set -e
if [ $rc -ne 0 ]; then
    echo "Automatic download failed. Please visit https://github.com/microsoft/onnxruntime/releases to download the correct package." >&2
    exit 1
fi
tar -xzf "$tmpfile" -C "$DEST_DIR" --strip-components=1
rm "$tmpfile"
echo "ONNX Runtime extracted to $DEST_DIR"
