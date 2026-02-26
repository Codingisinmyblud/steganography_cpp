# stegkit-cpp

A complete steganography toolkit written in C++. This project allows hiding arbitrary data (like text, images, or small files) inside cover images and videos.

## Directory Structure
- `apps/`: CLI applications (`stegimg`, `stegvid`)
- `src/`, `include/`: Core library containing algorithms, codecs, and bitstream manipulation
- `tests/`: Unit tests for the core logic (using doctest)
- `tools/`: Utility programs for noise generation and capacity analysis
- `examples/`: Example bash scripts demonstrating usage
- `docs/`: Design and algorithm documentation

## Building

If you are using the local build dependencies (CMake and FFmpeg installed in `~/.local`):

```bash
export PATH="$HOME/.local/bin:$PATH"
export PKG_CONFIG_PATH="$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH"
export CMAKE_PREFIX_PATH="$HOME/.local"

mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

## Running Tests

```bash
cd build
ctest --output-on-failure
```

---

## Features and Usage Examples

All commands assume you are running them from the `build/` directory where the executables are compiled, or that `build/` is in your PATH. 

For these examples, let's say we have a cover image `cover.png` and a secret file we want to hide called `secret.txt`.

### 1. Hiding Data in an Image (LSB Embedding)

This takes a cover image, embeds the payload (`secret.txt`) into the Least Significant Bits of the pixel data using a password, and outputs a new `stego.png`.

```bash
./stegimg --mode hide --in cover.png --payload secret.txt --out stego.png --pass "SuperSecretKey123!" --algo lsb
```

### 2. Extracting Data from an Image

To get your data back, you provide the stego image and the exact same password used during hiding. It will decrypt and extract the hidden payload.

```bash
./stegimg --mode extract --in stego.png --out recovered_secret.txt --pass "SuperSecretKey123!" --algo lsb
```

### 3. Generating Random Noise (Testing payloads)

We built a tool to generate random byte sequences if you want to test how much data you can fit into an image.

```bash
./gen_noise --size 4096 --out dummy_payload.bin
```
You can then try hiding `dummy_payload.bin` using `stegimg`.

### 4. Video Steganography (MV / LSB)

Video steganography uses the `stegvid` tool. *(Note: The MV components are currently stubbed in the architecture but this is how the CLI is structured to run them once the FFmpeg codecs are fully hooked up in the `src/video_codec.cpp`)*.

```bash
# Hide
./stegvid --mode hide --in cover.mp4 --payload secret.zip --out stego.mp4 --pass "VideoKey"

# Extract
./stegvid --mode extract --in stego.mp4 --out recovered.zip --pass "VideoKey"
```

## Documentation
See the `docs/` folder for information on the `DESIGN.md`, `ALGORITHMS.md`, and the future `ROADMAP.md`.
