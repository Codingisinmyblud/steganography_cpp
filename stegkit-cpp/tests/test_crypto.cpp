#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/rng.hpp"
#include "steg/key_schedule.hpp"

using namespace steg;

TEST_CASE("Key Schedule DJB2") {
    uint64_t seed1 = KeySchedule::derive_seed("password123");
    uint64_t seed2 = KeySchedule::derive_seed("password124");
    uint64_t seed3 = KeySchedule::derive_seed("password123");

    CHECK(seed1 != seed2);
    CHECK(seed1 == seed3); // Deterministic
}

TEST_CASE("LCG Pseudo-random generator") {
    LCG prng(12345);
    uint32_t v1 = prng.next();
    uint32_t v2 = prng.next();

    // Re-seeding with same seed should yield same sequence
    LCG prng2(12345);
    uint32_t v3 = prng2.next();

    CHECK(v1 != v2);
    CHECK(v1 == v3);
}
