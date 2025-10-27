#ifndef RANDOMLCG_H
#define RANDOMLCG_H
#include <cstdint>

class RandomLCG {
    uint64_t state;
    static constexpr uint64_t a = 6364136223846793005ULL;
    static constexpr uint64_t c = 1ULL;
    static constexpr uint64_t m = (1ULL << 63);
public:
    explicit RandomLCG(uint64_t seed = 88172645463393265ULL) : state(seed) {}
    uint32_t nextU32() { state = (a * state + c) % m; return static_cast<uint32_t>(state >> 16); }
    uint32_t uniform(uint32_t maxExclusive) { return maxExclusive ? (nextU32() % maxExclusive) : 0u; }
};

#endif // RANDOMLCG_H
