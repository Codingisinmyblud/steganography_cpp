#include "steg/rng.hpp"

namespace steg {

LCG::LCG(uint64_t seed) : state_(seed) {
    // skip first few to mix
    for (int i = 0; i < 10; ++i) next();
}

uint32_t LCG::next() {
    state_ = state_ * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<uint32_t>(state_ >> 32);
}

uint32_t LCG::next(uint32_t max) {
    return max == 0 ? 0 : (next() % max);
}

} // namespace steg
