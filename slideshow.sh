#!/bin/bash

set -e

PYBADGE_DIR=/Volumes/CIRCUITPY

if [ $# -lt 1 ]; then
  echo 1>&2 "$0: not enough arguments"
  exit 1
fi

if [ ! -d "$PYBADGE_DIR" ]; then
    echo "$PYBADGE_DIR not found"
    exit 1
fi

for f in "$1"/*; do
  filename=$(basename "$f")
  destination="${filename%.*}.bmp"
  temp=".temp.$destination"
  echo "Installing $filename"
  convert $f -auto-orient -resize 160x128^ -gravity Center -extent 160x128 -dither Riemersma -colors 16 "$temp"
  cp "$temp" "$PYBADGE_DIR/$destination"
  rm "$temp"
done
