#!/bin/bash
set -e
# Example script to hide a small video inside a larger cover video

mkdir -p out
../build/stegvid --mode hide --in cover.mp4 --payload small_secret.mp4 --out out/stego_vid.mp4 --pass "inception_key"
echo "Hidden. Extraction..."
../build/stegvid --mode extract --in out/stego_vid.mp4 --out out/recovered_video.mp4 --pass "inception_key"
echo "Done!"
