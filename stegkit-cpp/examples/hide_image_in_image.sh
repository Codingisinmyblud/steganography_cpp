#!/bin/bash
set -e
# Example script to hide a secret image inside a cover image
# Assumes executables are built in ../build

mkdir -p out
../build/stegimg --mode hide --in cover.png --payload secret.png --out out/stego.png --pass "my_secret_key"
echo "Hidden. Now extracting..."
../build/stegimg --mode extract --in out/stego.png --out out/recovered_secret.png --pass "my_secret_key"
echo "Done! Check out/recovered_secret.png"
