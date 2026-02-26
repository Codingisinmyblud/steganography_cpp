# Algorithms

`stegkit-cpp` supports multiple algorithms:

1. **Least Significant Bit (LSB)**
   - Modifies the 1 or 2 least significant bits of pixel channels.
   - Extremely high capacity, but fragile to compression.

2. **Discrete Cosine Transform (DCT) - *Stub***
   - Modifies the mid-frequency ranges of the 8x8 DCT blocks for JPEG-style images.
   - Robust against lossy compression.

3. **Discrete Wavelet Transform (DWT) - *Stub***
   - Embeds data recursively in HH and HL quadrants.

4. **Motion Vector Hiding (MV) - *Stub***
   - Appends minor sub-pixel variations to macroblock motion vectors in compressed video streams without perceptibly altering frame sequences.
