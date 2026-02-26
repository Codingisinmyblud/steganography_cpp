# Architecture Design

The core of `stegkit-cpp` is designed around embedding interfaces and data streaming architectures.

## Components
1. **Codecs**: `ImageCodec` and `VideoCodec` abstract the underlying representation of visual data. `ImageCodec` wraps `stb_image`, while `VideoCodec` interfaces with `FFmpeg`.
2. **Embedders**: Implementing either `ImageEmbedder` or `VideoEmbedder`, these classes define spatial or transform-domain manipulation of data (e.g. LSB, DCT, DWT).
3. **Container format**: Arbitrary payloads are enveloped into `Container` frames, which add error correction, checksum verification (Adler-32 stub), and length tags.
4. **Security**: `KeySchedule` combined with PRNG (`RNG`) adds deterministic pseudo-random distribution of bits across the cover medium based on a password.

## CLI Apps
- `stegimg`: Interacts with ImageEmbedders.
- `stegvid`: Interacts with VideoEmbedders.
