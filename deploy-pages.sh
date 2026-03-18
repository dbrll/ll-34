#!/bin/sh
# deploy-pages.sh - build and deploy ll-34 wasm to gh-pages branch
#
# Usage: ./deploy-pages.sh
#
# Prerequisites:
#   - Emscripten SDK activated (source emsdk/emsdk_env.sh)
#   - git remote "origin" pointing to GitHub repo
#
# What it does:
#   1. Builds the wasm target (make wasm)
#   2. Assembles deploy files in a temp directory
#   3. Force-pushes to the gh-pages orphan branch
#
# The gh-pages branch contains ONLY the static site,
# no source code history.

set -e

WASM_DIR="$(cd "$(dirname "$0")/wasm" && pwd)"
ROOT="$(cd "$(dirname "$0")" && pwd)"

# -- 1. Build wasm target --
echo "Building wasm target..."
cd "$ROOT"
make wasm

# -- 2. Assemble deploy tree in temp dir --
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

cp "$WASM_DIR/index.html"  "$TMPDIR/"
cp "$WASM_DIR/panel.js"    "$TMPDIR/"
cp "$WASM_DIR/ll-34.js"    "$TMPDIR/"
cp "$WASM_DIR/ll-34.wasm"  "$TMPDIR/"

cp -r "$WASM_DIR/assets"   "$TMPDIR/assets"
mkdir -p "$TMPDIR/demos"

# Copy only the disk images and binaries needed for demos.
# Add/remove files here as your demo list evolves.
for f in *
    ; do
    [ -f "$WASM_DIR/demos/$f" ] && cp "$WASM_DIR/demos/$f" "$TMPDIR/demos/"
done

echo "Deploy tree: $(du -sh "$TMPDIR" | cut -f1)"
ls -lh "$TMPDIR"

# -- 3. Push to gh-pages orphan branch --
cd "$TMPDIR"
git init -b gh-pages
git add -A
git commit -m "Deploy ll-34 wasm $(date +%Y-%m-%d)"

REMOTE=$(cd "$ROOT" && git remote get-url origin 2>/dev/null) || true
if [ -z "$REMOTE" ]; then
    echo "No git remote 'origin' found. Deploy tree is in: $TMPDIR"
    echo "Push manually: cd $TMPDIR && git remote add origin <url> && git push -f origin gh-pages"
    trap - EXIT  # keep tmpdir
    exit 0
fi

echo "Pushing to $REMOTE (gh-pages)..."
git remote add origin "$REMOTE"
git push -f origin gh-pages

echo "Done. Enable GitHub Pages on branch 'gh-pages' in repo Settings > Pages."
