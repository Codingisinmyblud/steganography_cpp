#pragma once
#include <cstdint>
#include <vector>

namespace steg {

class RNG {
public:
    virtual ~RNG() = default;
    virtual uint32_t next() = 0;
    virtual uint32_t next(uint32_t max) = 0;
};

class LCG : public RNG {
public:
    explicit LCG(uint64_t seed = 12345);
    uint32_t next() override;
    uint32_t next(uint32_t max) override;
private:
    uint64_t state_;
};

} // namespace steg
