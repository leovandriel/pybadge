#!/bin/bash

set -e

if [ $# -lt 1 ]; then
  echo 1>&2 "$0: not enough arguments"
  exit 2
fi

cp "$1" /Volumes/CIRCUITPY/code.py
