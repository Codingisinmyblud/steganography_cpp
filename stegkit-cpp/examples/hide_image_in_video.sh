#!/bin/bash
set -e
# Example script to hide an image in a video

mkdir -p out
../build/stegvid --mode hide --in cover.mp4 --payload secret.png --out out/stego.mp4 --pass "video_key"
echo "Hidden. Extraction..."
../build/stegvid --mode extract --in out/stego.mp4 --out out/recovered_image.png --pass "video_key"
echo "Done!"
