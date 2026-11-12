#!/usr/bin/env sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
OUT="$ROOT/GoatranceGaterV2-source.zip"
cd "$ROOT"
zip -r "$OUT" CMakeLists.txt README.md LICENSE Source scripts -x '*/.DS_Store'
printf 'Created %s\n' "$OUT"
