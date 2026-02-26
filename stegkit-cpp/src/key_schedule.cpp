#include "steg/key_schedule.hpp"

namespace steg {

uint64_t KeySchedule::derive_seed(const std::string& passphrase) {
    // Super basic DJB2 hash for the PRNG seed
    uint64_t hash = 5381;
    for (char c : passphrase) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

} // namespace steg
