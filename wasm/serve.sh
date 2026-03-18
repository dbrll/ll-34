#!/bin/sh
# serve.sh - serve ll-34 WebAssembly GUI on localhost
#
# Usage: ./wasm/serve.sh [port]
#   Default port: 1134
#
# Requires Python 3 (no other dependencies).

PORT="${1:-1134}"
DIR="$(cd "$(dirname "$0")" && pwd)"

printf 'll-34 running at http://localhost:%s\n' "$PORT"

python3 -m http.server "$PORT" -d "$DIR" 2>/dev/null
